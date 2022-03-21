//
// Created by xiaoyang on 18-3-29.
//

#include "STF.h"
#include "opencv/cv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
using namespace std;
using namespace cv;
namespace hitcrt
{
STF::STF()
{
    int measureNum = 1;
    int stateNum = 3;
    double delta1 = 10;
    double alpha = 100;
    measurement = Mat::zeros(measureNum,1,CV_32F);
    setIdentity(processNoiseCov,Scalar::all(0.0001));
    setIdentity(measurementNoiseCov,Scalar::all(0.01));
    setIdentity(measurementMatrix);
    randn(statePost,Scalar::all(320),Scalar::all(0));
    setIdentity(errorCovPost,Scalar::all(1));
    transitionMatrix = (Mat_<float> (3,3) << 1,delta1,(alpha*delta1-1+exp(-alpha*delta1))/(alpha*alpha),
                           0,1,(1-exp(-alpha*delta1))/alpha,0,0,exp(-alpha*delta1));

}
STF::~STF()
{

}

const cv::Mat& STF::predict(const cv::Mat& measurement,const cv::Mat& control)
{
    statePre = transitionMatrix*statePost;
    temp1 = measurement - measurementMatrix*statePre;
    if(k == 0)
    {
        S_0 = temp1*temp1.t();
        S_0pre = S_0;
        k++;
    }
    else
    {
        S_0 = (row*S_0pre+temp1*temp1.t())/(1+row);
        S_0pre = S_0;
        k++;
    }
    temp2 = S_0-measurementMatrix*processNoiseCov*measurementMatrix.t()-beta*measurementNoiseCov;
    temp3 = measurementMatrix*transitionMatrix*errorCovPost*transitionMatrix.t()*measurementMatrix.t();
    traceN = trace(temp2);
    traceM = trace(temp3);
    lamda_0 = (float)traceN(0)/(float)traceM(0);
    if(lamda_0 >= 1 )
        lamda = lamda_0;
    else
        lamda = 1;
    errorCovPre = lamda*transitionMatrix*errorCovPost*transitionMatrix.t()+processNoiseCov;
    temp4 = measurementMatrix*errorCovPre*measurementMatrix.t()+measurementNoiseCov;
    gain = errorCovPre*measurementMatrix.t()*temp4.inv();
    statePost = statePre+gain*temp1;
    statePre = transitionMatrix*statePost;
    //cout<<"                            statePre="<<statePre.at <float>(1)<<endl;
    return statePre;
}

}
