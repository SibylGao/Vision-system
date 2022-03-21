//
// Created by sun on 18-1-12.
//
#ifndef BUFF_TEST_BUFFHITER_H
#define BUFF_TEST_BUFFHITER_H

#include <opencv2/opencv.hpp>
#include <string>
#include <stdio.h>

#include "label_image.h"
#include "QuadExtractor.h"
#include "BuffParam.h"
#include "NumDecoder.h"
#include "AngleCal.h"

#define COMP_NEXT 0
#define  COMP_WAIT_FRAME 1
#define  COMP_WAIT_PICTURE 2
#define COMP_RESET 3

namespace hitcrt{
    /* Pair (label, confidence) representing a prediction. */
    typedef std::pair<int, float> BuffNum;
    class BuffHiter{
    public:

        BuffHiter();

        cv::Mat map1, map2;
        cv::Mat camera_matrix_;

        int cntFrameBuffRecord;
        string frameSavePath;

        int prcIndi;
        int hitNumID;
        int stateCodeNum;
        int laststateCodeNum;
        int changestateCodeNum;
        int stateflag;//初值为零，只有RE/NEXT逻辑可改变此值，即重新击打此值必为1
        //是否节省时间判断标志，初始化为1，与上一次compareresult的值相关，=0时表示上次为COMP_NEXT，可节省时间
        int saveTime;

        std::vector<int> buffNumLast;
        std::vector<int> codeNumLast;

        QuadExtractor extractor;
        std::vector<Quad> quadsNine;
        std::vector<Quad> quadsTen;
        vector<int> buffNum;

        ofstream out;

        classifier tf_classify;

        AngleCal absoluteAngleCal;
        float yaw;
        float pitch;

        NumDecoder numDecoder;
        vector<int> codeNum;
        double picture_time1 = 0;
        double picture_time2 = 0;
        double last_picture_time = 0;
        bool picture_time_flag = false;
        int no_color_picture=0;
        bool color=false;
        bool picture_color_flag = false;
        int reset_picture=0;
        bool picture_reset_flag = false;



        int cntFrameBuffDefQuick; //防止一直追击图像的情况出现
        int cntFrameBuffDefQuickWhole; //若该标志出现到第三次，则该帧不攻击


//        bool init(cv::Mat img);
        bool run(cv::Mat& frame, int hitBuffFlag);
        bool hitBigBuff(cv::Mat& frame);
        bool hitSmallBuff(cv::Mat& frame);
        bool getLabels(std::vector<Quad>& nineQuads, std::vector<int>& labels, cv::Mat& label_image,int buffFlag);
        bool getFeedback(std::vector<Quad>& quadsTen, std::vector<double>& feedback, cv::Mat& feedbackImg);
        int compFrame(std::vector<int> buffNum, std::vector<int> codeNum);
        bool getHitNum(vector<int> buffNum, vector<int> codeNum, int prcIndi);
        bool showHitNum(cv::Mat& image);
        bool recordBuffImg(cv::Mat BuffImg);
    };
}



#endif //BUFF_TEST_BUFFHITER_H
