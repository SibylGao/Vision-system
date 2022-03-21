//
// Created by sun on 18-1-12.
//
#ifndef BUFF_TEST_QUADEXTRACTOR_H
#define BUFF_TEST_QUADEXTRACTOR_H
#include <iostream>
#include "opencv2/opencv.hpp"
#include "apriltag.h"
#include "AprilTag.h"
#include "Quad.h"
#include "BuffParam.h"

using namespace std;
using namespace cv;

namespace hitcrt{
    class QuadExtractor{
    private:
        //preprocess param
        int dilate_ksize;
        int dilate1_n;
        int dilate2_n;
        int erode1_n ;

        //filter quad param
        float min_ratio;
        float max_ratio;
        float oppo_side_error;
        float is_equal_ratio;
        float min_area;
        float last_now_area_error;
        float min_angle;
        float k_min_area;

        static bool initQuad;
        static bool init();

    public:
        QuadExtractor();
        ~QuadExtractor(){};
        ofstream out;

        std::vector<Quad> lastQuadsNine;
        std::vector<Quad> lastQuadsTen;
        float lastQuadArea;
        float lastQuadstenArea;
        static std::vector<Quad> objectQuads;
        static std::vector<Quad> tenQuads;
        cv::Point2f lastQuadNineCenter;
        float averageWidth,averageHeight;

        static float quadNineX;
        static float quadNineY;
        static float quad2quadX;
        static float quad2quadY;

        static float quadTenX;
        static float quadTenY;
        static float quadTen2quadTenX;
        static float quadTen2quadTenY;

        static int functionFlag;
        static int lastFunctionFlag;


        bool runSmallBuff(cv::Mat& img, std::vector<Quad>& quads,std::vector<Quad>& quadsTen);
        bool runBigBuff(cv::Mat& img, std::vector<Quad>& quads, int index[5]);

        float distance(cv::Point2f& pt1, cv::Point2f& pt2);
        float angle(cv::Point pt1, cv::Point pt2, cv::Point crossPoint);
        void drawQuads(std::vector<Quad> quads, cv::Mat &image);
        bool completeQuads(std::vector<Quad>& quads);
        bool completeTenQuads(std::vector<Quad>& quads,int index[5]);
        void orderY(std::vector<Quad>& quads, int row1, int row2);
        void orderX(std::vector<Quad>& quads);
        bool insertQuad(vector<Quad>& quads,  int position1, int position2);
        bool insertTenQuad(vector<Quad>& quads,  int position1, int position2);
        cv::Point2f getImgPoint(cv::Point3f& targetPoint, cv::Mat MatrixW2C);
        bool getImgQuad(vector<Quad>& imageQuads, vector<Quad>& insertQuads, int index1[3], int index2[3], int index3[]);
        bool getImgQuad2(vector<Quad>& imageQuads, vector<Quad>& tenQuads);
        bool getTenQuad(vector<Quad>& imageQuads, vector<Quad>& insertQuads, int index1[3], int index2[3], int index3[]);
        static bool isEqualSmallBuff(Quad quad1, Quad quad2);
        static bool isEqualBigBuff(Quad quad1, Quad quad2);
    };
}







#endif //BUFF_TEST_QUADEXTRACTOR_H
