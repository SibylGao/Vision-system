#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/core/core.hpp>
#include <iostream>
using namespace cv;
using namespace std;

int main()
{
    VideoCapture capture(1);//如果是笔记本，0打开的是自带的摄像头，1 打开外接的相机
//    capture.set(CV_CAP_PROP_FRAME_WIDTH,1920);
//    capture.set(CV_CAP_PROP_FRAME_HEIGHT,1080);
    capture.set(CV_CAP_PROP_FRAME_WIDTH,640);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT,480);
    capture.set(CV_CAP_PROP_FPS,30);
//    double rate = 30.0;//视频的帧率
//    Size videoSize(1920,1080);
//    VideoWriter writer("VideoTest.avi", CV_FOURCC('M', 'J', 'P', 'G'), rate, videoSize);
    Mat frame;
    string frameSavePath = "/home/sibylgao/CLionProjects/img/1/";

    int i = 0;
    while (capture.isOpened()) {
        capture >> frame;
        if (frame.empty()){
            cout << "frame is empty" << endl;
        }


        double t1 = getTickCount();
        cv::Mat imgShow = frame.clone();
        double t2 = (getTickCount()-t1)*1000/getTickFrequency();
//        cout << t2 << endl;

        resize(imgShow,imgShow,Size(0, 0), 0.3, 0.3);
        namedWindow("video",WINDOW_NORMAL);
        imshow("video", imgShow);
        char key_board = waitKey(10);
        if (key_board == 's') {
            ostringstream imgName;
            imgName << i << ".jpg";
            cout << i << ".jpg " << "saved" << endl;
            imwrite(frameSavePath + imgName.str(), frame);

            i++;

            if (waitKey(20) == 27)//27是键盘摁下esc时，计算机接收到的ascii码值
            {
                break;
            }
        }
    }
}