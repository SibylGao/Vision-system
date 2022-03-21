//
// Created by sun on 18-1-12.
//
#include "Quad.h"

using namespace cv;
using namespace std;

namespace hitcrt{
    float Quad::real_width = 280;
    float Quad::real_height = 160;

/*void Quad::calculateRT() {

    vector<cv::Point3f> objectPoints;
    vector<Point2f> imagePoints;

    objectPoints.push_back(cv::Point3f(0,0,0));
    objectPoints.push_back(cv::Point3f(real_width,0,0));
    objectPoints.push_back(cv::Point3f(0,real_height,0));
    objectPoints.push_back(cv::Point3f(real_width,real_height,0));

//    cout << "111" << endl;

//    cout << up_left/0.3 << endl;
//    cout << up_right/0.3 << endl;
//    cout << down_right/0.3 << endl;
//    cout << down_left/0.3 << endl;

    imagePoints.push_back(up_left/(float)0.3);
    imagePoints.push_back(up_right/(float)0.3);
    imagePoints.push_back(down_right/(float)0.3);
    imagePoints.push_back(down_left/(float)0.3);

//    for(int i = 0; i < imagePoints.size(); i++){
//        cout << imagePoints[i] << endl;
//    }
//    cout << "222" << endl;



    cv::solvePnP(objectPoints, imagePoints, BuffParam::KK, BuffParam::DISTORT, m_R, m_T);
    m_distance = (float)cv::norm(m_T);
}*/









}


