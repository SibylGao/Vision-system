//
// Created by sun on 18-1-14.
//
#include <iostream>
#include <fstream>

#include "NumDecoder.h"
#include "QuadExtractor.h"

#define DEBUG_CODENUM_TIME
//#define DEBUG_CODENUM_IMG
//#define DEBUG_CODENUM_SIGLENUM_IMG
//#define DEBUG_CODENUM_IMGBIG
//#define DEBUG_CODENUM_PARAM

#define FILL_RATIO 0.5

using namespace std;
using namespace cv;

//识别数码管首先要找到数码管区域的位置，找位置采用了color+edge的方法，首先找出边缘，然后按颜色过滤边缘，再膨胀腐蚀找形状相似区域
//数码管识别先是按连通域找出每一个字符所在的位置，然后按http://kns.cnki.net/KCMS/detail/detail.aspx?dbcode=CJFQ&dbname=CJFD2010&filename=KXJS201016056&v=MDk0ODNCZmJHNEg5SE5xWTlBWW9SOGVYMUx1eFlTN0RoMVQzcVRyV00xRnJDVVJMMmZidVp1RnluZ1ZMdk5Malg=中的穿线法来做
namespace hitcrt {
    bool NumDecoder::initFlag = false;
    NumDecoder::NumDecoder(void): singleNumHeight(0),
                                  singleNumWidth(0),
                                  singleNumArea(0) {
        cv::FileStorage fs;
        fs.open("/home/sun/HITCRT/Infantry/data/buff/code_num_param.yml", cv::FileStorage::READ);
        assert(fs.isOpened());


        fs["color_edgeA"] >> color_edgeA;
        fs["color_edgeB"] >> color_edgeB;
        fs["color_edge_dilate"] >> color_edge_dilate;
        fs["rec_code_alpha"] >> rec_code_alpha;
        fs["rec_code_beta"] >> rec_code_beta;
        fs["element_size"] >> element_size;

        fs["ratio_rect_to_img"] >> ratio_rect_to_img;
        fs["min_width_to_height"] >> min_width_to_height;
        fs["max_width_to_height"] >> max_width_to_height;
        fs["min_small_width_to_height"] >> min_small_width_to_height;
        fs["max_small_width_to_height"] >> max_small_width_to_height;
        fs["small_area_ratio"] >> small_area_ratio;

        time_t log1;
        int name_log = time(&log1);
        ostringstream os_log;
        os_log <<"/home/infantry/Infantry/data/buff/logs/" << name_log <<"_NumDecoder.txt";
        out.open(os_log.str(),ios_base::out);
#ifdef DEBUG_CODENUM_PARAM
        cout << "color_edgeA is " << color_edgeA << endl;
        cout << "color_edgeB is " << color_edgeB << endl;
        cout << "color_edge_dilate is " << color_edge_dilate << endl;
        cout << "rec_code_alpha is " << rec_code_alpha << endl;
        cout << "rec_code_beta is " << rec_code_beta << endl;
        cout << "element_size is " << element_size << endl;

        cout << "ratio_rect_to_img is " << ratio_rect_to_img << endl;
        cout << "min_width_to_height is " << min_width_to_height << endl;
        cout << "max_width_to_height is " << max_width_to_height << endl;
        cout << "min_small_width_to_height is " << min_small_width_to_height << endl;
        cout << "max_small_width_to_height is " << max_small_width_to_height << endl;
#endif
    }

    bool NumDecoder::getCodeRect(cv::Mat& frame, vector<Quad>& quadsNine) {

        //Step1.按九宫格区域取点,九宫格边界向上映射
        cv::Mat findCodeRegionImg = frame.clone();
        vector<Point2f> pointsTop(4);
        vector<Point2f> srcPoint(4);

        double t1 = getTickCount();
        pointsTop[0] = quadsNine[0].up_left;
        pointsTop[1] = quadsNine[6].up_right;
        pointsTop[2] = quadsNine[8].down_right;
        pointsTop[3] = quadsNine[2].down_left;

        srcPoint[3] = pointsTop[0];
        srcPoint[2] = pointsTop[1];

        //将2相对于1对称， 3相对于0对称
        int xTemp0, yTemp0, xTemp1, yTemp1;

        xTemp0 = pointsTop[3].x - pointsTop[0].x;
        yTemp0 = pointsTop[3].y - pointsTop[0].y;

        xTemp1 = pointsTop[2].x - pointsTop[1].x;
        yTemp1 = pointsTop[2].y - pointsTop[1].y;


        if ((pointsTop[1].y - yTemp1 < 0)||(pointsTop[0].y - yTemp0 < 0)) {
            srcPoint[0].x = pointsTop[0].x - 0.5*xTemp0;
            srcPoint[0].y = pointsTop[0].y - 0.5*yTemp0;

            srcPoint[1].x = pointsTop[1].x - 0.5*xTemp1;
            srcPoint[1].y = pointsTop[1].y - 0.5*yTemp1;

        } else {
            srcPoint[0].x = pointsTop[0].x - xTemp0;
            srcPoint[0].y = pointsTop[0].y - yTemp0;
            srcPoint[1].x = pointsTop[1].x - xTemp1;
            srcPoint[1].y = pointsTop[1].y - yTemp1;
        }

        if((pointsTop[1].x - xTemp1 < 0)||(pointsTop[0].x - xTemp0 < 0)) {
            cout << "please reSet pointTop" << endl;
            return false;
        }

#ifdef DEBUG_CODENUM_IMG
        cout << "pointsTop" << endl << pointsTop << endl;
        cout << "yTemp0  " << yTemp0 << "yTemp1  " << yTemp1 <<endl;
        cout << "srcPoint" << endl << srcPoint << endl;
        cv::Mat codeNumRegionImg = frame.clone();
        line(codeNumRegionImg, pointsTop[0], pointsTop[1], Scalar(255, 255, 0), LINE_8);
        line(codeNumRegionImg, pointsTop[1], pointsTop[2], Scalar(0, 255, 0), LINE_8);
        line(codeNumRegionImg, pointsTop[2], pointsTop[3], Scalar(255, 255, 0), LINE_8);
        line(codeNumRegionImg, pointsTop[3], pointsTop[0], Scalar(0, 255, 255), LINE_8);

        line(codeNumRegionImg, srcPoint[0], srcPoint[1], Scalar(0, 255, 0), LINE_8);
        line(codeNumRegionImg, srcPoint[1], srcPoint[2], Scalar(0, 255, 0), LINE_8);
        line(codeNumRegionImg, srcPoint[2], srcPoint[3], Scalar(0, 255, 0), LINE_8);
        line(codeNumRegionImg, srcPoint[3], srcPoint[0], Scalar(0, 255, 0), LINE_8);
        imshow("codeNumRegionImg", codeNumRegionImg);
#endif

        float widthX = (srcPoint[1].x - srcPoint[0].x + srcPoint[2].x - srcPoint[3].x) / 2;
        float widthY = (srcPoint[1].y - srcPoint[0].y + srcPoint[2].y - srcPoint[3].y) / 2;
        float heightX = (srcPoint[3].x - srcPoint[0].x + srcPoint[2].x - srcPoint[1].x) / 2;
        float heightY = (srcPoint[3].y - srcPoint[0].y + srcPoint[2].y - srcPoint[1].y) / 2;
        float height = sqrt(heightX * heightX + heightY * heightY);
        float width = sqrt(widthX * widthX + widthY * widthY);

        vector<Point2f> dstPoint(4);
        dstPoint[0] = Point2f(0, 0);
        dstPoint[1] = Point2f(width, 0);
        dstPoint[2] = Point2f(width, height);
        dstPoint[3] = Point2f(0, height);

        vector<Point2f> srcAffinePoint(3);
        srcAffinePoint[0] = srcPoint[0];
        srcAffinePoint[1] = srcPoint[1];
        srcAffinePoint[2] = srcPoint[2];


        cv::Mat dstImgWarp;
        cv::Mat perspectiveMat = getPerspectiveTransform(srcPoint, dstPoint);
        warpPerspective(findCodeRegionImg,dstImgWarp,perspectiveMat,Size(width,height));
//        cv::Mat affineMat = getAffineTransform(srcAffinePoint, dstPoint);
//        warpAffine(findCodeImgbig,dstImgWarp, affineMat,Size(width,height));

        warpMatSave.release();
        warpMatSave = perspectiveMat;
        codeRegionSave.clear();
        codeRegionSave = srcPoint;
        codeImgBig = dstImgWarp;
        double t2=((double)cv::getTickCount()-t1)*1000/getTickFrequency();

#ifdef DEBUG_CODENUM_TIME
        cout<<"NumDecoder: get codeImgBig time is: "<<t2<<"ms"<<endl;
#endif

#ifdef DEBUG_CODENUM_IMG
        imshow("dstImgWarp0", dstImgWarp);
#endif
        //Step2.去除不符合颜色要求的边缘
        t1 = getTickCount();
        cv::Mat findCodeRectImg, dst;
        cv::Mat sobelDstX, absDstX;
        findCodeRectImg = dstImgWarp.clone();
        cvtColor(findCodeRectImg, dst, COLOR_BGR2GRAY);
        Sobel(dst, sobelDstX, CV_16S, 1, 0);
        convertScaleAbs(sobelDstX, absDstX);

        cv::Mat imgHSV;
        cvtColor(findCodeRectImg, imgHSV, COLOR_BGR2HSV);
        cv::Mat absDstX8U;
        absDstX.convertTo(absDstX8U, CV_8U);
        threshold(absDstX8U, absDstX8U, 0, 255, THRESH_OTSU);

#ifdef DEBUG_CODENUM_IMGBIG
        imshow("dstX", sobelDstX);
        imshow("absDstX",absDstX);
        imshow("imageHSV", imgHSV);
        imshow("edgeAfterOTSU",absDstX8U);
#endif // COLOREDGEDEBUG
        //t1 = getTickCount();
        for (int i = 0; i < imgHSV.rows; i++) {
            for (int j = 0; j < imgHSV.cols; j++) {
                Vec3b tempVec3b;
                tempVec3b = imgHSV.at<Vec3b>(i, j);
                if ((absDstX8U.at<uchar>(i, j) > 0)) {
                    if ((tempVec3b.val[0] > 150) || (tempVec3b.val[0] < 18)) {///find red region
                        absDstX8U.at<uchar>(i, j) = 255;
                    } else {
                        absDstX8U.at<uchar>(i, j) = 0;
                    }
                }
            }
        }

        //t2=((double)cv::getTickCount()-t1)*1000/getTickFrequency();
        //cout<<"get red region time is: "<<t2<<"ms"<<endl;
#ifdef DEBUG_CODENUM_IMGBIG
        imshow("colorEdge",absDstX8U);
#endif // COLOREDGEDEBUG

        //对图像求平均,求平均区域为(2a+1)*(2b+1)
//        t1 = getTickCount();
        int a = color_edgeA, b = color_edgeB;
        cv::Mat imgCal = Mat::zeros(Size(absDstX.cols + 2 * b, absDstX.rows + 2 * a), CV_8U);
        cv::Mat imgROI = imgCal(Rect(b, a, absDstX.cols, absDstX.rows));
        absDstX8U.copyTo(imgROI, absDstX8U);
        cv::Mat imgAver;
        imgAver = Mat::zeros(Size(absDstX.cols, absDstX.rows), CV_8U);
        for (int i = 0; i < absDstX.cols; i++)
            for (int j = 0; j < absDstX.rows; j++) {
                unsigned int aver = 0;
                for (int cntI = -b; cntI <= b; cntI++)
                    for (int cntJ = -a; cntJ <= a; cntJ++) {
                        aver += imgCal.at<uchar>(Point(b + i + cntI, a + j + cntJ));
                    }
                aver /= ((2 * a + 1) * (2 * b + 1));
                imgAver.at<uchar>(Point(i, j)) = aver;
            }

        t2=((double)cv::getTickCount()-t1)*1000/getTickFrequency();
#ifdef DEBUG_CODENUM_TIME
        cout<<"NumDecoder: get averImg time is: "<<t2<<"ms"<<endl;;
#endif
#ifdef DEBUG_CODENUM_IMGBIG
        imshow("imgCal", imgCal);
        imshow("imgROI", imgROI);
        imshow("imgAver",imgAver);
#endif

        cv::Mat imgOTSU;
        threshold(imgAver, imgOTSU, 0, 255, THRESH_OTSU);
        dilate(imgOTSU, imgOTSU, Mat(), Point(-1, -1), color_edge_dilate);
//        cout << "imgOTSUDilate size is: " << imgOTSU.size() << endl;
        t2=((double)cv::getTickCount()-t1)*1000/getTickFrequency();

#ifdef DEBUG_CODENUM_TIME
        cout<<"NumDecoder: get codeImgSmallPrc time is: "<<t2<<"ms"<<endl;;
#endif
#ifdef DEBUG_CODENUM_IMG
        imshow("imgOTSU",imgOTSU);
#endif

        //寻找连通域
        cv::Mat imgContours = findCodeRectImg.clone();
//        t1 = getTickCount();
        vector<vector<Point>> contours;
        vector<Vec4i> hierarchy;
        findContours(imgOTSU, contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);
        vector<Rect> contourRects(contours.size());
        for (int i = 0; i < contours.size(); i++) {
            contourRects[i] = boundingRect(contours[i]);
//            rectangle(imgContours,contourRects[i], Scalar(0, 250, 0));
        }
//        imshow("contourRects",imgContours);

        //Step3. 滤除不符合要求的contours
        Rect idealRect;
//        if (!filterRects(contourRects, image.size(), image.cols / 2, idealRect)) return false;
        if (!filterRects(contourRects, findCodeRectImg.size(), idealRect)) return false;
//        rectangle(imgContours,idealRect, Scalar(0, 0, 250));
//        imshow("idealRect",imgContours);

        if (idealRect.x - idealRect.width / 5 < 0) {
            codeRect = Rect(0, idealRect.y, idealRect.width + idealRect.x, idealRect.height);
        } else {
            codeRect = Rect(idealRect.x - idealRect.width / 5, idealRect.y, idealRect.width * 6 / 5, idealRect.height);
        }

        t2=((double)cv::getTickCount()-t1)*1000/getTickFrequency();
#ifdef DEBUG_CODENUM_TIME
        cout<<"NumDecoder: get codeImgSmall time is: "<<t2<<"ms"<<endl;;
#endif
#ifdef DEBUG_CODENUM_IMG
        cv::Mat imgCodeRect = codeImgBig.clone();
        rectangle(imgCodeRect,codeRect, Scalar(0, 0, 250));
        imshow("codeRect",imgCodeRect);
//        cout << "codeRect is [" << codeRect.x << " "<< codeRect.y <<"] " << codeRect.size() << endl;
#endif // DEBUG_CODEREC


//    cout << "codeImgBig size is " << img.size() << endl;
//    codeRect = Rect(Point(82,73), Size(256, 53));
//    if((codeRect.x + codeRect.width > img.cols)||(codeRect.y + codeRect.height > img.rows))
//    {
//        codeRect = Rect();
//        return false;
//    }
        return true;
    }

