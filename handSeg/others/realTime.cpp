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

//******************* Histogram Manipulation ***/Users/BboyKellen/Documents/Xcode/headSeg/handSeg/handSeg.xcodeproj*******************************************
MatND Hist_and_Backproj(Mat hsv, Mat mask, Mat change)
{
    int h_bins = 32; int s_bins = 32; int v_bin =32;
    int histSize[] = { h_bins, s_bins, v_bin};
    
    float h_range[] = { 0, 179 };
    float s_range[] = { 0, 255 };
    float v_range[] = { 0, 255 };
    const float* ranges[] = { h_range, s_range, v_range };
    int channels[] = { 0, 1, 2};

    MatND backproj;
    MatND backproj_noUpdate;
    cout << init << endl;

    /// Get the Histogram and normalize it if no update to the histogram
    calcHist(&hsv, 1, channels, mask, hist_noUpdate, 3, histSize, ranges, true, false );
    calcBackProject(&hsv, 1, channels, hist_noUpdate, backproj_noUpdate, ranges, 1, true );
    for(int i=0; i<backproj_noUpdate.rows; i++){
        for(int j=0; j<backproj_noUpdate.cols; j++){
            backproj_noUpdate.at<uchar>(i, j) = (backproj_noUpdate.at<uchar>(i, j)> 30? 255 : 0); //thresholding backprojection
        }
    }

    /// If there is no update to the histogram
    if(init==true)
    {
        /// Get the Histogram and normalize it
        calcHist(&hsv, 1, channels, mask, hist, 3, histSize, ranges, true, false );
        calcBackProject(&hsv, 1, channels, hist, backproj, ranges, 1, true );
        int weightCount=0;
        for(int i=0; i<mask.rows; i++){
          for(int j=0; j<mask.cols; j++){
              if(mask.at<uchar>(i, j)>150)
                  weightCount++;
          }
        }

        if(weightCount>(int)(mask.total()/100))
            init=false;
    }
    else
    {
        /// Get Backprojection using current histogram
        calcBackProject(&hsv, 1, channels, hist, backproj, ranges, 1, true );
        for(int i=0; i<backproj.rows; i++){
            for(int j=0; j<backproj.cols; j++){
                backproj.at<uchar>(i, j) = (backproj.at<uchar>(i, j)> 30? 255 : 0); //thresholding backprojection
            }
        }
        //processing backprojection to make it current handsegementation

        
        //update Histogram
        Mat updateMASK;
        bitwise_and(change, backproj, updateMASK);
        calcHist(&hsv, 1, channels, updateMASK, hist, 3, histSize, ranges, true, false );
        
        int weightCount=0;
        for(int i=0; i<change.rows; i++){
            for(int j=0; j<change.cols; j++){
                if(change.at<uchar>(i, j)>150)
                    weightCount++;
            }
        }
        if(weightCount<(int)(change.total()/15))
            init=true;
        
    }

    /// Draw the backproj
    imshow( "BackProjwithUpdate", backproj );
    imshow( "BackProj_NoUpdate", backproj_noUpdate );
    return backproj_noUpdate;
}


