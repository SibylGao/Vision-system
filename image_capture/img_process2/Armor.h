//
// Created by sibylgao on 18-11-16.
//

#ifndef IMG_PROCESS2_ARMOR_H
#define IMG_PROCESS2_ARMOR_H

#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/core/core.hpp>
#include <iostream>
#include "LightBar.h"

class Armor{
public:
    cv::Point2f m_upper_left;
    cv::Point2f m_upper_right;
    cv::Point2f m_bottom_left;
    cv::Point2f m_bottom_right;
    LightBar m_leftBar;
    LightBar m_rightBar;
    cv::Mat ROI_img;
    cv::Mat frame;
    int flag;
    cv::Mat mask;
    cv::Mat gray_img;
    cv::Point2f m_center_uv;
    cv::Mat img_Enermy;
    cv::Mat img_Self;

//    cv::vector<cv::Mat> channels;


    float MAX_LIGHT_BAR_AREA;
    float MIN_LIGHT_BAR_AREA;
    float BREADTH_LENGTH_SCALE_MAX;
    float BREADTH_LENGTH_SCALE_MIN;
    float BAR_SLOPE_MIN;
    int GRAY_THRESHOLD;
    float infantry_width;
    float infantry_height;
    float COLOR_DIFFERENCE;
    int BAR_POINTS_MIN;

    void img_process(cv::Mat &_frame,Armor &armor_tmp);
    void remap(cv::Mat &_frame,Armor &armor_tmp,int i);
    void gray(cv::Mat &_frame);
    void LightBarDetect(cv::Mat &_frame,Armor &armor_tmp);
private:
    static bool parallel(LightBar bar1, LightBar bar2);
};

#endif //IMG_PROCESS2_ARMOR_H
