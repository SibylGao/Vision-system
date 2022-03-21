#include "Armor.h"

using namespace std;
using namespace cv;

namespace hitcrt
{
float Armor::hereo_width = 230;
float Armor::hereo_height = 55;
float Armor::infantry_width = 135;
float Armor::infantry_height = 55;

void Armor::calculateRT(cv::Mat cam_matrix, cv::Mat distort_coeff)
{

    vector<Point3f> objectPoints_infantry;
    vector<Point3f> objectPoints_hero;
    float infantry_distance,hereo_distance;

    objectPoints_infantry.push_back(Point3f(-0.5*Armor::infantry_width,0.5*Armor::infantry_height,0));
    objectPoints_infantry.push_back(Point3f(0.5*Armor::infantry_width,0.5*Armor::infantry_height,0));
    objectPoints_infantry.push_back(Point3f(-0.5*Armor::infantry_width,-0.5*Armor::infantry_height,0));
    objectPoints_infantry.push_back(Point3f(0.5*Armor::infantry_width,-0.5*Armor::infantry_height,0));

    objectPoints_hero.push_back(Point3f(-0.5*Armor::hereo_width,0.5*Armor::hereo_height,0));
    objectPoints_hero.push_back(Point3f(0.5*Armor::hereo_width,0.5*Armor::hereo_height,0));
    objectPoints_hero.push_back(Point3f(-0.5*Armor::hereo_width,-0.5*Armor::hereo_height,0));
    objectPoints_hero.push_back(Point3f(0.5*Armor::hereo_width,-0.5*Armor::hereo_height,0));


    vector<Point2f> imagePoints;
    imagePoints.push_back(m_upper_left);
    imagePoints.push_back(m_upper_right);
    imagePoints.push_back(m_bottom_left);
    imagePoints.push_back(m_bottom_right);



    cv::solvePnP(objectPoints_infantry, imagePoints, cam_matrix, distort_coeff, m_R_small, m_T_small);
    infantry_distance=cv::norm(m_T_small);
    cv::solvePnP(objectPoints_hero, imagePoints, cam_matrix, distort_coeff, m_R_big, m_T_big);
    hereo_distance=cv::norm(m_T_big);

    m_type_judge = (hereo_distance-infantry_distance)/infantry_distance;        ///和solvepnp有关的一个阈值参数
//        cout<<"wh_ratio ="<< m_wh_ratio<<endl;
    float score_pnp,score_wh;                                    ///pnp和装甲长宽比分别对应一个评分，总评分大于0时判定大装甲，否则小装甲
//        score_wh = (m_wh_ratio - 3.3 ) * 10;
//        score_pnp = (m_type_judge - 0.65) * 40;
    score_wh = (m_wh_ratio - 3.5 ) * 20;
    score_pnp = (m_type_judge - 0.4) * 10;
    m_type_score = score_wh + score_pnp;
    if(m_type_score > 0)
    {
        m_type = HERO;
        m_distance = hereo_distance;
        Xc = m_T_big.at<double>(0,0);
        Yc = m_T_big.at<double>(1,0);
        Zc = m_T_big.at<double>(2,0);
    }
    else
    {
        m_type = INFANTRY;
        m_distance = infantry_distance;
        Xc = m_T_small.at<double>(0,0);
        Yc = m_T_small.at<double>(1,0);
        Zc = m_T_small.at<double>(2,0);
    }


}


}///hitcrt namespace
