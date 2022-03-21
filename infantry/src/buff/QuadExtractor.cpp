//
// Created by sun on 18-1-12.
//
#include "QuadExtractor.h"

//threshold of area
#define MIN_AERA 30*30//30~38
#define MAX_AERA 50*50
#define IS_EQUAL_RATIO 0.3
#define DEBUG_QUADNINE
#define DEBUG_QUAD
#define DEBUG_QUAD_TIME
//#define DEBUG_QUAD_CLASSIFIDE


using namespace std;
using namespace cv;

namespace hitcrt{

    float QuadExtractor::quadNineX = 280;
    float QuadExtractor::quadNineY = 160;
    float QuadExtractor::quad2quadX = 90;
    float QuadExtractor::quad2quadY = 60;

    float QuadExtractor::quadTenX = 140;
    float QuadExtractor::quadTenY = 69.4;
    float QuadExtractor::quadTen2quadTenX = 1200.6;
    float QuadExtractor::quadTen2quadTenY = 60.6;

    int QuadExtractor::functionFlag = 0;
    int QuadExtractor::lastFunctionFlag = 0;
    std::vector<cv::Point3f> tenQuadsPoints;

    vector<Quad> QuadExtractor::objectQuads = vector<Quad>(9);
    vector<Quad> QuadExtractor::tenQuads = vector<Quad>(10);
    bool QuadExtractor::initQuad = QuadExtractor::init();

    QuadExtractor::QuadExtractor(): lastQuadArea(-1){
        cv::FileStorage fs;
        fs.open("/home/sun/HITCRT/Infantry/data/buff/quad_param.yml", cv::FileStorage::READ);
        assert(fs.isOpened());

        fs["dilate_ksize"] >> dilate_ksize;
        fs["dilate1_n"] >> dilate1_n;
        fs["dilate2_n"] >> dilate2_n;
        fs["erode1_n"] >> erode1_n;

        fs["min_ratio"] >> min_ratio;
        fs["max_ratio"] >> max_ratio;
        fs["oppo_side_error"] >> oppo_side_error;
        fs["is_equal_ratio"] >> is_equal_ratio;
        fs["min_area"] >> min_area;
        fs["last_now_area_error"] >> last_now_area_error;
        fs["min_angle"] >> min_angle;
        fs["k_min_area"] >> k_min_area;

        time_t log1;
        int name_log = time(&log1);
        ostringstream os_log;
        os_log <<"/home/infantry/Infantry/data/buff/logs/" << name_log <<"_QuadExtractor.txt";
        out.open(os_log.str(),ios_base::out);

    };

    bool QuadExtractor::init(){

        objectQuads.clear();
        for(int i = 0; i < 3; i++){
            for (int j = 0; j < 3; j++){
                objectQuads[i*3+j].up_left = Point2f(i*(quadNineX+quad2quadX),j*(quadNineY+quad2quadY));
                objectQuads[i*3+j].up_right = Point2f(objectQuads[i*3+j].up_left.x+quadNineX, objectQuads[i*3+j].up_left.y);
                objectQuads[i*3+j].down_right = Point2f(objectQuads[i*3+j].up_left.x+quadNineX, objectQuads[i*3+j].up_left.y+quadNineY);
                objectQuads[i*3+j].down_left = Point2f(objectQuads[i*3+j].up_left.x, objectQuads[i*3+j].up_left.y+quadNineY);

                objectQuads[i*3+j].m_center = Point2f(objectQuads[i*3+j].up_left.x+quadNineX/2, objectQuads[i*3+j].up_left.y+quadNineY/2);
                objectQuads[i*3+j].m_width = quadNineX;
                objectQuads[i*3+j].m_height = quadNineY;
                objectQuads[i*3+j].area = quadNineX*quadNineY;

                objectQuads[i*3+j].counters.push_back(objectQuads[i*3+j].up_left);
                objectQuads[i*3+j].counters.push_back(objectQuads[i*3+j].up_right);
                objectQuads[i*3+j].counters.push_back(objectQuads[i*3+j].down_right);
                objectQuads[i*3+j].counters.push_back(objectQuads[i*3+j].down_left);
            }
        }
                tenQuads.clear();
        for (int i = 0; i < 2; i++){
            for (int j = 0; j < 5; j++){
                tenQuads[i*5+j].up_left = Point2f(-230.3+i*(quadTenX+quadTen2quadTenX), 5.3+j*(quadTenY+quadTen2quadTenY));
                tenQuads[i*5+j].up_right = Point2f(tenQuads[i*5+j].up_left.x+quadTenX, tenQuads[i*5+j].up_left.y);
                tenQuads[i*5+j].down_right = Point2f(tenQuads[i*5+j].up_left.x+quadTenX, tenQuads[i*5+j].up_left.y+quadTenY);
                tenQuads[i*5+j].down_left = Point2f(tenQuads[i*5+j].up_left.x, tenQuads[i*5+j].up_left.y+quadTenY);

                tenQuads[i*5+j].m_center = Point2f(tenQuads[i*5+j].up_left.x+quadTenX/2, tenQuads[i*5+j].up_left.y+quadTenY/2);
                tenQuads[i*5+j].m_width = quadTenX;
                tenQuads[i*5+j].m_height = quadTenY;
                tenQuads[i*5+j].area = quadTenX*quadTenY;

                tenQuads[i*5+j].counters.push_back(tenQuads[i*5+j].up_left);
                tenQuads[i*5+j].counters.push_back(tenQuads[i*5+j].up_right);
                tenQuads[i*5+j].counters.push_back(tenQuads[i*5+j].down_right);
                tenQuads[i*5+j].counters.push_back(tenQuads[i*5+j].down_left);
            }
        }
        tenQuadsPoints = vector<Point3f>(40);
        for(int i = 0; i < 10; i ++) {
            tenQuadsPoints[i*4] = Point3f(tenQuads[i].up_left.x, tenQuads[i].up_left.y, 0);
            tenQuadsPoints[i*4+1] = Point3f(tenQuads[i].up_right.x, tenQuads[i].up_right.y, 0);
            tenQuadsPoints[i*4+2] = Point3f(tenQuads[i].down_right.x, tenQuads[i].down_right.y, 0);
            tenQuadsPoints[i*4+3] = Point3f(tenQuads[i].down_left.x, tenQuads[i].down_left.y, 0);
        }

        return true;
    };

    bool QuadExtractor::isEqualSmallBuff(Quad quad1, Quad quad2) {

        float areaRatio = fabs(quad1.area - quad2.area)/max(quad2.area, quad1.area);
        float widthRatio  = fabs(quad1.m_width - quad2.m_width)/ max(quad2.m_width,quad1.m_width);
        float heightRatio = fabs(quad1.m_height - quad2.m_height)/max(quad2.m_height, quad1.m_height);
        if (areaRatio > IS_EQUAL_RATIO)
            return false;
        else if (widthRatio > IS_EQUAL_RATIO)
            return false;
        else if(heightRatio < IS_EQUAL_RATIO)
            return true;
        return true;
    }
    bool QuadExtractor::isEqualBigBuff(Quad quad1, Quad quad2) {

        float areaRatio = fabs(quad1.area - quad2.area)/max(quad2.area, quad1.area);
        float widthRatio  = fabs(quad1.m_width - quad2.m_width)/ max(quad2.m_width,quad1.m_width);
        float heightRatio = fabs(quad1.m_height - quad2.m_height)/max(quad2.m_height, quad1.m_height);
        if((quad1.up_right.x<quad2.up_left.x)||(quad2.up_right.x<quad1.up_left.x))
         return false;
        else if (areaRatio > IS_EQUAL_RATIO)
            return false;
        else if (widthRatio > IS_EQUAL_RATIO)
            return false;
        else if(heightRatio < IS_EQUAL_RATIO)
            return true;
        return true;
    }
    float QuadExtractor::distance(cv::Point2f& pt1, cv::Point2f& pt2) {
        return std::sqrt((pt1.x - pt2.x)*(pt1.x - pt2.x)+(pt1.y - pt2.y)*(pt1.y - pt2.y));
    }

    float QuadExtractor::angle(cv::Point pt1, cv::Point pt2, cv::Point crossPoint) {
        float dx1 = pt1.x - crossPoint.x;
        float dy1 = pt1.y - crossPoint.y;
        float dx2 = pt2.x - crossPoint.x;
        float dy2 = pt2.y - crossPoint.y;
        return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
    }

    void QuadExtractor::drawQuads(vector<Quad> quads, cv::Mat &image) {
        if(quads.size()!=0){
            line(image, quads[0].up_left, quads[0].up_right, Scalar(0, 255, 0), 2);
            line(image, quads[0].up_right, quads[0].down_right, Scalar(0, 255, 0), 2);
            line(image, quads[0].down_right, quads[0].down_left, Scalar(0, 255, 0), 2);
            line(image, quads[0].down_left, quads[0].up_left, Scalar(0, 255, 0), 2);

            for (int i = 0; i < quads.size(); i++){

                line(image, quads[i].up_left, quads[i].up_right, Scalar(0, 255, 0), 2);
                line(image, quads[i].up_right, quads[i].down_right, Scalar(0, 0, 255), 2);
                line(image, quads[i].down_right, quads[i].down_left, Scalar(255, 0, 0), 2);
                line(image, quads[i].down_left, quads[i].up_left, Scalar(255, 255, 0), 2);

//                stringstream cc;
//                cc << i;
//                String text = cc.str();
//                int fontface = FONT_HERSHEY_SCRIPT_SIMPLEX;
//                double fontscale = 0.5;
//                int baseline;
//                Size textsize = getTextSize(text, fontface, fontscale, 2, &baseline);
//                putText(image, text, quads[i].m_center, fontface, fontscale, Scalar(0xff, 0x99, 0), 2);
//                cout << i << " x:" << quads[i].m_center.x << " y:" << quads[i].m_center.y<< endl;
            }
        }
    }

    void QuadExtractor::orderY(vector<Quad>& quads, int row1Size, int row2Size){//row1Size is size of row1, row2Size is size of row1
        for (int i = 0; i < row1Size; i++) {
            for (int j = i + 1; j < row1Size; j++) {
                if (quads[j].m_center.y < quads[i].m_center.y)
                    swap(quads[i], quads[j]);
            }
        }
        for (int i = row1Size; i < row1Size+row2Size; i++) {
            for (int j = i + 1; j < row1Size+row2Size; j++) {
                if (quads[j].m_center.y < quads[i].m_center.y)
                    swap(quads[i], quads[j]);
            }
        }
        if((row1Size+row2Size)<quads.size()){
        for (int i = row1Size+row2Size; i < quads.size(); i++) {
            for (int j = i + 1; j < quads.size(); j++) {
                if (quads[j].m_center.y < quads[i].m_center.y)
                    swap(quads[i], quads[j]);
            }
        }
        }
    }

    void QuadExtractor::orderX(vector<Quad>& quads){
        for (int i=0; i<quads.size(); i++) {
            for (int j=i+1; j<quads.size(); j++) {
                if(quads[j].m_center.x < quads[i].m_center.x)
                    swap(quads[i], quads[j]);
            }
        }
    }

    cv::Point2f QuadExtractor::getImgPoint(cv::Point3f& targetPoint, cv::Mat MatrixW2C) {
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

        cv::Point2f imagePoint;
        targetInImg = cameraMatrix * targetInCamera / targetInCamera.at<double>(2, 0);
//        cout << "---------point in image--------" << endl;
//        cout << targetInImg << endl;
        imagePoint = Point2f(targetInImg.at<double>(0, 0), targetInImg.at<double>(1, 0));
        return  imagePoint;
    }

