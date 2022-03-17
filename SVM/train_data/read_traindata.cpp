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

int img_cols = 42;
int img_rows = 57;

int class_num = 8;
int samples_num_per_class = 700;     /////////////////900张图片剩下200张用于测试集

int main() {
    ifstream read_1;
    ifstream read_2;
    ifstream read_3;
    ifstream read_4;
    ifstream read_5;
    ifstream read_6;
    ifstream read_7;
    ifstream read_8;

    //训练数据集及标签的创建，共1200个数据，6个类，现在分别是1 2  3  4  5  6
    //每个类别中的样本数量为200

    Mat train_data(class_num*samples_num_per_class, img_cols*img_rows,CV_32FC1);
    Mat labels(class_num*samples_num_per_class,1,CV_32SC1);

    string line_1;
    string line_2;
    string line_3;
    string line_4;
    string line_5;
    string line_6;
    string line_7;
    string line_8;


    // log.txt文件的生成：在数据所在目录下使用 ls *.jpg >> log.txt 获得
    read_1.open("../data/1/log.txt");      //读取数字1的训练集，所有的文件名存在log.txt中
    read_2.open("../data/2/log.txt");
    read_3.open("../data/3/log.txt");      //读取数字3的训练集，所有的文件名存在log.txt中
    read_4.open("../data/4/log.txt");      //读取数字4的训练集，所有的文件名存在log.txt中
    read_5.open("../data/5/log.txt");      //读取数字5的训练集，所有的文件名存在log.txt中
    read_6.open("../data/6/log.txt");      //读取数字6的训练集，所有的文件名存在log.txt中
    read_7.open("../data/7/log.txt");
    read_8.open("../data/8/log.txt");


    int i = 0;
    ofstream out_train;
//    out_train.open("../LibsvmData0725EP1000/traindata1/trainlog_0727");
    out_train.open("../model&txt/trainlog_1202");


    cout << "Start processing label 1" << endl;
    for(int j=1; j<=samples_num_per_class; j++){
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
        j++;
        i++;
    }
    cout << "Have finished label 1" << endl;

    cout << "Start processing label 2" << endl;
    for(int j=1; j<=samples_num_per_class; j++){
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
    cout << "Have finished label 2" << endl;

    cout << "Start processing label 3" << endl;
    for(int j=1; j<=samples_num_per_class; j++){
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
    cout << "Have finished label 3" << endl;

    cout << "Start processing label 4" << endl;
    for(int j=1; j<=samples_num_per_class; j++){
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
    cout << "Have finished label 4" << endl;

    cout << "Start processing label 5" << endl;
    for(int j=1; j<=samples_num_per_class; j++){
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
    cout << "Have finished label 5" << endl;

    cout << "Start processing label 6" << endl;
    for(int j=1; j<=samples_num_per_class; j++){
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
    cout << "Have finished label 6" << endl;

    cout << "Start processing label 7" << endl;
    for(int j=1; j<=samples_num_per_class; j++){
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
    cout << "Have finished label 7" << endl;

    cout << "Start processing label 8" << endl;
    for(int j=1; j<=samples_num_per_class; j++){
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
    cout << "Have finished label 8" << endl;




    for(i = 0 ; i < train_data.size[0];i++)
    {
//        cout << labels.at<int>(i) << endl;
//        cout << train_data.size[1] << endl;
        int c;
        if(labels.at<int>(i) >=0)
            out_train << "+" << labels.at<int>(i) << " ";
        else
            out_train << labels.at<int>(i) << " ";
        for (c = 0; c < train_data.cols; c++) {
            out_train <<c+1<<":" << train_data.at<float>(i,c)<<" " ;
        }
//        cout << "\n"<<train_data.row(i) << endl;
        out_train << "\n";
    }
    cout << " Have finished!" << endl;
    out_train.close();

//    cout << "The training data is : " << train_data.size[0] << endl;      //可以查看自己赋值之后的数据集
//    cout << "The label data is : " << labels << endl;             //查看赋值之后的标签
    return 0;
}