    //write by sun on 2018/04/04
    bool NumDecoder::getCodeRect2(cv::Mat& frame, vector<Quad>& quadsNine) {

        //Step1.按九宫格区域取点,九宫格边界向上映射
        cv::Mat findCodeRegionImg = frame.clone();
        vector<Point2f> pointsTop(4);
        vector<Point2f> srcPoint(4);
//        imshow("frame0429",frame);
//        waitKey(0);
        double t1 = getTickCount();
        pointsTop[0] = quadsNine[0].up_left;
        pointsTop[1] = quadsNine[6].up_right;
        pointsTop[2] = quadsNine[8].down_right;
        pointsTop[3] = quadsNine[2].down_left;

        srcPoint[3] = pointsTop[0];
        srcPoint[2] = pointsTop[1];

        //将2相对于1对称， 3相对于0对称
        int xTemp0, yTemp0, xTemp1, yTemp1;

        xTemp0 = pointsTop[3].x - pointsTop[0].x;
        yTemp0 = pointsTop[3].y - pointsTop[0].y;

        xTemp1 = pointsTop[2].x - pointsTop[1].x;
        yTemp1 = pointsTop[2].y - pointsTop[1].y;


        if ((pointsTop[1].y - yTemp1 < 0)||(pointsTop[0].y - yTemp0 < 0)) {
            srcPoint[0].x = pointsTop[0].x - 0.5*xTemp0;
            srcPoint[0].y = pointsTop[0].y - 0.5*yTemp0;

            srcPoint[1].x = pointsTop[1].x - 0.5*xTemp1;
            srcPoint[1].y = pointsTop[1].y - 0.5*yTemp1;

        } else {
            srcPoint[0].x = pointsTop[0].x - xTemp0;
            srcPoint[0].y = pointsTop[0].y - yTemp0;
            srcPoint[1].x = pointsTop[1].x - xTemp1;
            srcPoint[1].y = pointsTop[1].y - yTemp1;
        }

        if((pointsTop[1].x - xTemp1 < 0)||(pointsTop[0].x - xTemp0 < 0)) {
            cout << "please reset pointTop" << endl;
            return false;
        }

#ifdef DEBUG_CODENUM_IMGBIG
        cout << "pointsTop" << endl << pointsTop << endl;
        cout << "yTemp0  " << yTemp0 << "yTemp1  " << yTemp1 <<endl;
        cout << "srcPoint" << endl << srcPoint << endl;
        cv::Mat codeNumRegionImg = frame.clone();
        cv::Mat codeNumRegionImg2 = frame.clone();
        line(codeNumRegionImg, pointsTop[0], pointsTop[1], Scalar(255, 255, 0), LINE_8);
        line(codeNumRegionImg, pointsTop[1], pointsTop[2], Scalar(0, 255, 0), LINE_8);
        line(codeNumRegionImg, pointsTop[2], pointsTop[3], Scalar(255, 255, 0), LINE_8);
        line(codeNumRegionImg, pointsTop[3], pointsTop[0], Scalar(0, 255, 255), LINE_8);

        line(codeNumRegionImg, srcPoint[0], srcPoint[1], Scalar(0, 255, 0), LINE_8);
        line(codeNumRegionImg, srcPoint[1], srcPoint[2], Scalar(0, 255, 0), LINE_8);
        line(codeNumRegionImg, srcPoint[2], srcPoint[3], Scalar(0, 255, 0), LINE_8);
        line(codeNumRegionImg, srcPoint[3], srcPoint[0], Scalar(0, 255, 0), LINE_8);

        //third method
        line(codeNumRegionImg2, AngleCal::codeNumImgPoints[0], AngleCal::codeNumImgPoints[1], Scalar(0, 255, 0), LINE_8);
        line(codeNumRegionImg2, AngleCal::codeNumImgPoints[1], AngleCal::codeNumImgPoints[2], Scalar(0, 255, 0), LINE_8);
        line(codeNumRegionImg2, AngleCal::codeNumImgPoints[2], AngleCal::codeNumImgPoints[3], Scalar(0, 255, 0), LINE_8);
        line(codeNumRegionImg2, AngleCal::codeNumImgPoints[3], AngleCal::codeNumImgPoints[0], Scalar(0, 255, 0), LINE_8);

        imshow("codeNumRegionImg", codeNumRegionImg);
        imshow("codeNumRegionImg2", codeNumRegionImg2);
#endif

        float widthX = (srcPoint[1].x - srcPoint[0].x + srcPoint[2].x - srcPoint[3].x) / 2;
        float widthY = (srcPoint[1].y - srcPoint[0].y + srcPoint[2].y - srcPoint[3].y) / 2;
        float heightX = (srcPoint[3].x - srcPoint[0].x + srcPoint[2].x - srcPoint[1].x) / 2;
        float heightY = (srcPoint[3].y - srcPoint[0].y + srcPoint[2].y - srcPoint[1].y) / 2;
        float height = sqrt(heightX * heightX + heightY * heightY);
        float width = sqrt(widthX * widthX + widthY * widthY);

        vector<Point2f> dstPoint(4);
        dstPoint[0] = Point2f(0, 0);
        dstPoint[1] = Point2f(width, 0);
        dstPoint[2] = Point2f(width, height);
        dstPoint[3] = Point2f(0, height);

        vector<Point2f> srcAffinePoint(3);
        srcAffinePoint[0] = srcPoint[0];
        srcAffinePoint[1] = srcPoint[1];
        srcAffinePoint[2] = srcPoint[2];


        cv::Mat dstImgWarp;
        cv::Mat perspectiveMat = getPerspectiveTransform(srcPoint, dstPoint);
        warpPerspective(findCodeRegionImg,dstImgWarp,perspectiveMat,Size(width,height));
//        cv::Mat affineMat = getAffineTransform(srcAffinePoint, dstPoint);
//        warpAffine(findCodeImgbig,dstImgWarp, affineMat,Size(width,height));

        //third method
//        cv::Mat codeRcodeRegion3;
//        Mat plane_coordinate(4,1,CV_32FC2),src_coordinate(4,1,CV_32FC2);
//        plane_coordinate.at<Point2f>(0, 0) = Point2f(0, 0);
//        plane_coordinate.at<Point2f>(1, 0) = Point2f(200, 0);
//        plane_coordinate.at<Point2f>(2, 0) = Point2f(200, 60);
//        plane_coordinate.at<Point2f>(3, 0) = Point2f(0, 60);
//
//        src_coordinate.at<Point2f>(0, 0) = AngleCal::codeNumImgPoints[0];
//        src_coordinate.at<Point2f>(1, 0) = AngleCal::codeNumImgPoints[1];
//        src_coordinate.at<Point2f>(2, 0) = AngleCal::codeNumImgPoints[2];
//        src_coordinate.at<Point2f>(3, 0) = AngleCal::codeNumImgPoints[3];
//        Mat trans_Perspective = getPerspectiveTransform(src_coordinate, plane_coordinate);
//        warpPerspective(frame, codeRcodeRegion3, trans_Perspective, Size(200, 60));
//        imshow("codeRcodeRegion3", codeRcodeRegion3);


        warpMatSave.release();
        warpMatSave = perspectiveMat;
        codeRegionSave.clear();
        codeRegionSave = srcPoint;
        codeImgBig = dstImgWarp;
        double t2=((double)cv::getTickCount()-t1)*1000/getTickFrequency();

#ifdef DEBUG_CODENUM_TIME
        cout<<"NumDecoder: get codeImgBig time is: "<<t2<<"ms"<<endl;
#endif

#ifdef DEBUG_CODENUM_IMG
        imshow("dstImgWarp", dstImgWarp);
#endif
        //Step2.去除不符合颜色要求的边缘
        t1 = getTickCount();
        cv::Mat findCodeRectImg;
        findCodeRectImg = dstImgWarp.clone();
        cv::Mat splitImg = findCodeRectImg.clone();
        cv::Mat RGBImg[3];
        split(splitImg, RGBImg);
        //显示图像
        threshold(RGBImg[2],RGBImg[2],0,255,CV_THRESH_OTSU);
        dilate(RGBImg[2], RGBImg[2], Mat(), Point(-1, -1), color_edge_dilate);
        t2=((double)cv::getTickCount()-t1)*1000/getTickFrequency();
#ifdef DEBUG_CODENUM_IMGBIG
        imshow("imageR",RGBImg[2]);
        cv::cvtColor(splitImg, splitImg, CV_BGR2HSV);
        cv::Mat HSVImg[3];
        split(splitImg, HSVImg);
        threshold(HSVImg[2],HSVImg[2],0,255,CV_THRESH_OTSU);
        dilate(HSVImg[2], HSVImg[2], Mat(), Point(-1, -1), color_edge_dilate);
        imshow("imageV",HSVImg[2]);
#endif // COLOREDGEDEBUG
#ifdef DEBUG_CODENUM_TIME
        cout<<"NumDecoder: get codeImgSmallPrc time is: "<<t2<<"ms"<<endl;;
#endif
        //Step3. 滤除不符合要求的contours
        t1 = getTickCount();
        vector<vector<Point>> contours;
        vector<Vec4i> hierarchy;
        findContours(RGBImg[2], contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);
        vector<Rect> contourRects(contours.size());
        for (int i = 0; i < contours.size(); i++) {
            contourRects[i] = boundingRect(contours[i]);
        }
#ifdef DEBUG_CODENUM_IMGBIG
        cv::Mat contoursImg = findCodeRectImg.clone();
         for (int i = 0; i < contours.size(); i++) {
            rectangle(contoursImg,contourRects[i], Scalar(0, 250, 0));
            }
        imshow("contoursImg",contoursImg);
#endif
        Rect idealRect;
        if (!filterRects(contourRects, findCodeRectImg.size(), idealRect)) return false;
#ifdef DEBUG_CODENUM_IMG
        cv::Mat imgIdealCodeRect = dstImgWarp.clone();
        rectangle(imgIdealCodeRect,idealRect, Scalar(0, 0, 250));
        imshow("idealCodeRect",imgIdealCodeRect);
        //waitKey(0);
#endif
        if (idealRect.x - 0.1*idealRect.width < 0){
            codeRect.x = 0;
            codeRect.y = idealRect.y;
        } else {
            codeRect.x = idealRect.x - 0.1*idealRect.width;
            codeRect.y = idealRect.y;
        }

        if (codeRect.x + idealRect.width*1.2 > dstImgWarp.cols) {
            codeRect.width = dstImgWarp.cols - codeRect.x;
            codeRect.height = idealRect.height;
        } else {
            codeRect.width = idealRect.width*1.2;
            codeRect.height = idealRect.height;
        }

        t2=((double)cv::getTickCount()-t1)*1000/getTickFrequency();
#ifdef DEBUG_CODENUM_TIME
        cout<<"NumDecoder: get codeRect time is: "<<t2<<"ms"<<endl;;
#endif
#ifdef DEBUG_CODENUM_IMG
        cv::Mat imgCodeRect = dstImgWarp.clone();
        rectangle(imgCodeRect,codeRect, Scalar(0, 0, 250));
        imshow("codeRect",imgCodeRect);
#endif
        return true;
    }

