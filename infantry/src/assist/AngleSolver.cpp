#include "AngleSolver.h"
using namespace std;
namespace hitcrt{

AngleSolver::AngleSolver()
{
    cv::FileStorage fs;
        fs.open("/home/sun/HITCRT/Infantry/data/armor/camin1_6mm_640.yml",  cv::FileStorage::READ);
    assert(fs.isOpened());

    fs["offset_x"] >> OFFSET_X;
    fs["offset_y"] >> OFFSET_Y;
    fs["offset_z"] >> OFFSET_Z;
    fs["RVect_c2h0"] >> RVect_c2h0;
    fs["RVect_c2h1"] >> RVect_c2h1;
    fs["RVect_c2h2"] >> RVect_c2h2;
    fs["yaw"] >> YAW;
    fs["pitch"] >> PITCH;


//    fs["velocity"] >> VELOCITY;
    fs["gravity"] >> G;
     T_c2h = cv::Mat::zeros(cv::Size(4,4),CV_64FC1);
    cv::Mat RVect = cv::Mat::zeros(cv::Size(3,1),CV_64FC1);
    cv::Mat RMat, RMatInv;
    //////input rVec from calibration
    RVect.at<double>(0,0) = RVect_c2h0;
    RVect.at<double>(0,1) = RVect_c2h1;
    RVect.at<double>(0,2) = RVect_c2h2;
    Rodrigues(RVect, RMat);
    RMatInv = RMat.inv();

    for(int i = 0; i < 3; i ++)
    {
        for(int j = 0; j < 3; j++)
        {
            T_c2h.at<double>(i,j) = RMatInv.at<double>(i,j);
        }
    }
    T_c2h.at<double>(0,3) = OFFSET_X;//x
    T_c2h.at<double>(1,3) = OFFSET_Y;//y
    T_c2h.at<double>(2,3) = OFFSET_Z;//z
    T_c2h.at<double>(3,3) = 1;
}

AngleSolver::~AngleSolver()
{

}


void AngleSolver::calculateSendDegree(float _pitch, Armor _armor)
{
    current_Pitch_rad = _pitch*CV_PI/180;
    Armor armor_tmp = _armor;
    P_c = cv::Mat(4,1,CV_64FC1);
    P_c.at<double>(0, 0) = _armor.Xc;
    P_c.at<double>(1, 0) = _armor.Yc;
    P_c.at<double>(2, 0) = _armor.Zc;
    P_c.at<double>(3, 0) = 1;
    P_h = cv::Mat(4,1,CV_64FC1);
    P_h = T_c2h * P_c;
    Xh = P_h.at<double>(0, 0);
    Yh = P_h.at<double>(1, 0);
    Zh = P_h.at<double>(2, 0);

//    Xh = _armor.Xc + OFFSET_X;
//    Yh = _armor.Yc + OFFSET_Y;
//    Zh = _armor.Zc + OFFSET_Z;
//    std::cout<<"xh="<<Xh<<std::endl;
    //计算Yaw轴转角，单位是角度。

    Yaw_Degree = atan((Xh)/(Zh))* 180/ CV_PI+YAW;    ///手动补偿？

    //Yaw_Degree = (armor_tmp.m_center_uv.x - 640) / 4290 * 180/ CV_PI;

    ///计算Pitch轴转角，单位是弧度
    float Alpha = atan(Yh/Zh);
    float Beta = current_Pitch_rad - Alpha;
    float X_distance = sqrt(Yh*Yh + Zh*Zh) * cos(Beta);
    float Y_distance = sqrt(Yh*Yh + Zh*Zh) * sin(Beta);
    float A = -1.0f/2*G*X_distance*X_distance / (VELOCITY*VELOCITY);
    float B = X_distance;
    float C = -1.0f/2*G*X_distance*X_distance / (VELOCITY*VELOCITY) - Y_distance;
    float tanTheta = (-B+sqrt(B*B-4*A*C)) / 2 / A;
    Eject_Degree = atan(tanTheta)* 180/ CV_PI+PITCH;
 }


} ///hitcrt namespace
