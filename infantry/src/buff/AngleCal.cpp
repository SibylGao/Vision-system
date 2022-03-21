//
// Created by sun on 18-3-5.
//

#include "AngleCal.h"

#define PI 3.14159265
//#define DEBUG_ANGLE_POINTS
//#define DEBUG_ANGLE_MATRIXS
using namespace cv;
using namespace std;
namespace hitcrt{
    vector<Point2f> AngleCal::codeNumImgPoints = vector<Point2f>(4);
    AngleCal::AngleCal():
            P0(0),
            Kp (1),
            Y0(0),
            Ky(1) {
        objectPoints = vector<Point3f>(36);
        for(int i = 0; i < 9; i ++) {
            objectPoints[i*4] = Point3f(QuadExtractor::objectQuads[i].up_left.x, QuadExtractor::objectQuads[i].up_left.y, 0);
            objectPoints[i*4+1] = Point3f(QuadExtractor::objectQuads[i].up_right.x, QuadExtractor::objectQuads[i].up_right.y, 0);
            objectPoints[i*4+2] = Point3f(QuadExtractor::objectQuads[i].down_right.x, QuadExtractor::objectQuads[i].down_right.y, 0);
            objectPoints[i*4+3] = Point3f(QuadExtractor::objectQuads[i].down_left.x, QuadExtractor::objectQuads[i].down_left.y, 0);
        }

        tenQuadsPoints = vector<Point3f>(40);
        for(int i = 0; i < 10; i ++) {
            tenQuadsPoints[i*4] = Point3f(QuadExtractor::tenQuads[i].up_left.x, QuadExtractor::tenQuads[i].up_left.y, 0);
            tenQuadsPoints[i*4+1] = Point3f(QuadExtractor::tenQuads[i].up_right.x, QuadExtractor::tenQuads[i].up_right.y, 0);
            tenQuadsPoints[i*4+2] = Point3f(QuadExtractor::tenQuads[i].down_right.x, QuadExtractor::tenQuads[i].down_right.y, 0);
            tenQuadsPoints[i*4+3] = Point3f(QuadExtractor::tenQuads[i].down_left.x, QuadExtractor::tenQuads[i].down_left.y, 0);
        }

        codeNumObjectPoints = vector<Point3f>(4);
        codeNumObjectPoints[0] = Point3f(235, -280, 0);
        codeNumObjectPoints[1] = Point3f(785, -280, 0);
        codeNumObjectPoints[2] = Point3f(785, 0, 0);
        codeNumObjectPoints[3] = Point3f(235, 0, 0);

//        cout << "tenQuadsPoints" << endl << tenQuadsPoints << endl;

        cv::FileStorage fs;
//        fs.open("/home/sun/HITCRT/Buff/data/angle_param.yml", cv::FileStorage::READ);
        fs.open("/home/sun/HITCRT/Infantry/data/buff/angle_param4_4.25.yml", cv::FileStorage::READ);
        assert(fs.isOpened());

        fs["heightC2G"] >> heightC2G;//y
        fs["widthC2G"] >> widthC2G;//x
        fs["lengthC2G"] >> lengthC2G;//z
        fs["rVectC2G0"] >> rVectC2G0;
        fs["rVectC2G1"] >> rVectC2G1;
        fs["rVectC2G2"] >> rVectC2G2;
    }