    //write by sun on 2018/04/24
    bool NumDecoder::getCodeRect3(cv::Mat& frame, vector<Quad>& quadsNine) {
        double t1 = getTickCount();
        cv::Mat findCodeRegionImg = frame.clone();
#ifdef DEBUG_CODENUM_IMGBIG
        cv::Mat codeNumRegionImg = frame.clone();
        //third method
        line(codeNumRegionImg, AngleCal::codeNumImgPoints[0], AngleCal::codeNumImgPoints[1], Scalar(0, 255, 0), LINE_8);
        line(codeNumRegionImg, AngleCal::codeNumImgPoints[1], AngleCal::codeNumImgPoints[2], Scalar(0, 255, 0), LINE_8);
        line(codeNumRegionImg, AngleCal::codeNumImgPoints[2], AngleCal::codeNumImgPoints[3], Scalar(0, 255, 0), LINE_8);
        line(codeNumRegionImg, AngleCal::codeNumImgPoints[3], AngleCal::codeNumImgPoints[0], Scalar(0, 255, 0), LINE_8);
        imshow("codeNumRegionImg2", codeNumRegionImg);
#endif
        //third method
        cv::Mat dstImgWarp;
        Mat plane_coordinate(4,1,CV_32FC2),src_coordinate(4,1,CV_32FC2);
        plane_coordinate.at<Point2f>(0, 0) = Point2f(0, 0);
        plane_coordinate.at<Point2f>(1, 0) = Point2f(110, 0);
        plane_coordinate.at<Point2f>(2, 0) = Point2f(110, 56);
        plane_coordinate.at<Point2f>(3, 0) = Point2f(0, 56);

        src_coordinate.at<Point2f>(0, 0) = AngleCal::codeNumImgPoints[0];
        src_coordinate.at<Point2f>(1, 0) = AngleCal::codeNumImgPoints[1];
        src_coordinate.at<Point2f>(2, 0) = AngleCal::codeNumImgPoints[2];
        src_coordinate.at<Point2f>(3, 0) = AngleCal::codeNumImgPoints[3];
        Mat perspectiveMat = getPerspectiveTransform(src_coordinate, plane_coordinate);
        warpPerspective(findCodeRegionImg, dstImgWarp, perspectiveMat, Size(110, 56));

        warpMatSave.release();
        warpMatSave = perspectiveMat;
        codeRegionSave.clear();
        codeRegionSave = AngleCal::codeNumImgPoints;
        codeImgBig = dstImgWarp;
        double t2=((double)cv::getTickCount()-t1)*1000/getTickFrequency();

#ifdef DEBUG_CODENUM_TIME
        cout<<"NumDecoder: get codeImgBig time is: "<<t2<<"ms"<<endl;
        out<<"-------------------------start------------------------------"<<endl;
        out<<"NumDecoder: get codeImgBig time is: "<<t2<<"ms"<<endl;
#endif

#ifdef DEBUG_CODENUM_IMG
        imshow("dstImgWarp", dstImgWarp);
#endif
        //Step2.去除不符合颜色要求的边缘
        t1 = getTickCount();
        cv::Mat findCodeRectImg;
        findCodeRectImg = dstImgWarp.clone();
        cv::Mat splitImg = findCodeRectImg.clone();
        cv::Mat RGBImg[3];
        split(splitImg, RGBImg);
        //显示图像
        threshold(RGBImg[2],RGBImg[2],0,255,CV_THRESH_OTSU);
        dilate(RGBImg[2], RGBImg[2], Mat(), Point(-1, -1), color_edge_dilate);
//        imshow("imageR",RGBImg[2]);
        t2=((double)cv::getTickCount()-t1)*1000/getTickFrequency();
#ifdef DEBUG_CODENUM_IMGBIG
        cv::cvtColor(splitImg, splitImg, CV_BGR2HSV);
        cv::Mat HSVImg[3];
        split(splitImg, HSVImg);
        threshold(HSVImg[2],HSVImg[2],0,255,CV_THRESH_OTSU);
        dilate(HSVImg[2], HSVImg[2], Mat(), Point(-1, -1), color_edge_dilate);
        imshow("imageV",HSVImg[2]);
#endif // COLOREDGEDEBUG
#ifdef DEBUG_CODENUM_TIME
        cout<<"NumDecoder: get codeImgSmallPrc time is: "<<t2<<"ms"<<endl;
        out<<"NumDecoder: get codeImgSmallPrc time is: "<<t2<<"ms"<<endl;
#endif
        //Step3. 滤除不符合要求的contours
        t1 = getTickCount();
        vector<vector<Point>> contours;
        vector<Vec4i> hierarchy;
        findContours(RGBImg[2], contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);
        vector<Rect> contourRects(contours.size());
        for (int i = 0; i < contours.size(); i++) {
            contourRects[i] = boundingRect(contours[i]);
        }
#ifdef DEBUG_CODENUM_IMGBIG
        cv::Mat contoursImg = findCodeRectImg.clone();
        for (int i = 0; i < contours.size(); i++) {
            rectangle(contoursImg,contourRects[i], Scalar(0, 250, 0));
        }
        imshow("contoursImg",contoursImg);
#endif
        Rect idealRect;
        if (!filterRects(contourRects, findCodeRectImg.size(), idealRect)) return false;
#ifdef DEBUG_CODENUM_IMG
        cv::Mat imgIdealCodeRect = dstImgWarp.clone();
        rectangle(imgIdealCodeRect,idealRect, Scalar(0, 0, 250));
        imshow("idealCodeRect",imgIdealCodeRect);
        //waitKey(0);
#endif
        if (idealRect.x - 0.1*idealRect.width < 0){
            codeRect.x = 0;
            codeRect.y = idealRect.y;
        } else {
            codeRect.x = idealRect.x - 0.1*idealRect.width;
            codeRect.y = idealRect.y;
        }

        if (codeRect.x + idealRect.width*1.2 > dstImgWarp.cols) {
            codeRect.width = dstImgWarp.cols - codeRect.x;
            codeRect.height = idealRect.height;
        } else {
            codeRect.width = idealRect.width*1.2;
            codeRect.height = idealRect.height;
        }

        t2=((double)cv::getTickCount()-t1)*1000/getTickFrequency();
#ifdef DEBUG_CODENUM_TIME
        cout<<"NumDecoder: get codeRect time is: "<<t2<<"ms"<<endl;
        out<<"NumDecoder: get codeRect time is: "<<t2<<"ms"<<endl;
#endif
#ifdef DEBUG_CODENUM_IMG
        cv::Mat imgCodeRect = dstImgWarp.clone();
        rectangle(imgCodeRect,codeRect, Scalar(0, 0, 250));
        imshow("codeRect",imgCodeRect);
#endif
        return true;
    }

