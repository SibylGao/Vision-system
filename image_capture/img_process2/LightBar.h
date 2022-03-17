//
// Created by sibylgao on 18-11-16.
//

#ifndef IMG_PROCESS2_LIGHTBAR_H
#define IMG_PROCESS2_LIGHTBAR_H


#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/core/core.hpp>
#include <iostream>

struct LightBar{
    std::vector<cv::Point> m_contour;
    float m_contourArea;
    std::vector<cv::Point2f> m_insidePoints;
    cv::RotatedRect m_rotateRect;
    float m_cos;
    float m_sin;
    float m_length;
    float m_color_ratio;

    cv::Point2f m_center;

    cv::Vec6f line_6f;
    cv::Point2f m_upper;
    cv::Point2f m_bottom;
//    bool parallel(LightBar bar1, LightBar bar2);
};

#endif //IMG_PROCESS2_LIGHTBAR_H