    float AngleCal::distance(cv::Point2f& pt1, cv::Point2f& pt2) {
        return std::sqrt((pt1.x - pt2.x)*(pt1.x - pt2.x)+(pt1.y - pt2.y)*(pt1.y - pt2.y));
    }
    bool AngleCal::getImgPoint(cv::Point3f& targetPoint, cv::Mat MatrixW2C, cv::Point2f& imagePoint) {
        cv::Mat targetMat(Size(1, 4), CV_64FC1);

        targetMat.at<double>(0, 0) = targetPoint.x;
        targetMat.at<double>(1, 0) = targetPoint.y;
        targetMat.at<double>(2, 0) = targetPoint.z;
        targetMat.at<double>(3, 0) = 1;

        cv::Mat MatrixW2CTrans = cv::Mat::zeros(Size(4,4),CV_64FC1);
        for(int i = 0; i < 3; i ++)
            for(int j = 0; j < 4; j++)
            {
                MatrixW2CTrans.at<double>(i,j) = MatrixW2C.at<double>(i,j);
            }
        MatrixW2CTrans.at<double>(3,3) = 1;

//        cout << "---------point in world--------" << endl;
//        cout << targetPoint << endl;

        cv::Mat targetInCamera;
        targetInCamera = MatrixW2CTrans * targetMat;
//        cout << "---------point in camera--------" << endl;
//        cout << targetInCamera << endl;

        cv::Mat targetInImg;
        cv::Mat cameraMatrix = cv::Mat(3, 4, CV_64FC1);

        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++) {
                cameraMatrix.at<double>(i, j) = BuffParam::KK.at<double>(i, j);
            }
        for (int i = 0; i < 3; i++) {
            cameraMatrix.at<double>(i, 3) = 0;
        }