    bool NumDecoder::getNumImg(cv::Mat& image, std::vector<Mat> &singleNumImgs) {
        //Step1. find num rects
        cv::Mat binaryImgFinCon = image.clone();
        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;
        findContours(binaryImgFinCon, contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);

        vector<Rect> numRects(contours.size());
        for(int i = 0; i < contours.size(); i ++) {
            numRects[i] = boundingRect(contours[i]);
        }

#ifdef DEBUG_CODENUM_SIGLENUM_IMG
        cout << "NumDecoder: numRects is " << numRects.size() << endl;
        cv::Mat numRectsImg = codeImgBig(codeRect).clone();
        for(int i = 0; i < numRects.size(); i ++) {
            rectangle(numRectsImg, numRects[i], Scalar(0, 255, 0));
        }
        namedWindow("numRectsImg", WINDOW_NORMAL);
        imshow("numRectsImg", numRectsImg);
#endif

        //Step2. filter rect
        vector<Rect> numRectsRest, numRectsTmp, numRectsOut;
        for (int i = 0; i < numRects.size(); i++) {
            float ratio = (float)numRects[i].width/numRects[i].height;
//            cout << "width" << contourRects[i].width << endl;
//            cout << "height" << contourRects[i].height << endl;
//            cout << "num ratio  " << ratio << endl;
            if ((ratio < max_small_width_to_height) && (ratio> min_small_width_to_height)) {
                numRectsTmp.push_back(numRects[i]);
            } else if(ratio <= min_small_width_to_height){
                numRectsRest.push_back(numRects[i]);
            }
        }

        if(numRectsTmp.size() == 0) {
            cout << "NumDecoder: can't get get single number images" << endl;
            return false;
        }

#ifdef DEBUG_CODENUM_SIGLENUM_IMG
        cout << "NumDecoder: numRects after filter by width_to_height_ratio is " << numRectsTmp.size() << endl;
        cv::Mat numRectsImg0 = codeImgBig(codeRect).clone();
        for(int i = 0; i < numRectsTmp.size(); i ++) {
            rectangle(numRectsImg0, numRectsTmp[i], Scalar(0, 255, 0));
        }
        namedWindow("numRectsFilterRatio", WINDOW_NORMAL);
        imshow("numRectsFilterRatio", numRectsImg0);
#endif

        //按面积过滤
        for (int i = 0; i < numRectsTmp.size(); i++) {
            float imgSizeThreash = image.size().area() * small_area_ratio;
//            cout<<"an important num for area"<<RectsTmp[i].width * RectsTmp[i].height<<endl;
//                  cout<<" imgSizeThreash"<< imgSizeThreash<<endl;
            if(numRectsTmp[i].width * numRectsTmp[i].height> imgSizeThreash) {
                numRectsOut.push_back(numRectsTmp[i]);
            } else{
                numRectsRest.push_back(numRectsTmp[i]);
            }
        }

        if(numRectsOut.size() == 0) {
            cout << "NumDecoder: can't get number images" << endl;
            return false;
        }


#ifdef DEBUG_CODENUM_SIGLENUM_IMG
        cout << "NumDecoder: numRects after filter by area is " << numRectsOut.size() << endl;
        cv::Mat numRectsImg1 = codeImgBig(codeRect).clone();
        for(int i = 0; i < numRectsOut.size(); i ++) {
            rectangle(numRectsImg1, numRectsOut[i], Scalar(0, 255, 0));
        }
        namedWindow("numRectsFilterArea", WINDOW_NORMAL);
        imshow("numRectsFilterArea", numRectsImg1);
#endif
        vector<int> smallRectIndex;
        for(int i = 0; i < numRectsOut.size(); i++){
            for(int j = 0; j < numRectsOut.size(); j++){
                if(j != i){
                    if(numRectsOut[j].x < numRectsOut[i].x +numRectsOut[i].width
                       && numRectsOut[j].x > numRectsOut[i].x
                       && numRectsOut[j].y < numRectsOut[i].y +numRectsOut[i].height
                       && numRectsOut[j].y > numRectsOut[i].y){
                        smallRectIndex.push_back(j);
                    }
                }
            }
        }

        //cout << "smallRectIndex" << smallRectIndex.size() <<endl;

        vector<Rect> newNumRectOut;
        bool compareFlag[100] = {false};
        if(smallRectIndex.size() > 0){
            for(int i = 0; i < smallRectIndex.size(); i++){
                for (int j = 0; j < numRectsOut.size(); j++){
                    if(j != smallRectIndex[i] && compareFlag[j] == false){
                        newNumRectOut.push_back(numRectsOut[j]);
                        compareFlag[j] = true;
                    }
                }
            }
        } else{
            newNumRectOut.assign(numRectsOut.begin(), numRectsOut.end());
        }

        if(newNumRectOut.size() == 0) {
            cout << "NumDecoder: can't get number images" << endl;
            return false;
        }

#ifdef DEBUG_CODENUM_SIGLENUM_IMG
        cout << "NumDecoder: numRects after filter by area smaller is " << newNumRectOut.size() << endl;
        cv::Mat numRectsImg2 = codeImgBig(codeRect).clone();
        for(int i = 0; i < newNumRectOut.size(); i ++) {
            rectangle(numRectsImg2, newNumRectOut[i], Scalar(0, 255, 0));
        }
        namedWindow("numRectsFilterAreaSmaller", WINDOW_NORMAL);
        imshow("numRectsFilterAreaSmaller", numRectsImg2);
#endif

        //在找出符合标准的矩形后对丢失的1进行再发现
        if(newNumRectOut.size() == 4){
            float RectsHeight = 0;
            float RectsWidth = 0;
            for(int i = 0; i < newNumRectOut.size(); i ++) {
                RectsHeight += newNumRectOut[i].height;
                RectsWidth += newNumRectOut[i].width;
            }

            singleNumHeight = RectsHeight /= newNumRectOut.size();
            singleNumWidth = RectsWidth /= newNumRectOut.size();
            singleNumArea = singleNumHeight*singleNumWidth;
            for(int i = 0; i < numRectsRest.size(); i ++) {
//            cout << "rectHeight" << RectsHeight << endl;
//            cout << "RectsRestHeight" << RectsRest[i].height << endl;
                if(isEqual(numRectsRest[i].height, RectsHeight, RectsHeight/3)) {
                    newNumRectOut.push_back(numRectsRest[i]);
                }
            }
        }

#ifdef DEBUG_CODENUM_SIGLENUM_IMG
        cout << "NumDecoder: numRects after ReFindOne is " << newNumRectOut.size() << endl;
        cv::Mat numRectsImg3 = codeImgBig(codeRect).clone();
        for(int i = 0; i < newNumRectOut.size(); i ++) {
            rectangle(numRectsImg3, newNumRectOut[i], Scalar(0, 255, 0));
        }
        namedWindow("numRectsRefindOne", WINDOW_NORMAL);
        imshow("numRectsRefindOne", numRectsImg3);
#endif

        //Step3 get five number images which order is right
        for(int i = 0; i < newNumRectOut.size(); i ++) {
            for(int j = i+1; j < newNumRectOut.size(); j++){
                if(newNumRectOut[i].x > newNumRectOut[j].x)
                    swap(newNumRectOut[i], newNumRectOut[j]);
            }
        }

        singleNumImgs.clear();
        cv::Mat finalRectsImg = getNumImg0.clone();
        for(int i = 0; i < newNumRectOut.size(); i ++) {
            singleNumImgs.push_back(finalRectsImg(newNumRectOut[i]).clone());
        }


#ifdef DEBUG_CODENUM_SIGLENUM_IMG
        for(int i = 0; i < singleNumImgs.size(); i ++) {
            ostringstream oss;
            oss<< i << ".jpg"<<endl;
//            imshow(oss.str(), singleNumImgs[i]);
            imwrite(oss.str(), singleNumImgs[i]);
        }
#endif
        return true;
    }

