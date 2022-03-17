//
// Created by sibylgao on 18-11-16.
//
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/core/core.hpp>
#include <iostream>
#include <sys/time.h>
#include "Armor.h"
#include "LightBar.h"
using namespace cv;
using namespace std;
//#define  _DEBUG

bool Armor::parallel(LightBar bar1, LightBar bar2)
{
    double product = fabs(bar1.line_6f[0] * bar2.line_6f[1] - bar1.line_6f[1] * bar2.line_6f[0]);
    if(product > sin(5.0 / 180 * 3.1415926))
        return false;
    double length1 = bar1.line_6f[4];
    double length2 = bar2.line_6f[4];
    if(length1 < length2 * 0.6 || length2 < length1 * 0.6)      //两者大致相等
        return false;
    double length = (length1 + length2) / 2;
    double scale = fabs(log10(length1 / length2));
    double distance = sqrt(pow(bar1.line_6f[2] - bar2.line_6f[2], 2) + pow(bar1.line_6f[3] - bar2.line_6f[3], 2));
    if(fabs(bar1.line_6f[2] - bar2.line_6f[2]) / distance < 0.9) //cos(theta) = 0.5 即两个灯条之间的距离基本上都是由横向确定的
        return false;

    if(distance > 6.0*length || distance < 0.5*length)
        return false;
    if(scale > 0.3)
        return false;
    return true;
}

void Armor::gray(cv::Mat &_frame){
#ifdef _DEBUG
#endif
    int height = _frame.rows;
    int width = _frame.cols;
    Mat img = _frame.clone();
    std::vector<Mat> channels;
    split(_frame, channels);
    img_Enermy = channels.at(2);   /////红色通道
    img_Self = channels.at(0);
    Mat _mask = img_Self.clone();
    threshold(_mask, _mask, GRAY_THRESHOLD, 255, THRESH_BINARY);
    gray_img = _mask;
}


