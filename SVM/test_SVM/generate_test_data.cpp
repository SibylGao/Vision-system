//
// Created by sibylgao on 18-12-7.
//


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/opencv.hpp"
#include <fstream>

using namespace cv;
using namespace std;

int main(){
    ofstream out_test;
    out_test.open("../model&txt/testlog_1202");

    int samples_num = 200;
    int num_class = 8;
    int start_num = 700;
    int img_cols = 42;
    int img_rows = 57;

    Mat train_data(samples_num*num_class, img_cols*img_rows,CV_32FC1);
    Mat labels(samples_num*num_class,1,CV_32SC1);
    int i = 0;


    cout << "Start processing label 1" << endl;
    for(int j=start_num; j<samples_num + start_num; j++){
        stringstream imagename;
        string imagename1;
        imagename <<j;
        imagename >> imagename1;
        string filename = "../data/1/" + imagename1 +".jpg";
        Mat img_temp = imread(filename);
//        cv::cvtColor(img_temp,img_temp,CV_RGB2GRAY);
        int col = img_temp.cols;
        int row = img_temp.rows;
        int c,r;
        Mat img_row = Mat::zeros(1,col*row,CV_8U);
        for(r = 0;r<row;r++)
        {
            for(c = 0; c<col;c++)
            {
                img_row.at<uchar>(0,c+col*r) = img_temp.at<uchar>(r,c);
            }
        }
        img_row.copyTo(train_data.row(i));
        labels.row(i) = 1;
        i++;
        j++;
    }
    cout << "have finished label 1" << endl;

    cout << "Start processing label 2" << endl;
    for(int j=start_num; j<samples_num + start_num; j++){
        stringstream imagename;
        string imagename1;
        imagename <<j;
        imagename >> imagename1;
        string filename = "../data/2/" + imagename1 +".jpg";
        Mat img_temp = imread(filename);
//        cv::cvtColor(img_temp,img_temp,CV_RGB2GRAY);
        int col = img_temp.cols;
        int row = img_temp.rows;
        int c,r;
        Mat img_row = Mat::zeros(1,col*row,CV_8U);
        for(r = 0;r<row;r++)
        {
            for(c = 0; c<col;c++)
            {
                img_row.at<uchar>(0,c+col*r) = img_temp.at<uchar>(r,c);
            }
        }
        img_row.copyTo(train_data.row(i));
        labels.row(i) = 2;
        i++;
        j++;
    }
    cout << "have finished label 2" << endl;



    cout << "Start processing label 3" << endl;
    for(int j=start_num; j<samples_num + start_num; j++){
        stringstream imagename;
        string imagename1;
        imagename <<j;
        imagename >> imagename1;
        string filename = "../data/3/" + imagename1 +".jpg";
        Mat img_temp = imread(filename);
//        cv::cvtColor(img_temp,img_temp,CV_RGB2GRAY);
        int col = img_temp.cols;
        int row = img_temp.rows;
        int c,r;
        Mat img_row = Mat::zeros(1,col*row,CV_8U);
        for(r = 0;r<row;r++)
        {
            for(c = 0; c<col;c++)
            {
                img_row.at<uchar>(0,c+col*r) = img_temp.at<uchar>(r,c);
            }
        }
        img_row.copyTo(train_data.row(i));
        labels.row(i) = 3;
        i++;
        j++;
    }
    cout << "have finished label 3" << endl;



    cout << "Start processing label 4" << endl;
    for(int j=start_num; j<samples_num + start_num; j++){
        stringstream imagename;
        string imagename1;
        imagename <<j;
        imagename >> imagename1;
        string filename = "../data/4/" + imagename1 +".jpg";
        Mat img_temp = imread(filename);
//        cv::cvtColor(img_temp,img_temp,CV_RGB2GRAY);
        int col = img_temp.cols;
        int row = img_temp.rows;
        int c,r;
        Mat img_row = Mat::zeros(1,col*row,CV_8U);
        for(r = 0;r<row;r++)
        {
            for(c = 0; c<col;c++)
            {
                img_row.at<uchar>(0,c+col*r) = img_temp.at<uchar>(r,c);
            }
        }
        img_row.copyTo(train_data.row(i));
        labels.row(i) = 4;
        i++;
        j++;
    }
    cout << "have finished label 4" << endl;


    cout << "Start processing label 5" << endl;
    for(int j=start_num; j<samples_num + start_num; j++){
        stringstream imagename;
        string imagename1;
        imagename <<j;
        imagename >> imagename1;
        string filename = "../data/5/" + imagename1 +".jpg";
        Mat img_temp = imread(filename);
//        cv::cvtColor(img_temp,img_temp,CV_RGB2GRAY);
        int col = img_temp.cols;
        int row = img_temp.rows;
        int c,r;
        Mat img_row = Mat::zeros(1,col*row,CV_8U);
        for(r = 0;r<row;r++)
        {
            for(c = 0; c<col;c++)
            {
                img_row.at<uchar>(0,c+col*r) = img_temp.at<uchar>(r,c);
            }
        }
        img_row.copyTo(train_data.row(i));
        labels.row(i) = 5;
        i++;
        j++;
    }
    cout << "have finished label 5" << endl;




    cout << "Start processing label 6" << endl;
    for(int j=start_num; j<samples_num + start_num; j++){
        stringstream imagename;
        string imagename1;
        imagename <<j;
        imagename >> imagename1;
        string filename = "../data/6/" + imagename1 +".jpg";
        Mat img_temp = imread(filename);
//        cv::cvtColor(img_temp,img_temp,CV_RGB2GRAY);
        int col = img_temp.cols;
        int row = img_temp.rows;
        int c,r;
        Mat img_row = Mat::zeros(1,col*row,CV_8U);
        for(r = 0;r<row;r++)
        {
            for(c = 0; c<col;c++)
            {
                img_row.at<uchar>(0,c+col*r) = img_temp.at<uchar>(r,c);
            }
        }
        img_row.copyTo(train_data.row(i));
        labels.row(i) = 6;
        i++;
        j++;
    }
    cout << "have finished label 6" << endl;



    cout << "Start processing label 7" << endl;
    for(int j=start_num; j<samples_num + start_num; j++){
        stringstream imagename;
        string imagename1;
        imagename <<j;
        imagename >> imagename1;
        string filename = "../data/7/" + imagename1 +".jpg";
        Mat img_temp = imread(filename);
//        cv::cvtColor(img_temp,img_temp,CV_RGB2GRAY);
        int col = img_temp.cols;
        int row = img_temp.rows;
        int c,r;
        Mat img_row = Mat::zeros(1,col*row,CV_8U);
        for(r = 0;r<row;r++)
        {
            for(c = 0; c<col;c++)
            {
                img_row.at<uchar>(0,c+col*r) = img_temp.at<uchar>(r,c);
            }
        }
        img_row.copyTo(train_data.row(i));
        labels.row(i) = 7;
        i++;
        j++;
    }
    cout << "have finished label 7" << endl;



    cout << "Start processing label 8" << endl;
    for(int j=start_num; j<samples_num + start_num; j++){
        stringstream imagename;
        string imagename1;
        imagename <<j;
        imagename >> imagename1;
        string filename = "../data/8/" + imagename1 +".jpg";
        Mat img_temp = imread(filename);
//        cv::cvtColor(img_temp,img_temp,CV_RGB2GRAY);
        int col = img_temp.cols;
        int row = img_temp.rows;
        int c,r;
        Mat img_row = Mat::zeros(1,col*row,CV_8U);
        for(r = 0;r<row;r++)
        {
            for(c = 0; c<col;c++)
            {
                img_row.at<uchar>(0,c+col*r) = img_temp.at<uchar>(r,c);
            }
        }
        img_row.copyTo(train_data.row(i));
        labels.row(i) = 8;
        i++;
        j++;
    }
    cout << "have finished label 8" << endl;




    for( i = 0 ; i < train_data.size[0];i++)
    {
//        cout << labels.at<int>(i) << endl;
//        cout << train_data.size[1] << endl;
        int c;
        if(labels.at<int>(i) >=0)
            out_test << "+" << labels.at<int>(i) << " ";
        else
            out_test << labels.at<int>(i) << " ";
        for (c = 0; c < train_data.cols; c++) {
            out_test <<c+1<<":" << train_data.at<float>(i,c)<<" " ;
        }
//        cout << "\n"<<train_data.row(i) << endl;
        out_test << "\n";
    }
    cout << " Have finished!" << endl;
    out_test.close();
}