        targetInImg = cameraMatrix * targetInCamera / targetInCamera.at<double>(2, 0);
//        cout << "---------point in image--------" << endl;
//        cout << targetInImg << endl;
        imagePoint = Point2f(targetInImg.at<double>(0, 0), targetInImg.at<double>(1, 0));
//        cout << "---------point in image2--------" << endl;
//        cout << imagePoint << endl;

    }
    //used in bigBuff
    bool AngleCal::MatrixW2CCal2(std::vector<Quad> &tenQuads, std::vector<Quad> &nineQuads, cv::Mat camera_matrix_,int index[5], cv::Mat image) {

        cv::Mat rVec, tVec, rMat;
        cv::Mat distort = cv::Mat::zeros(1,4,CV_32FC1);
        vector<Point3f> tenQuadsObjectPoints;

        if (index[0] == 5 || tenQuads.size() == 10){
            tenQuadsImgPoints = vector<Point2f>(40);
            for(int i = 0; i < 10; i ++) {
                tenQuadsImgPoints[i*4] = tenQuads[i].up_left/BuffParam::resizePara;
                tenQuadsImgPoints[i*4+1] = tenQuads[i].up_right/BuffParam::resizePara;
                tenQuadsImgPoints[i*4+2] = tenQuads[i].down_right/BuffParam::resizePara;
                tenQuadsImgPoints[i*4+3] = tenQuads[i].down_left/BuffParam::resizePara;
            }

            for (int i = 0; i < 40; i++){
                tenQuadsObjectPoints.push_back(tenQuadsPoints[i]);
            }

        } else if(tenQuads.size() < 10) {
            tenQuadsImgPoints = vector<Point2f>(20);
            for(int i = 0; i < 5; i ++) {
                tenQuadsImgPoints[i*4] = tenQuads[index[i]].up_left/BuffParam::resizePara;
                tenQuadsImgPoints[i*4+1] = tenQuads[index[i]].up_right/BuffParam::resizePara;
                tenQuadsImgPoints[i*4+2] = tenQuads[index[i]].down_right/BuffParam::resizePara;
                tenQuadsImgPoints[i*4+3] = tenQuads[index[i]].down_left/BuffParam::resizePara;
            }

            if (index[0] == 0){
                for (int i = 0; i < 20; i++)
                    tenQuadsObjectPoints.push_back(tenQuadsPoints[i]);
            } else if(index[0] == 4){
                for (int i = 0; i < 20; i++)
                    tenQuadsObjectPoints.push_back(tenQuadsPoints[i+20]);
            } else {
                cout << "tenQuads index is wrong" << endl;
                return false;
            }

        }

#ifdef DEBUG_ANGLE_POINTS
        cout << "solvePnP" << endl;
        cout<<"objectPoints.size() = "<<tenQuadsObjectPoints.size()<<endl;
        cout << "objectPoints " << endl << tenQuadsObjectPoints << endl;

        cout<<"imagePoints.size() = "<<tenQuadsImgPoints.size()<<endl;
        cout << "imagePoints " << endl << tenQuadsImgPoints << endl;
#endif

        if(!solvePnP(tenQuadsObjectPoints, tenQuadsImgPoints, BuffParam::KK, distort, rVec, tVec)) {
            cout << "solvePnP false" << endl;
            cout << tenQuadsObjectPoints.size() << endl;
            cout << tenQuadsImgPoints.size() << endl;
            cout << camera_matrix_ << endl;
            for(int i = 0; i < 20; i ++) {
                cout << tenQuadsObjectPoints[i] << "      " << tenQuadsImgPoints[i] << endl;
            }
            return false;
        }
        else {
            Rodrigues(rVec, rMat);
            MatrixW2C = cv::Mat(3,4,CV_64FC1);

            for(int i = 0; i < 3; i ++)
                for(int j = 0; j < 3; j++) {
                    MatrixW2C.at<double>(i,j) = rMat.at<double>(i,j);
                }

            for(int i = 0; i < 3; i ++) {
                MatrixW2C.at<double>(i,3) = tVec.at<double>(i,0);
            }

            for(int i = 0; i < codeNumObjectPoints.size(); i++){
                getImgPoint(codeNumObjectPoints[i], MatrixW2C, codeNumImgPoints[i]);
                codeNumImgPoints[i] = codeNumImgPoints[i]*BuffParam::resizePara;
            }
        }
        //get nineQuads
//        for (int i = 0; i <objectPoints.size(); i++){
//            cout << "objectPoints " << endl << objectPoints[i] << endl;
//        }
        vector<Point2f> targetImgPoints;
        for (int i = 0; i <objectPoints.size(); i++){
            Point2f targetImgPointTmp;
            getImgPoint(objectPoints[i], MatrixW2C, targetImgPointTmp);
            targetImgPoints.push_back(targetImgPointTmp);

//            cout << "image point " << endl << targetImgPointTmp << endl;
        }

        for(int i = 0; i < 9; i++) {
                Quad quadTmp;
                quadTmp.up_left = targetImgPoints[i*4+0]*BuffParam::resizePara;
                quadTmp.up_right = targetImgPoints[i*4+1]*BuffParam::resizePara;
                quadTmp.down_right = targetImgPoints[i*4+2]*BuffParam::resizePara;
                quadTmp.down_left = targetImgPoints[i*4+3]*BuffParam::resizePara;


                quadTmp.m_center = Point2f((quadTmp.up_left.x + quadTmp.down_right.x)/2, (quadTmp.up_left.y + quadTmp.down_right.y)/2);
                quadTmp.m_width = (distance(quadTmp.up_left, quadTmp.up_right) + distance(quadTmp.down_left, quadTmp.down_right))/2;
                quadTmp.m_height = (distance(quadTmp.up_left, quadTmp.down_left) + distance(quadTmp.up_right, quadTmp.down_right))/2;
                quadTmp.area = quadTmp.m_width*quadTmp.m_height;

                quadTmp.counters.push_back(quadTmp.up_left);
                quadTmp.counters.push_back(quadTmp.up_right);
                quadTmp.counters.push_back(quadTmp.down_right);
                quadTmp.counters.push_back(quadTmp.down_left);
            nineQuads.push_back(quadTmp);
        }

//        for(int i = 0; i < 9; i++) {
//            cout <<"AngleCal quadsNine " << nineQuads[i].up_left << endl;
//        }
    }

    bool AngleCal::MatrixW2CCal(std::vector<Quad> &nineQuads, cv::Mat camera_matrix_, cv::Mat image) {

        cv::Mat rVec, tVec, rMat;
        cv::Mat distort = cv::Mat::zeros(1,4,CV_32FC1);
        imagePoints = vector<Point2f>(36);
        for(int i = 0; i < 9; i ++) {
            imagePoints[i*4] = nineQuads[i].up_left/BuffParam::resizePara;
            imagePoints[i*4+1] = nineQuads[i].up_right/BuffParam::resizePara;
            imagePoints[i*4+2] = nineQuads[i].down_right/BuffParam::resizePara;
            imagePoints[i*4+3] = nineQuads[i].down_left/BuffParam::resizePara;
        }

#ifdef DEBUG_ANGLE_POINTS
        cout << "solvePnP" << endl;
        cout<<"objectPoints.size() = "<<objectPoints.size()<<endl;
        cout << "objectPoints " << endl << objectPoints << endl;

        cout<<"imagePoints.size() = "<<imagePoints.size()<<endl;
        cout << "imagePoints " << endl << imagePoints << endl;
#endif
        if(!solvePnP(objectPoints, imagePoints, BuffParam::KK, distort, rVec, tVec)) {
            cout << "solvePnP false" << endl;
            cout << objectPoints.size() << endl;
            cout << imagePoints.size() << endl;
            cout << camera_matrix_ << endl;
            for(int i = 0; i < 36; i ++) {
                cout << objectPoints[i] << "      " << imagePoints[i] << endl;
            }
            return false;
        }
        else {
            Rodrigues(rVec, rMat);
            MatrixW2C = cv::Mat(3,4,CV_64FC1);

            for(int i = 0; i < 3; i ++)
                for(int j = 0; j < 3; j++) {
                    MatrixW2C.at<double>(i,j) = rMat.at<double>(i,j);
                }

            for(int i = 0; i < 3; i ++) {
                MatrixW2C.at<double>(i,3) = tVec.at<double>(i,0);
            }

            //codeNumRect
            for(int i = 0; i < codeNumObjectPoints.size(); i++){
                getImgPoint(codeNumObjectPoints[i], MatrixW2C, codeNumImgPoints[i]);
                codeNumImgPoints[i] = codeNumImgPoints[i]*BuffParam::resizePara;
            }

            return true;
        }
    }


    bool AngleCal::MatrixW2GCal(void) {

        cv::Mat MatrixW2CTrans = cv::Mat::zeros(Size(4,4),CV_64FC1);
        for(int i = 0; i < 3; i ++)
            for(int j = 0; j < 4; j++) {
                MatrixW2CTrans.at<double>(i,j) = MatrixW2C.at<double>(i,j);
            }
        MatrixW2CTrans.at<double>(3,3) = 1;

#ifdef DEBUG_ANGLE_MATRIXS
        cout << "MatrixW2CTrans is " <<endl;
        cout << MatrixW2CTrans << endl;
#endif

        MatrixC2G = cv::Mat::zeros(Size(4,3),CV_64FC1);
        cv::Mat rVec = cv::Mat::zeros(Size(3,1),CV_64FC1);
        cv::Mat rMat, rMatInv;
        //////input rVec from calibration
        rVec.at<double>(0,0) = rVectC2G0;
        rVec.at<double>(0,1) = rVectC2G1;
        rVec.at<double>(0,2) = rVectC2G2;
        Rodrigues(rVec, rMat);
        rMatInv = rMat.inv();

        for(int i = 0; i < 3; i ++)
            for(int j = 0; j < 3; j++) {
                MatrixC2G.at<double>(i,j) = rMatInv.at<double>(i,j);
            }

        MatrixC2G.at<double>(0,3) = widthC2G;//x
        MatrixC2G.at<double>(1,3) = heightC2G;//y
        MatrixC2G.at<double>(2,3) = lengthC2G;//z

        cv::Mat MatrixC2GTrans = cv::Mat::zeros(Size(4,4),CV_64FC1);
        for(int i = 0; i < 3; i ++)
            for(int j = 0; j < 4; j++) {
                MatrixC2GTrans.at<double>(i,j) = MatrixC2G.at<double>(i,j);
            }
        MatrixC2GTrans.at<double>(3,3) = 1;

#ifdef DEBUG_ANGLE_MATRIXS
        cout << " rMatInv" << rMatInv << endl;
        cout << "MatrixC2GTrans is " <<endl;
        cout << MatrixC2GTrans << endl;
#endif

        //计算MatrixW2G
        cv::Mat MatrixW2GTrans(4,4,CV_64FC1);
        MatrixW2GTrans = MatrixC2GTrans * MatrixW2CTrans;
        MatrixW2G = cv::Mat(3,4,CV_64FC1);
        for(int i = 0; i < 3; i ++)
            for(int j = 0; j < 4; j++) {
                MatrixW2G.at<double>(i,j) = MatrixW2GTrans.at<double>(i,j);
            }

#ifdef DEBUG_ANGLE_MATRIXS
        cout << "MatrixW2GTrans is "<< endl;
        cout << MatrixW2GTrans <<endl;
#endif
        return true;
    }


    bool AngleCal::calAngleYandP(Point3f targetPoint, vector<float>& YandP) {
        Mat targetMat(Size(1,4),CV_64FC1);
        targetMat.at<double>(0,0) =  targetPoint.x;
        targetMat.at<double>(1,0) =  targetPoint.y;
        targetMat.at<double>(2,0) =  targetPoint.z;
        targetMat.at<double>(3,0) =  1;

        Mat targetMatGaxis;
        targetMatGaxis = MatrixW2G * targetMat;

        float yawAngle, pitchAngle;
        yawAngle = -atan(targetMatGaxis.at<double>(0,0)/targetMatGaxis.at<double>(2,0)) * 180.0 /PI;
        pitchAngle = -atan(targetMatGaxis.at<double>(1,0)/targetMatGaxis.at<double>(2,0)) * 180.0 /PI;

        float yawVal, pitchVal;
        yawVal = (yawAngle * Ky + Y0);
        pitchVal = (pitchAngle * Kp + P0);

        //加补偿
        if(pitchVal < -50)  pitchVal -= 15;
        if(pitchVal > 50) pitchVal += 15;

        YandP.clear();
        YandP.push_back(yawVal);
        YandP.push_back(pitchVal);
        return true;
    }

    bool AngleCal::nineYawPitchCal(void) {
        //首先计算需要打击的9个点的位置
        //输入距离左上角点的距离
        vector<Point3f> nineHitPoints(9);
        float xDistance = 140, yDistance = 40;

        nineYawPitch = Mat(2,9,CV_32FC1);
        for(int i = 0; i < 9; i ++) {
            vector<float > YandP;
            nineHitPoints[i].x = objectPoints[i*4].x + xDistance;
            nineHitPoints[i].y = objectPoints[i*4].y + yDistance;
            nineHitPoints[i].z = objectPoints[i*4].z;
            //换算
            calAngleYandP(nineHitPoints[i], YandP);
            nineYawPitch.at<float>(0,i) = (float)YandP[0];
            nineYawPitch.at<float>(1,i) = (float)YandP[1];
        }

//        cout << "absolute nineYawPitch" << endl;
        cout << nineYawPitch << endl;
#ifdef DEBUG_ANGLE_MATRIXS
        cout << "nineHitPoints" << endl << nineHitPoints << endl;
        cout << "absolute nineYawPitch" << endl;
        cout << nineYawPitch << endl;
#endif

//    float middleYaw = nineYawPitch.at<float>(0,4);
//    float middlePitch = nineYawPitch.at<float>(1,4);
//    for(int i = 0; i < 9; i ++) {
//       nineYawPitch.at<float>(0,i) = nineYawPitch.at<float>(0,i) - middleYaw;
//       nineYawPitch.at<float>(1,i) = nineYawPitch.at<float>(1,i) - middlePitch;
//    }

        return true;
    }





