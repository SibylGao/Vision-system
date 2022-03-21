//
// Created by sun on 18-1-12.
//

#include "BuffHiter.h"
#include "myVideoCap.h"

using namespace std;
using namespace cv;
using namespace hitcrt;


int main()
{
    BuffHiter buffHiter;
    VideoCapture cap;
    Mat frame,image;

//    string videoPath = "/home/sun/HITCRT/Buff/video_3.24/3m_3.avi";
//    string videoPath = "/home/sun/HITCRT/Infantry/video/big_buff_1m——1.avi";
//    string videoPath = "/home/sun/HITCRT/Infantry/video/buff.avi";
    string videoPath = "/home/sun/HITCRT/Infantry/video_0524/big_buff_2.avi";

    //string videoPath = "/home/sun/HITCRT/AprilTag/buff_1.avi";
//    string videoPath = "/home/sun/HITCRT/Buff/video/my_video-4.mkv";

    if(!cap.open(videoPath)){
        cout<<"cannot open video "<<endl;
    }


//    open camera
//    hitcrt::myVideoCap cap1("/dev/video1", 1920, 1080, 30, 1, 42, 1);

//    VideoCapture capture(1);//如果是笔记本，0打开的是自带的摄像头，1 打开外接的相机
//    capture.set(CV_CAP_PROP_FRAME_WIDTH,1920);
//    capture.set(CV_CAP_PROP_FRAME_HEIGHT,1080);
//    capture.set(CV_CAP_PROP_FPS,30);
//    double rate = 30.0;//视频的帧率
//    Size videoSize(1920,1080);

    //open image
//    frame = imread("/home/sun/Buff/2.jpg");
//    if(frame.empty())
//        cout <<"no frame "<<endl;
//    imshow("frame",frame);
//    buffHiter.run(frame);

    Mat R,new_Matrix;
    cv::initUndistortRectifyMap(hitcrt::BuffParam::KK,hitcrt::BuffParam::DISTORT,R,new_Matrix,Size(1920,1080),CV_32FC1,
                                hitcrt::BuffParam::MAPX,hitcrt::BuffParam::MAPY);

    while(1) {
        cap >> frame;
        if(frame.empty()){
            cout <<"no frame "<<endl;
            return  false;
        }

//        cap1.VideoGrab(frame);
        cv::Mat img = frame.clone();
        //resize(img, img, Size(0,0), 0.3, 0.3);
        //imshow("img",img);
        cout << "width" << frame.cols << " height" <<frame.rows << endl;

        remap(frame,frame,hitcrt::BuffParam::MAPX,hitcrt::BuffParam::MAPY,INTER_LINEAR);
//        imshow("frame_remap",frame);

        cout << "-------------------START------------------"<< endl;
        double t1 = getTickCount();
        if(!buffHiter.run(frame, 2)){
            cout<<"hit buff failed!!!"<<endl;
            waitKey(10);

            continue;
        }

        double t2 = (getTickCount()-t1)*1000/getTickFrequency();
        cout << "Master: total time is: " << t2 << "ms" << endl;
        cout << "-------------------END------------------"<< endl << endl;

//        waitKey(0);
        if (waitKey(30)==0)
            break;
    }
    return true;
}
