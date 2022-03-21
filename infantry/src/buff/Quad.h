//
// Created by sun on 18-1-12.
//
#ifndef BUFF_TEST_QUAD_H
#define BUFF_TEST_QUAD_H

#include <iostream>
#include <opencv2/opencv.hpp>
#include "BuffParam.h"

using namespace cv;
using namespace std;

namespace hitcrt{
    class Quad{
    public:
        Quad(){}
        ~Quad(){}

        static float real_width;
        static float real_height;

        cv::Point2f up_right;
        cv::Point2f down_right;
        cv::Point2f up_left;
        cv::Point2f down_left;
        cv::Point2f m_center;
        float m_width;
        float m_height;
        vector<Point2f> counters;
        float area;

        int id;
        int number;
        float confidence;
        bool flag = false;

//        cv::Mat m_R;
//        cv::Mat m_T;
//        float m_distance;
//        float m_X;
//        float m_Y;
//        float m_theta;//atan(Y/X)
//        float m_eject_theta;


//        void calculateRT();
    };
}
#endif //BUFF_TEST_QUAD_H