    bool NumDecoder::codeImg2Num(vector<Mat> singleNumImgs, vector<int>& codeNum) {

        int lineHorizon1, lineHorizon2;
        for(int i = 0; i < singleNumImgs.size(); i++) {
//            cv::Mat imgTmp;
//            imgTmp = singleNumImgs[i].clone();
            cv::Mat numImg = singleNumImgs[i].clone();
            //cvtColor(singleNumImgs[i], singleNumImgs[i], CV_RGB2GRAY);
            //cv::threshold(singleNumImgs[i], singleNumImgs[i], 128, 255, THRESH_OTSU);
//            Thin(singleNumImgs[i], singleNumImgs[i], 2);
#ifdef DEBUG_CODENUM_SIGLENUM_IMG
            ostringstream os;
            os << i;
            cv::namedWindow(os.str(),CV_WINDOW_NORMAL);
            cv::imshow(os.str(), singleNumImgs[i]);
#endif

//            cout << "find one ratio: " <<singleNumImgs[i].cols/(double)singleNumImgs[i].rows << endl;
//            if(singleNumImgs[i].cols/(double)singleNumImgs[i].rows <= MIN_SMALL_WIDTH_TO_HEIGHT ||
//                    fabs(singleNumImgs[i].size().area() - singleNumArea)/singleNumArea > 0.4) {
//                codeNum.push_back(1);
//                cout << "find one" << endl;
//                continue;
//            }
//            cout << "find one ratio: " <<singleNumImgs[i].cols/(double)singleNumImgs[i].rows << endl;

            if(singleNumImgs[i].cols/(double)singleNumImgs[i].rows <= min_small_width_to_height) {
                codeNum.push_back(1);
                cout << "NumDecoder: find one" << endl;
                continue;
            }
            lineHorizon1 = singleNumImgs[i].rows * 1/4;//y
            lineHorizon2 = singleNumImgs[i].rows * 3/4;
//            cout << "line1 " << lineHorizon1 << " line2 " << lineHorizon2 << endl;
//            cout << "ImgSize " << singleNumImgs[i].size() << endl;

            Point2f pointA, pointB;
            pointA = Point2f(singleNumImgs[i].cols/30.0*15,0);
            pointB = Point2f(singleNumImgs[i].cols/30.0*12,singleNumImgs[i].rows);
            //一条斜线
            float lineA, lineB;
            Mat Y = Mat::zeros(1,2,CV_32FC1);
            Mat X = Mat::zeros(2,2,CV_32FC1);
            Y.at<float>(0,0) = pointA.y;
            Y.at<float>(0,1) = pointB.y;
            X.at<float>(0,0) = pointA.x;
            X.at<float>(0,1) = pointB.x;
            X.at<float>(1,0) = 1;
            X.at<float>(1,1) = 1;
            Mat paraMat = Y * X.inv();
            lineA = paraMat.at<float>(0,0);
            lineB = paraMat.at<float>(0,1);
            vector<int> codeSingelDistribute{0,0,0,0,0}; //5个部分的顺序是中，左上，右上，左下，右下

            //计算5个部分的分布情况
            int initVal = 0;
            for(int k = 0; k < singleNumImgs[i].rows; k ++) {
                int x = (int)((k - lineB)/lineA);
                if(((initVal > 0)&&(singleNumImgs[i].at<uchar>(k, x) == 0))||
                   ((initVal == 0)&&(singleNumImgs[i].at<uchar>(k, x) > 0))) {
                    codeSingelDistribute[0] ++;
                    initVal = singleNumImgs[i].at<uchar>(k, x);
                }
            }
            if(initVal > 0) codeSingelDistribute[0]++;

            initVal = 0;
            int judgeK = (int)((lineHorizon1 - lineB)/lineA);
            for(int k = 0; k < singleNumImgs[i].cols; k ++) {
                if(((initVal > 0)&&(singleNumImgs[i].at<uchar>(lineHorizon1, k) == 0))||
                   ((initVal == 0)&&(singleNumImgs[i].at<uchar>(lineHorizon1, k) > 0))) {
                    if(k <= judgeK) {
                        codeSingelDistribute[1] ++;
                        initVal = singleNumImgs[i].at<uchar>(lineHorizon1, k);
                    }
                    else {
                        codeSingelDistribute[2] ++;
                        initVal = singleNumImgs[i].at<uchar>(lineHorizon1, k);
                    }
                }
            }
            if(initVal > 0) codeSingelDistribute[2] ++;

            initVal = 0;
            judgeK = (int)((lineHorizon2 - lineB)/lineA);
            for(int k = 0; k < singleNumImgs[i].cols; k ++) {
                if(((initVal > 0)&&(singleNumImgs[i].at<uchar>(lineHorizon2, k) == 0))||
                   ((initVal == 0)&&(singleNumImgs[i].at<uchar>(lineHorizon2, k) > 0))) {
                    if(k < judgeK) {
                        codeSingelDistribute[3] ++;
                        initVal = singleNumImgs[i].at<uchar>(lineHorizon2, k);
                    }
                    else {
//                        cout << "judgeK" << judgeK << endl;
//                        cout << "K" << k << endl;
                        codeSingelDistribute[4] ++;
                        initVal = singleNumImgs[i].at<uchar>(lineHorizon2, k);
                    }
                }
            }
            if(initVal > 0) codeSingelDistribute[4] ++;

#ifdef DEBUG_CODENUM_SIGLENUM_IMG
            cout << "codeSingelDistribute1" << endl;
            for(int i = 0; i < 5; i ++) {
                cout << codeSingelDistribute[i] << "  ";
            }
            cout << endl;
#endif

            for(int i = 0; i < 5; i ++) {
                if((codeSingelDistribute[i] % 2) == 1) continue;
                codeSingelDistribute[i] /= 2;
            }

            for(int i = 1; i < 5; i ++) {
                if(codeSingelDistribute[i] > 1)
                    codeSingelDistribute[i] = 1;
            }

#ifdef DEBUG_CODENUM_SIGLENUM_IMG
            cout << "codeSingelDistribute2" << endl;
            for(int i = 0; i < 5; i ++) {
                cout << codeSingelDistribute[i] << "  ";
            }
            cout << endl;
#endif

            static int errorCount = 1;
            //判断数码管数字
            if((codeSingelDistribute[0] == 3)&&(codeSingelDistribute[1] == 0)&&(codeSingelDistribute[2] == 1)&&(codeSingelDistribute[3] == 1)&&(codeSingelDistribute[4] == 0)) codeNum.push_back(2);
            else if((codeSingelDistribute[0] == 3)&&(codeSingelDistribute[1] == 0)&&(codeSingelDistribute[2] == 1)&&(codeSingelDistribute[3] == 0)&&(codeSingelDistribute[4] == 1)) codeNum.push_back(3);
            else if((codeSingelDistribute[0] == 3)&&(codeSingelDistribute[1] == 0)&&(codeSingelDistribute[2] == 1)&&(codeSingelDistribute[3] == 1)&&(codeSingelDistribute[4] == 1)) codeNum.push_back(3);
            else if((codeSingelDistribute[0] == 1)&&(codeSingelDistribute[1] == 1)&&(codeSingelDistribute[2] == 1)&&(codeSingelDistribute[3] == 0)&&(codeSingelDistribute[4] == 1)) codeNum.push_back(4);
            else if((codeSingelDistribute[0] == 2)&&(codeSingelDistribute[1] == 1)&&(codeSingelDistribute[2] == 1)&&(codeSingelDistribute[3] == 0)&&(codeSingelDistribute[4] == 1)) codeNum.push_back(4);
            else if((codeSingelDistribute[0] == 1)&&(codeSingelDistribute[1] == 1)&&(codeSingelDistribute[2] == 1)&&(codeSingelDistribute[3] == 0)&&(codeSingelDistribute[4] == 0)) codeNum.push_back(4);
            else if((codeSingelDistribute[0] == 3)&&(codeSingelDistribute[1] == 1)&&(codeSingelDistribute[2] == 0)&&(codeSingelDistribute[3] == 0)&&(codeSingelDistribute[4] == 1)) codeNum.push_back(5);
            else if((codeSingelDistribute[0] == 3)&&(codeSingelDistribute[1] == 1)&&(codeSingelDistribute[2] == 0)&&(codeSingelDistribute[3] == 1)&&(codeSingelDistribute[4] == 1)) codeNum.push_back(6);
            else if((codeSingelDistribute[0] == 1)&&(codeSingelDistribute[1] == 0)&&(codeSingelDistribute[2] == 1)&&(codeSingelDistribute[3] == 0)&&(codeSingelDistribute[4] == 1)) codeNum.push_back(7);
            else if((codeSingelDistribute[0] == 2)&&(codeSingelDistribute[1] == 0)&&(codeSingelDistribute[2] == 1)&&(codeSingelDistribute[3] == 0)&&(codeSingelDistribute[4] == 1)) codeNum.push_back(7);
            else if((codeSingelDistribute[0] == 1)&&(codeSingelDistribute[1] == 0)&&(codeSingelDistribute[2] == 1)&&(codeSingelDistribute[3] == 0)&&(codeSingelDistribute[4] == 0)) codeNum.push_back(7);
            else if((codeSingelDistribute[0] == 1)&&(codeSingelDistribute[1] == 1)&&(codeSingelDistribute[2] == 0)&&(codeSingelDistribute[3] == 0)&&(codeSingelDistribute[4] == 1)) codeNum.push_back(7);
            else if((codeSingelDistribute[0] == 3)&&(codeSingelDistribute[1] == 1)&&(codeSingelDistribute[2] == 1)&&(codeSingelDistribute[3] == 1)&&(codeSingelDistribute[4] == 1)) codeNum.push_back(8);
            else if((codeSingelDistribute[0] == 3)&&(codeSingelDistribute[1] == 1)&&(codeSingelDistribute[2] == 1)&&(codeSingelDistribute[3] == 0)&&(codeSingelDistribute[4] == 1)) codeNum.push_back(9);
            else {
                for(int i = 0; i < 5; i ++) {
                    cout << codeSingelDistribute[i] << " ";
                }
                cout << endl;
                ostringstream oss;
                oss <<"/home/infantry/Infantry/data/buff/buff_error/code_num";
                oss << "/" << errorCount <<".jpg";
                //namedWindow(oss.str(),WINDOW_NORMAL);
                line(numImg, pointA, pointB, Scalar(0, 255, 0));
                line(numImg,Point(0,lineHorizon1),Point(numImg.cols, lineHorizon1), Scalar(0, 255, 0));
                line(numImg,Point(0,lineHorizon2),Point(numImg.cols, lineHorizon2), Scalar(0, 255, 0));
                //imshow(oss.str(), numImg);
                if(errorCount<1000)
                {
                   imwrite(oss.str(), singleNumImgs[i]);
                }
                errorCount ++;
//                waitKey(0);
                continue;
            }

//#ifdef DEBUG_CODENUM_SIGLENUM_IMG
//            ostringstream oss2;
//            oss2 << i << "RGB";
//            namedWindow(oss2.str(),WINDOW_NORMAL);
//            line(numImg, pointA, pointB, Scalar(0, 255, 0));
//            line(numImg,Point(0,lineHorizon1),Point(numImg.cols, lineHorizon1), Scalar(0, 255, 0));
//            line(numImg,Point(0,lineHorizon2),Point(numImg.cols, lineHorizon2), Scalar(0, 255, 0));
//            imshow(oss2.str(), numImg);
//#endif
        }

        if (codeNum.size() !=5){
            cout << "NumDecoder: codeImg2Num faild" << endl;
        }
        return true;
    }

    //used in getCodeRect function by sun
    bool NumDecoder::filterRects(std::vector<Rect>& Rects, Size imgSize, Rect& idealRect) {

        if (Rects.size() < 1) return false;
        vector<Rect> idealRects;
        for (int i = 0; i < Rects.size(); i++){
            float imgArea = imgSize.area();
            if ((Rects[i].area() > imgArea * ratio_rect_to_img)) {///0.01
                    idealRects.push_back(Rects[i]);
            }
        }

#ifdef DEBUG_CODENUM_IMG
        cout << "NumDecoder: filter by ratio_rect_to_img, codeRects size is " << idealRects.size() << endl;
#endif

        for(vector<Rect>::iterator it = idealRects.begin(); it != idealRects.end();){
            Rect& myRect  = *it;

            //cout << "width: " << myRect.width << endl;
            //cout <<"height: " << myRect.height << endl;
            float ratio =(float)myRect.width/myRect.height;

            //cout<<"getCodeRect function ratio ="<< ratio<<endl;

            if((ratio > min_width_to_height) && (ratio < max_width_to_height)){
                it++;
            } else {
                idealRects.erase(it);
            }
        }
#ifdef DEBUG_CODENUM_IMG
        cout << "NumDecoder: filter by length_to_width, codeRects size is " << idealRects.size() << endl;
#endif

        if (idealRects.size() < 1) return false;

        if(idealRects.size() > 1){
                for (int i = 0; i < idealRects.size(); i++) {
                    for (int j = i + 1; j < idealRects.size(); j++) {
                    if (idealRects[j].y + 0.5*idealRects[j].height > idealRects[i].y + 0.5*idealRects[i].height)
                        swap(idealRects[j], idealRects[i]);
                }
            }
        }
        //old
//        float minDist = 9999;
//        int idealIndex = -1;
//        for (int i = 0; i < idealRects.size(); i++) {
//            float thisDist = fabs(idealRects[i].x + idealRects[i].width / 2 - imgSize.width/2);
//            if (thisDist < minDist) {
//                minDist = thisDist;
//                idealIndex = i;
//            }
//        }
//        if (idealIndex == -1) return false;
        idealRect = idealRects[0];
        return true;
    }

