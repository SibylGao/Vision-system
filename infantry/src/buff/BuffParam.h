//
// Created by sun on 18-1-14.
//

#ifndef BUFF_TEST_BUFFPARAM_H
#define BUFF_TEST_BUFFPARAM_H

#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

namespace  hitcrt{
    class BuffParam{
    public:
        static cv::Mat KK;
        static cv::Mat DISTORT;
        static cv::Mat MAPX;
        static cv::Mat MAPY;
        static float resizePara;
        static float EXPOSURE;

    };

    class ArmorParam{
    public:
        static float MAX_ROBOT_PITCH;   //机器人最大仰角
        static float MIN_ROBOT_PITCH;   //机器人最小仰角
        static int EXPOSURE;
        static int WIDHT;
        static int HEIGHT;
        static int FPS;
    };
}




#endif //BUFF_TEST_BUFFPARAM_H
