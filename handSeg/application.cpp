#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <string>
#include <array>
#include <fstream>
using namespace cv;
using namespace std;


MatND hist;
MatND hist_noUpdate;
Mat visited;
Mat frame;
Mat segmentation;
int maxCol;
int maxRow;
int minCol;
int minRow;
bool init=true;
double paraTable[24][7];

//HSV range
int iLowH = 0;
int iHighH = 20;
int iLowS = 30;
int iHighS = 150;
int iLowV = 80;
int iHighV = 255;
int changeThreshold = 20;


//Convert from RBG to luma
float RGBtoLuma(float R, float G, float B )
{
	float Y;
	Y = 0.2126 * R+ 0.7152 * G + 0.0722 * B;
	return Y;
}


//Recursive search for hand Square Region (Region of interest)
void searchHand_loop(int x, int y, int m)
{
    //x, y is the center position(moment), m determines the search square size
    int tempMaxCol = maxCol;
    int tempMaxRow = maxRow;;
    int tempMinCol = minCol;
    int tempMinRow = minRow;;
    // search the current region (x-m,y-m) to (x+m,y+m)
    for(int i=y-m; i< y+m; i++){
        for(int j=x-m; j<x+m; j++){
            //index out of range check:
            if( i<0 || j<0 || i > frame.rows-1 || j > frame.cols-1)
            {}
            else
            {
                if(frame.at<uchar>(i,j)==255)
                {
                    //if a 255 is found, update max, min value of col and row
                    maxCol = maxCol< j? j : maxCol;
                    maxRow = maxRow< i? i : maxRow;
                    minCol = minCol> j? j : minCol;
                    minRow = minRow> i? i : minRow;
                }
            }
        }
    }
    if( tempMaxCol != maxCol || tempMaxRow !=maxRow || tempMinCol != minCol || tempMinRow !=minRow )
        searchHand_loop(x, y, m+1); //recursive to search with m+1
    
    //if max, min value of col and row don't change in the current search range, stop search and use current range as the final hand square region of interest
    
}

//******************************************************************************************************************************√