////////////////////////////////////////////////////////////////////////////////////////////////
    /*bool transformAxis(Mat projMatrix, vector<Point3f> inPoints, vector<Point3f> &outPoints)
    {
        if((projMatrix.rows != 3)||(projMatrix.cols != 4)) return false;
        outPoints.clear();
        for(vector<Point3f>::iterator itr = inPoints.begin(); itr != inPoints.end(); itr ++)
        {
            Mat inPointVec(Size(1,4), CV_64FC1);
            Mat outPointVec(Size(1,3), CV_64FC1);
            Point3f outPointsTemp;
            inPointVec.at<double>(0,0) = (*itr).x;
            inPointVec.at<double>(1,0) = (*itr).y;
            inPointVec.at<double>(2,0) = (*itr).z;
            inPointVec.at<double>(3,0) = 1;
            outPointVec = projMatrix * inPointVec;
            outPointsTemp.x = (float)outPointVec.at<double>(0,0);
            outPointsTemp.y = (float)outPointVec.at<double>(0,1);
            outPointsTemp.z = (float)outPointVec.at<double>(0,2);
            outPoints.push_back(outPointsTemp);
        }
        return true;
    }*/

/*    bool getEulerAngles(Mat rMat, Mat &eulerAngles)
    {
        if((rMat.rows != 3)||(rMat.cols != 3)) return false;
        eulerAngles = Mat(Size(1,3),CV_64FC1);
        eulerAngles.at<double>(2,0) = asin(-rMat.at<double>(2,0)) * 180.0 / PI; //thetaZ
        eulerAngles.at<double>(0,0) = atan(rMat.at<double>(1,0) / rMat.at<double>(0,0)) * 180.0 /PI;    //thetaX
        eulerAngles.at<double>(1,0) = atan(rMat.at<double>(2,1) / rMat.at<double>(2,2)) * 180.0 /PI;    //thetaY
        return true;
    }

    void AngleCal::calibratePandY()
    {
        ofstream outfile("data.txt",ios::app);//声明一个写文件流对象outfile并打开文件
        if(!outfile){//检查是否打开成功
            cout << "Failed to create file..."<<endl;
            return;
        }

        cout << "MatrixS2C is " << endl;
        cout << MatrixW2C << endl;

        ///计算MatrixC2G,此时应当已求出标定图片的MatrixS2C
        double heightC2G = 161;//y
        double widthC2G = -109;//x
        double lengthC2G = 156.08;//z
        //使用标定图片旋转矩阵的逆矩阵作为C2G的旋转阵
        Mat rMatS2C(Size(3,3), CV_64FC1);
        Mat rMatC2G(Size(3,3), CV_64FC1);
        for(int i = 0; i < 3; i ++)
            for(int j = 0; j < 3; j ++)
            {
                rMatS2C.at<double>(i,j) = MatrixW2C.at<double>(i,j);
            }
        rMatC2G = rMatS2C.inv();

        cout << endl << "rMatS2C = " << endl;
        cout << endl << rMatS2C << endl;

        cout << "rMatC2G = " << endl;
        cout << rMatC2G << endl;

        MatrixC2G = Mat(Size(4,3),CV_64FC1);
        for(int i = 0; i < 3; i ++)
            for(int j = 0; j < 3; j++)
            {
                MatrixC2G.at<double>(i,j) = rMatC2G.at<double>(i,j);
            }
        MatrixC2G.at<double>(0,3) = widthC2G;
        MatrixC2G.at<double>(1,3) = heightC2G;
        MatrixC2G.at<double>(2,3) = lengthC2G;

        cout << "projMatrixC2G is " << endl;
        cout << MatrixC2G << endl;
        outfile << "projMatrixC2G is " << endl;
        outfile << MatrixC2G << endl;

        //调整objectPoints的位置, 原来是左下右下右上左上, 调节为左上右上右下左下
        for(int i = 0; i < objectPoints.size()/4; i ++)
        {
            swap(objectPoints[i *4 + 0], objectPoints[i *4 + 3]);
            swap(objectPoints[i *4 + 1], objectPoints[i *4 + 2]);
        }

        //将大符坐标系的点转换到炮塔坐标系
        vector<Point3f> objectPointsGaxis;
        MatrixW2GCal();

        cout << "MatrixW2G is " << endl;
        cout << MatrixW2G << endl;

        transformAxis(MatrixW2G, objectPoints, objectPointsGaxis);

        for(vector<Point3f>::iterator itr = objectPointsGaxis.begin(); itr != objectPointsGaxis.end(); itr++)
        {
            cout << *itr <<endl;
        }

        vector<double> pVec{7163, 7160, 7156, 7156, 7157, 7158, 7235, 7264, 7340, 7369, 7444};

        vector<double> yVec{7860, 7730, 7689, 7550, 7515, 7378, 7859, 7863, 7862, 7861, 7863};

        //生成pMat和yMat
        Mat pMat(Size(pVec.size(),1), CV_64FC1);
        Mat yMat(Size(yVec.size(),1), CV_64FC1);

        for(int i = 0; i < pMat.cols; i ++)
        {
            pMat.at<double>(0,i) = pVec[i];
            yMat.at<double>(0,i) = yVec[i];
        }

        cout << "pMat is" <<endl << pMat << endl;
        cout << "yMat is" <<endl << yMat << endl;

        vector<int> pointsIndex{0, 1, 12, 13, 24, 25, 3, 4, 7, 8, 11};
        //计算出thetaP, thetaY并放在一个2*n的矩阵中
        Mat thetaPMat(Size(pointsIndex.size(), 2), CV_64FC1);
        Mat thetaYMat(Size(pointsIndex.size(), 2), CV_64FC1);
        Mat P0andKp;
        Mat Y0andKy;

        for(int i = 0; i < pointsIndex.size(); i ++)
        {
            thetaPMat.at<double>(0, i) = 1;
            thetaPMat.at<double>(1, i) = atan(objectPointsGaxis[pointsIndex[i]].y/objectPointsGaxis[pointsIndex[i]].z) * 180.0 /PI;
            thetaYMat.at<double>(0, i) = 1;
            thetaYMat.at<double>(1, i) = atan(objectPointsGaxis[pointsIndex[i]].x/objectPointsGaxis[pointsIndex[i]].z) * 180.0 /PI;
        }

        cout << "thetaPMat is" <<endl << thetaPMat << endl;
        cout << "thetaYMat is" <<endl << thetaYMat << endl;

        // x = b*A'*inv(A*A')
        Mat inverseMat = thetaPMat * thetaPMat.t();
        P0andKp = pMat * thetaPMat.t() * inverseMat.inv();

        cout << "P0andKp is " << endl;
        cout << P0andKp << endl;
        outfile << endl << "P0andKp is " << endl;
        outfile << P0andKp << endl << endl;

        inverseMat = thetaYMat * thetaYMat.t();
        Y0andKy = yMat * thetaYMat.t() * inverseMat.inv();

        cout << "Y0andKy is " << endl;
        cout << Y0andKy << endl;
        outfile << endl << "Y0andKy is " << endl;
        outfile << Y0andKy << endl << endl;

        outfile.close();
        cout << "Finish writing !" << endl;
    }*/


    void AngleCal::verifyResult()
    {
        //调整objectPoints的位置, 原来是左下右下右上左上, 调节为左上右上右下左下
        for(int i = 0; i < objectPoints.size()/4; i ++)
        {
            swap(objectPoints[i *4 + 0], objectPoints[i *4 + 3]);
            swap(objectPoints[i *4 + 1], objectPoints[i *4 + 2]);
        }

        vector<int> pointsIndex{0, 1, 12, 13, 24, 25, 3, 4, 7, 8, 11};
        vector<Point3f> verifyPoints(pointsIndex.size());

        for(int i = 0; i < pointsIndex.size(); i ++)
        {
            vector<float> YandP;
            verifyPoints[i].x = objectPoints[pointsIndex[i]].x;
            verifyPoints[i].y = objectPoints[pointsIndex[i]].y;
            verifyPoints[i].z = objectPoints[pointsIndex[i]].z;
            //换算
            calAngleYandP(verifyPoints[i], YandP);
            cout << objectPoints[pointsIndex[i]] << "\t\t" << "[" << YandP[0] << "\t" << YandP[1] << "]" << endl;
        }
    }

}
