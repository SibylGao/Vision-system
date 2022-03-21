//
// Created by sun on 18-1-14.
//
#ifndef BUFF_TEST_NUMDECODER_H
#define BUFF_TEST_NUMDECODER_H
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include "Quad.h"
#include "AngleCal.h"
namespace hitcrt{
    class NumDecoder{
    public:

        NumDecoder(void);
//        std::vector<int> codeNum;
        float singleNumHeight;
        float singleNumWidth;
        float singleNumArea;
        cv::Rect codeRect;
        cv::Mat getNumImg0;
        static bool initFlag;
        ofstream out;
        bool init(cv::Mat& frame, vector<Quad>& quadsNine);
        bool getCodeRect(cv::Mat& frame, vector<Quad>& quadsNine);
        bool getCodeRect2(cv::Mat& frame, vector<Quad>& quadsNine);
        bool getCodeRect3(cv::Mat& frame, vector<Quad>& quadsNine);
        bool run(cv::Mat& frame, vector<Quad>& quadsNine, vector<int>& codeNum);
        bool getNumImg(cv::Mat& binaryImg, std::vector<cv::Mat> &singleNumImgs);

        bool filterRects(std::vector<cv::Rect> RectsIn, cv::Size imgSize, float centerX,cv::Rect &RectOut);

        bool filterRects(std::vector<Rect>& Rects, cv::Size imgSize, Rect& idealRect);
        bool isEqual(float x, float y, float err);

        bool codeImg2Num(std::vector<cv::Mat> singleNumImgs, vector<int>& codeNum);
        void Thin(cv::Mat& src, cv::Mat& dst, int intera);
        bool getSmallRects(cv::Mat& frame, std::vector<cv::Mat> &singleNumImgs);


    private:
        std::vector<cv::Point2f> codeRegionSave;//quadsNine four point
        cv::Mat warpMatSave;//perspective Mat
        cv::Mat codeImgBig;

        int color_edgeA;
        int color_edgeB;
        int color_edge_dilate;
        int rec_code_alpha;
        int rec_code_beta;
        int element_size;

        float ratio_rect_to_img;
        float min_width_to_height;
        float max_width_to_height;
        float min_small_width_to_height;
        float max_small_width_to_height;
        float small_area_ratio;

    };


}
#endif //BUFF_TEST_NUMDECODER_H