int startProcessing(String vdFileName, VideoCapture cap)
{
    //Define where the output will be stored at
    String videoOut = String("/Users/BboyKellen/Documents/Xcode/headSeg/handSeg/handSeg/output_") + vdFileName;
    
    if ( !cap.isOpened() )  // if not success, exit program
    {
        cout << "Cannot open the web cam/video file: "<< videoOut << endl;
        return -1;
    }
    
    
    //Capture a temporary image from the camera
    Mat imgTmp;
    cap.read(imgTmp);
    
    double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
    double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video
    Size frameSize(static_cast<int>(dWidth), static_cast<int>(dHeight));
    VideoWriter oVideoWriter (videoOut, CV_FOURCC('M', 'P', 'E', 'G'), 20, frameSize, true); //initialize the VideoWriter object
    
    if ( !oVideoWriter.isOpened() ) //if not initialize the VideoWriter successfully, exit the program
    {
        cout << "ERROR: Failed to write the video of "<< vdFileName<< endl;
        return -1;
    }
    //Create a black image with the size as the camera/video output
    Mat imgLines = Mat::zeros(imgTmp.size(), CV_8UC3 );
    //initialize some parameters for change detection
    bool first =true;
    Mat previousFrame;
    
    //******************************* frame loop/ For each frame of the input source **************************
    
    while (true)
    {
        Mat imgOriginal;
        Mat imgHSV;
        Mat imgThresholded;
        Mat luma(imgTmp.rows, imgTmp.cols, CV_8UC1, Scalar(0));
        Mat change=luma.clone();
        
        bool bSuccess = cap.read(imgOriginal); // read a new frame from the input source
        if (!bSuccess) {  //if not success, break loop
            cout << "Cannot read a frame from video stream" << endl;
            break;
        }
        cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the frame from BGR to HSV
        inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image
        Mat andOperation = imgThresholded.clone();
        
        //morphological opening (remove small objects from the foreground)
        erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
        dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
        //morphological closing (fill small holes in the foreground)
        dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
        erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
        
        //convert to greyscale image
        for(int i=0; i<luma.rows; i++){
            for(int j=0; j<luma.cols; j++)
            {
                Vec3b intensity = imgOriginal.at<Vec3b>(i, j);
                uchar blue = intensity.val[0];
                uchar green = intensity.val[1];
                uchar red = intensity.val[2];
                luma.at<uchar>(i, j) = RGBtoLuma(red, green, blue );//calls the function that converts image from RGB to Luma
            }
        }
        
        //frame change detection
        if(first==true)
        {
            previousFrame=luma;
            first=false;
        }
        else
        {
            for(int i=0; i<luma.rows; i++){
                for(int j=0; j<luma.cols; j++)
                {
                    //for each pixel, compares the value with previous frame
                    //if bigger than threshold, it is marked as 255, else 0
                    uchar intensity_now = luma.at<uchar>(i, j);
                    uchar intensity_previous = previousFrame.at<uchar>(i, j);
                    if(abs(intensity_now-intensity_previous)>changeThreshold)
                        change.at<uchar>(i, j) = 255;
                    else
                        change.at<uchar>(i, j) = 0;
                }
            }
            previousFrame=luma;
        }
        //obtain the ANDed frame by doing and operation on HSV threshold and frame change
        bitwise_and(imgThresholded, change, andOperation);
        
        //Calculate the moments of the thresholded image
        Moments oMoments = moments(andOperation);
        double dM01 = oMoments.m01;
        double dM10 = oMoments.m10;
        double dArea = oMoments.m00;
        
        imgLines =Mat::zeros( imgTmp.size(), CV_8UC3 );
        Mat filter = Mat::zeros( imgTmp.size(), CV_8UC1 );
        
        // if the area <= 10000, I consider that the there are no hand in the image and it's because of the size.
        if (dArea > 500000)
        {
            //calculate the position of the hand
            int posX = dM10 / dArea;
            int posY = dM01 / dArea;
            //get hand square region
            frame = andOperation; //to search hand in ANDed frame
            maxRow=posY; maxCol=posX; minRow=posY; minCol=posX;
            //call the hand search function that runs the algorithm to get the hand region of interest
            searchHand_loop(posX,posY,150);
            
            //construct a square line for presentation
            if ( posX >= 0 && posY >= 0)
            {
                line(imgLines, Point(minCol, minRow), Point(maxCol, minRow), Scalar(0,255,0), 3);
                line(imgLines, Point(minCol, minRow), Point(minCol, maxRow), Scalar(0,255,0), 3);
                line(imgLines, Point(minCol, maxRow), Point(maxCol, maxRow), Scalar(0,255,0), 3);
                line(imgLines, Point(maxCol, maxRow), Point(maxCol, minRow), Scalar(0,255,0), 3);
                line(imgLines, Point(posX-10, posY+10), Point(posX+10, posY-10), Scalar(0,0,255), 3);
                line(imgLines, Point(posX-10, posY-10), Point(posX+10, posY+10), Scalar(0,0,255), 3);
            }
            imgOriginal = imgOriginal + imgLines;
            
            //construct and define the region of interest
            for(int i=minRow; i<maxRow; i++){
                for(int j=minCol; j<maxCol; j++)
                {
                    filter.at<uchar>(i,j)=255; //filter: hand square region of interest
                }
            }
        }
        
        Mat finalShow; // create a new img for storing the final hand segementation
        bitwise_and(filter, imgThresholded, finalShow);//apply the square hand region to HSV threshold img. Use it as the final hand segmentation and store it in finalshow
        
        cvtColor(finalShow, finalShow, COLOR_GRAY2BGR); //Convert the final segmentation from gray to RGB for output video
        oVideoWriter.write(finalShow); //writer the segmentation into the video output

        imshow("segmentation", finalShow); //show the hand segmentation
        imshow("Original+frame", imgOriginal); //show the original image with the ROI
        finalShow = imgOriginal+finalShow;
        imshow("final", finalShow); //show the combined

        if (waitKey(1) == 1) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
        {
            cout << "esc key is pressed by user" << endl;
            break;
        }
    }
    return 0;
}