    bool NumDecoder::isEqual(float x, float y, float err) {
        if (fabs(x - y) < 1e-8) return true;
        if (fabs(y) < 10 && fabs(x - y) < 10) return true;///mindis约等于0怎么办?
        if (err > 1) {///改成绝对值处理
            if (fabs(x - y) - err < 1e-8)
                return true;
            else
                return false;
        }
        if (fabs(x - y) / x < err) return true;
        return false;
    }

    bool NumDecoder::init(cv::Mat& frame, vector<Quad>& quadsNine){
        //Step1. Find code rect
        vector<Mat> imgVec;
        cv::Mat image = frame.clone();
        double t1 = getTickCount();

//        if (!getCodeRect(image, quadsNine)) {
//            cout << "NumDecoder: get code rect failed" << endl;
//        };
        cout<<"numdecoder:init for get code rect"<<endl;
//                imshow("image0429",image);
//                waitKey(0);
        if (!getCodeRect3(image, quadsNine)) {
            cout << "NumDecoder: get code rect failed" << endl;
            return false;
        };

        if ((codeRegionSave.size() == 0) || (codeRect.size() == Size(0, 0))) {
            cout << "NumDecoder: no code imgs" << endl;
            return false;
        }
        double t2=((double)cv::getTickCount()-t1)*1000/getTickFrequency();
#ifdef DEBUG_CODENUM_TIME
        cout<<"NumDecoder: find code rect time is: "<<t2<<"ms"<<endl;
#endif
        return true;
    }

