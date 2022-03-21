#ifndef ARMOR_H
#define ARMOR_H


#include <opencv2/opencv.hpp>
#include<iostream>

namespace hitcrt {
struct LightBar{
    LightBar(){}
    ~LightBar(){}
    std::vector<cv::Point> m_contour;
    float m_contourArea;
    std::vector<cv::Point2f> m_insidePoints;
    cv::RotatedRect m_rotateRect;
    cv::Vec6f line_6f;
    cv::Point2f m_upper;
    cv::Point2f m_bottom;
};


class Armor {
public:
    Armor(){}
    ~Armor(){}
    typedef enum type{
        INFANTRY = 0,   //大装甲
        HERO = 1            //小装甲
    }TYPE;
    static float hereo_width;           //230mm
    static float hereo_height;         //55mm
    static float infantry_width;      //130mm
    static float infantry_height;     //55mm
    TYPE m_type;
    float m_distance;   //装甲到相机的直线距离
    float Xc;            //装甲到相机的水平距离
    float Yc;            //装甲到相机的垂直距离
    float Zc;
    float m_X;
    float m_Y;
    float m_theta;        //atan(Y/X)
    float m_eject_theta;   //发射时的pitch角度
    cv::Point2f m_center_uv;    //装甲中心点在图像中的坐标
    LightBar m_leftBar;
    LightBar m_rightBar;

    cv::Point2f m_upper_left;
    cv::Point2f m_upper_right;
    cv::Point2f m_bottom_left;
    cv::Point2f m_bottom_right;

    ///新增数据成员
    cv::RotatedRect m_armor_rect;
    float m_armor_width;
    float m_armor_height;
    float m_wh_ratio;
    float m_bar_length_ratio;
    float m_type_judge;
    float m_type_score;

    cv::Mat m_R_big;
    cv::Mat m_T_big;
    cv::Mat m_R_small;
    cv::Mat m_T_small;

    void calculateRT(cv::Mat cam_matrix, cv::Mat distort_coeff);

};
}///hitcrt namespace

#endif // ARMOR_H
