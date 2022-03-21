//
// Created by xiaoyang on 18-3-29.
//

#ifndef LEARNINGRM_STF_H
#define LEARNINGRM_STF_H
#include "opencv2/opencv.hpp"
#include "stdlib.h"
//#include <opencv2/core/mat.hpp>
#include <iostream>

using namespace std;
using namespace cv;
namespace hitcrt
{
class STF
{

public:
    STF();
    virtual ~STF();
    const Mat& predict(const cv::Mat& measurement,const cv::Mat& control = cv::Mat());


    int stateNum = 3;
    int measurementNum =1;
    double row = 0.95;
    double beta = 2;
    int k=0;
    float lamda;
    Mat measurement;
    Mat transitionMatrix = Mat (stateNum,stateNum,CV_32F);
    Mat measurementMatrix = Mat (measurementNum,stateNum,CV_32F);
    Mat processNoiseCov = Mat (stateNum,stateNum,CV_32F);
    Mat measurementNoiseCov = Mat (measurementNum,measurementNum,CV_32F);
    Mat errorCovPre = Mat (stateNum,stateNum,CV_32F);
    Mat statePre = Mat (stateNum,1,CV_32F);
    Mat statePost = Mat (stateNum,1,CV_32F);
    Mat gain = Mat (stateNum,measurementNum,CV_32F);
    Mat errorCovPost = Mat (stateNum,stateNum,CV_32F);

//    const cv::Mat& correct(const cv::Mat& measurement);


private:
    cv::Scalar traceN;
    cv::Scalar traceM;
    float lamda_0;
//    cv::Mat gama = cv::Mat (measurementNum,measurementNum,CV_64F);
    cv::Mat S_0 = cv::Mat(measurementNum,measurementNum,CV_32F);
    cv::Mat S_0pre = cv::Mat(measurementNum,measurementNum,CV_32F);
//    cv::Mat Zpre;
    cv::Mat temp1 = cv::Mat(measurementNum,1,CV_32F);    //ksi
    cv::Mat temp2 = cv::Mat(measurementNum,measurementNum,CV_32F);    //N
    cv::Mat temp3 = cv::Mat(measurementNum,measurementNum,CV_32F);    //M
    cv::Mat temp4 = cv::Mat(measurementNum,measurementNum,CV_32F);
//    cv::Mat temp5;
//    cv::Mat temp6;
};


#endif //LEARNINGRM_STF_H



}