    bool NumDecoder::run(cv::Mat& frame, vector<Quad> &quadsNine, vector<int>& codeNum) {

        cv::Mat image = frame.clone();
        if((codeRegionSave.size() == 0)||(codeRect.size() == Size(0,0))) {
            cout << "no code imgs" << endl;
            return false;
        }

        float widthX = (codeRegionSave[1].x - codeRegionSave[0].x + codeRegionSave[2].x - codeRegionSave[3].x) / 2;
        float widthY = (codeRegionSave[1].y - codeRegionSave[0].y + codeRegionSave[2].y - codeRegionSave[3].y) / 2;
        float heightX = (codeRegionSave[3].x - codeRegionSave[0].x + codeRegionSave[2].x - codeRegionSave[1].x) / 2;
        float heightY = (codeRegionSave[3].y - codeRegionSave[0].y + codeRegionSave[2].y - codeRegionSave[1].y) / 2;
        float height = sqrt(heightX * heightX + heightY * heightY);
        float width = sqrt(widthX * widthX + widthY * widthY);

        cv::Mat dstImgWarp;
        //getCodeRect2
        //warpPerspective(image,dstImgWarp,warpMatSave,Size(width,height));
        //getCodeRect3
        warpPerspective(image,dstImgWarp,warpMatSave,Size(110,56));

//        warpAffine(image,dstImgWarp, warpMatSave,Size(width,height));
        codeImgBig = dstImgWarp;

#ifdef DEBUG_CODENUM_IMG
        imshow("codeImgBig", codeImgBig);
#endif // DEBUG_CODEREC
        //Step2. Find single number rectangle
        //调节对比度凸显数码管区域
        double t1 = getTickCount();

//        cv::Mat splitImg = codeImgBig(codeRect).clone();
//        cv::Mat RGBImg[3];
//        split(splitImg, RGBImg);
//        //显示图像
//        threshold(RGBImg[0],RGBImg[0],0,255,CV_THRESH_OTSU);
//        threshold(RGBImg[1],RGBImg[1],0,255,CV_THRESH_OTSU);
//        threshold(RGBImg[2],RGBImg[2],0,255,CV_THRESH_OTSU);
////        erode(RGBImg[2], RGBImg[2], element_size, Point(-1,-1), 1);
//
//        imshow("imageB",RGBImg[0]);
//        imshow("imageG",RGBImg[1]);
//        imshow("imageR",RGBImg[2]);
//        cv::cvtColor(splitImg, splitImg, CV_BGR2HSV);
//        cv::Mat HSVImg[3];
//        split(splitImg, HSVImg);
//        //创建图像显示窗口
////        namedWindow("imageV",CV_WINDOW_NORMAL);
//        //显示图像
//        threshold(HSVImg[0],HSVImg[0],0,255,CV_THRESH_OTSU);
//        threshold(HSVImg[1],HSVImg[1],0,255,CV_THRESH_OTSU);
//        threshold(HSVImg[2],HSVImg[2],0,255,CV_THRESH_OTSU);
//        imshow("imageH",HSVImg[0]);
//        imshow("imageS",HSVImg[1]);
//        imshow("imageV",HSVImg[2]);

        cv::Mat codeImgSmall = codeImgBig(codeRect);
        double alpha = rec_code_alpha, beta = rec_code_beta;
        cv::Mat codeImgPrc = Mat::zeros(codeImgSmall.size(), codeImgSmall.type());
        //执行运算 new_image(i,j) = alpha*image(i,j) + beta
        for (int y = 0; y < codeImgSmall.rows; y++) {
            for (int x = 0; x < codeImgSmall.cols; x++) {
                for (int c = 0; c < 3; c++) {
                    codeImgPrc.at<Vec3b>(y, x)[c] = saturate_cast<uchar>(alpha * (codeImgSmall.at<Vec3b>(y, x)[c]) + beta);
                }
            }
        }

#ifdef DEBUG_CODENUM_IMG
        namedWindow("sharpCodeNum", WINDOW_NORMAL);
        imshow("sharpCodeNum", codeImgPrc);
#endif
        //preprocess for find singleNum rects
//        cout << "codeImgSmall.cols  " << codeImgSmall.cols << endl;
        if(codeImgSmall.cols > 120) {
            boxFilter(codeImgPrc, codeImgPrc, -1, Size(3, 3));
            //imshow("boxFilterCodeNum", codeImgPrc);
        }

        cvtColor(codeImgPrc, codeImgPrc, COLOR_BGR2GRAY);
        /***
        //for blurred camera
        //cv::Mat  codeImgPrc0= codeImgPrc.clone();
        cv::Mat  codeImgPrc1= codeImgPrc.clone();
        cv::Mat element1 = getStructuringElement(MORPH_RECT, Size(3, 3));
        morphologyEx(codeImgPrc1, codeImgPrc1, CV_MOP_CLOSE, element1, Point(-1,-1), 1);
        dilate(codeImgPrc1, codeImgPrc1, element1, Point(-1,-1), 1);
        erode(codeImgPrc1, codeImgPrc1, element1, Point(-1,-1), 1);
        erode(codeImgPrc1, codeImgPrc1, element1, Point(-1,-1), 1);
        vector<vector<Point>> contours1;
        vector<Vec4i> hierarchy1;
        findContours(codeImgPrc1, contours1, hierarchy1, RETR_LIST, CHAIN_APPROX_SIMPLE);
        for(auto i=0;i<contours1.size();i++){

                drawContours(codeImgPrc1,contours1,i,Scalar(255,255,0),2,CV_FILLED);

        }
        namedWindow("codeImgPrc11", WINDOW_NORMAL);
        imshow("codeImgPrc11", codeImgPrc1);
        cout<<"contours1.size()  =  "<<contours1.size()<<endl;
        cv::Mat  codeImgPrc2= codeImgPrc.clone();
        cv::Mat element2 = getStructuringElement(MORPH_RECT, Size(2, 2));
        morphologyEx(codeImgPrc2, codeImgPrc2, CV_MOP_CLOSE, element1, Point(-1,-1), 1);
        dilate(codeImgPrc2, codeImgPrc2, element2, Point(-1,-1), 1);
        erode(codeImgPrc2, codeImgPrc2, element2, Point(-1,-1), 1);
        vector<vector<Point>> contours2;
        vector<Vec4i> hierarchy2;
        findContours(codeImgPrc2, contours2, hierarchy2, RETR_LIST, CHAIN_APPROX_SIMPLE);

        namedWindow("codeImgPrc22", WINDOW_NORMAL);
        imshow("codeImgPrc22", codeImgPrc2);
        cout<<"contours2.size()  =  "<<contours2.size()<<endl;

        //threshold(codeImgPrc, codeImgPrc, 0, 255, THRESH_OTSU);
        threshold(codeImgPrc, codeImgPrc, 70, 254, CV_THRESH_BINARY);
        vector<vector<Point>> contours;
        vector<Vec4i> hierarchy;
        findContours(codeImgPrc, contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);
        cout<<"contours0.size()  =  "<<contours.size()<<endl;
        if(contours.size()==5)
        {
            cout<<"success"<<endl;
        }
        erode(codeImgPrc, codeImgPrc, element2, Point(-1,-1), 1);
        ***/
//        //for blurred camera
//        cv::Mat  codeImgPrc0= codeImgPrc.clone();
//        vector<vector<Point>> contours0;
//        vector<Vec4i> hierarchy0;
//        findContours(codeImgPrc0, contours0, hierarchy0, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
//        namedWindow("codeImgPrc0", WINDOW_NORMAL);
//        imshow("codeImgPrc0", codeImgPrc0);
//        cout<<"contours0.size()  =  "<<contours0.size()<<endl;
//        if((contours0.size()>=5)&&(contours0.size()<=7))
//        {
//            threshold(codeImgPrc, codeImgPrc, 0, 255, THRESH_OTSU);
//        }else if(contours0.size()>7){
//
//            boxFilter(codeImgPrc, codeImgPrc, -1, Size(3, 3));
//            threshold(codeImgPrc, codeImgPrc, 0, 255, THRESH_OTSU);
//
//        }else{
//            cv::Mat  codeImgPrc2= codeImgPrc.clone();
//            threshold(codeImgPrc2, codeImgPrc2, 70, 254, CV_THRESH_BINARY);
//            vector<vector<Point>> contours2;
//            vector<Vec4i> hierarchy2;
//            findContours(codeImgPrc2, contours2, hierarchy2, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
//            namedWindow("codeImgPrc22", WINDOW_NORMAL);
//            imshow("codeImgPrc22", codeImgPrc2);
//            cout<<"contours2.size()  =  "<<contours2.size()<<endl;
//            if((contours2.size()>=5)&&(contours2.size()<=7))
//            {
//                threshold(codeImgPrc, codeImgPrc, 70, 254, CV_THRESH_BINARY);
//            }else if(contours2.size()>7)
//            {
//                threshold(codeImgPrc, codeImgPrc, 30, 254, CV_THRESH_BINARY);
//            }else{
//                threshold(codeImgPrc, codeImgPrc, 90, 254, CV_THRESH_BINARY);
//            }
//        }
        //for blurred or thin number
        cv::Mat  codeImgPrc0= codeImgPrc.clone();
        vector<vector<Point>> contours0;
        vector<Vec4i> hierarchy0;
        threshold(codeImgPrc0, codeImgPrc0, 0, 255, THRESH_OTSU);
        findContours(codeImgPrc0, contours0, hierarchy0, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
        //namedWindow("codeImgPrc0", WINDOW_NORMAL);
        //imshow("codeImgPrc0", codeImgPrc0);
        cout<<"contours0.size()  =  "<<contours0.size()<<endl;
        if((contours0.size()==5))
        {
            threshold(codeImgPrc, codeImgPrc, 0, 255, THRESH_OTSU);

        }else if(contours0.size()>5){
            cv::Mat  codeImgPrc1= codeImgPrc.clone();
            boxFilter(codeImgPrc1, codeImgPrc1, -1, Size(3, 3));
            threshold(codeImgPrc1, codeImgPrc1, 0, 255, THRESH_OTSU);
            vector<vector<Point>> contours1;
            vector<Vec4i> hierarchy1;
            findContours(codeImgPrc1, contours1, hierarchy1, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
            //namedWindow("codeImgPrc11", WINDOW_NORMAL);
            //imshow("codeImgPrc11", codeImgPrc1);
            cout<<"contours1.size()  =  "<<contours1.size()<<endl;
            if((contours1.size()>=5)&&(contours1.size()<=7))
            {
                boxFilter(codeImgPrc, codeImgPrc, -1, Size(3, 3));
                threshold(codeImgPrc, codeImgPrc, 0, 255, THRESH_OTSU);
            }else if(contours1.size()>7)
            {
                boxFilter(codeImgPrc, codeImgPrc, -1, Size(5, 5));
                threshold(codeImgPrc, codeImgPrc, 0, 255, THRESH_OTSU);
            }else{
                boxFilter(codeImgPrc, codeImgPrc, -1, Size(2, 2));
                threshold(codeImgPrc, codeImgPrc, 0, 255, THRESH_OTSU);
            }

        }else{
            cv::Mat  codeImgPrc2= codeImgPrc.clone();
            threshold(codeImgPrc2, codeImgPrc2, 70, 254, CV_THRESH_BINARY);
            vector<vector<Point>> contours2;
            vector<Vec4i> hierarchy2;
            findContours(codeImgPrc2, contours2, hierarchy2, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
            //namedWindow("codeImgPrc22", WINDOW_NORMAL);
            //imshow("codeImgPrc22", codeImgPrc2);
            cout<<"contours2.size()  =  "<<contours2.size()<<endl;
            if((contours2.size()>=5)&&(contours2.size()<=7))
            {
                threshold(codeImgPrc, codeImgPrc, 70, 254, CV_THRESH_BINARY);
            }else if(contours2.size()>7)
            {
                threshold(codeImgPrc, codeImgPrc, 30, 254, CV_THRESH_BINARY);
            }else{
                threshold(codeImgPrc, codeImgPrc, 90, 254, CV_THRESH_BINARY);
            }
        }
        /***
        //for blurred camera
        cv::Mat  codeImgPrc1= codeImgPrc.clone();
        cv::Mat element1 = getStructuringElement(MORPH_RECT, Size(1, 1));
        morphologyEx(codeImgPrc1, codeImgPrc1, CV_MOP_CLOSE, element1, Point(-1,-1), 1);
        dilate(codeImgPrc1, codeImgPrc1, element1, Point(-1,-1), 1);
        erode(codeImgPrc1, codeImgPrc1, element1, Point(-1,-1), 1);
        vector<vector<Point>> contours1;
        vector<Vec4i> hierarchy1;
        findContours(codeImgPrc1, contours1, hierarchy1, RETR_LIST, CHAIN_APPROX_SIMPLE);
        for(auto i=0;i<contours1.size();i++){

                drawContours(codeImgPrc1,contours1,i,Scalar(255,255,0),2,CV_FILLED);

        }
        namedWindow("codeImgPrc11", WINDOW_NORMAL);
        imshow("codeImgPrc11", codeImgPrc1);
        cout<<"contours1.size()  =  "<<contours1.size()<<endl;
        cv::Mat  codeImgPrc2= codeImgPrc.clone();
        cv::Mat element2 = getStructuringElement(MORPH_RECT, Size(5, 5));
        morphologyEx(codeImgPrc2, codeImgPrc2, CV_MOP_CLOSE, element1, Point(-1,-1), 1);
        dilate(codeImgPrc2, codeImgPrc2, element2, Point(-1,-1), 1);
        erode(codeImgPrc2, codeImgPrc2, element2, Point(-1,-1), 1);
        vector<vector<Point>> contours2;
        vector<Vec4i> hierarchy2;
        findContours(codeImgPrc2, contours2, hierarchy2, RETR_LIST, CHAIN_APPROX_SIMPLE);

        namedWindow("codeImgPrc22", WINDOW_NORMAL);
        imshow("codeImgPrc22", codeImgPrc2);
        cout<<"contours2.size()  =  "<<contours2.size()<<endl;

        if((contours1.size()==5)&&(contours2.size()==5))
        {
            boxFilter(codeImgPrc, codeImgPrc, -1, Size(3, 3));
            threshold(codeImgPrc, codeImgPrc, 0, 255, THRESH_OTSU);
        }else if((contours1.size()>5)||(contours2.size()>5))
        {
            threshold(codeImgPrc, codeImgPrc, 0, 255, THRESH_OTSU);
        }else if((contours1.size()<3)||(contours2.size()<3))
        {
            erode(codeImgPrc, codeImgPrc, element2, Point(-1,-1), 1);

            threshold(codeImgPrc, codeImgPrc, 50, 254, CV_THRESH_BINARY);
        }else{

            threshold(codeImgPrc, codeImgPrc, 30, 254, CV_THRESH_BINARY);
        }
        ***/
        //
        //threshold(codeImgPrc, codeImgPrc, 0, 255, THRESH_OTSU);
//        cv::Mat element = getStructuringElement(MORPH_RECT, Size(element_size, element_size));
//        morphologyEx(codeImgPrc, codeImgPrc, CV_MOP_CLOSE, element, Point(-1,-1), 1);
//        dilate(codeImgPrc, codeImgPrc, element, Point(-1,-1), 1);
//        erode(codeImgPrc, codeImgPrc, element, Point(-1,-1), 1);

#ifdef DEBUG_CODENUM_IMG
        namedWindow("codeImgPrc", WINDOW_NORMAL);
        imshow("codeImgPrc", codeImgPrc);
#endif
        getNumImg0=codeImgPrc.clone();
        vector<int> rowNum(codeImgPrc.rows, 0);
        for(int i = 0; i < codeImgPrc.rows; i++){
            for(int j = 0; j < codeImgPrc.cols; j++){
                int val = codeImgPrc.at<uchar>(i,j);
                if(val == 255)
                    rowNum[i]++;
            }
//            cout << "rowNum: " << rowNum[i] << endl;
        }

        vector<int> rowID;
        rowID.clear();

        for(int i = 1; i < rowNum.size()-1; i++) {
            if(rowNum[i]) {
                if ((rowNum[i-1]==0) || (rowNum[i+1]==0)) {
//                    cout << "rowID: " << i << endl;
                    rowID.push_back(i);
                }
            }
        }

        if(rowID.size() < 2){
            cout << "NumDecoder: numHeight is less than 2" << endl;
//            waitKey(0);
        }

        int numHeight = 0;
        if (rowID.size() == 2){
            numHeight = rowID[1] - rowID[0];
//            cout << "numHeight: " << numHeight << endl;
        }

        if (rowID.size() == 2){
            for(int i = 0; i < codeImgPrc.cols; i++){
                int total = 0;
                int value = 0;
                for(int j = 0; j < codeImgPrc.rows; j++){
                    value = codeImgPrc.at<uchar>(j, i);
                    if (value == 255) total++;
                }

//            cout << "total" << total << endl;
                if ((float)total/numHeight > FILL_RATIO) {
                    for(int j = rowID[0]; j <= rowID[1]; j++){
                        value = codeImgPrc.at<uchar>(j, i);
                        if (value == 0) codeImgPrc.at<uchar>(j, i) = 255;
                    }
                }
            }
        }



#ifdef DEBUG_CODENUM_IMG
        namedWindow("codeImgPrc1", WINDOW_NORMAL);
        imshow("codeImgPrc1", codeImgPrc);
#endif

        vector<Mat> singleNumImgs;
        if(!getNumImg(codeImgPrc, singleNumImgs)) {
            cout << "NumDecoder: can't get singleNumImgs" << endl;
            return false;
        }

        double t2=((double)cv::getTickCount()-t1)*1000/getTickFrequency();

#ifdef DEBUG_CODENUM_TIME
        cout<<"NumDecoder: get single num rect time is: "<<t2<<"ms"<<endl;
        out<<"NumDecoder: get single num rect time is: "<<t2<<"ms"<<endl;
#endif

        //Step3. Decode singleNumImgs to numbers
        t1 = getTickCount();
        if(!codeImg2Num(singleNumImgs, codeNum)){
            cout << "NumDecoder: can't decode Img to Num" << endl;
            return false;
        }

        t2=((double)cv::getTickCount()-t1)*1000/getTickFrequency();
#ifdef DEBUG_CODENUM_TIME
        for (int i = 0; i < codeNum.size(); i++) {
            cout << "Img" << i << " is: " << codeNum[i] <<endl;
        }
        cout<<"NumDecoder: decode img to num time is: "<<t2<<"ms"<<endl;
        out<<"NumDecoder: decode img to num time is: "<<t2<<"ms"<<endl;
        out<<"-------------------------end------------------------------"<<endl;
#endif
        return true;
    }

    //used in getCodeRect function written by jzy
    bool NumDecoder::filterRects(vector<Rect> RectsIn, Size imgSize, float centerX, Rect &RectOut) {
        //step1.按面积过滤
        vector<int> indexAfter, indexTemp;
        double imgArea = imgSize.area();

        cout << "imgArea: " << imgArea << endl;
        for (int i = 0; i < RectsIn.size(); i++) {
            cout << "RectArea: " << RectsIn[i].area() << endl;
            if (RectsIn[i].area() > imgArea * ratio_rect_to_img) {///0.001
                indexAfter.push_back(i);
            }
        }

        indexTemp.assign(indexAfter.begin(), indexAfter.end());///vector 尽量不要用等号相互赋值
        //indexTemp = indexAfter;
        indexAfter.clear();

        //step2.按长宽比过滤
        for (int i = 0; i < indexTemp.size(); i++) {
            float ratio = RectsIn[indexTemp[i]].width / RectsIn[indexTemp[i]].height;
            cout << "ratio: " << ratio << endl;
            if ((ratio > min_width_to_height) && (ratio < max_width_to_height)) {///1.5~6
                indexAfter.push_back(indexTemp[i]);
            }
        }
        indexTemp.assign(indexAfter.begin(), indexAfter.end());
        indexAfter.clear();

        //step3.按中心点过滤
        double minDist = 9999;
        int idealIndex = -1;
        for (int i = 0; i < indexTemp.size(); i++) {
            double thisDist = fabs(RectsIn[indexTemp[i]].x + RectsIn[indexTemp[i]].width / 2 - centerX);
            if (thisDist < minDist) {
                minDist = thisDist;
                idealIndex = indexTemp[i];
            }
        }

        cout << "idealIndex: " << idealIndex << endl;

        if (idealIndex == -1) return false;
        RectOut = RectsIn[idealIndex];
        return true;
    }

    //Thin 轮子 intera 迭代次数
    void NumDecoder::Thin(cv::Mat& src, cv::Mat& dst, int intera) {
        if(src.type()!=CV_8UC1) {
            printf("只能处理二值或灰度图像\n");
            return;
        }
        //非原地操作时候，copy src到dst
        if(dst.data!=src.data) {
            src.copyTo(dst);
        }

        int i, j, n;
        int width, height;
        width = src.cols -1;
        //之所以减1，是方便处理8邻域，防止越界
        height = src.rows -1;
        int step = src.step;
        int  p2,p3,p4,p5,p6,p7,p8,p9;
        uchar* img;
        bool ifEnd;
        int A1;
        cv::Mat tmpimg;
        //n表示迭代次数
        for(n = 0; n<intera; n++) {
            dst.copyTo(tmpimg);
            ifEnd = false;
            img = tmpimg.data;
            for(i = 1; i < height; i++) {
                img += step;
                for(j =1; j<width; j++) {
                    uchar* p = img + j;
                    A1 = 0;
                    if( p[0] > 0) {
                        if(p[-step]==0&&p[-step+1]>0) {//p2,p3 01模式
                            A1++;
                        }
                        if(p[-step+1]==0&&p[1]>0){ //p3,p4 01模式
                            A1++;
                        }
                        if(p[1]==0&&p[step+1]>0){ //p4,p5 01模式
                            A1++;
                        }
                        if(p[step+1]==0&&p[step]>0){ //p5,p6 01模式
                            A1++;
                        }
                        if(p[step]==0&&p[step-1]>0){ //p6,p7 01模式
                            A1++;
                        }
                        if(p[step-1]==0&&p[-1]>0){ //p7,p8 01模式
                            A1++;
                        }
                        if(p[-1]==0&&p[-step-1]>0){ //p8,p9 01模式
                            A1++;
                        }
                        if(p[-step-1]==0&&p[-step]>0){ //p9,p2 01模式
                            A1++;
                        }
                        p2 = p[-step]>0?1:0;
                        p3 = p[-step+1]>0?1:0;
                        p4 = p[1]>0?1:0;
                        p5 = p[step+1]>0?1:0;
                        p6 = p[step]>0?1:0;
                        p7 = p[step-1]>0?1:0;
                        p8 = p[-1]>0?1:0;
                        p9 = p[-step-1]>0?1:0;
                        if((p2+p3+p4+p5+p6+p7+p8+p9)>1 && (p2+p3+p4+p5+p6+p7+p8+p9)<7  &&  A1==1) {
                            if((p2==0||p4==0||p6==0)&&(p4==0||p6==0||p8==0)){ //p2*p4*p6=0 && p4*p6*p8==0
                                dst.at<uchar>(i,j) = 0; //满足删除条件，设置当前像素为0
                                ifEnd = true;
                            }
                        }
                    }
                }
            }

            dst.copyTo(tmpimg);
            img = tmpimg.data;
            for(i = 1; i < height; i++) {
                img += step;
                for(j =1; j<width; j++) {
                    A1 = 0;
                    uchar* p = img + j;
                    if( p[0] > 0) {
                        if(p[-step]==0&&p[-step+1]>0){ //p2,p3 01模式
                            A1++;
                        }
                        if(p[-step+1]==0&&p[1]>0){ //p3,p4 01模式
                            A1++;
                        }
                        if(p[1]==0&&p[step+1]>0){ //p4,p5 01模式
                            A1++;
                        }
                        if(p[step+1]==0&&p[step]>0){ //p5,p6 01模式
                            A1++;
                        }
                        if(p[step]==0&&p[step-1]>0){ //p6,p7 01模式
                            A1++;
                        }
                        if(p[step-1]==0&&p[-1]>0){ //p7,p8 01模式
                            A1++;
                        }
                        if(p[-1]==0&&p[-step-1]>0){ //p8,p9 01模式
                            A1++;
                        }
                        if(p[-step-1]==0&&p[-step]>0) { //p9,p2 01模式
                            A1++;
                        }
                        p2 = p[-step]>0?1:0;
                        p3 = p[-step+1]>0?1:0;
                        p4 = p[1]>0?1:0;
                        p5 = p[step+1]>0?1:0;
                        p6 = p[step]>0?1:0;
                        p7 = p[step-1]>0?1:0;
                        p8 = p[-1]>0?1:0;
                        p9 = p[-step-1]>0?1:0;
                        if((p2+p3+p4+p5+p6+p7+p8+p9)>1 && (p2+p3+p4+p5+p6+p7+p8+p9)<7  &&  A1==1) {
                            if((p2==0||p4==0||p8==0)&&(p2==0||p6==0||p8==0)){ //p2*p4*p8=0 && p2*p6*p8==0
                                dst.at<uchar>(i,j) = 0; //满足删除条件，设置当前像素为0
                                ifEnd = true;
                            }
                        }
                    }
                }
            }

            //如果两个子迭代已经没有可以细化的像素了，则退出迭代
            if(!ifEnd) break;
        }
    }

    bool NumDecoder::getSmallRects(cv::Mat& frame, std::vector<cv::Mat> &singleNumImgs){
        Mat codeImg = frame.clone();
        double t1 = getTickCount();
        cout << "frame size： " << frame.size() << endl;
//        boxFilter(codeImg, codeImg, -1, Size(3, 3));
        cvtColor(codeImg, codeImg, COLOR_BGR2GRAY);
        threshold(codeImg, codeImg, 128, 255, THRESH_BINARY);
        cv::Mat codeImgPrc;
        cv::Mat element = getStructuringElement(MORPH_RECT, Size(2, 2));
//////        morphologyEx(gray, gray, CV_MOP_OPEN, element);
        morphologyEx(codeImg, codeImgPrc, CV_MOP_CLOSE, element);
        namedWindow("codeImgPrc",WINDOW_NORMAL);
//    imshow("codeImgPrc", codeImgPrc);
        cout << "codeImgPrc size： " << codeImgPrc.size() << endl;

        vector<int> colNum(codeImgPrc.cols, 0), rowNum(codeImgPrc.rows, 0);
        for(int i = 0; i < codeImgPrc.cols; i++){
            for (int j = 0; j < codeImgPrc.rows; j++){
                int val = codeImgPrc.at<uchar>(j,i);
                if (val == 255)
                    colNum[i]++;
            }
//            cout << "colNum: " << colNum[i] << endl;
        }

        for(int i = 0; i < codeImgPrc.rows; i++){
            for(int j = 0; j < codeImgPrc.cols; j++){
                int val = codeImgPrc.at<uchar>(i,j);
                if(val == 255)
                    rowNum[i]++;
            }
//            cout << "rowNum: " << rowNum[i] << endl;
        }

        vector<int> colID, rowID;
        for(int i = 1; i < colNum.size()-1; i++) {
            if(colNum[i]) {
                if ((colNum[i-1]==0) || (colNum[i+1]==0)) {
//                    cout << "colID: " << i << endl;
                    colID.push_back(i);
                }
            }
        }

        for(int i = 1; i < rowNum.size()-1; i++) {
            if(rowNum[i]) {
                if ((rowNum[i-1]==0) || (rowNum[i+1]==0)) {
//                    cout << "rowID: " << i << endl;
                    rowID.push_back(i);
                }
            }
        }


        int numHeight = 0;
        if (rowID.size() > 1){
            singleNumHeight = rowID[1] - rowID[0];
//            cout << "singleNumHeight: " << numHeight << endl;
        }

        vector<Rect> Rects;
        int n = 0;
        for(int i = 0; i < colID.size(); i = i+2){
            Rect rectTmp;
            rectTmp.x = colID[i]-2;
            rectTmp.y = rowID[0]-1;
            rectTmp.height = singleNumHeight+2;
            rectTmp.width = colID[i+1]-colID[i]+2;
            Rects.push_back(rectTmp);
//            cout << "rect width" << rectTmp.width << endl;
//            if(rectTmp.width > 8){
                singleNumWidth += rectTmp.width;
//                n++;
//            }

        }
        singleNumWidth = singleNumWidth/Rects.size();
        singleNumArea = singleNumHeight*singleNumWidth;
        cv::Mat rectsImg =  frame.clone();
        for (int i = 0; i < Rects.size(); i++){
            rectangle(rectsImg,Rects[i],Scalar(0,255,0));
        }
        cout << "Rects size: " << Rects.size() << endl;
        namedWindow("Rects",WINDOW_NORMAL);
        imshow("Rects", rectsImg);

        singleNumImgs.clear();
        cv::Mat finalRectsImg = codeImgBig(codeRect).clone();
        for(int i = 0; i < Rects.size(); i ++) {
            singleNumImgs.push_back(finalRectsImg(Rects[i]).clone());
//            ostringstream oss;
//            oss<< i << ".jpg"<<endl;
//            imshow(oss.str(), finalRectsImg(RectsOut[i]).clone());
//            imwrite(oss.str(), singleNumImgs[i]);
        }

        double t2 = (getTickCount() - t1)*1000/getTickFrequency();
        std::cout << "get smallRect time is: " << t2 << "ms" << endl;
        return true;
    }
}



