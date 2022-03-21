#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>
#include "myVideoCap.h"
using namespace hitcrt;
using namespace cv;
using namespace std;

int main(int argc, char* argv)
{

//    myVideoCap cap0("/dev/video0",800,600,60);
//    myVideoCap cap0("/dev/video1",1280,720,60,0,22,1);
    myVideoCap cap0("/dev/video1",1920,1080,30,1,200,1);
    Mat frame;
    double t1=0,t2=0;
    double fps;
    int num = 0;
    char frameName[50];
    while(1)
    {



        t1 = (double)cv::getTickCount();
        cap0.VideoGrab(frame);
        t2 = ((double)cv::getTickCount() - t1) / cv::getTickFrequency();
        fps = 1.0 / t2;
        std::cout<<"fps:"<<fps<<std::endl;
        imshow("frame",frame);
        char ch = waitKey(1);
        if(ch == 'q')
           break;
   }
    return 0;
}
