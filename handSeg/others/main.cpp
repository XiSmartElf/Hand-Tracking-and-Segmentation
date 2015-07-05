#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/core/core.hpp"
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

int main( int argc, const char** argv )
{
    Mat img = imread("/Users/BboyKellen/Documents/Xcode/headSeg/handSeg/hand.jpg", CV_LOAD_IMAGE_COLOR); //read the
    
    if (img.empty())
    {
        cout << "Error : Image cannot be loaded..!!" << endl;
        return -1;
    }
    Mat imgHSV;
    Size size(1000,500);
    resize(img,img,size, 0, 0, INTER_CUBIC);
    //cvNamedWindow("original", CV_WINDOW_NORMAL);
    //imshow("original", img);
    cvtColor(img, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
    //imshow("hsv", img);
    Mat imgThresholded;
    int iLowH = 0;
    int iHighH = 255;
    int iLowS = 0;
    int iHighS = 255;
    int iLowV = 0;
    int iHighV = 255;
    inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image
    imshow("thresholdHSV", imgThresholded);
    
    waitKey(0); //wait infinite time for a keypress
    destroyWindow("original");
    destroyWindow("hsv");
    
    Mat imgDumy(500, 1000, CV_8UC3, Scalar(0,0, 255));
    namedWindow("MyWindow", CV_WINDOW_AUTOSIZE); 
    imshow("MyWindow", imgDumy);
    waitKey(0);
    destroyWindow("MyWindow");
    
    
//**************************VIDEO Capture**************************************************************//
    VideoCapture cap("/Users/BboyKellen/Documents/Xcode/headSeg/handSeg/test.mp4"); // open the video file for reading
    
    if ( !cap.isOpened() )  // if not success, exit program
    {
        cout << "Cannot open the video file" << endl;
        return -1;
    }

    
    double fps = cap.get(CV_CAP_PROP_FPS); //get the frames per seconds of the video
    cout << "Frame per seconds : " << fps << endl;
    namedWindow("MyVideo",CV_WINDOW_AUTOSIZE); //create a window called "MyVideo"
    cap.set(CV_CAP_PROP_FPS, 800); //start the video at 300ms
    
    while(1)
    {
        Mat frame;
        
        bool bSuccess = cap.read(frame); // read a new frame from video
        
        if (!bSuccess) //if not success, break loop
        {
            cout << "Cannot read the frame from video file" << endl;
            destroyWindow("MyWindow");
            break;
        }
        
        imshow("MyVideo", frame); //show the frame in "MyVideo" window
        double position = cap.get(CV_CAP_PROP_POS_MSEC);
        cout << "currently at: " << position << "ms of the video" <<endl;
        if(waitKey(30) == 27) //wait for 'esc' key press for 30 ms. If 'esc' key is pressed, break loop
        {
            cout << "esc key is pressed by user" << endl;
            destroyWindow("MyWindow");
            break;
        }
    }
    
    
    
    //real time video
    VideoCapture cap_realTime(0); // open the video camera no. 0
    
    if (!cap_realTime.isOpened())  // if not success, exit program
    {
        cout << "Cannot open the video cam" << endl;
        return -1;
    }
    
    double dWidth = cap_realTime.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
    double dHeight = cap_realTime.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video
    
    cout << "Frame size : " << dWidth << " x " << dHeight << endl;
    
    namedWindow("MyVideo",CV_WINDOW_AUTOSIZE); //create a window called "MyVideo"
    
    while (1)
    {
        Mat frame;
        
        bool bSuccess = cap_realTime.read(frame); // read a new frame from video
        
        if (!bSuccess) //if not success, break loop
        {
            cout << "Cannot read a frame from video stream" << endl;
            break;
        }
        
        imshow("MyVideo", frame); //show the frame in "MyVideo" window
        
        if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
        {
            cout << "esc key is pressed by user" << endl;
            break; 
        }
    }
    return 0;
}