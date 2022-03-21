//
// Created by sun on 18-3-5.
//

#ifndef BUFF_TEST_ANGLECAL_H
#define BUFF_TEST_ANGLECAL_H
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "Quad.h"
#include "QuadExtractor.h"
#include "BuffParam.h"

using namespace std;
using namespace cv;

namespace hitcrt{
    class AngleCal {
    public:
        AngleCal();
        static std::vector<Point2f> codeNumImgPoints;

        //常量
        float P0;   //初始pitch
        float Y0;   //初始yaw
        float Kp;   //pitch轴 码盘变化值/角度变化值
        float Ky;   //yaw轴 码盘变化值/角度变化值

        //calibrate param
        double heightC2G;//y
        double widthC2G;//x
        double lengthC2G;//z

        double rVectC2G0;
        double rVectC2G1;
        double rVectC2G2;

        //三个转换矩阵
        ///世界坐标系的点到相机坐标系的点的转换矩阵r(C)=MatrixW2C*r(W)
        cv::Mat MatrixW2C;
        ///相机坐标系的点到云台坐标系的点的转换矩阵r(G)=MatrixC2G*r(C)
        cv::Mat MatrixC2G;
        ///世界坐标系的点到云台坐标系的点的转换矩阵r(G)=MatrixW2G*r(W)
        ///r(G)=MatrixC2G*MatrixW2C*r(W)
        ///MatrixW2G=MatrixC2G*MatrixW2C
        cv::Mat MatrixW2G;

        //需要打击的九个点的yaw和pitch
        cv::Mat nineYawPitch;

        std::vector<cv::Point3f> objectPoints; //通过已知的值得到的
        std::vector<cv::Point3f> tenQuadsPoints; //通过已知的值得到的
        std::vector<cv::Point2f> imagePoints;
        std::vector<cv::Point2f> tenQuadsImgPoints; //通过已知的值得到的
        std::vector<Point3f> codeNumObjectPoints;

        bool nineYawPitchCal(void);
        float distance(cv::Point2f& pt1, cv::Point2f& pt2);
        bool getImgPoint(cv::Point3f& targetPoint, cv::Mat MatrixW2C, cv::Point2f& imagePoint);
        bool calAngleYandP(cv::Point3f targetPoint, std::vector<float> &YandP);
        bool MatrixW2CCal(std::vector<Quad> &nineQuads, cv::Mat camera_matrix_, cv::Mat image = cv::Mat());
        bool MatrixW2CCal2(std::vector<Quad> &tenQuads, std::vector<Quad> &nineQuads,cv::Mat camera_matrix_, int index[5], cv::Mat image = cv::Mat());

        bool MatrixW2GCal(void);


        void verifyResult(void);
        void calibratePandY();

    };

//    bool getEulerAngles(cv::Mat rMat, cv::Mat &eulerAngles);
//    bool transformAxis(cv::Mat projMatrix, std::vector<cv::Point3f> inPoints, std::vector<cv::Point3f> &outPoints);
}







#endif //BUFF_TEST_ANGLECAL_H
