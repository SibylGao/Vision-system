#ifndef ARMORDETECTOR_H
#define ARMORDETECTOR_H


#include "Armor.h"


namespace hitcrt{

class ArmorDetector {
public:
    ArmorDetector();
    ~ArmorDetector(){}
    bool Apply(std::vector<Armor>& _armors, cv::Mat& _frame, cv::Mat &origin_frame,int _roi_flag, cv::Point2f _roi_origin);
    void showImage(std::vector<Armor> &_armors, cv::Mat& _frame);
    cv::RotatedRect boundingRRect(const cv::RotatedRect & left, const cv::RotatedRect & right);      ///新增函数成员，计算装甲长宽比
    cv::Mat CAMERA_MATRIX = cv::Mat(3,3,CV_64FC1);
    cv::Mat DISTORT = cv::Mat(5,1,CV_64FC1);
    cv::Mat MAPX ;
    cv::Mat MAPY ;
    float FOCU_LENGTH = 0.0f;
    void templateMatch(std::vector<Armor>& _armors,cv::Mat& frame);

private:
    static bool parallel(LightBar bar1, LightBar bar2);
      //用模板匹配剔除异常
    void loadTemplate();


private:
    volatile float m_now_pitch;     //当前仰角,注意这个变量随时可以修改,访问需要加入锁,建议速度小于10Hz
    cv::Mat m_hereo_template;
    cv::Mat m_infantry_template;
    cv::Mat m_frame;
    cv::Point2f m_roi_origin;
    int m_roi_flag;



    int POINTS_MASK_SIZE_MAX = 0;
    int GRAY_THRESHOLD = 0;
    int CONTOURS_SIZE_MIN = 0;
    int CONTOURS_SIZE_MAX = 0;
    float MAX_LIGHT_BAR_AREA = 0;    //灯柱最大像素数目
    float MIN_LIGHT_BAR_AREA = 0;    //灯柱最小像素数目
    float BREADTH_LENGTH_SCALE_MIN = 0;
    float BREADTH_LENGTH_SCALE_MAX = 0;
    float BAR_POINTS_MIN = 0;
    float BAR_SLOPE_MIN = 0;
    float MAX_DISTANCE = 0;
    static float PARALL_DELTA_THETA;
    static float TWO_LENGTH_SCLAE;
    static float X_DISTANCE_SCALE;
    static float DISTANCE_LENGTH_SCALE_MAX;
    static float DISTANCE_LENGTH_SCALE_MIN;
    float TEMPLATE_RATIO=0;
};

}


#endif // ArmorDetector_H