//*************************  Load Parameters   **************************
int setPara()
{   //read parameters from file testPara.txt (H,S,V threshold and frame change threshold)
    ifstream inFile;
    inFile.open("/Users/BboyKellen/Documents/Xcode/headSeg/handSeg/handSeg/testPara.txt", ios::in);
    if (! inFile) {
        cerr << "unable to open file testPara.txt for reading" << endl;
        return 1;
    }
    
    for(int i=0; i<20; i++)
        for(int j=0; j<7; j++)
            inFile >> paraTable[i][j];
    
    inFile.close();
    return 0;
}
//****************************************  Main function   **************************
int main( int argc, char** argv )
{
    bool realTimeMode;
    bool trainMode;
    cout << "rea-time? 1 if yes 0 if false: ";
    cin >> realTimeMode;
    
    String videos_train[] = {"noise1.mpeg","noise2.mpeg","noise3.mpeg","noise4.mpeg"};
    String videos_test[] = {"noiseTest1.mpeg","noiseTest2.mpeg","noiseTest3.mpeg","noiseTest4.mpeg"};
    
    if(realTimeMode==false)
    {
        cout << "train mode or test mode? 1 if train mode, 0 if test mode: ";
        cin >> trainMode;
        //load all the parameters for test
        int error = setPara();
        if (error==true)
            return -1;
        for(int s=0; s<sizeof(paraTable)/sizeof(paraTable[0]);s++)
        {   //each iteration is one parameter set
            iLowH = paraTable[s][0];
            iHighH = paraTable[s][1];
            iLowS = paraTable[s][2];
            iHighS = paraTable[s][3];
            iLowV = paraTable[s][4];
            iHighV = paraTable[s][5];
            changeThreshold = paraTable[s][6];
            if(iLowH==0 && iHighH==0 && iLowS==00 && iHighS==00 && iLowV==0 && iHighV==0 && changeThreshold==0)
            {
                cout<<"******************************************************************************************"<<endl;
                cout<<"*                                                                                        *"<<endl;
                cout<<"*                                Test Done!!!                                            *"<<endl;
                cout<<"*                                                                                        *"<<endl;
                cout<<"******************************************************************************************"<<endl;
                break;
            }
            
            //run for each input video with the current parameter set
            for(int i=0;i<4;i++)
            {
                String vdFileName;
                if(trainMode==true)
                    vdFileName = videos_train[i];
                else
                    vdFileName = videos_test[i];
                String videoLoc = String("/Users/BboyKellen/Documents/Xcode/headSeg/handSeg/") + vdFileName;
                VideoCapture cap(videoLoc); // open the video file for reading
                startProcessing(vdFileName,cap);
            }
            //call script to upload the segmentation videos to the ECE server account for evalution
            if(trainMode==true)
                system("/Users/BboyKellen/Documents/Xcode/headSeg/handSeg/handSeg/visionTrain.sh");
            else
                system("/Users/BboyKellen/Documents/Xcode/headSeg/handSeg/handSeg/visionTest.sh");

            cout<<"******************************************************************************************"<<endl;
            cout<<"*                                                                                        *"<<endl;
            cout<<"*                                                                                        *"<<endl;
            cout<<"     Finished parameter set #: "<<s+1<<" ------ Parameter: "<<iLowH<<", "<<iHighH<<", "<<iLowS<<", "<<iHighS<<", "<<iLowV<<", "<<iHighV<<", "<<changeThreshold<<endl;
            cout<<"*                                                                                        *"<<endl;
            cout<<"*                                                                                        *"<<endl;
            cout<<"******************************************************************************************"<<endl;

        }
    }
    else
    {
        VideoCapture cap(0); //capture the video from web cam
        startProcessing("test.mpeg",cap);
    }
    return 0;
}

