//
// Created by sun on 18-1-14.
//
//Buff param
#include "BuffParam.h"
namespace hitcrt{
    //Buff
    cv::Mat BuffParam::KK = cv::Mat(3,3,CV_64FC1);
    cv::Mat BuffParam::DISTORT = cv::Mat(4,1,CV_64FC1);
    cv::Mat BuffParam::MAPX;
    cv::Mat BuffParam::MAPY;
    float BuffParam::resizePara = 0.3;
    float BuffParam::EXPOSURE = 103;

    //Armor
    float ArmorParam::MAX_ROBOT_PITCH = 70.0f;
    float ArmorParam::MIN_ROBOT_PITCH = -60.0f;
    int ArmorParam::EXPOSURE = 0;
    int ArmorParam::WIDHT = 0;
    int ArmorParam::HEIGHT = 0;
    int ArmorParam::FPS = 0;

}