void Armor::LightBarDetect(cv::Mat &_frame,Armor &armor_tmp){

    GRAY_THRESHOLD = 150;
    BAR_POINTS_MIN =3;

    int height = _frame.rows;
    int width = _frame.cols;
    Mat img = _frame.clone();
    std::vector<Mat> channels;
    split(_frame, channels);
    img_Enermy = channels.at(2);   /////红色通道
    img_Self = channels.at(0);

    Mat _mask = img_Self.clone();
//    namedWindow("i");
//    imshow("red",img_Enermy);
//    namedWindow("blue");
//    imshow("blue",img_Self);
    Mat con_mask=_frame.clone();
//    Mat _mask = img_Enermy.clone();
    threshold(_mask, _mask, GRAY_THRESHOLD, 255, THRESH_BINARY);

    namedWindow("_mask");
    imshow("_mask",_mask);
    waitKey(1);
//    dilate(_mask, _mask, getStructuringElement(MORPH_RECT, Size(3, 3)));
//    vector<vector<Point2f>> contours;
    vector<vector<Point>> contours;
    cv::findContours(_mask, contours, RETR_LIST, CHAIN_APPROX_NONE);      ////////这里有点问题
    drawContours(con_mask,contours,-1,cv::Scalar(0, 255, 255),2,8);
    namedWindow("con_mask");
    imshow("con_mask",con_mask);
    waitKey(1);

    if (contours.size() < 2 || contours.size() > 200) {
        cout << "countour's number wrong!!\n";
    }
    std::vector<LightBar> bars;
    ostringstream debug;
    //////////////////////////////////////////////////////////////////////////////////////////
    for (int i = 0; i < contours.size(); ++i){
        LightBar bar;
        bar.m_contour = contours[i];
        bar.m_contourArea = cv::contourArea(contours[i]);
        namedWindow("error");
        cv::Mat img_debug=_frame.clone();
        if (bar.m_contourArea > 1100 || bar.m_contourArea < 300){            ////////600 and 2
#ifdef _DEBUG
            drawContours(img_debug,contours,i,cv::Scalar(0, 255, 255),2,8);
            imshow("error",img_debug);
            waitKey(0);
            debug << i;
            cout<<"the  "+debug.str()+"'s contourArea is wrong!!\n";
            while (1){
                char key_board = waitKey(10);
                if(key_board == 'n'){
                    break;
                }
            }
#endif
            continue;
        }

        bar.m_rotateRect = minAreaRect(contours[i]);
        float length = max(bar.m_rotateRect.size.height, bar.m_rotateRect.size.width);
        float thick = min(bar.m_rotateRect.size.height, bar.m_rotateRect.size.width);
        if (thick > 0.5 * length || thick < 0.1 * length){    ///0.3            //////这里参数有点问题
#ifdef _DEBUG
            drawContours(img_debug,contours,i,cv::Scalar(0, 255, 255),2,8);
            imshow("error",img_debug);
            debug << i;
            cout<<"the "+debug.str()+"'s width and length is wrong!!\n";
            while (1){
                char key_board = waitKey(10);
                if(key_board == 'n'){
                    break;
                }
            }
#endif
            continue;
        }

        Rect rect = boundingRect(contours[i]);
        Point TL_P = rect.tl();
        Point BR_P = rect.br();
        int X_min =  TL_P.x;
        int Y_min =  TL_P.y;
        int X_max =  BR_P.x;
        int Y_max =  BR_P.y;

        float color_pts_num=0;
        for (int j= Y_min; j < Y_max; j++)
        {
            //uchar* p_mask = mask.ptr<uchar>(j);
            uchar* p_Enermy = img_Enermy.ptr<uchar>(j);
            uchar* p_Self = img_Self.ptr<uchar>(j);
            for(int k = X_min;k < X_max; k++)
            {
                if(p_Enermy[k]>=GRAY_THRESHOLD)
                {
                    Point2f  point_if(k,j);
                    double is_in_contour = pointPolygonTest(contours[i], point_if, false);
                    if (is_in_contour >= 0)
                    {
                        bar.m_insidePoints.push_back(point_if);
                        if((p_Enermy[k]-p_Self[k])>=COLOR_DIFFERENCE)
                        {
                            color_pts_num++;
                        }
                    }
                }
            }
        }
//        cout<<"color_pts_num"<<endl;





//        bar.m_color_ratio = color_pts_num / bar.m_contourArea;       ///符合颜色的像素点数目和轮廓面积之比，其实这里没看懂
//        if(bar.m_color_ratio < 0.6){
//#ifdef _DEBUG
//            drawContours(img_debug,contours,i,cv::Scalar(0, 255, 255),2,8);
//            imshow("error",img_debug);
//            debug << i;
//            cout<<"the "+debug.str()+"'s color_ratio is wrong!!\n";
//            while (1){
//                char key_board = waitKey(10);
//                if(key_board == 'n'){
//                    break;
//                }
//            }
//#endif
//            continue;
//        }





        if (bar.m_insidePoints.size() < BAR_POINTS_MIN){
#ifdef _DEBUG
            drawContours(img_debug,contours,i,cv::Scalar(0, 255, 255),2,8);
            imshow("error",img_debug);
            debug << i;
            cout<<"the "+debug.str()+"'s bar_point is too samall!!\n";
            while (1){
                char key_board = waitKey(10);
                if(key_board == 'n'){
                    break;
                }
            }
#endif
            continue;
        }

        Vec4f line_4f_tmp;
        fitLine(bar.m_insidePoints, line_4f_tmp, CV_DIST_L2, 0, 0.01, 0.01);


        Point2f P1;                                 /////////////////////////////////////////拟合直线显示程序
        P1.x=line_4f_tmp[2];
        P1.y=line_4f_tmp[3];
        bar.m_cos = line_4f_tmp[0];
        bar.m_sin = line_4f_tmp[1];
        float k= bar.m_sin/bar.m_cos;
        Point2f P2,P3;
        P2.x=0;
        P2.y=k*(0-P1.x)+P1.y;
        P3.x=480;
        P3.y=k*(480-P1.x)+P1.y;
        cv::line(_frame, P2, P3, cv::Scalar(0, 255, 0), 1, 8, 0);
        drawContours(_frame,contours,i,cv::Scalar(0, 255, 255),2,8);
        namedWindow("line");
        imshow("line",_frame);


        bar.m_length = length;
        bar.m_center = Point2f(line_4f_tmp[2], line_4f_tmp[3]);
        float upper = 10000, lower = -1;

        for (int j = 0; j < bar.m_contour.size(); ++j)             /////////////////这里感觉写反了所以改过，但是好像改错类后面图像反透视变换之后是反的
        {
            if(bar.m_contour[j].y < upper)
            {
                upper = bar.m_contour[j].y;
                bar.m_bottom = bar.m_contour[j];
            }
            if(bar.m_contour[j].y > lower)
            {
                lower = bar.m_contour[j].y;
                bar.m_upper = bar.m_contour[j];
            }
        }
#ifdef _DEBUG
        while (1){
            char key_board = waitKey(10);
            if(key_board == 'b'){
                break;
            }
        }
#endif
        bars.push_back(bar);
    }
    ///////////////////////////////////////////////////////////////////////////////////
    if(bars.size() < 2){
        cout<<"bars number wrong!\n"<<endl;
    }

    vector<int> labels;
    partition(bars, labels,parallel);                               //////////parallel这里忘记给line_6f赋值。。。

    int search_flag[20] = { 0 };
    for (int i = 0; i < labels.size(); i++) {
        if (search_flag[i])continue;
        int sum = 0;
        int index[2] = {-1};
        for (int j = 0; j < labels.size(); j++) {
            if (labels[i] == labels[j]) {
                sum++;
                if(sum==2){
                    index[0]=j;
                }
                if(sum==3){
                    index[1]=j;
                }
                search_flag[j] = 1;
            }
        }
        if (sum == 2) {
//            Armor armor_tmp;
            if (bars[i].m_center.x > bars[index[0]].m_center.x) {
                armor_tmp.m_leftBar = bars[index[0]];
                armor_tmp.m_rightBar = bars[i];
            } else {
                armor_tmp.m_leftBar = bars[i];
                armor_tmp.m_rightBar = bars[index[0]];
            }

            armor_tmp.m_upper_left = armor_tmp.m_leftBar.m_upper;
            armor_tmp.m_bottom_left = armor_tmp.m_leftBar.m_bottom;
            armor_tmp.m_upper_right = armor_tmp.m_rightBar.m_upper;
            armor_tmp.m_bottom_right = armor_tmp.m_rightBar.m_bottom;

//            _armors.push_back(armor_tmp);
        }
        if(sum==3){
            ///平行线平行度最高的聚为一类

            //cv::sortIdx()
            double product1 = fabs(bars[i].m_cos * bars[index[0]].m_sin - bars[i].m_sin * bars[index[0]].m_cos);
            double product2 = fabs(bars[i].m_cos * bars[index[1]].m_sin - bars[i].m_sin * bars[index[1]].m_cos);
            double product3 = fabs(bars[index[0]].m_cos * bars[index[1]].m_sin - bars[index[0]].m_sin * bars[index[1]].m_cos);
            vector<double> product{product1,product2,product3};
            sort(product.begin(),product.end());///从小到大排序
            int id[2]={-1};
            if(fabs(product[0]-product1)<1e-6){
                id[0]=i;
                id[1]=index[0];
            }
            else if(fabs(product[0]-product2)<1e-6){
                id[0]=i;
                id[1]=index[1];
            }
            else if(fabs(product[0]-product3)<1e-6){
                id[0]=index[0];
                id[1]=index[1];
            }
//            Armor armor_tmp;
            if (bars[id[0]].m_center.x > bars[id[1]].m_center.x) {
                armor_tmp.m_leftBar = bars[id[1]];
                armor_tmp.m_rightBar = bars[id[0]];
            } else {
                armor_tmp.m_leftBar = bars[id[0]];
                armor_tmp.m_rightBar = bars[id[1]];
            }

            armor_tmp.m_upper_left = armor_tmp.m_leftBar.m_upper;
            armor_tmp.m_bottom_left = armor_tmp.m_leftBar.m_bottom;
            armor_tmp.m_upper_right = armor_tmp.m_rightBar.m_upper;
            armor_tmp.m_bottom_right = armor_tmp.m_rightBar.m_bottom;

//            _armors.push_back(armor_tmp);
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////
    infantry_height=55;
    infantry_width=135;
    Mat frame_copy = _frame.clone();

    std::vector<cv::Point2f> dst_pt;
//    dst_pt.push_back(Point2f(0,35));
//    dst_pt.push_back(Point2f(135,35));
//    dst_pt.push_back(Point2f(0,90));
//    dst_pt.push_back(Point2f(135, 90));
    dst_pt.push_back(Point2f(0,90));
    dst_pt.push_back(Point2f(135, 90));
    dst_pt.push_back(Point2f(0,35));
    dst_pt.push_back(Point2f(135,35));

    Size temsize = Size(135,125);
    Rect imgRect=Rect(25,5,85,115);

    std::vector<Point2f> points_ori;
    points_ori.push_back(Point2f(67.5,62.5));    ///正视图中装甲中心的坐标

    cv::Mat transmtx;       ///从原图到正视图的透视变换矩阵
    cv::Mat transmtxInv;    ///从正视图到原图的透视变换矩阵

    vector<Point3f> objectPoints_infantry;
    vector<Point3f> objectPoints_hero;

    objectPoints_infantry.push_back(Point3f(-0.5*Armor::infantry_width,0.5*Armor::infantry_height,0));
    objectPoints_infantry.push_back(Point3f(0.5*Armor::infantry_width,0.5*Armor::infantry_height,0));
    objectPoints_infantry.push_back(Point3f(-0.5*Armor::infantry_width,-0.5*Armor::infantry_height,0));
    objectPoints_infantry.push_back(Point3f(0.5*Armor::infantry_width,-0.5*Armor::infantry_height,0));

//    objectPoints_hero.push_back(Point3f(-0.5*Armor::hero_width,0.5*Armor::hero_height,0));
//    objectPoints_hero.push_back(Point3f(0.5*Armor::hero_width,0.5*Armor::hero_height,0));
//    objectPoints_hero.push_back(Point3f(-0.5*Armor::hero_width,-0.5*Armor::hero_height,0));
//    objectPoints_hero.push_back(Point3f(0.5*Armor::hero_width,-0.5*Armor::hero_height,0));
    std::vector<cv::Point2f> corners;
    corners.push_back(armor_tmp.m_upper_left);
    corners.push_back(armor_tmp.m_upper_right);
    corners.push_back(armor_tmp.m_bottom_left);
    corners.push_back(armor_tmp.m_bottom_right);
    std::vector<Point2f> points_trans;


    transmtxInv = cv::getPerspectiveTransform(dst_pt, corners);
//            cout << "transtxInv=" << transmtxInv << endl;
    perspectiveTransform(points_ori,points_trans,transmtxInv);

//    namedWindow("")
    armor_tmp.m_center_uv = points_trans.front();             ///利用透视变换求原图中装甲中心点的坐标

//            cout<< "transformed x =" <<points_trans.front().x <<endl;
//            cout<< "transformed y =" <<points_trans.front().y <<endl;


    transmtx = cv::getPerspectiveTransform(corners, dst_pt);

    circle(frame_copy, armor_tmp.m_upper_left, 20, Scalar(0, 255, 0));
    circle(frame_copy, armor_tmp.m_upper_right, 20, Scalar(0, 255, 0));
    circle(frame_copy, armor_tmp.m_bottom_left, 20, Scalar(0, 255, 0));
    circle(frame_copy, armor_tmp.m_bottom_right, 20, Scalar(0, 255, 0));
    namedWindow("5");
    imshow("5",frame_copy);
    waitKey(1);

    cv::Mat img_perspective;
    cv::warpPerspective(frame_copy, img_perspective, transmtx, temsize);

    namedWindow("5");
    imshow("5",frame_copy);
    namedWindow("3");
    imshow("3",img_perspective);
    waitKey(1);

    cv::Mat tagnum = img_perspective(imgRect);

    namedWindow("4");
    imshow("4",tagnum);
    //vector<Mat> channels;
    //split(tagnum,channels);
    //imshow("b",channels.at(0));

    cvtColor(tagnum, tagnum, CV_RGB2GRAY);

    //imshow("taggray",tagnum);
    cv::threshold(tagnum, tagnum,0,255,CV_THRESH_OTSU);
    //imshow("before",tagnum);
    //waitKey(1);
    erode(tagnum,tagnum,getStructuringElement(MORPH_RECT, Size(3,3)));
    dilate(tagnum,tagnum,getStructuringElement(MORPH_RECT, Size(7,7)));
    //imshow("tag",tagnum);
    //waitKey(1);
    cv::Mat tagnum_resize;
    resize(tagnum,tagnum_resize,Size(42,57));
    ROI_img = tagnum_resize;

    namedWindow("tag_resize");
    imshow("tag_resize",tagnum_resize);
    waitKey(1);

    while (1){
        char key_board = waitKey(10);
        if(key_board == 's'){
            break;
        }
    }

//    stringstream imagename;
//    string imagename1;
//    imagename <<i;
//    imagename >> imagename1;
//    string filename = "/home/sibylgao/CLionProjects/img_process/数据集/1/ " + imagename1 + ".jpg";
//    imwrite(filename, tagnum_resize);
}
