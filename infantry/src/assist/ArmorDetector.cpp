#include <sys/time.h>

#include "ArmorDetector.h"
//#define _DEBUG
#define POINT_DIST(p1,p2) std::sqrt((p1.x-p2.x)*(p1.x-p2.x) + (p1.y-p2.y)*(p1.y-p2.y))
//#define SHOW_IMG
using namespace std;
using namespace cv;

namespace hitcrt
{
float ArmorDetector::PARALL_DELTA_THETA;
float ArmorDetector::TWO_LENGTH_SCLAE;
float ArmorDetector::X_DISTANCE_SCALE;
float ArmorDetector::DISTANCE_LENGTH_SCALE_MAX;
float ArmorDetector::DISTANCE_LENGTH_SCALE_MIN;

ArmorDetector::ArmorDetector()
{
    cv::FileStorage fs;
    fs.open("/home/sun/HITCRT/Infantry/data/armor/camin1_6mm_640.yml", cv::FileStorage::READ);
    assert(fs.isOpened());

    fs["camera_matrix"] >> CAMERA_MATRIX;
    fs["distortion_coefficients"] >> DISTORT;
    FOCU_LENGTH = (CAMERA_MATRIX.at<double>(0,0) + CAMERA_MATRIX.at<double>(1,1)) / 2.0;
    fs["points_mask_size_max"] >> POINTS_MASK_SIZE_MAX;
    fs["gray_threshold"] >> GRAY_THRESHOLD;
    fs["contours_size_min"] >> CONTOURS_SIZE_MIN;
    fs["contours_size_max"] >> CONTOURS_SIZE_MAX;
    fs["light_bar_area_max"] >> MAX_LIGHT_BAR_AREA;
    fs["light_bar_area_min"] >> MIN_LIGHT_BAR_AREA;
    fs["breadth_length_scale_min"] >> BREADTH_LENGTH_SCALE_MIN;
    fs["breadth_length_scale_max"] >> BREADTH_LENGTH_SCALE_MAX;
    fs["bar_insidepoints_size_min"] >> BAR_POINTS_MIN;
    fs["bar_slope_min"] >> BAR_SLOPE_MIN;
    fs["max_distance"] >> MAX_DISTANCE;
    fs["parallel_delta_theta"] >> PARALL_DELTA_THETA;
    fs["two_length_scale"] >> TWO_LENGTH_SCLAE;
    fs["deltax_distance_scale"] >> X_DISTANCE_SCALE;
    fs["distance_length_scale_max"] >> DISTANCE_LENGTH_SCALE_MAX;
    fs["distance_length_scale_min"] >> DISTANCE_LENGTH_SCALE_MIN;
    fs["template_ratio"] >> TEMPLATE_RATIO;
    loadTemplate();

}

void ArmorDetector::loadTemplate()
{
    m_hereo_template = imread("/home/sun/HITCRT/Infantry/data/armor/big_template.bmp");
    cv::cvtColor(m_hereo_template, m_hereo_template, CV_BGR2GRAY);
    cv::threshold(m_hereo_template, m_hereo_template, 128, 255, THRESH_OTSU);

    m_infantry_template = imread("/home/sun/HITCRT/Infantry/data/armor/small_template.bmp");
    cv::cvtColor(m_infantry_template, m_infantry_template, CV_BGR2GRAY);
    cv::threshold(m_infantry_template, m_infantry_template, 128, 255, THRESH_OTSU);
    cout<<"loadtemplate successed"<<endl;
}
void ArmorDetector::templateMatch(std::vector<Armor> &_armors,cv::Mat& frame)
{
    Mat _frame=frame.clone();
    //cout<<"frame_size is "<<_frame.size()<<endl;

    for(std::vector<Armor>::iterator it = _armors.begin(); it != _armors.end();)
    {
        Armor& armor_tmp = *it;
        std::vector<cv::Point2f> corners;
        corners.push_back(armor_tmp.m_upper_left);
        corners.push_back(armor_tmp.m_upper_right);
        corners.push_back(armor_tmp.m_bottom_left);
        corners.push_back(armor_tmp.m_bottom_right);

        std::vector<cv::Point2f> dst_pt;
        cv::Mat template_img;
        if(armor_tmp.m_type == Armor::INFANTRY)
        {
            template_img = m_infantry_template;
        }
        else
        {
            template_img = m_hereo_template;
        }
//cout<<"armor_type is "<<armor_tmp.m_type<<endl;

        dst_pt.push_back(Point2f(0,0));
        dst_pt.push_back(Point2f(template_img.cols,0));
        dst_pt.push_back(Point2f(0,template_img.rows));
        dst_pt.push_back(Point2f(template_img.cols, template_img.rows));

        cv::Mat transmtx = cv::getPerspectiveTransform(corners, dst_pt);
        //cout<<"transmtx="<<transmtx<<endl;


        cv::Mat img_perspective;
        cv::warpPerspective(_frame, img_perspective, transmtx, template_img.size());
        //cout<<"_frame perspective"<<endl;
        cvtColor(img_perspective, img_perspective, CV_BGR2GRAY);
        cv::threshold(img_perspective, img_perspective, 128, 255, THRESH_OTSU);

        assert(img_perspective.size() == template_img.size());
        //

        int non_zero_num = 0;
        for (int j = 0; j < img_perspective.rows; ++j)
        {
            uchar* p1 = img_perspective.ptr<uchar>(j);
            uchar* p2 = template_img.ptr<uchar>(j);
            for (int k = 0; k < img_perspective.cols; ++k)
            {
                if(p1[k]*p2[k] + (255-p1[k])*(255-p2[k]) > 0)
                    ++non_zero_num;
            }
        }

        float ratio = non_zero_num*1.0 / template_img.cols / template_img.rows;

//            cout << "non_zero_num = " << non_zero_num << endl;
//            cout << "ratio" << ratio << endl;
//cout<<"starting match!"<<endl;
        if(ratio < TEMPLATE_RATIO)
        {
            _armors.erase(it);
            cout<<"do not match!"<<endl;
        }
        else
        {
            ++it;
            //cout<<"match!"<<endl;
        }


//#ifdef _DEBUG
//        imshow("raw", m_frame(imgRect));
//        imshow("perspective", img_perspective);
//        waitKey(1);
//#endif

    }


}



bool ArmorDetector::parallel(LightBar bar1, LightBar bar2)
{
    double product = fabs(bar1.line_6f[0] * bar2.line_6f[1] - bar1.line_6f[1] * bar2.line_6f[0]);
    if(product > sin(PARALL_DELTA_THETA / 180 * 3.1415926))
    {
        //cout<<"平行度不够"<<endl;
        return false;
    }

    double length1 = bar1.line_6f[4];
    double length2 = bar2.line_6f[4];
    if(length1 < length2 * TWO_LENGTH_SCLAE || length2 < length1 * TWO_LENGTH_SCLAE)      //两者大致相等
    {
        //cout<<"灯条长度相差太大"<<endl;
        return false;
    }
    double length = (length1 + length2) / 2;
    double distance = sqrt(pow(bar1.line_6f[2] - bar2.line_6f[2], 2) + pow(bar1.line_6f[3] - bar2.line_6f[3], 2));
//        cout << distance << endl;
//        cout << fabs(bar1.line_6f[2] - bar2.line_6f[2]) << endl;
//        cout << fabs(bar1.line_6f[3] - bar2.line_6f[3]) << endl;
    if(fabs(bar1.line_6f[2] - bar2.line_6f[2]) / distance < X_DISTANCE_SCALE) //cos(theta) = 0.5 即两个灯条之间的距离基本上都是由横向确定的
        return false;

    if(distance > DISTANCE_LENGTH_SCALE_MAX*length || distance < DISTANCE_LENGTH_SCALE_MIN*length)
        return false;

    return true;


}


cv::RotatedRect ArmorDetector::boundingRRect(const cv::RotatedRect & left, const cv::RotatedRect & right)     ///返回一个和装甲目标有关的旋转矩形，用于计算长宽比
{
    const Point & pl = left.center, & pr = right.center;
    Point2f center = (pl + pr) / 2.0;
    cv::Size2f wh_l = left.size;
    cv::Size2f wh_r = right.size;
    wh_l.height = std::max(wh_l.height, wh_l.width);
    wh_r.height = std::max(wh_r.height, wh_r.width);
    wh_l.width = std::min(wh_l.height, wh_l.width);
    wh_r.width = std::min(wh_r.height, wh_r.width);
    float width = POINT_DIST(pl, pr);
    float height = std::min(wh_l.height, wh_r.height);
    float angle = std::atan2(right.center.y - left.center.y, right.center.x - left.center.x);
    return RotatedRect(center, Size2f(width, height), angle * 180 / CV_PI);
}



void ArmorDetector::showImage(std::vector<Armor> &_armors, cv::Mat& _frame)
{
    Scalar color = Scalar(255, 255, 0);
    Mat img = _frame.clone();
    //float length_ratio;

    for (int i = 0; i < _armors.size(); i++)
    {
        circle(img, _armors[i].m_center_uv, 10, color);
//            stringstream ss;
//            ss << i;
//            String num(ss.str());
//            putText(img, num, _armors[i].m_center_uv, FONT_HERSHEY_SIMPLEX, 1, color);
        circle(img, _armors[i].m_upper_left, 7, color);
        circle(img, _armors[i].m_bottom_right, 7, color);
        Point2f vertices[4];      //定义4个点的数组
        _armors[i].m_armor_rect.points(vertices);   //将四个点存储到vertices数组中
        for (int i = 0; i < 4; i++)
            line(img, vertices[i], vertices[(i+1)%4], color);


        stringstream Distance_Infantry, Distance_Hereo, XC, YC, ZC;
        XC << "Xc = " << _armors[i].Xc;
        putText(img, XC.str(), Point(30,30), FONT_HERSHEY_PLAIN, 1, color);
        YC << "Yc = " << _armors[i].Yc;
        putText(img, YC.str(), Point(30,50), FONT_HERSHEY_PLAIN, 1, color);
        ZC << "Zc = " << _armors[i].Zc;
        putText(img, ZC.str(), Point(30,70), FONT_HERSHEY_PLAIN, 1, color);


        if(_armors[i].m_type == Armor::type::INFANTRY)
        {
            Distance_Infantry << "Infantry_distance = " << _armors[i].m_distance  ;
            putText(img,Distance_Infantry.str(), _armors[i].m_center_uv, FONT_HERSHEY_PLAIN, 1, color);
        }
        else
        {
            Distance_Hereo << "Hero_distance = " << _armors[i].m_distance  ;
            putText(img,Distance_Hereo.str(), _armors[i].m_center_uv, FONT_HERSHEY_PLAIN, 1, color);
        }
//            cout << _armors[i].m_leftBar.m_contourArea << endl;
//            cout << _armors[i].m_rightBar.m_contourArea << endl;
    }
    _frame = img.clone();
    cv::Point imgcenterpoint;
    imgcenterpoint.x = img.cols / 2;
    imgcenterpoint.y = img.rows / 2;
    circle(img, imgcenterpoint, 7, Scalar(0, 0, 255));
     #ifdef SHOW_IMG
    imshow("img", img);
    waitKey(1);
    #endif // SHOW_IMG
}



bool ArmorDetector::Apply(std::vector<Armor> &_armors, cv::Mat &_frame, cv::Mat &origin_frame,int _roi_flag, cv::Point2f _roi_origin)
{
    m_frame = _frame;
    Mat originframe =origin_frame;
    m_roi_flag = _roi_flag;
    m_roi_origin = _roi_origin;

    struct timeval st,en;
    gettimeofday(&st,NULL);
    int height = _frame.rows;
    int width = _frame.cols;

    Mat img = _frame.clone();
//    Mat img_R(height, width, CV_8U);
//    Mat img_B(height, width, CV_8U);
    Mat img_gray;


//    std::vector<Mat> channels;
//    split(img,channels);
//    img_B = channels.at(0);
//    img_R = channels.at(2);
//       cvtColor(img, img, CV_RGB2GRAY);
//       vector<Point2f> points_mask;
//    threshold(img_R, img_R, 200, 255, THRESH_BINARY);
//    threshold(img_B, img_B, 200, 255, THRESH_BINARY);
    cvtColor(img, img_gray, CV_BGR2GRAY);
    threshold(img_gray, img_gray, GRAY_THRESHOLD, 255, THRESH_BINARY);
    Mat mask = img_gray.clone();
    Mat mask_debug;
    img_gray.copyTo(mask_debug);
    //imshow("mask",mask);
    //waitKey(1);
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    dilate(mask,mask,cv::Mat());
    // dilate(mask,mask,cv::Mat());
//        erode(mask,mask,cv::Mat());
//        erode(mask,mask,cv::Mat());

    findContours(mask, contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);
    if (contours.size() < CONTOURS_SIZE_MIN || contours.size() > CONTOURS_SIZE_MAX)
    {
#ifdef _DEBUG
        cout<<"coutous size wrong!   =  "<<contours.size()<<endl;
#endif // _DEBUG
        return false;
    }

    std::vector<LightBar> bars;
#ifdef _DEBUG
    cout << "contours_size() = " << contours.size() << endl;

    for (int i = 0; i < contours.size(); ++i)
    {
        //int i =0;
        stringstream Contour_Num;
        Contour_Num << i ;
        putText(mask_debug, Contour_Num.str(), contours[i][0], FONT_HERSHEY_PLAIN, 1, Scalar(255));
    }
    imshow("mask_Debug",mask_debug);
    waitKey(1);

#endif // _DEBUG

    for (int i = 0; i < contours.size(); ++i)
    {
//            gettimeofday(&st,NULL);
        LightBar bar;
        bar.m_contour = contours[i];

        bar.m_contourArea = contourArea(contours[i]);
        if(bar.m_contourArea > MAX_LIGHT_BAR_AREA || bar.m_contourArea < MIN_LIGHT_BAR_AREA)
        {
#ifdef _DEBUG
            cout <<  i << "  countourArea wrong!  and bar. m_contourArea = "<< bar.m_contourArea << endl;
#endif // _DEBUG
            continue;
        }
        bar.m_rotateRect = minAreaRect(contours[i]);

        float length = max(bar.m_rotateRect.size.height, bar.m_rotateRect.size.width);
        float breadth = min(bar.m_rotateRect.size.height, bar.m_rotateRect.size.width);

        if (breadth > length*BREADTH_LENGTH_SCALE_MAX || breadth < length*BREADTH_LENGTH_SCALE_MIN)
        {
#ifdef _DEBUG
            cout << i << "  breadth_length_scale wrong! and breadth = "<< breadth << ", length ="<<  length <<endl;
#endif // _DEBUG
            continue;
        }

        Rect rect = boundingRect(contours[i]);  ///得到包围轮廓的rect

        Point TL_P = rect.tl();
        Point BR_P = rect.br();
        int X_min =  TL_P.x;
        int Y_min =  TL_P.y;
        int X_max =  BR_P.x;
        int Y_max =  BR_P.y;

        for (int j= Y_min; j < Y_max; j++)
        {
            uchar* p_gray = img_gray.ptr<uchar>(j);
            for(int k = X_min; k < X_max; k++)
            {
                if(p_gray[k]>200)
                {

                    Point2f  point_if(k,j);
                    double is_in_contour = pointPolygonTest(contours[i], point_if, false);
                    if (is_in_contour >= 0)
                    {
                        bar.m_insidePoints.push_back(point_if);
                    }
                }
            }
        }


        if (bar.m_insidePoints.size() < BAR_POINTS_MIN)
        {
#ifdef _DEBUG
            cout << i << "  m_insidePoints wrong " <<endl;
#endif // _DEBUG
            continue;
        }
        Vec4f line_4f_tmp;
        fitLine(bar.m_insidePoints, line_4f_tmp, CV_DIST_L2, 0, 0.01, 0.01);

        if (BAR_SLOPE_MIN*fabs(line_4f_tmp[0]) > fabs(line_4f_tmp[1]))//斜率
        {
#ifdef _DEBUG
            cout << i << "  Slope Wrong! "<<endl;
#endif // _DEBUG
            continue;
        }

        bar.line_6f = Vec6f(line_4f_tmp[0], line_4f_tmp[1], line_4f_tmp[2], line_4f_tmp[3], length,0);

        float upper = 10000, lower = -1;
        for (int j = 0; j < bar.m_insidePoints.size(); ++j)
        {
            if(bar.m_insidePoints[j].y < upper)
            {
                upper = bar.m_insidePoints[j].y;
                bar.m_upper = bar.m_insidePoints[j];
            }
            if(bar.m_insidePoints[j].y > lower)
            {
                lower = bar.m_insidePoints[j].y;
                bar.m_bottom = bar.m_insidePoints[j];
            }
        }
#ifdef _DEBUG
        Point pt1(line_4f_tmp[2] + 100 * line_4f_tmp[0], line_4f_tmp[3] + 100 * line_4f_tmp[1]);
        Point pt2(line_4f_tmp[2] - 100 * line_4f_tmp[0], line_4f_tmp[3] - 100 * line_4f_tmp[1]);
        Scalar color = Scalar(255);
        line(mask, pt1, pt2, color);
        rectangle(mask, rect, color);
        imshow("mask",mask);
        waitKey(1);
#endif
        bars.push_back(bar);
    }


    if(bars.size() < 2)
    {
#ifdef _DEBUG
        cout<< "bars_size < 2" << endl;
#endif
        return false;
    }



    vector<int> labels;
    partition(bars, labels,parallel);


    int search_flag[20] = { 0 };
    for (int i = 0; i < labels.size(); i++)
    {
        if (search_flag[i])continue;
        int sum = 0;
        int index[2] = {-1};
        for (int j = 0; j < labels.size(); j++)
        {
            if (labels[i] == labels[j])
            {
                sum++;
                if(sum==2)
                {
                    index[0]=j;
                }
                if(sum==3)
                {
                    index[1]=j;
                }
                search_flag[j] = 1;
            }
        }
        if (sum == 2)
        {
            Armor armor_tmp;
            armor_tmp.m_center_uv = Point2f((bars[i].line_6f[2] + bars[index[0]].line_6f[2]) / 2,
                                            (bars[i].line_6f[3] + bars[index[0]].line_6f[3]) / 2);
            if (bars[i].line_6f[2] > bars[index[0]].line_6f[2])
            {
                armor_tmp.m_leftBar = bars[index[0]];
                armor_tmp.m_rightBar = bars[i];
            }
            else
            {
                armor_tmp.m_leftBar = bars[i];
                armor_tmp.m_rightBar = bars[index[0]];
            }

            armor_tmp.m_upper_left = armor_tmp.m_leftBar.m_upper;
            armor_tmp.m_bottom_left = armor_tmp.m_leftBar.m_bottom;
            armor_tmp.m_upper_right = armor_tmp.m_rightBar.m_upper;
            armor_tmp.m_bottom_right = armor_tmp.m_rightBar.m_bottom;

            if(m_roi_flag == 1)             ///如果此时处理的图像取了ROI，需要补偿装甲坐标点数值，保证solvepnp结算距离不出错
            {
                armor_tmp.m_center_uv += m_roi_origin;
                armor_tmp.m_upper_left += m_roi_origin;
                armor_tmp.m_bottom_left += m_roi_origin;
                armor_tmp.m_upper_right += m_roi_origin;
                armor_tmp.m_bottom_right += m_roi_origin;
                armor_tmp.m_leftBar.m_rotateRect.center += m_roi_origin;
                armor_tmp.m_rightBar.m_rotateRect.center += m_roi_origin;
            }

            ///计算装甲长宽比
            armor_tmp.m_armor_rect = boundingRRect(armor_tmp.m_leftBar.m_rotateRect,armor_tmp.m_rightBar.m_rotateRect);
            armor_tmp.m_armor_width=armor_tmp.m_armor_rect.size.width;
            armor_tmp.m_armor_height=armor_tmp.m_armor_rect.size.height;
            armor_tmp.m_wh_ratio = armor_tmp.m_armor_width/armor_tmp.m_armor_height;

            ///calculate length_ratio
            if(armor_tmp.m_leftBar.line_6f[4] > armor_tmp.m_rightBar.line_6f[4])
                armor_tmp.m_bar_length_ratio = armor_tmp.m_leftBar.line_6f[4] /armor_tmp.m_rightBar.line_6f[4] ;
            else
                armor_tmp.m_bar_length_ratio = armor_tmp.m_rightBar.line_6f[4] /armor_tmp.m_leftBar.line_6f[4] ;



            armor_tmp.calculateRT(CAMERA_MATRIX, DISTORT);
            if(armor_tmp.m_distance >= 3000)
                armor_tmp.m_distance = armor_tmp.m_distance - ((armor_tmp.m_distance-3100)*0.21+100)*((armor_tmp.m_bar_length_ratio-1.02)/0.18);

            if(armor_tmp.m_distance < MAX_DISTANCE)
            {
                _armors.push_back(armor_tmp);
            }

        }
        if(sum==3)
        {
            ///平行线平行度最高
            double product1 = fabs(bars[i].line_6f[0] * bars[index[0]].line_6f[1] - bars[i].line_6f[1] * bars[index[0]].line_6f[0]);
            double product2 = fabs(bars[i].line_6f[0] * bars[index[1]].line_6f[1] - bars[i].line_6f[1] * bars[index[1]].line_6f[0]);
            double product3 = fabs(bars[index[0]].line_6f[0] * bars[index[1]].line_6f[1] - bars[index[0]].line_6f[1] * bars[index[1]].line_6f[0]);
            vector<double> product{product1,product2,product3};
            sort(product.begin(),product.end());///从小到大排序
            int id[2]= {-1};
            if(fabs(product[0]-product1)<1e-6)
            {
                id[0]=i;
                id[1]=index[0];
            }
            else if(fabs(product[0]-product2)<1e-6)
            {
                id[0]=i;
                id[1]=index[1];
            }
            else if(fabs(product[0]-product3)<1e-6)
            {
                id[0]=index[0];
                id[1]=index[1];
            }

            ///平行线距离最近
            double distance1 = sqrt(pow(bars[i].line_6f[2] - bars[index[0]].line_6f[2], 2) + pow(bars[i].line_6f[3] - bars[index[0]].line_6f[3], 2));
            double distance2 = sqrt(pow(bars[i].line_6f[2] - bars[index[1]].line_6f[2], 2) + pow(bars[i].line_6f[3] - bars[index[1]].line_6f[3], 2));
            double distance3 = sqrt(pow(bars[index[0]].line_6f[2] - bars[index[1]].line_6f[2], 2) + pow(bars[index[0]].line_6f[3] - bars[index[1]].line_6f[3], 2));
            vector<double> distance{distance1,distance2,distance3};
            sort(distance.begin(),distance.end());
            int id_distance[2]= {-1};
            if(fabs(distance[0]-distance1)<1e-6)
            {
                id_distance[0]=i;
                id_distance[1]=index[0];
            }
            else if(fabs(distance[0]-distance2)<1e-6)
            {
                id_distance[0]=i;
                id_distance[1]=index[1];
            }
            else if(fabs(distance[0]-distance3)<1e-6)
            {
                id_distance[0]=index[0];
                id_distance[1]=index[1];
            }

            if(!(id[0]==id_distance[0]&&id[1]==id_distance[1]))
            {
#ifdef _DEBUG
                cout<< "  The two bars which are closer are not most parallel" << endl;
#endif
                continue;
            }


            Armor armor_tmp;
            armor_tmp.m_center_uv = Point2f((bars[id[0]].line_6f[2] + bars[id[1]].line_6f[2]) / 2,
                                            (bars[id[0]].line_6f[3] + bars[id[1]].line_6f[3]) / 2);
            if (bars[id[0]].line_6f[2] > bars[id[1]].line_6f[2])
            {
                armor_tmp.m_leftBar = bars[id[1]];
                armor_tmp.m_rightBar = bars[id[0]];
            }
            else
            {
                armor_tmp.m_leftBar = bars[id[0]];
                armor_tmp.m_rightBar = bars[id[1]];
            }

            armor_tmp.m_upper_left = armor_tmp.m_leftBar.m_upper;
            armor_tmp.m_bottom_left = armor_tmp.m_leftBar.m_bottom;
            armor_tmp.m_upper_right = armor_tmp.m_rightBar.m_upper;
            armor_tmp.m_bottom_right = armor_tmp.m_rightBar.m_bottom;

            if(m_roi_flag == 1)             ///如果此时处理的图像取了ROI，需要补偿装甲坐标点数值，保证solvepnp结算距离不出错
            {
                armor_tmp.m_center_uv += m_roi_origin;
                armor_tmp.m_upper_left += m_roi_origin;
                armor_tmp.m_bottom_left += m_roi_origin;
                armor_tmp.m_upper_right += m_roi_origin;
                armor_tmp.m_bottom_right += m_roi_origin;
                armor_tmp.m_leftBar.m_rotateRect.center += m_roi_origin;
                armor_tmp.m_rightBar.m_rotateRect.center += m_roi_origin;
            }

            ///计算装甲长宽比
            armor_tmp.m_armor_rect = boundingRRect(armor_tmp.m_leftBar.m_rotateRect,armor_tmp.m_rightBar.m_rotateRect);
            armor_tmp.m_armor_width=armor_tmp.m_armor_rect.size.width;
            armor_tmp.m_armor_height=armor_tmp.m_armor_rect.size.height;
            armor_tmp.m_wh_ratio = armor_tmp.m_armor_width/armor_tmp.m_armor_height;

            if(armor_tmp.m_leftBar.line_6f[4] > armor_tmp.m_rightBar.line_6f[4])
                armor_tmp.m_bar_length_ratio = armor_tmp.m_leftBar.line_6f[4] /armor_tmp.m_rightBar.line_6f[4] ;
            else
                armor_tmp.m_bar_length_ratio = armor_tmp.m_rightBar.line_6f[4] /armor_tmp.m_leftBar.line_6f[4] ;

            armor_tmp.calculateRT(CAMERA_MATRIX, DISTORT);

//            if(armor_tmp.m_distance >= 3100)
//                armor_tmp.m_distance = armor_tmp.m_distance - ((armor_tmp.m_distance-3100)*0.21+100)*((armor_tmp.m_bar_length_ratio-1.02)/0.18);

            if(armor_tmp.m_distance < MAX_DISTANCE)
            {
                _armors.push_back(armor_tmp);
            }

        }
    }
    gettimeofday(&en,NULL);
    templateMatch(_armors,originframe);
   if(_armors.size()<1)

            {
                return false;
            }
            else return true;
}

}///hitcrt namespace
