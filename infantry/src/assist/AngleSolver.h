#ifndef ANGLESOLVER_H
#define ANGLESOLVER_H


#include "Armor.h"

namespace hitcrt{

class AngleSolver
{
public:
    AngleSolver();
    virtual ~AngleSolver();
        void calculateSendDegree(float _pitch, Armor _armor);
        float Eject_Degree = 0;
        float Yaw_Degree = 0;
        float current_Pitch_rad = 0;       ///当前pitch轴角度，单位为弧度。以水平线为零线，向上为正，向下为负

        ///装甲中心在云台坐标系下的坐标
        float Xh;
        float Yh;
        float Zh;
        float VELOCITY = 0;

    private:
        ///相机坐标系相对于云台坐标系的偏移
        float OFFSET_X = 0;
        float OFFSET_Y = 0;
        float OFFSET_Z = 0;
        double RVect_c2h0=0;
        double RVect_c2h1=0;
        double RVect_c2h2=0;
        cv::Mat R_c2h;
        cv::Mat T_c2h;
        cv::Mat P_c;
        cv::Mat P_h;
        float G = 9800.0f ;
        float YAW=0.0f;
        float PITCH=0.0f;
};


}
#endif // ANGLESOLVER_H