    bool QuadExtractor::getImgQuad(vector<Quad>& imageQuads, vector<Quad>& insertQuads, int index1[3], int index2[3], int index3[]){
        //index1 is objectQuads index
        //index2 is imageQuads index
        //index3 is targetQuads index
        cv::Mat distort = cv::Mat::zeros(1,4,CV_32FC1);
        vector<Point3f> objectPoints(12);
        for(int i = 0; i < 3; i ++) {
            objectPoints[i*4] = Point3f(objectQuads[index1[i]].up_left.x, objectQuads[index1[i]].up_left.y, 0);
            objectPoints[i*4+1] = Point3f(objectQuads[index1[i]].up_right.x, objectQuads[index1[i]].up_right.y, 0);
            objectPoints[i*4+2] = Point3f(objectQuads[index1[i]].down_right.x, objectQuads[index1[i]].down_right.y, 0);
            objectPoints[i*4+3] = Point3f(objectQuads[index1[i]].down_left.x, objectQuads[index1[i]].down_left.y, 0);
        }

        vector<Point2f> imagePoints(12);
        for(int i = 0; i < 3; i ++) {
            imagePoints[i*4] = imageQuads[index2[i]].up_left/BuffParam::resizePara;
            imagePoints[i*4+1] = imageQuads[index2[i]].up_right/BuffParam::resizePara;
            imagePoints[i*4+2] = imageQuads[index2[i]].down_right/BuffParam::resizePara;
            imagePoints[i*4+3] = imageQuads[index2[i]].down_left/BuffParam::resizePara;
        }

//        cout << "--------objectPoint---------------"<< endl;
//        cout << objectPoints << endl;
//
//        cout << "--------imagePoints---------------"<< endl;
//        cout << imagePoints << endl;

        cv::Mat rVec, tVec, rMat, MatrixW2C;
        if(!solvePnP(objectPoints, imagePoints, BuffParam::KK, distort, rVec, tVec)) {
            cout << "solvePnP false" << endl;
            cout << objectPoints.size() << endl;
            cout << imagePoints.size() << endl;
            cout << BuffParam::KK << endl;
            for(int i = 0; i < 12; i ++) {
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
        }

        vector<Point3f> targetPoints;
        targetPoints.push_back(Point3f(objectQuads[index3[0]].up_left.x, objectQuads[index3[0]].up_left.y, 0));
        targetPoints.push_back(Point3f(objectQuads[index3[0]].up_right.x, objectQuads[index3[0]].up_right.y, 0));
        targetPoints.push_back(Point3f(objectQuads[index3[0]].down_right.x, objectQuads[index3[0]].down_right.y, 0));
        targetPoints.push_back(Point3f(objectQuads[index3[0]].down_left.x, objectQuads[index3[0]].down_left.y, 0));

        if (index3[0] != index3[1]){
            targetPoints.push_back(Point3f(objectQuads[index3[1]].up_left.x, objectQuads[index3[1]].up_left.y, 0));
            targetPoints.push_back(Point3f(objectQuads[index3[1]].up_right.x, objectQuads[index3[1]].up_right.y, 0));
            targetPoints.push_back(Point3f(objectQuads[index3[1]].down_right.x, objectQuads[index3[1]].down_right.y, 0));
            targetPoints.push_back(Point3f(objectQuads[index3[1]].down_left.x, objectQuads[index3[1]].down_left.y, 0));
        }

        vector<Point2f> targetImgPoints;
        for (int i = 0; i <targetPoints.size(); i++){
            targetImgPoints.push_back(getImgPoint(targetPoints[i], MatrixW2C));
        }

        Quad quadTmp1;
        quadTmp1.up_left = targetImgPoints[0]*BuffParam::resizePara;
        quadTmp1.up_right = targetImgPoints[1]*BuffParam::resizePara;
        quadTmp1.down_right = targetImgPoints[2]*BuffParam::resizePara;
        quadTmp1.down_left = targetImgPoints[3]*BuffParam::resizePara;


        quadTmp1.m_center = Point2f((quadTmp1.up_left.x + quadTmp1.down_right.x)/2, (quadTmp1.up_left.y + quadTmp1.down_right.y)/2);
        quadTmp1.m_width = (distance(quadTmp1.up_left, quadTmp1.up_right) + distance(quadTmp1.down_left, quadTmp1.down_right))/2;
        quadTmp1.m_height = (distance(quadTmp1.up_left, quadTmp1.down_left) + distance(quadTmp1.up_right, quadTmp1.down_right))/2;
        quadTmp1.area = quadTmp1.m_width*quadTmp1.m_height;

        quadTmp1.counters.push_back(quadTmp1.up_left);
        quadTmp1.counters.push_back(quadTmp1.up_right);
        quadTmp1.counters.push_back(quadTmp1.down_right);
        quadTmp1.counters.push_back(quadTmp1.down_left);

        insertQuads.clear();
        insertQuads.push_back(quadTmp1);


        if(targetImgPoints.size() == 8){
            Quad quadTmp2;
            quadTmp2.up_left = targetImgPoints[4]*BuffParam::resizePara;
            quadTmp2.up_right = targetImgPoints[5]*BuffParam::resizePara;
            quadTmp2.down_right = targetImgPoints[6]*BuffParam::resizePara;
            quadTmp2.down_left = targetImgPoints[7]*BuffParam::resizePara;

            quadTmp2.m_center = Point2f((quadTmp2.up_left.x + quadTmp2.down_right.x)/2, (quadTmp2.up_left.y + quadTmp2.down_right.y)/2);
            quadTmp2.m_width = (distance(quadTmp2.up_left, quadTmp2.up_right) + distance(quadTmp2.down_left, quadTmp2.down_right))/2;
            quadTmp2.m_height = (distance(quadTmp2.up_left, quadTmp2.down_left) + distance(quadTmp2.up_right, quadTmp2.down_right))/2;
            quadTmp2.area = quadTmp2.m_width*quadTmp2.m_height;

            quadTmp1.counters.push_back(quadTmp2.up_left);
            quadTmp1.counters.push_back(quadTmp2.up_right);
            quadTmp1.counters.push_back(quadTmp2.down_right);
            quadTmp1.counters.push_back(quadTmp2.down_left);

            insertQuads.push_back(quadTmp2);
        }
        return true;
    }
        bool QuadExtractor::getImgQuad2(vector<Quad>& imageQuads, vector<Quad>& tenQuads){
        cv::Mat distort = cv::Mat::zeros(1,4,CV_32FC1);
        vector<Point3f> objectPoints(36);
        for(int i = 0; i < 9; i ++) {
            objectPoints[i*4] = Point3f(objectQuads[i].up_left.x, objectQuads[i].up_left.y, 0);
            objectPoints[i*4+1] = Point3f(objectQuads[i].up_right.x, objectQuads[i].up_right.y, 0);
            objectPoints[i*4+2] = Point3f(objectQuads[i].down_right.x, objectQuads[i].down_right.y, 0);
            objectPoints[i*4+3] = Point3f(objectQuads[i].down_left.x, objectQuads[i].down_left.y, 0);
        }

        vector<Point2f> imagePoints(36);
        for(int i = 0; i < 9; i ++) {
            imagePoints[i*4] = imageQuads[i].up_left/BuffParam::resizePara;
            imagePoints[i*4+1] = imageQuads[i].up_right/BuffParam::resizePara;
            imagePoints[i*4+2] = imageQuads[i].down_right/BuffParam::resizePara;
            imagePoints[i*4+3] = imageQuads[i].down_left/BuffParam::resizePara;
        }

//        cout << "--------objectPoint---------------"<< endl;
//        cout << objectPoints << endl;
//
//        cout << "--------imagePoints---------------"<< endl;
//        cout << imagePoints << endl;

        cv::Mat rVec, tVec, rMat, MatrixW2C;
        if(!solvePnP(objectPoints, imagePoints, BuffParam::KK, distort, rVec, tVec)) {
            cout << "solvePnP false" << endl;
            cout << objectPoints.size() << endl;
            cout << imagePoints.size() << endl;
            cout << BuffParam::KK << endl;
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
        }


        vector<Point2f> targetImgPoints;
        for (int i = 0; i <tenQuadsPoints.size(); i++){
            targetImgPoints.push_back(getImgPoint(tenQuadsPoints[i], MatrixW2C));
        }

        for(int i = 0; i < 10; i++) {
                Quad quadTmp2;
                quadTmp2.up_left = targetImgPoints[i*4+0]*BuffParam::resizePara;
                quadTmp2.up_right = targetImgPoints[i*4+1]*BuffParam::resizePara;
                quadTmp2.down_right = targetImgPoints[i*4+2]*BuffParam::resizePara;
                quadTmp2.down_left = targetImgPoints[i*4+3]*BuffParam::resizePara;


                quadTmp2.m_center = Point2f((quadTmp2.up_left.x + quadTmp2.down_right.x)/2, (quadTmp2.up_left.y + quadTmp2.down_right.y)/2);
                quadTmp2.m_width = (distance(quadTmp2.up_left, quadTmp2.up_right) + distance(quadTmp2.down_left, quadTmp2.down_right))/2;
                quadTmp2.m_height = (distance(quadTmp2.up_left, quadTmp2.down_left) + distance(quadTmp2.up_right, quadTmp2.down_right))/2;
                quadTmp2.area = quadTmp2.m_width*quadTmp2.m_height;

                quadTmp2.counters.push_back(quadTmp2.up_left);
                quadTmp2.counters.push_back(quadTmp2.up_right);
                quadTmp2.counters.push_back(quadTmp2.down_right);
                quadTmp2.counters.push_back(quadTmp2.down_left);
            tenQuads.push_back(quadTmp2);
        }


        return true;
    }
    bool QuadExtractor::getTenQuad(vector<Quad>& imageQuads, vector<Quad>& insertQuads, int index1[3], int index2[3], int index3[]){
        //index1 is objectQuads index
        //index2 is imageQuads index
        //index3 is targetQuads index
        cv::Mat distort = cv::Mat::zeros(1,4,CV_32FC1);
        vector<Point3f> objectPoints(16);
        for(int i = 0; i < 4; i ++) {
            objectPoints[i*4] = Point3f(tenQuads[index1[i]].up_left.x, tenQuads[index1[i]].up_left.y, 0);
            objectPoints[i*4+1] = Point3f(tenQuads[index1[i]].up_right.x, tenQuads[index1[i]].up_right.y, 0);
            objectPoints[i*4+2] = Point3f(tenQuads[index1[i]].down_right.x, tenQuads[index1[i]].down_right.y, 0);
            objectPoints[i*4+3] = Point3f(tenQuads[index1[i]].down_left.x, tenQuads[index1[i]].down_left.y, 0);
        }

        vector<Point2f> imagePoints(16);
        for(int i = 0; i < 4; i ++) {
            imagePoints[i*4] = imageQuads[index2[i]].up_left/BuffParam::resizePara;
            imagePoints[i*4+1] = imageQuads[index2[i]].up_right/BuffParam::resizePara;
            imagePoints[i*4+2] = imageQuads[index2[i]].down_right/BuffParam::resizePara;
            imagePoints[i*4+3] = imageQuads[index2[i]].down_left/BuffParam::resizePara;
        }

//        cout << "--------objectPoint---------------"<< endl;
//        cout << objectPoints << endl;
//
//        cout << "--------imagePoints---------------"<< endl;
//        cout << imagePoints << endl;

        cv::Mat rVec, tVec, rMat, MatrixW2C;
        if(!solvePnP(objectPoints, imagePoints, BuffParam::KK, distort, rVec, tVec)) {
            cout << "solvePnP false" << endl;
            cout << objectPoints.size() << endl;
            cout << imagePoints.size() << endl;
            cout << BuffParam::KK << endl;
            for(int i = 0; i < 16; i ++) {
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
        }

//        cout << "MatrixW2C is " <<endl;
//        cout << MatrixW2C << endl;

        vector<Point3f> targetPoints;
        targetPoints.push_back(Point3f(tenQuads[index3[0]].up_left.x, tenQuads[index3[0]].up_left.y, 0));
        targetPoints.push_back(Point3f(tenQuads[index3[0]].up_right.x, tenQuads[index3[0]].up_right.y, 0));
        targetPoints.push_back(Point3f(tenQuads[index3[0]].down_right.x, tenQuads[index3[0]].down_right.y, 0));
        targetPoints.push_back(Point3f(tenQuads[index3[0]].down_left.x, tenQuads[index3[0]].down_left.y, 0));

//        if (index3[0] != index3[1]){
//            targetPoints.push_back(Point3f(objectQuads[index3[1]].up_left.x, objectQuads[index3[1]].up_left.y, 0));
//            targetPoints.push_back(Point3f(objectQuads[index3[1]].up_right.x, objectQuads[index3[1]].up_right.y, 0));
//            targetPoints.push_back(Point3f(objectQuads[index3[1]].down_right.x, objectQuads[index3[1]].down_right.y, 0));
//            targetPoints.push_back(Point3f(objectQuads[index3[1]].down_left.x, objectQuads[index3[1]].down_left.y, 0));
//        }

        vector<Point2f> targetImgPoints;
        for (int i = 0; i <targetPoints.size(); i++){
            targetImgPoints.push_back(getImgPoint(targetPoints[i], MatrixW2C));
        }

//        cout << "final image point" << targetImgPoints << endl;
//        vector<Quad> targetImgQuads;
        Quad quadTmp1;
        quadTmp1.up_left = targetImgPoints[0]*BuffParam::resizePara;
        quadTmp1.up_right = targetImgPoints[1]*BuffParam::resizePara;
        quadTmp1.down_right = targetImgPoints[2]*BuffParam::resizePara;
        quadTmp1.down_left = targetImgPoints[3]*BuffParam::resizePara;


        quadTmp1.m_center = Point2f((quadTmp1.up_left.x + quadTmp1.down_right.x)/2, (quadTmp1.up_left.y + quadTmp1.down_right.y)/2);
        quadTmp1.m_width = (distance(quadTmp1.up_left, quadTmp1.up_right) + distance(quadTmp1.down_left, quadTmp1.down_right))/2;
        quadTmp1.m_height = (distance(quadTmp1.up_left, quadTmp1.down_left) + distance(quadTmp1.up_right, quadTmp1.down_right))/2;
        quadTmp1.area = quadTmp1.m_width*quadTmp1.m_height;

        quadTmp1.counters.push_back(quadTmp1.up_left);
        quadTmp1.counters.push_back(quadTmp1.up_right);
        quadTmp1.counters.push_back(quadTmp1.down_right);
        quadTmp1.counters.push_back(quadTmp1.down_left);

        insertQuads.clear();
        insertQuads.push_back(quadTmp1);


//        if(targetImgPoints.size() == 8){
//            Quad quadTmp2;
//            quadTmp2.up_left = targetImgPoints[4]*0.3;
//            quadTmp2.up_right = targetImgPoints[5]*0.3;
//            quadTmp2.down_right = targetImgPoints[6]*0.3;
//            quadTmp2.down_left = targetImgPoints[7]*0.3;
//
//            quadTmp2.m_center = Point2f((quadTmp2.up_left.x + quadTmp2.down_right.x)/2, (quadTmp2.up_left.y + quadTmp2.down_right.y)/2);
//            quadTmp2.m_width = (distance(quadTmp2.up_left, quadTmp2.up_right) + distance(quadTmp2.down_left, quadTmp2.down_right))/2;
//            quadTmp2.m_height = (distance(quadTmp2.up_left, quadTmp2.down_left) + distance(quadTmp2.up_right, quadTmp2.down_right))/2;
//            quadTmp2.area = quadTmp2.m_width*quadTmp2.m_height;
//
//            quadTmp1.counters.push_back(quadTmp2.up_left);
//            quadTmp1.counters.push_back(quadTmp2.up_right);
//            quadTmp1.counters.push_back(quadTmp2.down_right);
//            quadTmp1.counters.push_back(quadTmp2.down_left);
//
//            insertQuads.push_back(quadTmp2);
//        }
        return true;
    }

    bool QuadExtractor::insertQuad(vector<Quad>& quads, int position1, int position2){
        if (quads.size() == 8){
            int index1[3] = {0};
            int index2[3] = {0};
            int index3[2] = {0};
            if (position1 == 2){
                for (int i = 0; i < 3; i++) {
                    index2[i] = position1 + i;
                    index1[i] = index2[i] + 1;
                }
            }else if(position1 == 5){
                for (int i = 0; i < 3; i++) {
                    index2[i] = position1 + i;
                    index1[i] = index2[i] + 1;
                }
            } else if(position1 == 8){
                for (int i = 0; i < 3;i++){
                    index2[i] = (position1+1)%3+i;
                    index1[i] = index2[i];
                }
            }
            float totalY = quads[index2[0]].m_center.y+quads[index2[1]].m_center.y+quads[index2[2]].m_center.y;
            float row_y = quads[position1-1].m_center.y+quads[position1-2].m_center.y;

            vector<Quad> insertQuad;
            if (totalY - (row_y+quads[index2[0]].m_center.y) < 20){
                index3[0] = index3[1] = position1 - 2;
            } else if(totalY - (row_y+quads[index2[1]].m_center.y) < 20){
                index3[0] = index3[1] = position1 - 1;
            } else if(totalY - (row_y+quads[index2[2]].m_center.y) < 20){
                index3[0] = index3[1] = position1;
            } else {
                cout << "Extractor: get index3 failed" << endl;
                return false;
            }


            cout << "index1" << endl << index1[0] << " " << index1[1] << " " << index1[2] << endl;
            cout << "index2" << endl << index2[0] << " " << index2[1] << " " << index2[2] << endl;
            cout << "index3" << endl << index3[0] << " " << index3[1] << endl;

            if(!getImgQuad(quads, insertQuad, index1, index2, index3)){
                cout << "Extractor: getImgQuad failed" << endl;
                return false;
            }
            cout << "insert quad size: " << insertQuad.size() << endl;
            quads.insert(quads.begin()+index3[0], insertQuad[0]);
        }
        else if(quads.size() == 7){
            if(fabs(position1 - position2) == 1){
                int index1[3] = {0};
                int index2[3] = {0};
                int index3[2] = {0};
                if (position1 == 1){
                    for (int i = 0; i < 3; i++) {
                        index2[i] = position1 + i;
                        index1[i] = index2[i] + 2;
                    }
                }else if(position1 == 4){
                    for (int i = 0; i < 3; i++) {
                        index2[i] = position1 + i;
                        index1[i] = index2[i] + 2;
                    }
                }
                else if(position1 == 7){
                    for (int i = 0; i < 3;i++){
                        index2[i] = (position1+2)%3+i;
                        index1[i] = index2[i];
                    }
                }

                vector<Quad> insertQuad;
                if (quads[position1-1].m_center.y - quads[index2[0]].m_center.y < 20){
                    index3[0] = position1;
                    index3[1] = position2;
                } else if(quads[position1-1].m_center.y - quads[index2[1]].m_center.y < 20){
                    index3[0] = position1-1;
                    index3[1] = position2;

                }else if(quads[position1-1].m_center.y - quads[index2[2]].m_center.y < 20) {
                    index3[0] = position1-1;
                    index3[1] = position2-1;
                } else{
                    cout << "Extractor: get index3 failed" << endl;
                    return false;
                }


                cout << "index1" << endl << index1[0] << " " << index1[1] << " " << index1[2] << endl;
                cout << "index2" << endl << index2[0] << " " << index2[1] << " " << index2[2] << endl;
                cout << "index3" << endl << index3[0] << " " << index3[1] << endl;

                if(!getImgQuad(quads, insertQuad, index1, index2, index3)){
                    cout << "Extractor: getImgQuad failed" << endl;
                    return false;
                }
                quads.insert(quads.begin()+index3[0], insertQuad[0]);
                quads.insert(quads.begin()+index3[1], insertQuad[1]);

            } else {
                int index1[3] = {0};
                int index2[3] = {0};
                int index3[2] = {0};

                if ((position1 == 2) && (position2 == 5)){
                    for (int i = 0; i < 3; i++) {
                        index2[i] = i+4;
                        index1[i] = index2[i] + 2;
                    }
                } else if((position1 == 5) && (position2 == 8)){
                    for (int i = 0; i < 3;i++){
                        index2[i] = i;
                        index1[i] = index2[i];
                    }
                }else if((position1 == 2) && (position2 == 8)) {
                    for (int i = 0; i < 3; i++) {
                        index2[i] = i+2;
                        index1[i] = index2[i] + 1;
                    }
                }

                float totalY = quads[index2[0]].m_center.y+quads[index2[1]].m_center.y+quads[index2[2]].m_center.y;
                float row1_y = quads[position1-1].m_center.y+quads[position1-2].m_center.y;
                float row2_y = quads[position2-3].m_center.y+quads[position2-2].m_center.y;


                vector<Quad> insertQuad;
                cout << "total" << totalY << endl;
                cout << "row1 " <<  row1_y << endl;
                cout << "row2 " <<  row2_y << endl;


                if (totalY - (row1_y+quads[index2[0]].m_center.y) < 20){
                    index3[0] = position1-2;
                } else if(totalY - (row1_y+quads[index2[1]].m_center.y) < 20){
                    index3[0] = position1-1;
                }else if(totalY - (row1_y+quads[index2[2]].m_center.y) < 20){
                    index3[0] = position1;
                }else{
                    cout << "Extractor: get index3[0] failed" << endl;
                    return false;
                }

                if (totalY - (row2_y+quads[index2[0]].m_center.y) < 20){
                    index3[1] = position2-2;
                } else if(totalY - (row2_y+quads[index2[1]].m_center.y) < 20){
                    index3[1] = position2-1;
                }else if(totalY - (row2_y+quads[index2[2]].m_center.y) < 20){
                    index3[1] = position2;
                }else{
                    cout << "Extractor: get index3[1] failed" << endl;
                    return false;
                }

                cout << "index1" << endl << index1[0] << " " << index1[1] << " " << index1[2] << endl;
                cout << "index2" << endl << index2[0] << " " << index2[1] << " " << index2[2] << endl;
                cout << "index3" << endl << index3[0] << " " << index3[1] << endl;

                if(!getImgQuad(quads, insertQuad, index1, index2, index3)){
                    cout << "Extractor: getImgQuad failed" << endl;
                    return false;
                }
                cout << "insert quad size: " << insertQuad.size() << endl;
                quads.insert(quads.begin()+index3[0], insertQuad[0]);
                quads.insert(quads.begin()+index3[1], insertQuad[1]);
            }
        }
        return true;
    }
    bool QuadExtractor::insertTenQuad(vector<Quad>& quads, int position1, int position2){
        if (quads.size() == 9){
            int index1[5] = {0};
            int index2[5] = {0};
            int index3[2] = {0};
            int index4[4]={0};
            if (position1 == 4){
                for (int i = 0; i < 5; i++) {
                    index2[i] = position1 + i;
                    index1[i] = index2[i] + 1;
                    if(i!=4){
                    index4[i]=i;
                    }
                }

            }else if(position1 == 9){
                for (int i = 0; i < 5; i++) {
                    index2[i] = i;
                    index1[i] = i;
                    if(i!=4){
                    index4[i]=i+5;
                    }
                }
            }
            cout<<" index4[i]"<<index4[0]<<index4[1]<<index4[2]<<index4[3]<<endl;
            float totalY = quads[index2[0]].m_center.y+quads[index2[1]].m_center.y+quads[index2[2]].m_center.y+quads[index2[3]].m_center.y+quads[index2[4]].m_center.y;
            float row_y = quads[position1-1].m_center.y+quads[position1-2].m_center.y+quads[position1-3].m_center.y+quads[position1-4].m_center.y;
//            cout<<"totalY - (row_y+quads[index2[0]].m_center.y) "<<totalY - (row_y+quads[index2[0]].m_center.y) <<endl;
//            cout<<"totalY - (row_y+quads[index2[1]].m_center.y) "<<totalY - (row_y+quads[index2[1]].m_center.y)<<endl;
//            cout<<"totalY - (row_y+quads[index2[2]].m_center.y) "<<totalY - (row_y+quads[index2[2]].m_center.y) <<endl;
//            cout<<"totalY - (row_y+quads[index2[3]].m_center.y) "<<totalY - (row_y+quads[index2[3]].m_center.y) <<endl;
//            cout<<"totalY - (row_y+quads[index2[4]].m_center.y) "<<totalY - (row_y+quads[index2[4]].m_center.y) <<endl;
            vector<Quad> insert;
//            if (totalY - (row_y+quads[index2[0]].m_center.y) < 16){
//                index3[0] = index3[1] = position1 - 4;
//            } else if(totalY - (row_y+quads[index2[1]].m_center.y) < 16){
//                index3[0] = index3[1] = position1 - 3;
//            } else if(totalY - (row_y+quads[index2[2]].m_center.y) < 16){
//                index3[0] = index3[1] = position1 - 2;
//            } else if(totalY - (row_y+quads[index2[3]].m_center.y) < 16){
//                index3[0] = index3[1] = position1 - 1;
//            }else {
//                index3[0] = index3[1] = position1;
//            }
            cout<<"totalY"<<totalY<<endl;
            cout<<"row_y"<<row_y<<endl;
            cout<<"quads[index2[4]].m_center.y"<<endl;
            cout<<quads[index2[0]].m_center.y<<endl;
            cout<<quads[index2[1]].m_center.y<<endl;
            cout<<quads[index2[2]].m_center.y<<endl;
            cout<<quads[index2[3]].m_center.y<<endl;
            cout<<quads[index2[4]].m_center.y<<endl;
            cout<<"quads[index4[3]].m_center.y"<<endl;
            cout<<quads[index4[0]].m_center.y<<endl;
            cout<<quads[index4[1]].m_center.y<<endl;
            cout<<quads[index4[2]].m_center.y<<endl;
            cout<<quads[index4[3]].m_center.y<<endl;
            float ave_y=(-totalY+quads[index2[4]].m_center.y+row_y)/4;
            float ave_self_y=(quads[index4[3]].m_center.y-quads[index4[0]].m_center.y)/3;
            cout<<"ave_y"<<ave_y<<endl;
            cout<<"(quads[index4[0]].m_center.y-quads[index2[0]].m_center.y)"<<(quads[index4[0]].m_center.y-quads[index2[0]].m_center.y)<<endl;
            cout<<"(quads[index4[1]].m_center.y-quads[index2[1]].m_center.y)"<<(quads[index4[1]].m_center.y-quads[index2[1]].m_center.y)<<endl;
            cout<<"(quads[index4[2]].m_center.y-quads[index2[2]].m_center.y) "<<(quads[index4[2]].m_center.y-quads[index2[2]].m_center.y) <<endl;
            cout<<"(quads[index4[3]].m_center.y-quads[index2[3]].m_center.y) "<<(quads[index4[3]].m_center.y-quads[index2[3]].m_center.y)<<endl;
//            if (((quads[index4[3]].m_center.y-quads[index2[3]].m_center.y)-(quads[index4[0]].m_center.y-quads[index2[0]].m_center.y))<3*ave_y){
//                index3[0] = index3[1] = position1 - 4;
//            } else if((quads[index4[1]].m_center.y-quads[index2[1]].m_center.y) > ave_y){
//                index3[0] = index3[1] = position1 - 3;
//            } else if((quads[index4[2]].m_center.y-quads[index2[2]].m_center.y) > ave_y){
//                index3[0] = index3[1] = position1 - 2;
//            } else if((quads[index4[3]].m_center.y-quads[index2[3]].m_center.y) > ave_y){
//                index3[0] = index3[1] = position1 - 1;
//            }else {
//                index3[0] = index3[1] = position1;
//            }
            cout<<"ave_self_y*1.2"<<ave_self_y*1.35<<endl;
            cout<<"(quads[index4[1]].m_center.y-quads[index4[0]].m_center.y)"<<(quads[index4[1]].m_center.y-quads[index4[0]].m_center.y)<<endl;
            cout<<"(quads[index4[2]].m_center.y-quads[index4[1]].m_center.y)"<<(quads[index4[2]].m_center.y-quads[index4[1]].m_center.y)<<endl;
            cout<<"(quads[index4[3]].m_center.y-quads[index4[2]].m_center.y) "<<(quads[index4[3]].m_center.y-quads[index4[2]].m_center.y) <<endl;

            float dis_y2=quads[index4[3]].m_center.y-quads[index2[3]].m_center.y;
            float dis_y1=quads[index4[0]].m_center.y-quads[index2[0]].m_center.y;
             cout<<"dis_y2-dis_y1"<<dis_y2-dis_y1<<endl;
             cout<<"quads[index2[3]].m_height"<<quads[index2[3]].m_height<<endl;
             cout<<"fabs(quads[index4[3]].m_center.y-quads[index2[3]].m_center.y)"<<fabs(quads[index4[3]].m_center.y-quads[index2[3]].m_center.y)<<endl;
             cout<<"fabs(quads[index4[3]].m_center.y-quads[index2[4]].m_center.y)"<<fabs(quads[index4[3]].m_center.y-quads[index2[4]].m_center.y)<<endl;
            if ((quads[index4[1]].m_center.y-quads[index4[0]].m_center.y)>ave_self_y*1.35){
                index3[0] = index3[1] = position1 - 3;
            } else if((quads[index4[2]].m_center.y-quads[index4[1]].m_center.y)>ave_self_y*1.35){
                index3[0] = index3[1] = position1 - 2;
            } else if((quads[index4[3]].m_center.y-quads[index4[2]].m_center.y)>ave_self_y*1.35){
                index3[0] = index3[1] = position1 - 1;
            } else if(fabs(quads[index4[3]].m_center.y-quads[index2[3]].m_center.y)>fabs(quads[index4[3]].m_center.y-quads[index2[4]].m_center.y)){
                index3[0] = index3[1] = position1-4;
            }else {
                index3[0] = index3[1] = position1;
            }
            cout << "index1" << endl << index1[0] << " " << index1[1] << " " << index1[2] << " " << index1[3] << " " << index1[4] << endl;
            cout << "index2" << endl << index2[0] << " " << index2[1] << " " << index2[2] << " " << index2[3] << " " << index2[4] << endl;
            cout << "index3" << endl << index3[0] << " " << index3[1] << endl;

            if(!getTenQuad(quads, insert, index1, index2, index3)){
                cout << "Extractor: getTenQuad failed" << endl;
                return false;
            }
            cout << "insert quad size: " << insert.size() << endl;
            quads.insert(quads.begin()+index3[0], insert[0]);
        }
//        else if(quads.size() == 7){
//            if(fabs(position1 - position2) == 1){
//                int index1[3] = {0};
//                int index2[3] = {0};
//                int index3[2] = {0};
//                if (position1 == 1){
//                    for (int i = 0; i < 3; i++) {
//                        index2[i] = position1 + i;
//                        index1[i] = index2[i] + 2;
//                    }
//                }else if(position1 == 4){
//                    for (int i = 0; i < 3; i++) {
//                        index2[i] = position1 + i;
//                        index1[i] = index2[i] + 2;
//                    }
//                }
//                else if(position1 == 7){
//                    for (int i = 0; i < 3;i++){
//                        index2[i] = (position1+2)%3+i;
//                        index1[i] = index2[i];
//                    }
//                }
//
//                vector<Quad> insert;
//                if (quads[position1-1].m_center.y - quads[index2[0]].m_center.y < 20){
//                    index3[0] = position1;
//                    index3[1] = position2;
//                } else if(quads[position1-1].m_center.y - quads[index2[1]].m_center.y < 20){
//                    index3[0] = position1-1;
//                    index3[1] = position2;
//
//                }else if(quads[position1-1].m_center.y - quads[index2[2]].m_center.y < 20) {
//                    index3[0] = position1-1;
//                    index3[1] = position2-1;
//                }
//
//                cout << "index1" << endl << index1[0] << " " << index1[1] << " " << index1[2] << endl;
//                cout << "index2" << endl << index2[0] << " " << index2[1] << " " << index2[2] << endl;
//                cout << "index3" << endl << index3[0] << " " << index3[1] << endl;
//
//                getImgQuad(quads, insert, index1, index2, index3);
//                quads.insert(quads.begin()+index3[0], insert[0]);
//                quads.insert(quads.begin()+index3[1], insert[1]);
//
//            } else {
//                int index1[3] = {0};
//                int index2[3] = {0};
//                int index3[2] = {0};
//
//                if ((position1 == 2) && (position2 == 5)){
//                    for (int i = 0; i < 3; i++) {
//                        index2[i] = i+4;
//                        index1[i] = index2[i] + 2;
//                    }
//                } else if((position1 == 5) && (position2 == 8)){
//                    for (int i = 0; i < 3;i++){
//                        index2[i] = i;
//                        index1[i] = index2[i];
//                    }
//                }else if((position1 == 2) && (position2 == 8)) {
//                    for (int i = 0; i < 3; i++) {
//                        index2[i] = i+2;
//                        index1[i] = index2[i] + 1;
//                    }
//                }
//
//                float totalY = quads[index2[0]].m_center.y+quads[index2[1]].m_center.y+quads[index2[2]].m_center.y;
//                float row1_y = quads[position1-1].m_center.y+quads[position1-2].m_center.y;
//                float row2_y = quads[position2-3].m_center.y+quads[position2-2].m_center.y;
//
//
//                vector<Quad> insert;
//                cout << "total" << totalY << endl;
//                cout << "row1 " <<  row1_y << endl;
//                cout << "row2 " <<  row2_y << endl;
//
//
//                if (totalY - (row1_y+quads[index2[0]].m_center.y) < 20){
//
//
//                    index3[0] = position1-2;
//                } else if(totalY - (row1_y+quads[index2[1]].m_center.y) < 20){
//                    index3[0] = position1-1;
//                }else {
//                    index3[0] = position1;
//                }
//
//                if (totalY - (row2_y+quads[index2[0]].m_center.y) < 20){
//                    index3[1] = position2-2;
//                } else if(totalY - (row2_y+quads[index2[1]].m_center.y) < 20){
//                    index3[1] = position2-1;
//                }else {
//                    index3[1] = position2;
//                }
//
//                cout << "index1" << endl << index1[0] << " " << index1[1] << " " << index1[2] << endl;
//                cout << "index2" << endl << index2[0] << " " << index2[1] << " " << index2[2] << endl;
//                cout << "index3" << endl << index3[0] << " " << index3[1] << endl;
//
//                getImgQuad(quads, insert, index1, index2, index3);
//                cout << "insert quad size: " << insert.size() << endl;
//                quads.insert(quads.begin()+index3[0], insert[0]);
//                quads.insert(quads.begin()+index3[1], insert[1]);
//            }
//        }
        return true;
    }
    bool QuadExtractor::completeQuads(vector<Quad>& quads) {
        orderX(quads); //order by x
        if (quads.size() == 9) {
            orderY(quads, 3, 3);//order by y
//            for(int i = 0; i<quads.size(); i++){
//                cout << "quad" << i << quads[i].m_center << endl;
//            }
        }
        else if(quads.size() == 8){
            if ((fabs(quads[1].m_center.x - quads[2].m_center.x))> 10){
                cout << "第1列缺少" << endl;
                orderY(quads, 2, 3);
//                for(int i = 0; i<quads.size(); i++){
//                    cout << "quad" << i << quads[i].m_center << endl;
//                }
                if(!insertQuad(quads, 2, 2)){
                    cout << "Extractor: insertQuad failed!!!" << endl;
                    return false;
                }
            } else if ((fabs(quads[2].m_center.x - quads[3].m_center.x)) > 10 &&
                     (fabs(quads[4].m_center.x - quads[5].m_center.x)) > 10){
                cout << "第2列缺少" << endl;
                //y排序
                orderY(quads, 3, 2);
//                for(int i = 0; i<quads.size(); i++){
//                    cout << "quad" << i << quads[i].m_center << endl;
//                }
                if(!insertQuad(quads, 5, 5)){
                    cout << "Extractor: insertQuad failed!!!" << endl;
                    return false;
                }
                //insertQuad(quads, 5, 5);
            }else if ((fabs(quads[5].m_center.x - quads[6].m_center.x)) > 10){
                cout << "第3列缺少" << endl;
                //y排序
                orderY(quads, 3, 3);
//                for(int i = 0; i<quads.size(); i++){
//                    cout << "quad" << i << quads[i].m_center << endl;
//                }
                if(!insertQuad(quads, 8, 8)){
                    cout << "Extractor: insertQuad failed!!!" << endl;
                    return false;
                }
//                insertQuad(quads, 8, 8);
            }
        }else if(quads.size() == 7){
            if((fabs(quads[1].m_center.x - quads[2].m_center.x))> 10 &&
               (fabs(quads[3].m_center.x - quads[4].m_center.x))> 10){
                cout << "第1,2列缺少" << endl;
                orderY(quads, 2, 2);
//                for(int i = 0; i<quads.size(); i++){
//                    cout << "quad" << i << quads[i].m_center << endl;
//                }
                if(!insertQuad(quads, 2, 5)){
                    cout << "Extractor: insertQuad failed!!!" << endl;
                    return false;
                }
//                insertQuad(quads, 2, 5);
            }
            else if((fabs(quads[2].m_center.x - quads[3].m_center.x))> 10 &&
               (fabs(quads[4].m_center.x - quads[5].m_center.x))> 10){
                cout << "第2,3列缺少" << endl;
                orderY(quads, 3, 2);
//                for(int i = 0; i<quads.size(); i++){
//                    cout << "quad" << i << quads[i].m_center << endl;
//                }
                if(!insertQuad(quads, 5, 8)){
                    cout << "Extractor: insertQuad failed!!!" << endl;
                    return false;
                }
                //insertQuad(quads, 5, 8);
            }
            else if((fabs(quads[1].m_center.x - quads[2].m_center.x))> 10 &&
               (fabs(quads[4].m_center.x - quads[5].m_center.x))> 10){
                cout << "第1,3列缺少" << endl;
                orderY(quads, 2, 3);
//                for(int i = 0; i<quads.size(); i++){
//                    cout << "quad" << i << quads[i].m_center << endl;
//                }
                if(!insertQuad(quads, 2, 8)){
                    cout << "Extractor: insertQuad failed!!!" << endl;
                    return false;
                }
//                insertQuad(quads, 2, 8);
            }

            else if((fabs(quads[0].m_center.x - quads[1].m_center.x))> 10 &&
               (fabs(quads[3].m_center.x - quads[4].m_center.x))> 10){
                cout << "第1列缺少2个" << endl;
                orderY(quads, 1, 3);
//                for(int i = 0; i<quads.size(); i++){
//                    cout << "quad" << i << quads[i].m_center << endl;
//                }
                if(!insertQuad(quads, 1, 2)){
                    cout << "Extractor: insertQuad failed!!!" << endl;
                    return false;
                }
//                insertQuad(quads, 1, 2);
            }

            else if((fabs(quads[2].m_center.x - quads[3].m_center.x))> 10 &&
               (fabs(quads[3].m_center.x - quads[4].m_center.x))> 10){
                cout << "第2列缺少2个" << endl;
                orderY(quads, 3, 1);
//                for(int i = 0; i<quads.size(); i++){
//                    cout << "quad" << i << quads[i].m_center << endl;
//                }
                if(!insertQuad(quads, 4, 5)){
                    cout << "Extractor: insertQuad failed!!!" << endl;
                    return false;
                }
//                insertQuad(quads, 4, 5);
            }
            else if((fabs(quads[2].m_center.x - quads[3].m_center.x))> 10 &&
               (fabs(quads[5].m_center.x - quads[6].m_center.x))> 10){
                cout << "第3列缺少2个" << endl;
                orderY(quads, 3, 3);
//                for(int i = 0; i<quads.size(); i++){
//                    cout << "quad" << i << quads[i].m_center << endl;
//                }
                if(!insertQuad(quads, 7, 8)){
                    cout << "Extractor: insertQuad failed!!!" << endl;
                    return false;
                }
//                insertQuad(quads, 7, 8);
            }
        }
        return true;
    }
    bool QuadExtractor::completeTenQuads(vector<Quad>& quads, int index[5]) {
        orderX(quads); //order by x
        if (quads.size() == 10) {
            orderY(quads, 5, 5);//order by y
//            for(int i = 0; i<quads.size(); i++){
//                cout << "quad" << i << quads[i].m_center << endl;
//            }
            for (int i = 0; i < 5; i++){
                index[i] = i+5;
            }
        }
        else if(quads.size() == 9){
            if ((fabs(quads[3].m_center.x - quads[4].m_center.x))> 10){
                cout << "第1列缺少" << endl;
                orderY(quads, 4, 5);
//                for(int i = 0; i<quads.size(); i++){
//                    cout << "quad" << i << quads[i].m_center << endl;
//                }
                for (int i = 0; i < 5; i++){
                    index[i] = i + 4;
                }
                if(!insertTenQuad(quads, 4, 4)){
                    cout << "Extractor: insertTenQuad failed!!!" << endl;
                    return false;
                }
//                insertTenQuad(quads, 4, 4);
            }else if ((fabs(quads[4].m_center.x - quads[5].m_center.x)) > 10){
                cout << "第2列缺少" << endl;
                //y排序
                orderY(quads, 5, 4);
//                for(int i = 0; i<quads.size(); i++){
//                    cout << "quad" << i << quads[i].m_center << endl;
//                }
                for (int i = 0; i < 5; i++){
                    index[i] = i;
                }
                if(!insertTenQuad(quads, 9, 9)){
                    cout << "Extractor: insertTenQuad failed!!!" << endl;
                    return false;
                }
//                insertTenQuad(quads, 9, 9);
            }
        }
//        else if(quads.size() == 8){
//            if((fabs(quads[3].m_center.x - quads[4].m_center.x))> 10 ){
//                cout << "第1,2列缺少" << endl;
//                orderY(quads, 4, 4);
////                for(int i = 0; i<quads.size(); i++){
////                    cout << "quad" << i << quads[i].m_center << endl;
////                }
//                insertTenQuad(quads, 4, 9);
//            }
//
//            else if((fabs(quads[1].m_center.x - quads[2].m_center.x))<10 &&
//               (fabs(quads[2].m_center.x - quads[3].m_center.x))> 10){
//                cout << "第1列缺少2个" << endl;
//                orderY(quads, 3, 5);
////                for(int i = 0; i<quads.size(); i++){
////                    cout << "quad" << i << quads[i].m_center << endl;
////                }
//                insertTenQuad(quads, 3, 4);
//            }
//
//            else if((fabs(quads[3].m_center.x - quads[4].m_center.x))< 10 &&
//               (fabs(quads[4].m_center.x - quads[5].m_center.x))> 10){
//                cout << "第2列缺少2个" << endl;
//                orderY(quads, 5, 3);
////                for(int i = 0; i<quads.size(); i++){
////                    cout << "quad" << i << quads[i].m_center << endl;
////                }
//                insertTenQuad(quads, 8, 9);
//            }
//        }
        return true;
    }

    bool QuadExtractor::runSmallBuff(cv::Mat& frame, std::vector<Quad>& quadsNine,std::vector<Quad>& quadsTen) {
        //Step1. Detect quads by AprilTag
        // Initialize tag detector with options
        double start = (double)cv::getTickCount();
        if(frame.empty()){
            cout<<"Extractor: get frame failed"<<endl;
            return false;
        }

        apriltag_detector_t *td = apriltag_detector_create();
        td->debug = false;
        td->nthreads = 20;

        cv::Mat image, gray;
        double t1 = (double)cv::getTickCount();
        image = frame.clone();

//        medianBlur(image,image,5);
//        GaussianBlur(image,image,Size(5, 5),1);
//        imshow("image blur",image);
        Mat edgeImg;
        Canny(image,edgeImg, 500, 1500,5);
//        imshow("canny edge",edgeImg);

        ///去激光点影响 参数待定
        vector<vector<Point>> contours;
        vector<Vec4i> hierarchy;
        findContours(edgeImg,contours,hierarchy, RETR_LIST,CV_CHAIN_APPROX_SIMPLE);
        for(auto i=0;i<contours.size();i++){
            if (contourArea(contours[i])<10)
            {
                drawContours(image,contours,i,Scalar(0,0,0),2,CV_FILLED);
//                drawContours(edgeImg,contours,i,Scalar(0,0,0),2,CV_FILLED);
//                cout<<"fill the hole with black "<<i<<":"<<contours[i]<<endl;
            }
        }
//       imshow("fill_hole",image);

        //apriltag_param
        //多次膨胀
        cv::Mat element = getStructuringElement(MORPH_RECT, Size(dilate_ksize, dilate_ksize));
        dilate(image, image, element, Point(-1,-1), dilate1_n);
                cout << "1" << endl;

#ifdef DEBUG_QUAD
        imshow("raw_frame", frame);
        imshow("image_dilate1",image);
#endif
//        GaussianBlur(image,image,Size(1,1),3);
//        medianBlur(image, image, 1);
        cvtColor(image, gray, COLOR_BGR2GRAY);
        dilate(gray, gray, element, Point(-1,-1), dilate2_n);
        erode(gray, gray, element, Point(-1,-1), erode1_n);

//        dilate(image, image, Mat(), Point(-1,-1), 1);
//        erode(image, image, Mat(), Point(-1,-1), 1);
//        cvtColor(image, gray, COLOR_BGR2GRAY);

#ifdef DEBUG_QUAD
        imshow("gray",gray);
//           int blackPoints = 0;
//        for(int i = 0; i < gray.rows; i++){
//            for(int j = 0; j < gray.cols; j++){
//                double pixelVal = gray.at<uchar>(i, j);
//                if(pixelVal > 140)
//                    blackPoints++;
//            }
//        }

//        double blackPointsRatio;
//        blackPointsRatio = blackPoints/((double)gray.rows*gray.cols);
//        cout << "blackPoints is " << blackPoints << endl << "image size is "<< gray.rows*gray.cols<< endl;
//        cout << "blackPointsRatio is " << blackPointsRatio << endl;
#endif
        gray = 255-gray;//white background to black background

        vector<vector<Point>> contoursAprilTag;
        vector<Vec4i> hierarchyAprilTag;
        findContours(gray,contoursAprilTag,hierarchyAprilTag, RETR_LIST,CV_CHAIN_APPROX_SIMPLE);

        cout <<"contours size before apriltag is: " << contoursAprilTag.size() << endl;
        //imwrite("grayImg.jpg", gray);
        //imshow("grayImg", gray);
        //waitKey(10);


        // Make an image_u8_t header for the Mat data
        image_u8_t im = { .width = gray.cols,
                .height = gray.rows,
                .stride = gray.cols,
                .buf = gray.data
        };
        cout << "1.5" << endl;

        zarray_t *detections = apriltag_detector_detect_Quads(td, &im);

        cout << "2" << endl;
        double t2=((double)cv::getTickCount()-t1)*1000/getTickFrequency();
#ifdef DEBUG_QUAD_TIME
        cout<<"Extractor: detect tags time is: "<<t2<<"ms"<<endl;
        out<<"-------------------------start------------------------------"<<endl;
        out<<"Extractor: detect tags time is: "<<t2<<"ms"<<endl;
#endif

        if(zarray_size(detections) < 8) {
            cout << "Extractor: no buff" << endl;
            return false;
        }
#ifdef DEBUG_QUAD
        //Draw the quads detected
        cv::Mat aprilTagImg = frame.clone();
        for (int i = 0; i < zarray_size(detections); i++) {
            struct quad *quad;
            zarray_get_volatile(detections, i, &quad);

            line(aprilTagImg, Point2f(quad->p[0][0], quad->p[0][1]), Point2f(quad->p[1][0], quad->p[1][1]), Scalar(0, 0xff, 0),2);
            line(aprilTagImg, Point2f(quad->p[1][0], quad->p[1][1]), Point2f(quad->p[2][0], quad->p[2][1]), Scalar(0, 0xff, 0), 2);
            line(aprilTagImg, Point2f(quad->p[2][0], quad->p[2][1]), Point2f(quad->p[3][0], quad->p[3][1]), Scalar(0, 0xff, 0), 2);
            line(aprilTagImg, Point2f(quad->p[3][0], quad->p[3][1]), Point2f(quad->p[0][0], quad->p[0][1]), Scalar(0, 255, 0), 2);
        }
        imshow("aprilTagImg", aprilTagImg);
#endif

        //Step.2 Using Quad class to save AprilTag result and
        //       filter quads by area and angle
        t1 = (double)cv::getTickCount();
        vector<Quad> quads;

        //filter by min area
        for (int i = 0; i < zarray_size(detections); i++) {
            struct quad *quad;
            Quad myQuad;
            zarray_get_volatile(detections, i, &quad);

            myQuad.up_left.x = quad->p[3][0];
            myQuad.up_left.y = quad->p[3][1];
            myQuad.up_right.x = quad->p[0][0];
            myQuad.up_right.y = quad->p[0][1];
            myQuad.down_right.x = quad->p[1][0];
            myQuad.down_right.y = quad->p[1][1];
            myQuad.down_left.x = quad->p[2][0];
            myQuad.down_left.y = quad->p[2][1];

            myQuad.m_center.x = (myQuad.up_left.x + myQuad.down_right.x)/2;
            myQuad.m_center.y = (myQuad.up_left.y + myQuad.down_right.y)/2;

            myQuad.m_width = (distance(myQuad.up_left, myQuad.up_right) + distance(myQuad.down_left, myQuad.down_right))/2;
            myQuad.m_height = (distance(myQuad.up_left, myQuad.down_left) + distance(myQuad.up_right, myQuad.down_right))/2;

            myQuad.counters.push_back(myQuad.up_left);
            myQuad.counters.push_back(myQuad.up_right);
            myQuad.counters.push_back(myQuad.down_right);
            myQuad.counters.push_back(myQuad.down_left);

            //Get area by method in AprilTag
            float area = 0;
            // get area of triangle formed by points 0, 1, 2, 0
            double length[3], p;
            for (int i = 0; i < 3; i++) {
                int idxa = i; // 0, 1, 2,
                int idxb = (i + 1) % 3; // 1, 2, 0
                length[i] = sqrt(powf((quad->p[idxb][0] - quad->p[idxa][0]),2) +
                                 powf((quad->p[idxb][1] - quad->p[idxa][1]),2));
            }
            p = (length[0] + length[1] + length[2]) / 2;

            area += sqrt(p * (p - length[0]) * (p - length[1]) * (p - length[2]));

            // get area of triangle formed by points 2, 3, 0, 2
            for (int i = 0; i < 3; i++) {
                int idxs[] = {2, 3, 0, 2};
                int idxa = idxs[i];
                int idxb = idxs[i + 1];
                length[i] = sqrt(powf((quad->p[idxb][0] - quad->p[idxa][0]),2) +
                                 powf((quad->p[idxb][1] - quad->p[idxa][1]),2));
            }
            p = (length[0] + length[1] + length[2]) / 2;
            area += sqrt(p * (p - length[0]) * (p - length[1]) * (p - length[2]));
            myQuad.area = area;
//            cout << "area: "<< area <<endl;

            if (myQuad.area > min_area){
                quads.push_back(myQuad);
            }
        }

#ifdef DEBUG_QUAD
        cv::Mat filterAreaImg = frame.clone();
        drawQuads(quads, filterAreaImg);
        imshow("filterAreaImg ", filterAreaImg);
#endif

        //filter by ratio
        for(vector<Quad>::iterator it = quads.begin(); it != quads.end();){
            Quad& myQuad  = *it;

            float ratio = myQuad.m_width/myQuad.m_height;
            if(ratio > min_ratio && ratio < max_ratio){
                it++;
            } else {
                quads.erase(it);
            }
        }

#ifdef DEBUG_QUAD
        cv::Mat filterRatioImg = frame.clone();
        drawQuads(quads, filterRatioImg);
        imshow("filterRatioImg ", filterRatioImg);
#endif
        //filter by oppo side
        for(vector<Quad>::iterator it = quads.begin(); it != quads.end();){
            Quad& myQuad  = *it;

            float left_side, right_side, up_side, down_side;
            left_side = distance(myQuad.up_left, myQuad.down_left);
            right_side = distance(myQuad.up_right, myQuad.down_right);
            up_side = distance(myQuad.up_left, myQuad.up_right);
            down_side = distance(myQuad.down_left, myQuad.down_right);

            float error1 = fabs(left_side-right_side)/max(left_side, right_side);
            float error2 = fabs(up_side-down_side)/max(up_side, down_side);

#ifdef DEBUG_QUAD
            //cout << "quads error1 " << error1 << endl;
            //cout << "quads error2 " << error2 << endl;
#endif

            if((error1 < oppo_side_error)&&(error2 < oppo_side_error)){
                it++;
            } else {
                quads.erase(it);
            }
        }

#ifdef DEBUG_QUAD
        cv::Mat filterOppoSideImg = frame.clone();
        drawQuads(quads, filterOppoSideImg);
        imshow("filterOppoSideImg ", filterOppoSideImg);
#endif

        //filter by angle
        for(vector<Quad>::iterator it = quads.begin(); it != quads.end();){
            Quad& myQuad  = *it;

            float angles[4];
            int n = 0;
            angles[0] = fabs(angle(myQuad.up_right, myQuad.down_left, myQuad.up_left));
            angles[1] = fabs(angle(myQuad.up_left, myQuad.down_right, myQuad.up_right));
            angles[2] = fabs(angle(myQuad.up_right, myQuad.down_left, myQuad.down_right));
            angles[3] = fabs(angle(myQuad.up_left, myQuad.down_right, myQuad.down_left));

            for (int i = 0; i < 4; i++) {
                if (angles[i] < min_angle)
                    n++;
            }

#ifdef DEBUG_QUAD
            for (int i = 0; i < 4; i++) {
//                cout << "angle " << i << " "<< angles[i] << endl;
            }
            //cout << "n: "<< n << endl;
#endif
            if(n > 2){
                it++;
            } else {
                quads.erase(it);
            }
        }

#ifdef DEBUG_QUAD
        cv::Mat filterAngleImg = frame.clone();
        drawQuads(quads, filterAngleImg);
        imshow("filterAngleImg ", filterAngleImg);
        cout << "Extractor: filter quad size is " << quads.size() << endl;
#endif

        zarray_destroy(detections);
        apriltag_detector_destroy(td);
        t2=((double)cv::getTickCount()-t1)*1000/getTickFrequency();

#ifdef DEBUG_QUAD_TIME
        cout<<"Extractor: filter quads time is "<<t2<<"ms"<<endl;
        out<<"Extractor: filter quads time is "<<t2<<"ms"<<endl;
#endif
        //Step3. Classify quads
        t1 = cv::getTickCount();
        vector<int> labels;
        partition(quads, labels,isEqualSmallBuff);
//        cout << "labels size: " << labels.size() <<endl;
//        for (int i=0; i<labels.size(); i++)
//            cout <<"lable: "<< labels[i] <<" ";
//        cout << endl;

        vector<vector<Quad>> quadsClassified;
        vector<Quad> newQuads;
        int search_flag[40] = { 0 };
        for (int i = 0; i < labels.size(); i++) {
            if (search_flag[i]) continue;
            newQuads.clear();
            search_flag[i] = 1;
            newQuads.push_back(quads[i]);
            for (int j = 0; j < labels.size(); j++) {
                if (search_flag[j]) continue;
                if (labels[i] == labels[j]) {
                    newQuads.push_back(quads[j]);
                    search_flag[j] = 1;
                }
            }
            quadsClassified.push_back(newQuads);
        }

        t2=((double)cv::getTickCount()-t1)*1000/getTickFrequency();
#ifdef DEBUG_QUAD_TIME
        cout<<"Extractor: classify quads time is "<<t2<<"ms"<<endl;
        out<<"Extractor: classify quads time is "<<t2<<"ms"<<endl;
#endif
        //Step4.Find the first and second larger quads
        t1 = getTickCount();
        if(quadsClassified.size() > 1){
            //order by size
            for (int i = 0; i<quadsClassified.size(); i++){
                for (int j = i+1; j<quadsClassified.size(); j++){
                    if (quadsClassified[i].size() < quadsClassified[j].size())
                        swap(quadsClassified[i], quadsClassified[j]);
                }
            }

#ifdef DEBUG_QUAD
            cout <<"Extractor: quadsClassified size is " << quadsClassified.size() <<endl;
            for (int i = 0; i<quadsClassified.size(); i++){
                cout <<"Extractor: quads" << i << "size is " << quadsClassified[i].size() <<endl;
            }
#endif

#ifdef DEBUG_QUAD_CLASSIFIDE
        cv::Mat image_quads[quadsClassified.size()];
        for (int i = 0; i<quadsClassified.size(); i++){
            image_quads[i] = image.clone();
            drawQuads(quadsClassified[i], image_quads[i]);
            stringstream cc;
            cc << "quadsClassified" << i;
            cc.str();
            imshow(cc.str(), image_quads[i]);
        }
#endif
            float totalArea1 = 0;
            float totalArea2 = 0;

            for (int i = 0; i<quadsClassified[0].size(); i++){
                totalArea1 += quadsClassified[0][i].area;
            }
            for (int i = 0; i<quadsClassified[1].size(); i++){
                totalArea2 += quadsClassified[1][i].area;
            }
            float firstArea = totalArea1 /quadsClassified[0].size();
            float secondArea = totalArea2 /quadsClassified[1].size();

            if(quadsClassified.size() > 2){
                float totalArea3 = 0;
                for (int i = 0; i<quadsClassified[2].size(); i++){
                    totalArea3 += quadsClassified[2][i].area;
                }
                float thirdArea = totalArea3 /quadsClassified[2].size();

                if ((thirdArea - secondArea) > 10){
                    swap(quadsClassified[1],quadsClassified[2]);
                    secondArea = thirdArea;
                }
            }
//            cout <<"firstArea" <<firstArea<<endl;
//            cout <<"secondArea" <<secondArea<<endl;

            //avoid choosing large quad
            if(quadsClassified[0].size() - quadsClassified[1].size() > 5){
                quadsNine.assign(quadsClassified[0].begin(), quadsClassified[0].end());
            } else {
                if (firstArea-secondArea > 10)
                    quadsNine.assign(quadsClassified[0].begin(), quadsClassified[0].end());
                else quadsNine.assign(quadsClassified[1].begin(), quadsClassified[1].end());
            }
        }
        else if (quadsClassified.size() == 1) {
            quadsNine.assign(quadsClassified[0].begin(), quadsClassified[0].end());
        }
        else {
            cout << "Extractor: quadsClassified failed!!!" << endl;
            return false;
        }

        if(fabs(functionFlag - lastFunctionFlag) == 0){
        if(quadsNine.size() > 7){
            if((lastQuadsNine.size() == 9) && (lastQuadArea > 0)){
                for(vector<Quad>::iterator it = quadsNine.begin(); it != quadsNine.end();){
                    Quad& quadTmp  = *it;
                    bool isRightQuad = false;
//                cout << quadTmp.m_center << endl;
//                cout << lastQuadsNine[8].down_right << endl;
//                cout << lastQuadsNine[0].up_left << endl;
                    if ((quadTmp.m_center.x < lastQuadsNine[8].down_right.x+50 && quadTmp.m_center.x > lastQuadsNine[0].up_left.x-50)
                        && (quadTmp.m_center.y < lastQuadsNine[8].down_right.y+50 && quadTmp.m_center.y > lastQuadsNine[0].up_left.y-50)){
//                    cout << "last quads area: " << lastQuadArea << endl;
//                    cout << "this quad area: " << quadTmp.area<< endl;
//                    cout << "now area and last area: " <<fabs(quadTmp.area - lastQuadArea) << endl;
                        if(fabs(quadTmp.area - lastQuadArea) /lastQuadArea < last_now_area_error)
                            isRightQuad = true;
                    }
                    if(isRightQuad){
                        it++;
                    } else {
                        quadsNine.erase(it);
                    }
                }
            }
        }
        }


#ifdef DEBUG_QUAD
        cout << "Extractor: filter after comparing with lastQuadsNine size: " << quadsNine.size() << endl;
        cv::Mat imageQuadsNine = frame.clone();
        drawQuads(quadsNine, imageQuadsNine);
        imshow("quadsNine", imageQuadsNine);
#endif

        //step3.5. avoid quads away from center
        //averagewidth & averageheight
        if(fabs(functionFlag - lastFunctionFlag) == 0){
                 cout<<"quadssize before away"<<quadsNine.size()<<endl;
        averageWidth=0;
        averageHeight=0;
        for(int i=0;i<quadsNine.size();i++)
        {
            averageWidth= averageWidth+quadsNine[i].m_width;
            averageHeight= averageHeight+quadsNine[i].m_height;
        }
        averageWidth= averageWidth/quadsNine.size();
        averageHeight= averageHeight/quadsNine.size();

//avoid quads away from center
        if(quadsNine.size() > 9){
            if((lastQuadNineCenter.x!=0) && (lastQuadNineCenter.y!=0)){
                for(vector<Quad>::iterator it = quadsNine.begin(); it != quadsNine.end();){
                    Quad& quadTmp  = *it;
                    bool isRightQuad2 = false;
                    if ((fabs(lastQuadNineCenter.x-quadTmp.m_center.x)<averageWidth*2)
                        &&(fabs(lastQuadNineCenter.y-quadTmp.m_center.y)<averageHeight*2)){
                        isRightQuad2 = true;
                    }
                    if(isRightQuad2){
                        it++;
                        cout<<"fabs(lastQuadallcenter.x-quadTmp.m_center.x)<averagewidth*2 ="<<fabs(lastQuadNineCenter.x-quadTmp.m_center.x)/(averageWidth*2)<<endl;
                        cout<<"fabs(lastQuadallcenter.y-quadTmp.m_center.y)<averageheight*2 = "<<fabs(lastQuadNineCenter.y-quadTmp.m_center.y)/(averageHeight*2)<<endl;
                    } else {
                        cout<<"fabs(lastQuadallcenter.x-quadTmp.m_center.x)<averagewidth*2 ="<<fabs(lastQuadNineCenter.x-quadTmp.m_center.x)/(averageWidth*2)<<endl;
                        cout<<"fabs(lastQuadallcenter.y-quadTmp.m_center.y)<averageheight*2 = "<<fabs(lastQuadNineCenter.y-quadTmp.m_center.y)/(averageHeight*2)<<endl;
                        quadsNine.erase(it);

                    }
                }
            }
        }

        }


        //lastQuadallcenter为上一次平均中点
        lastQuadNineCenter.x=0;
        lastQuadNineCenter.y=0;
        for(int i=0;i<quadsNine.size();i++)
        {
            lastQuadNineCenter.x= lastQuadNineCenter.x+quadsNine[i].m_center.x;
            lastQuadNineCenter.y= lastQuadNineCenter.y+quadsNine[i].m_center.y;
        }
        lastQuadNineCenter.x= lastQuadNineCenter.x/quadsNine.size();
        lastQuadNineCenter.y= lastQuadNineCenter.y/quadsNine.size();

        cout<<"quadssize after away"<<quadsNine.size()<<endl;

        //Step4. Order and complete nine quads
        ///complete quads which size is 7 or 8
        if (quadsNine.size() < 10){
            if(!completeQuads(quadsNine)){
                cout << "Extractor: completeQuads failed!!!" << endl;
                lastQuadArea=0;
                lastQuadNineCenter.x=0;
                lastQuadNineCenter.y=0;
                return false;
            }
        }

        if(quadsNine.size() == 9){
            lastQuadsNine.clear();
            lastQuadsNine.assign(quadsNine.begin(),quadsNine.end());
            lastQuadArea = 0;
            for(int i = 0; i < quadsNine.size(); i++){
                lastQuadArea += quadsNine[i].area;
            }
            lastQuadArea = lastQuadArea/quadsNine.size();
        }
        //quadsTen
        if(!getImgQuad2(quadsNine, quadsTen)){
                    cout << "Extractor: getImgQuad2 failed" << endl;
                    return false;
                }
#ifdef DEBUG_QUADNINE
        cout <<"Extractor: final quadsTen size is " << quadsTen.size() <<endl;
        cv::Mat imageQuadsTen = frame.clone();
        drawQuads(quadsTen, imageQuadsTen);
        imshow("quadsTen_0701", imageQuadsTen);
#endif


#ifdef DEBUG_QUADNINE
        cout <<"Extractor: final quadsNine size is " << quadsNine.size() <<endl;
        cv::Mat imageQuadsNine1 = frame.clone();
        drawQuads(quadsNine, imageQuadsNine1);
        imshow("quadsNine1 ", imageQuadsNine1);
#endif

        t2=((double)cv::getTickCount()-t1)*1000/getTickFrequency();

        double start_to_end =((double)cv::getTickCount()-start)*1000/getTickFrequency();

#ifdef DEBUG_QUAD_TIME
        cout<<"Extractor: complete quadsNine time is "<<t2<<"ms"<<endl;
        out<<"Extractor: complete quadsNine time is "<<t2<<"ms"<<endl;
        out<<"Extractor: total time is "<<start_to_end<<"ms"<<endl;
        out<<"-------------------------end------------------------------"<<endl;
#endif
        return true;
    }

    bool QuadExtractor::runBigBuff(cv::Mat& frame, std::vector<Quad>& quadsTen, int index[5]) {
        //Step1. Detect quads by AprilTag
        // Initialize tag detector with options
        double start = (double)cv::getTickCount();
        if(frame.empty()){
            cout<<"Extractor: get frame failed"<<endl;
            return false;
        }
        cout<<"quadextractor: apriltag error No.-3"<<endl;
        apriltag_detector_t *td = apriltag_detector_create();
        td->debug = false;
        td->nthreads = 12;

        cv::Mat image, gray;
        double t1 = (double)cv::getTickCount();
        image = frame.clone();

//        medianBlur(image,image,5);
//        GaussianBlur(image,image,Size(5, 5),1);
//        imshow("image blur",image);
        Mat edgeImg;
        Canny(image,edgeImg, 500, 1500,5);
//        imshow("canny edge",edgeImg);
        cout<<"quadextractor: apriltag error No.-2"<<endl;

        ///去激光点影响 参数待定
        vector<vector<Point>> contours;
        vector<Vec4i> hierarchy;
        findContours(edgeImg,contours,hierarchy, RETR_LIST,CV_CHAIN_APPROX_SIMPLE);
        for(auto i=0;i<contours.size();i++){
            if (contourArea(contours[i])<10)
            {
                drawContours(image,contours,i,Scalar(0,0,0),2,CV_FILLED);
                drawContours(edgeImg,contours,i,Scalar(0,0,0),2,CV_FILLED);
//                cout<<"fill the hole with black "<<i<<":"<<contours[i]<<endl;
            }
        }
//       imshow("fill_hole",image);

        //apriltag_param
        //多次膨胀
        cout<<"quadextractor: apriltag error No.-1"<<endl;
        cv::Mat element = getStructuringElement(MORPH_RECT, Size(dilate_ksize, dilate_ksize));
        dilate(image, image, element, Point(-1,-1), dilate1_n);
#ifdef DEBUG_QUAD
        imshow("raw_frame", frame);
        imshow("image_dilate1",image);
#endif
//        GaussianBlur(image,image,Size(1,1),3);
//        medianBlur(image, image, 1);
        cvtColor(image, gray, COLOR_BGR2GRAY);
        dilate(gray, gray, element, Point(-1,-1), dilate2_n);
        erode(gray, gray, element, Point(-1,-1), erode1_n);
//        dilate(image, image, Mat(), Point(-1,-1), 1);
//        erode(image, image, Mat(), Point(-1,-1), 1);
//        cvtColor(image, gray, COLOR_BGR2GRAY);

#ifdef DEBUG_QUAD
        imshow("gray",gray);
//           int blackPoints = 0;
//        for(int i = 0; i < gray.rows; i++){
//            for(int j = 0; j < gray.cols; j++){
//                double pixelVal = gray.at<uchar>(i, j);
//                if(pixelVal > 140)
//                    blackPoints++;
//            }
//        }
//
//        double blackPointsRatio;
//        blackPointsRatio = blackPoints/((double)gray.rows*gray.cols);
//        cout << "blackPoints is " << blackPoints << endl << "image size is "<< gray.rows*gray.cols<< endl;
//        cout << "blackPointsRatio is " << blackPointsRatio << endl;
#endif
        cout<<"quadextractor: apriltag error No.0"<<endl;
        gray = 255-gray;//white background to black background
        // Make an image_u8_t header for the Mat data
        image_u8_t im = { .width = gray.cols,
                .height = gray.rows,
                .stride = gray.cols,
                .buf = gray.data
        };
        cout<<"quadextractor: apriltag error No.1"<<endl;
        zarray_t *detections = apriltag_detector_detect_Quads(td, &im);
        cout<<"quadextractor: apriltag error No.2"<<endl;
        double t2=((double)cv::getTickCount()-t1)*1000/getTickFrequency();
#ifdef DEBUG_QUAD_TIME
        cout<<"Extractor: detect tags time is: "<<t2<<"ms"<<endl;
        out<<"-------------------------start------------------------------"<<endl;
        out<<"Extractor: detect tags time is: "<<t2<<"ms"<<endl;
#endif

        if(zarray_size(detections) < 8) {
            cout << "Extractor: no buff" << endl;
            return false;
        }
        cout<<"quadextractor: apriltag error No.3"<<endl;
#ifdef DEBUG_QUAD
        //Draw the quads detected
        cv::Mat aprilTagImg = frame.clone();
        for (int i = 0; i < zarray_size(detections); i++) {
            struct quad *quad;
            zarray_get_volatile(detections, i, &quad);

            line(aprilTagImg, Point2f(quad->p[0][0], quad->p[0][1]), Point2f(quad->p[1][0], quad->p[1][1]), Scalar(0, 0xff, 0),2);
            line(aprilTagImg, Point2f(quad->p[1][0], quad->p[1][1]), Point2f(quad->p[2][0], quad->p[2][1]), Scalar(0, 0xff, 0), 2);
            line(aprilTagImg, Point2f(quad->p[2][0], quad->p[2][1]), Point2f(quad->p[3][0], quad->p[3][1]), Scalar(0, 0xff, 0), 2);
            line(aprilTagImg, Point2f(quad->p[3][0], quad->p[3][1]), Point2f(quad->p[0][0], quad->p[0][1]), Scalar(0, 255, 0), 2);
        }
        imshow("aprilTagImg", aprilTagImg);
#endif

        //Step.2 Using Quad class to save AprilTag result and
        //       filter quads by area and angle
        t1 = (double)cv::getTickCount();
        vector<Quad> quads;

        //filter by min area
        for (int i = 0; i < zarray_size(detections); i++) {
            struct quad *quad;
            Quad myQuad;
            zarray_get_volatile(detections, i, &quad);

            myQuad.up_left.x = quad->p[3][0];
            myQuad.up_left.y = quad->p[3][1];
            myQuad.up_right.x = quad->p[0][0];
            myQuad.up_right.y = quad->p[0][1];
            myQuad.down_right.x = quad->p[1][0];
            myQuad.down_right.y = quad->p[1][1];
            myQuad.down_left.x = quad->p[2][0];
            myQuad.down_left.y = quad->p[2][1];

            myQuad.m_center.x = (myQuad.up_left.x + myQuad.down_right.x)/2;
            myQuad.m_center.y = (myQuad.up_left.y + myQuad.down_right.y)/2;

            myQuad.m_width = (distance(myQuad.up_left, myQuad.up_right) + distance(myQuad.down_left, myQuad.down_right))/2;
            myQuad.m_height = (distance(myQuad.up_left, myQuad.down_left) + distance(myQuad.up_right, myQuad.down_right))/2;

            myQuad.counters.push_back(myQuad.up_left);
            myQuad.counters.push_back(myQuad.up_right);
            myQuad.counters.push_back(myQuad.down_right);
            myQuad.counters.push_back(myQuad.down_left);

            //Get area by method in AprilTag
            float area = 0;
            // get area of triangle formed by points 0, 1, 2, 0
            double length[3], p;
            for (int i = 0; i < 3; i++) {
                int idxa = i; // 0, 1, 2,
                int idxb = (i + 1) % 3; // 1, 2, 0
                length[i] = sqrt(powf((quad->p[idxb][0] - quad->p[idxa][0]),2) +
                                 powf((quad->p[idxb][1] - quad->p[idxa][1]),2));
            }
            p = (length[0] + length[1] + length[2]) / 2;

            area += sqrt(p * (p - length[0]) * (p - length[1]) * (p - length[2]));

            // get area of triangle formed by points 2, 3, 0, 2
            for (int i = 0; i < 3; i++) {
                int idxs[] = {2, 3, 0, 2};
                int idxa = idxs[i];
                int idxb = idxs[i + 1];
                length[i] = sqrt(powf((quad->p[idxb][0] - quad->p[idxa][0]),2) +
                                 powf((quad->p[idxb][1] - quad->p[idxa][1]),2));
            }
            p = (length[0] + length[1] + length[2]) / 2;
            area += sqrt(p * (p - length[0]) * (p - length[1]) * (p - length[2]));
            myQuad.area = area;
//            cout << "area: "<< area <<endl;

            if (myQuad.area > min_area*k_min_area){
                quads.push_back(myQuad);
            }
        }

#ifdef DEBUG_QUAD
        cv::Mat filterAreaImg = frame.clone();
        drawQuads(quads, filterAreaImg);
        imshow("filterAreaImg ", filterAreaImg);
#endif

        //filter by ratio
        for(vector<Quad>::iterator it = quads.begin(); it != quads.end();){
            Quad& myQuad  = *it;

            float ratio = myQuad.m_width/myQuad.m_height;
            if(ratio > min_ratio && ratio < max_ratio){
                it++;
            } else {
                quads.erase(it);
            }
        }

#ifdef DEBUG_QUAD
        cv::Mat filterRatioImg = frame.clone();
        drawQuads(quads, filterRatioImg);
        imshow("filterRatioImg ", filterRatioImg);
#endif
        //filter by oppo side
        for(vector<Quad>::iterator it = quads.begin(); it != quads.end();){
            Quad& myQuad  = *it;

            float left_side, right_side, up_side, down_side;
            left_side = distance(myQuad.up_left, myQuad.down_left);
            right_side = distance(myQuad.up_right, myQuad.down_right);
            up_side = distance(myQuad.up_left, myQuad.up_right);
            down_side = distance(myQuad.down_left, myQuad.down_right);

            float error1 = fabs(left_side-right_side)/max(left_side, right_side);
            float error2 = fabs(up_side-down_side)/max(up_side, down_side);

#ifdef DEBUG_QUAD
            cout << "quads error1 " << error1 << endl;
            cout << "quads error2 " << error2 << endl;
#endif

            if((error1 < oppo_side_error)&&(error2 < oppo_side_error)){
                it++;
            } else {
                quads.erase(it);
            }
        }

#ifdef DEBUG_QUAD
        cv::Mat filterOppoSideImg = frame.clone();
        drawQuads(quads, filterOppoSideImg);
        imshow("filterOppoSideImg ", filterOppoSideImg);
#endif

        //filter by angle
        for(vector<Quad>::iterator it = quads.begin(); it != quads.end();){
            Quad& myQuad  = *it;

            float angles[4];
            int n = 0;
            angles[0] = fabs(angle(myQuad.up_right, myQuad.down_left, myQuad.up_left));
            angles[1] = fabs(angle(myQuad.up_left, myQuad.down_right, myQuad.up_right));
            angles[2] = fabs(angle(myQuad.up_right, myQuad.down_left, myQuad.down_right));
            angles[3] = fabs(angle(myQuad.up_left, myQuad.down_right, myQuad.down_left));

            for (int i = 0; i < 4; i++) {
                if (angles[i] < min_angle)
                    n++;
            }

#ifdef DEBUG_QUAD
            for (int i = 0; i < 4; i++) {
//                cout << "angle " << i << " "<< angles[i] << endl;
            }
            cout << "n: "<< n << endl;
#endif
            if(n > 2){
                it++;
            } else {
                quads.erase(it);
            }
        }

#ifdef DEBUG_QUAD
        cv::Mat filterAngleImg = frame.clone();
        drawQuads(quads, filterAngleImg);
        imshow("filterAngleImg ", filterAngleImg);
        cout << "Extractor: filter quad size is " << quads.size() << endl;
#endif

        zarray_destroy(detections);
        apriltag_detector_destroy(td);
        t2=((double)cv::getTickCount()-t1)*1000/getTickFrequency();

#ifdef DEBUG_QUAD_TIME
        cout<<"Extractor: filter quads time is "<<t2<<"ms"<<endl;
        out<<"Extractor: filter quads time is "<<t2<<"ms"<<endl;
#endif
        //Step3. Classify quads
        t1 = cv::getTickCount();
        vector<int> labels;
        partition(quads, labels,isEqualBigBuff);
//        cout << "labels size: " << labels.size() <<endl;
//        for (int i=0; i<labels.size(); i++)
//            cout <<"lable: "<< labels[i] <<" ";
//        cout << endl;

        vector<vector<Quad>> quadsClassified;
        vector<Quad> newQuads;
        int search_flag[40] = { 0 };
        for (int i = 0; i < labels.size(); i++) {
            if (search_flag[i]) continue;
            newQuads.clear();
            search_flag[i] = 1;
            newQuads.push_back(quads[i]);
            for (int j = 0; j < labels.size(); j++) {
                if (search_flag[j]) continue;
                if (labels[i] == labels[j]) {
                    newQuads.push_back(quads[j]);
                    search_flag[j] = 1;
                }
            }
            quadsClassified.push_back(newQuads);
        }

        t2=((double)cv::getTickCount()-t1)*1000/getTickFrequency();
#ifdef DEBUG_QUAD_TIME
        cout<<"Extractor: classify quads time is "<<t2<<"ms"<<endl;
        out<<"Extractor: classify quads time is "<<t2<<"ms"<<endl;
#endif
        //Step4.Find the first and second larger quads
        t1 = getTickCount();
        if(quadsClassified.size() > 1){
            //order by size
            for (int i = 0; i<quadsClassified.size(); i++){
                for (int j = i+1; j<quadsClassified.size(); j++){
                    if (quadsClassified[i].size() < quadsClassified[j].size())
                        swap(quadsClassified[i], quadsClassified[j]);
                }
            }

#ifdef DEBUG_QUAD
            cout <<"Extractor: quadsClassified size is " << quadsClassified.size() <<endl;
            for (int i = 0; i<quadsClassified.size(); i++){
                cout <<"Extractor: quads" << i << "size is " << quadsClassified[i].size() <<endl;
            }
#endif

#ifdef DEBUG_QUAD_CLASSIFIDE
        cv::Mat image_quads[quadsClassified.size()];
        for (int i = 0; i<quadsClassified.size(); i++){
            image_quads[i] = image.clone();
            drawQuads(quadsClassified[i], image_quads[i]);
            stringstream cc;
            cc << "quadsClassified"<< i;
            cc.str();
            imshow(cc.str(), image_quads[i]);
        }
#endif
            float totalArea1 = 0;
            float totalArea2 = 0;

            for (int i = 0; i<quadsClassified[0].size(); i++){
                totalArea1 += quadsClassified[0][i].area;
            }
            for (int i = 0; i<quadsClassified[1].size(); i++){
                totalArea2 += quadsClassified[1][i].area;
            }
            float firstArea = totalArea1 /quadsClassified[0].size();
            float secondArea = totalArea2 /quadsClassified[1].size();

            if(quadsClassified.size() > 2){
                float totalArea3 = 0;
                for (int i = 0; i<quadsClassified[2].size(); i++){
                    totalArea3 += quadsClassified[2][i].area;
                }
                float thirdArea = totalArea3 /quadsClassified[2].size();

                if(((thirdArea - secondArea) > 10)&&(quadsClassified[2].size()>=4)){
                    swap(quadsClassified[1],quadsClassified[2]);
                    secondArea = thirdArea;
                }
            }
//            cout <<"firstArea" <<firstArea<<endl;
//            cout <<"secondArea" <<secondArea<<endl;

            //avoid choosing large quad
//            if(quadsClassified[0].size() - quadsClassified[1].size() > 5){
//                quadsTen.assign(quadsClassified[0].begin(), quadsClassified[0].end());
//            }
           if((quadsClassified[0].size()>=4)&&(quadsClassified[1].size()>=4)){
                 cout<<"QuadExtractor: quadsClassified[0] and quadsClassified[1] are integrated"<<endl;
                 quadsTen.clear();
                 //cout<<"extractor: integrated error No.1"<<endl;
                 for(int i=0;i<quadsClassified[0].size();i++){
                 quadsTen.push_back(quadsClassified[0][i]);
                 }
                 for(int i=0;i<quadsClassified[1].size();i++){
                 quadsTen.push_back(quadsClassified[1][i]);
                 }
            }
            //cout<<"extractor: integrated error No.2"<<endl;
//            else {
//                if (firstArea-secondArea > 10)
//                   quadsTen.assign(quadsClassified[1].begin(), quadsClassified[1].end());
//                else quadsTen.assign(quadsClassified[0].begin(), quadsClassified[0].end());
//            }
        }
//        else if (quadsClassified.size() == 1) {
//            quadsTen.assign(quadsClassified[0].begin(), quadsClassified[0].end());
//
//        }
//        else {
//            cout << "Extractor: quadsClassified failed!!!" << endl;
//            return false;
//        }
//last
        if(fabs(functionFlag - lastFunctionFlag) == 0){
        if(quadsTen.size() > 10){
            if((lastQuadsTen.size() == 10) && (lastQuadstenArea > 0)){
                for(vector<Quad>::iterator it =quadsTen.begin(); it != quadsTen.end();){
                    Quad& quadTmp  = *it;
                    bool isRightQuad = false;
                //cout << quadTmp.m_center << endl;
                //cout << lastQuadsTen[8].down_right << endl;
                //cout << lastQuadsTen[0].up_left << endl;
              if ((quadTmp.m_center.x < lastQuadsTen[9].down_right.x+50 && quadTmp.m_center.x > lastQuadsTen[0].up_left.x-50)
                        && (quadTmp.m_center.y < lastQuadsTen[9].down_right.y+50 && quadTmp.m_center.y > lastQuadsTen[0].up_left.y-50)){
                    //cout << "last quads area: " << lastQuadstenArea << endl;
                    //cout << "this quad area: " << quadTmp.area<< endl;
                    //cout << "now area and last area: " <<fabs(quadTmp.area - lastQuadstenArea) << endl;
                        if(fabs(quadTmp.area - lastQuadstenArea) /lastQuadstenArea < last_now_area_error)
                            isRightQuad = true;
                    }
                    if(isRightQuad){
                        it++;
                    } else {
                        quadsTen.erase(it);
                    }
                }
            }
        }
        }
        //cout<<"extractor: integrated error No.3"<<endl;
#ifdef DEBUG_QUAD
        cout << "Extractor: filter after comparing with lastQuadsTen size: " << quadsTen.size() << endl;
        cv::Mat imageQuadsTen = frame.clone();
        drawQuads(quadsTen, imageQuadsTen);
        imshow("quadsTen",imageQuadsTen);
#endif
        //step3.5.avoid quads away two lines


        //step3.5. avoid quads away from center
        //averagewidth & averageheight
      /***  cout<<"quadssize before away"<<quadsNine.size()<<endl;
        averageWidth=0;
        averageHeight=0;
        for(int i=0;i<quadsNine.size();i++)
        {
            averageWidth= averageWidth+quadsNine[i].m_width;
            averageHeight= averageHeight+quadsNine[i].m_height;
        }
        averageWidth= averageWidth/quadsNine.size();
        averageHeight= averageHeight/quadsNine.size();

//avoid quads away from center
        if(quadsNine.size() > 9){
            if((lastQuadNineCenter.x!=0) && (lastQuadNineCenter.y!=0)){
                for(vector<Quad>::iterator it = quadsNine.begin(); it != quadsNine.end();){
                    Quad& quadTmp  = *it;
                    bool isRightQuad2 = false;
                    if ((fabs(lastQuadNineCenter.x-quadTmp.m_center.x)<averageWidth*2)
                        &&(fabs(lastQuadNineCenter.y-quadTmp.m_center.y)<averageHeight*2)){
                        isRightQuad2 = true;
                    }
                    if(isRightQuad2){
                        it++;
                        cout<<"fabs(lastQuadallcenter.x-quadTmp.m_center.x)<averagewidth*2 ="<<fabs(lastQuadNineCenter.x-quadTmp.m_center.x)/(averageWidth*2)<<endl;
                        cout<<"fabs(lastQuadallcenter.y-quadTmp.m_center.y)<averageheight*2 = "<<fabs(lastQuadNineCenter.y-quadTmp.m_center.y)/(averageHeight*2)<<endl;
                    } else {
                        cout<<"fabs(lastQuadallcenter.x-quadTmp.m_center.x)<averagewidth*2 ="<<fabs(lastQuadNineCenter.x-quadTmp.m_center.x)/(averageWidth*2)<<endl;
                        cout<<"fabs(lastQuadallcenter.y-quadTmp.m_center.y)<averageheight*2 = "<<fabs(lastQuadNineCenter.y-quadTmp.m_center.y)/(averageHeight*2)<<endl;
                        quadsNine.erase(it);

                    }
                }
            }

        }
        //lastQuadallcenter为上一次平均中点
        lastQuadNineCenter.x=0;
        lastQuadNineCenter.y=0;
        for(int i=0;i<quadsNine.size();i++)
        {
            lastQuadNineCenter.x= lastQuadNineCenter.x+quadsNine[i].m_center.x;
            lastQuadNineCenter.y= lastQuadNineCenter.y+quadsNine[i].m_center.y;
        }
        lastQuadNineCenter.x= lastQuadNineCenter.x/quadsNine.size();
        lastQuadNineCenter.y= lastQuadNineCenter.y/quadsNine.size();

        cout<<"quadssize after away"<<quadsNine.size()<<endl;
***/
        //Step4. Order and complete ten quads
        ///complete quads which size is 8 or 9
        if (quadsTen.size() < 11){
            if(!completeTenQuads(quadsTen, index)){
                cout << "Extractor: completeTenQuads failed!!!" << endl;
                lastQuadstenArea=0;
                return false;
            }
        }
        //cout<<"extractor: integrated error No.4"<<endl;
        if(quadsTen.size() == 10){
            lastQuadsTen.clear();
            lastQuadsTen.assign(quadsTen.begin(),quadsTen.end());
            lastQuadstenArea = 0;
            for(int i = 0; i < quadsTen.size(); i++){
                lastQuadstenArea += quadsTen[i].area;
            }
            lastQuadstenArea = lastQuadstenArea/quadsTen.size();
        }
        //cout<<"extractor: integrated error No.5"<<endl;
#ifdef DEBUG_QUAD
        cout <<"Extractor: final quadsTen size is " << quadsTen.size() <<endl;
        cv::Mat imageQuadsTen1 = frame.clone();
        drawQuads(quadsTen, imageQuadsTen1);
        imshow("quadsTen1 ", imageQuadsTen1);
        waitKey(10);
#endif

        t2=((double)cv::getTickCount()-t1)*1000/getTickFrequency();
        double start_to_end =((double)cv::getTickCount()-start)*1000/getTickFrequency();
#ifdef DEBUG_QUAD_TIME
        cout<<"Extractor: complete quadsNine time is "<<t2<<"ms"<<endl;
        out<<"Extractor: complete quadsNine time is "<<t2<<"ms"<<endl;
        out<<"Extractor: total time is "<<start_to_end<<"ms"<<endl;
        out<<"-------------------------end------------------------------"<<endl;
#endif
        return true;
    }
}