//Recursive search for hand Square Region (Region of interest)
void searchHand_loop(int x, int y, int m)
{
    //x, y is the center position(moment), m determines the search square size
    int tempMaxCol = maxCol;
    int tempMaxRow = maxRow;;
    int tempMinCol = minCol;
    int tempMinRow = minRow;;
    // search the current region (x-m,y-m) (x+m,y+m)
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
    
    //***** frame loop ************
    
    while (true)
    {
        Mat imgOriginal;
        Mat imgHSV;
        Mat imgThresholded;
        Mat luma(imgTmp.rows, imgTmp.cols, CV_8UC1, Scalar(0));
        Mat change=luma.clone();
        
        cap.set(CV_CAP_PROP_FPS, 0); //start the video at 0ms
        
        bool bSuccess = cap.read(imgOriginal); // read a new frame from video
        if (!bSuccess) {  //if not success, break loop
            cout << "Cannot read a frame from video stream" << endl;
            break;
        }
        cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
        inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image
        Mat andOperation = imgThresholded.clone();
        
                Mat temp_noise = imgThresholded.clone();
        
        
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
        
        bitwise_and(imgThresholded, change, andOperation);
        
        //Histogram update method
        //Mat hist_result = Hist_and_Backproj(imgHSV, andOperation, change);
        
        //Calculate the moments of the thresholded image
        Moments oMoments = moments(andOperation);
        double dM01 = oMoments.m01;
        double dM10 = oMoments.m10;
        double dArea = oMoments.m00;
        
        imgLines =Mat::zeros( imgTmp.size(), CV_8UC3 );
        Mat filter = Mat::zeros( imgTmp.size(), CV_8UC1 );
        
        // if the area <= 10000, I consider that the there are no object in the image and it's because of the noise, the area is not zero
            imshow("Original", imgOriginal); //show the original image
        if (dArea > 500000)
        {
            //calculate the position of the ball
            int posX = dM10 / dArea;
            int posY = dM01 / dArea;
            //get hand square region
            frame = andOperation; //each frame
            maxRow=posY; maxCol=posX; minRow=posY; minCol=posX;
            searchHand_loop(posX,posY,150);
            
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
            
            for(int i=minRow; i<maxRow; i++){
                for(int j=minCol; j<maxCol; j++)
                {
                    filter.at<uchar>(i,j)=255; //filter: hand square region of interestet
                }
            }
        }
        
        Mat finalShow; // create a new img for storing the final hand segementation
        bitwise_and(filter, imgThresholded, finalShow);//apply the square hand region to HSV threshold img. Use it as the final hand segmentation and store it in finalshow
        
        cvtColor(finalShow, finalShow, COLOR_GRAY2BGR); //Convert the captured frame from gray to RGB for output video
        oVideoWriter.write(finalShow); //writer the frame into the file]

        imshow("segmentation", finalShow);
        imshow("noise", temp_noise);

        finalShow = imgOriginal+finalShow;
        //cvtColor(imgThresholded, imgThresholded, COLOR_GRAY2BGR); //Convert the captured frame from gray to RGB for output video
        //oVideoWriter.write(imgThresholded);
        imshow("Thresholded Image", imgThresholded); //show the thresholded image
        imshow("Original+frame", imgOriginal); //show the original image
        imshow("luma", luma); //show the luma image
        imshow("change", change); //show the luma image
        imshow("andOperation", andOperation); //show the luma image
            cvtColor(andOperation, andOperation, COLOR_GRAY2BGR);
            Mat temp_andfilter =andOperation+imgLines;
            imshow("and with frame", temp_andfilter);
        imshow("final", finalShow); //show the luma image
        
        if (waitKey(1) == 1) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
        {
            cout << "esc key is pressed by user" << endl;
            break;
        }
    }
    return 0;
}

//****************************************  Load Parameters   **************************
int setPara()
{
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
    cout << "rea-time? 1 if yes 0 if false ";
    cin >> realTimeMode;
    
    String videos_train[] = {"noise1.mpeg","noise2.mpeg","noise3.mpeg","noise4.mpeg"};
    String videos_test[] = {"noiseTest1.mpeg","noiseTest2.mpeg","noiseTest3.mpeg","noiseTest4.mpeg"};
    
    if(realTimeMode==false)
    {
        cout << "train mode or test mode? 1 if train mode, 0 if test mode ";
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
            cout<<"Finished test #: "<<s+1<<" ------ Parameter: "<<iLowH<<", "<<iHighH<<", "<<iLowS<<", "<<iHighS<<", "<<iLowV<<", "<<iHighV<<", "<<changeThreshold<<endl;
        }
    }
    else
    {
        VideoCapture cap(0); //capture the video from web cam
        startProcessing("test.mpeg",cap);
    }
    return 0;
}


