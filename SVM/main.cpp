//
//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/imgproc/imgproc.hpp>
//#include <opencv2/ml/ml.hpp>
//#include <iostream>
//#include <fstream>
//#include "opencv2/opencv.hpp"
//#include "src_svm/svm.h"
//
//using namespace cv;
//using namespace std;
//using namespace cv::ml;
//
//
//int main() {
////    ifstream read_1;
////    ifstream read_3;
////    ifstream read_4;
////    ifstream read_5;
////    ifstream read_misrecognition;
////
////    //训练数据集及标签的创建，共500个数据，五个类，现在分别是1  3  4  5  误识别
////    //每个类别中的样本数量为100
////    //到时候可以创建hard_sample的训练，或者，完了我在自己试一试
////    Mat train_data(500,1500,CV_32FC1);
////    Mat labels(500,1,CV_32SC1);
////
////    string line_1;
////    string line_3;
////    string line_4;
////    string line_5;
////    string line_misrecognition;
////
////    // log.txt文件的生成：在数据所在目录下使用 ls *.jpg >> log.txt 获得
////    read_1.open("/home/xiaoyang/SVMDATA/traindata/1/log.txt");      //读取数字1的训练集，所有的文件名存在log.txt中
////    read_3.open("/home/xiaoyang/SVMDATA/traindata/3/log.txt");      //读取数字3的训练集，所有的文件名存在log.txt中
////    read_4.open("/home/xiaoyang/SVMDATA/traindata/4/log.txt");      //读取数字4的训练集，所有的文件名存在log.txt中
////    read_5.open("/home/xiaoyang/SVMDATA/traindata/5/log.txt");      //读取数字5的训练集，所有的文件名存在log.txt中
////    read_misrecognition.open("/home/xiaoyang/SVMDATA/traindata/misrecognition/log.txt");    //读取误识别的训练集，所有的文件名存在log.txt中
////    int i = 0;
////
////
////    while(getline(read_1,line_1))   //读取数据集1
////    {
////        string filename = "/home/xiaoyang/SVMDATA/traindata/1/" + line_1;  //文件名
////        Mat img_temp = imread(filename);
////        cv::cvtColor(img_temp,img_temp,CV_RGB2GRAY);  //存的是二值图最后并不是，就很难受，并不知道为什么
////        int col = img_temp.cols;
////        int row = img_temp.rows;
////        int c,r;
////        Mat img_row = Mat::zeros(1,1500,CV_8U);  //新建一个行向量
////
////        //将原来的图片转换成行向量，实际上新建的时候最好 Mat img_row = Mat::zeros(1,col*row,CV_8U);
////        //因为我知道我的数据集为50x30的图片，所以并没有使用上述的办法进行创建
////
////        for(r = 0;r<row;r++)
////        {
////            for(c = 0; c<col;c++)
////            {
////                img_row.at<uchar>(0,c+50*r) = img_temp.at<uchar>(r,c);
////            }
////        }
////        img_row.copyTo(train_data.row(i));  //将行向量赋值到整个训练数据集中
////        labels.row(i) = 1;  //类别1的标签赋值
//////        cout << "The label is " << labels.row(i) << endl;
//////        cout << "train_data is " << train_data.row(i) << endl;
////        i++;
////    }
////
////
////    while(getline(read_3,line_3))  //读取数据集3
////    {
////        string filename = "/home/xiaoyang/SVMDATA/traindata/3/" + line_3;
////        Mat img_temp = imread(filename);
////        cv::cvtColor(img_temp,img_temp,CV_RGB2GRAY);
////        int col = img_temp.cols;
////        int row = img_temp.rows;
////        int c,r;
////        Mat img_row = Mat::zeros(1,1500,CV_8U);
////        for(r = 0;r<row;r++)
////        {
////            for(c = 0; c<col;c++)
////            {
////                img_row.at<uchar>(0,c+50*r) = img_temp.at<uchar>(r,c);
////            }
////        }
////        img_row.copyTo(train_data.row(i));
////        labels.row(i) = 3;
//////        cout << "The label is " << labels.row(i) << endl;
//////        cout << "train_data is " << train_data.row(i) << endl;
////        i++;
////    }
////
////
////    while(getline(read_4,line_4))  //读取数据集4
////    {
////        string filename = "/home/xiaoyang/SVMDATA/traindata/4/" + line_4;
////        Mat img_temp = imread(filename);
////        cv::cvtColor(img_temp,img_temp,CV_RGB2GRAY);
////        int col = img_temp.cols;
////        int row = img_temp.rows;
////        int c,r;
////        Mat img_row = Mat::zeros(1,1500,CV_8U);
////        for(r = 0;r<row;r++)
////        {
////            for(c = 0; c<col;c++)
////            {
////                img_row.at<uchar>(0,c+50*r) = img_temp.at<uchar>(r,c);
////            }
////        }
////        img_row.copyTo(train_data.row(i));
////        labels.row(i) = 4;
//////        cout << "The label is " << labels.row(i) << endl;
//////        cout << "train_data is " << train_data.row(i) << endl;
////        i++;
////    }
////
////
////    while(getline(read_5,line_5))  //读取数据集5
////    {
////        string filename = "/home/xiaoyang/SVMDATA/traindata/5/" + line_5;
////        Mat img_temp = imread(filename);
////        cv::cvtColor(img_temp,img_temp,CV_RGB2GRAY);
////        int col = img_temp.cols;
////        int row = img_temp.rows;
////        int c,r;
////        Mat img_row = Mat::zeros(1,1500,CV_8U);
////        for(r = 0;r<row;r++)
////        {
////            for(c = 0; c<col;c++)
////            {
////                img_row.at<uchar>(0,c+50*r) = img_temp.at<uchar>(r,c);
////            }
////        }
////        img_row.copyTo(train_data.row(i));
////        labels.row(i) = 5;
//////        cout << "The label is " << labels.row(i) << endl;
//////        cout << "train_data is " << train_data.row(i) << endl;
////        i++;
////    }
////
////
////    while(getline(read_misrecognition,line_misrecognition))  //读取数据集误识别
////    {
////        string filename = "/home/xiaoyang/SVMDATA/traindata/misrecognition/" + line_misrecognition;
////        Mat img_temp = imread(filename);
////        cv::cvtColor(img_temp,img_temp,CV_RGB2GRAY);
////        int col = img_temp.cols;
////        int row = img_temp.rows;
////        int c,r;
////        Mat img_row = Mat::zeros(1,1500,CV_8U);
////        for(r = 0;r<row;r++)
////        {
////            for(c = 0; c<col;c++)
////            {
////                img_row.at<uchar>(0,c+50*r) = img_temp.at<uchar>(r,c);
////            }
////        }
////        img_row.copyTo(train_data.row(i));
////        labels.row(i) = -1;
//////        cout << "The label is " << labels.row(i) << endl;
//////        cout << "train_data is " << train_data.row(i) << endl;
////        i++;
////    }
////
////
//////    cout << "The training data is : " << train_data << endl;      //可以查看自己赋值之后的数据集
//////    cout << "The label data is : " << labels << endl;             //查看赋值之后的标签
//
//
//
//
//    //  SVM分类器的创建
//    Ptr<SVM> svm = SVM::create();
//    svm->setType(SVM::NU_SVC);   //分类器的种类，nu_classification
//    svm->setNu(0.1);        //设置NU值，具体设置crtl+B(使用Clion时的快捷键）
////    svm->setKernel(SVM::RBF);
//    svm->setGamma(5);
//    svm->setCoef0(2);
//    svm->setP(0);
//    svm->setKernel(ml::SVM::POLY);      //分类器的和函数为多项式和函数
//    svm->setDegree(2.0);
//    svm->setC(1);
//    svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 1000000, 1e-12));
//    svm->train(train_data, ROW_SAMPLE, labels);     //进行svm训练，具体参数也是ctrl+B
//    cout << train_data.size << endl;
//
//
////    svm->save("misrecognition.xml");  //保存训练数据
//
//
//    ifstream read_test;
//    string test_line;
//
//    //将所有的预测集放在一个目录下
//    // testdata
//    //-  1   3   4   5   misrecognition
//    // 我的保存的路径为 /home/xiaoyang/SVMDATA/testdata/
//    // 利用命令 find /home/xiaoyang/SVMDATA/testdata/ >> test.txt 可以把该文件夹下的所有的文件名保存到 test.txt文件中
//    // 需要查看一下文件test.txt文件里面不属于.jpg的文件名，并删除掉
//
//    read_test.open("/home/xiaoyang/SVMDATA/testdata/test.txt");
//    float prediction[1643];     //创建预测结果数组
//    float label[1643];      //创建标签数组
//
//    //给所有的标签进行赋值，按照训练时候的数据进行赋值
//    for(int u = 0;u<1643;u++)
//    {
//        if(u<(574))
//            label[u] = 5;
//        else if(u<948)
//            label[u] = 4;
//        else if(u<1263)
//            label[u] = 1;
//        else if(u<1484)
//            label[u] = 3;
//        else
//            label[u] = -1;
//
//    }
//     int i = 0;
//
//
//    double same_times = 0;
//    while(getline(read_test,test_line))
//    {
//        double t1 = (double)getTickCount();
//        Mat test_img = imread(test_line);
//        cv::cvtColor(test_img,test_img,CV_RGB2GRAY);
//        int col = test_img.cols;
//        int row = test_img.rows;
//        int c,r;
//        Mat img_row = Mat::zeros(1,1500,CV_8U);
//        for(r = 0;r<row;r++)
//        {
//            for(c = 0; c<col;c++)
//            {
//                img_row.at<uchar>(0,c+50*r) = test_img.at<uchar>(r,c);
//            }
//        }
//        //必须注意，输入到predict函数中的向量的格式必须为5,好象是CV_32S,还是查一下比较好，或者直接这行的函数
//        img_row.convertTo(img_row,5);
//        float response = svm->predict(img_row);  //进行数据预测
//        prediction[i] = response;
//        cout << "The label is " << label[i] << endl;
//        cout << "The prediction is " << response << endl;
//        if(label[i]==prediction[i])
//            same_times++;
//        else
//        {
//            cout << "The filename is :" << test_line << endl;
////            imshow("The wrong example",test_img);
////            waitKey();
//        }
//        i++;
//        double t2 = (double)getTickCount();
//        cout << "One Test Time is : " << (t2-t1)/getTickFrequency()*1000<< "ms" << endl;  //一次预测的时间
//    }
//    cout << "in the 1484 test samples,the correct ratio is:" << same_times/1643 << endl;   //返回所有的预测正确率，验证集的
//    return 0;
//}


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ml/ml.hpp>
#include <iostream>
#include <fstream>
#include "opencv2/opencv.hpp"
#include "src_svm/svm.h"

using namespace cv;
using namespace std;
using namespace cv::ml;

//#define DEBUG

int main(){
    struct svm_node *test_data;
    int img_cols = 42;
    int img_rows = 57;
    int samples_num = 200;
    int num_class = 8;
    int start_num = 700;
    int max_nr_attr = img_cols*img_rows;

    struct svm_model *model;

    ofstream output;
    model = svm_load_model("../src_svm/trainlog_1202.model");

    int svm_type=svm_get_svm_type(model);
    int nr_class=svm_get_nr_class(model);
    double *prob_estimates ;
    prob_estimates = (double *) malloc(nr_class*sizeof(double));
//    int j;

    output.open("../skyer_data/data");

    for(int j = 0; j<8 ;j++){
        if(j == 0){
            cout << "start label 1" << endl;
            double sum = 0;
            int correct_num = 0;
            for(int b = start_num ; b < start_num +samples_num ; b++){
                stringstream imagename;
                string imagename1;
                imagename <<b;
                imagename >> imagename1;
                string filename = "../data/1/" + imagename1 +".jpg";
                Mat img_temp = imread(filename);
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
                int i = 0;
                test_data = (struct svm_node *) malloc(max_nr_attr*sizeof(struct svm_node));
                for( i = 1;i<=img_cols*img_rows;i++)
                {
                    test_data[i-1].index = i;
                    test_data[i-1].value = img_row.at<double>(0,i-1);
//                    cout << i << endl;
                }
                test_data[i].index= -1;
//                  cout << test_data[i].index << endl;
//                  cout << i << endl;

                double predict_label;
                predict_label = svm_predict(model,test_data);
                cout << predict_label << endl;
//                double predict_probability;
                double  predict_class;
                predict_class = svm_predict_probability(model,test_data,prob_estimates);
                int int_class = predict_class;
                double pro = prob_estimates[int_class - 1];

#ifdef DEBUG
                cout << "probability is " << pro << endl;
                cout << "the predicted class is " << int_class << endl;
#endif
                sum = sum + pro ;
                if(predict_class == 1  && pro > 0.8){
                    correct_num ++ ;
                }
            }
            cout << "correct num is " <<  correct_num << endl;
            cout << "the probability is " << sum/200 << endl;
        }


        if(j == 1){
            cout << "start label 2" << endl;
            double sum = 0;
            int correct_num = 0;
            for(int b = start_num ; b < start_num +samples_num ; b++){
                stringstream imagename;
                string imagename1;
                imagename <<b;
                imagename >> imagename1;
                string filename = "../data/2/" + imagename1 +".jpg";
                Mat img_temp = imread(filename);
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
                int i = 0;
                test_data = (struct svm_node *) malloc(max_nr_attr*sizeof(struct svm_node));
                for( i = 1;i<=img_cols*img_rows;i++)
                {
                    test_data[i-1].index = i;
                    test_data[i-1].value = img_row.at<float>(0,i-1);
                }
                test_data[i].index=-1;

                double predict_class;
                predict_class = svm_predict(model,test_data);
                double predict_probability;
                predict_probability = svm_predict_probability(model,test_data,prob_estimates);
                int int_class = predict_class;
                double pro = *(prob_estimates + int_class - 1);
                sum = sum + pro ;
                if(predict_class == 1  && pro > 0.8){
                    correct_num ++ ;
                }
            }
            cout << "correct num is " <<  correct_num << endl;
            cout << "the probability is " << sum/200 << endl;
        }



        if(j == 2){
            cout << "start label 3" << endl;
            double sum = 0;
            int correct_num = 0;
            for(int b = start_num ; b < start_num +samples_num ; b++){
                stringstream imagename;
                string imagename1;
                imagename <<b;
                imagename >> imagename1;
                string filename = "../data/3/" + imagename1 +".jpg";
                Mat img_temp = imread(filename);
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
                int i = 0;
                test_data = (struct svm_node *) malloc(max_nr_attr*sizeof(struct svm_node));
                for( i = 1;i<=img_cols*img_rows;i++)
                {
                    test_data[i-1].index = i;
                    test_data[i-1].value = img_row.at<float>(0,i-1);
                }
                test_data[i].index=-1;

                double predict_class;
                predict_class = svm_predict(model,test_data);
                double predict_probability;
                predict_probability = svm_predict_probability(model,test_data,prob_estimates);
                int int_class = predict_class;
                double pro = *(prob_estimates + int_class - 1);
                sum = sum + pro ;
                if(predict_class == 1  && pro > 0.8){
                    correct_num ++ ;
                }
            }
            cout << "correct num is " <<  correct_num << endl;
            cout << "the probability is " << sum/200 << endl;
        }



        if(j == 3){
            cout << "start label 4" << endl;
            double sum = 0;
            int correct_num = 0;
            for(int b = start_num ; b < start_num +samples_num ; b++){
                stringstream imagename;
                string imagename1;
                imagename <<b;
                imagename >> imagename1;
                string filename = "../data/4/" + imagename1 +".jpg";
                Mat img_temp = imread(filename);
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
                int i = 0;
                test_data = (struct svm_node *) malloc(max_nr_attr*sizeof(struct svm_node));
                for( i = 1;i<=img_cols*img_rows;i++)
                {
                    test_data[i-1].index = i;
                    test_data[i-1].value = img_row.at<float>(0,i-1);
                }
                test_data[i].index=-1;

                double predict_class;
                predict_class = svm_predict(model,test_data);
                double predict_probability;
                predict_probability = svm_predict_probability(model,test_data,prob_estimates);
                int int_class = predict_class;
                double pro = *(prob_estimates + int_class - 1);
                sum = sum + pro ;
                if(predict_class == 1  && pro > 0.8){
                    correct_num ++ ;
                }
            }
            cout << "correct num is " <<  correct_num << endl;
            cout << "the probability is " << sum/200 << endl;
        }


        if(j == 4){
            cout << "start label 5" << endl;
            double sum = 0;
            int correct_num = 0;
            for(int b = start_num ; b < start_num +samples_num ; b++){
                stringstream imagename;
                string imagename1;
                imagename <<b;
                imagename >> imagename1;
                string filename = "../data/5/" + imagename1 +".jpg";
                Mat img_temp = imread(filename);
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
                int i = 0;
                test_data = (struct svm_node *) malloc(max_nr_attr*sizeof(struct svm_node));
                for( i = 1;i<=img_cols*img_rows;i++)
                {
                    test_data[i-1].index = i;
                    test_data[i-1].value = img_row.at<float>(0,i-1);
                }
                test_data[i].index=-1;

                double predict_class;
                predict_class = svm_predict(model,test_data);
                double predict_probability;
                predict_probability = svm_predict_probability(model,test_data,prob_estimates);
                int int_class = predict_class;
                double pro = *(prob_estimates + int_class - 1);
                sum = sum + pro ;
                if(predict_class == 1  && pro > 0.8){
                    correct_num ++ ;
                }
            }
            cout << "correct num is " <<  correct_num << endl;
            cout << "the probability is " << sum/200 << endl;
        }


        if(j == 5){
            cout << "start label 6" << endl;
            double sum = 0;
            int correct_num = 0;
            for(int b = start_num ; b < start_num +samples_num ; b++){
                stringstream imagename;
                string imagename1;
                imagename <<b;
                imagename >> imagename1;
                string filename = "../data/6/" + imagename1 +".jpg";
                Mat img_temp = imread(filename);
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
                int i = 0;
                test_data = (struct svm_node *) malloc(max_nr_attr*sizeof(struct svm_node));
                for( i = 1;i<=img_cols*img_rows;i++)
                {
                    test_data[i-1].index = i;
                    test_data[i-1].value = img_row.at<float>(0,i-1);
                }
                test_data[i].index=-1;

                double predict_class;
                predict_class = svm_predict(model,test_data);
                double predict_probability;
                predict_probability = svm_predict_probability(model,test_data,prob_estimates);
                int int_class = predict_class;
                double pro = *(prob_estimates + int_class - 1);
                sum = sum + pro ;
                if(predict_class == 1  && pro > 0.8){
                    correct_num ++ ;
                }
            }
            cout << "correct num is " <<  correct_num << endl;
            cout << "the probability is " << sum/200 << endl;
        }


        if(j == 6){
            cout << "start label 7" << endl;
            double sum = 0;
            int correct_num = 0;
            for(int b = start_num ; b < start_num +samples_num ; b++){
                stringstream imagename;
                string imagename1;
                imagename <<b;
                imagename >> imagename1;
                string filename = "../data/7/" + imagename1 +".jpg";
                Mat img_temp = imread(filename);
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
                int i = 0;
                test_data = (struct svm_node *) malloc(max_nr_attr*sizeof(struct svm_node));
                for( i = 1;i<=img_cols*img_rows;i++)
                {
                    test_data[i-1].index = i;
                    test_data[i-1].value = img_row.at<float>(0,i-1);
                }
                test_data[i].index=-1;

                double predict_class;
                predict_class = svm_predict(model,test_data);
                double predict_probability;
                predict_probability = svm_predict_probability(model,test_data,prob_estimates);
                int int_class = predict_class;
                double pro = *(prob_estimates + int_class - 1);
                sum = sum + pro ;
                if(predict_class == 1  && pro > 0.8){
                    correct_num ++ ;
                }
            }
            cout << "correct num is " <<  correct_num << endl;
            cout << "the probability is " << sum/200 << endl;
        }


        if(j == 7){
            cout << "start label 8" << endl;
            double sum = 0;
            int correct_num = 0;
            for(int b = start_num ; b < start_num +samples_num ; b++){
                stringstream imagename;
                string imagename1;
                imagename <<b;
                imagename >> imagename1;
                string filename = "../data/8/" + imagename1 +".jpg";
                Mat img_temp = imread(filename);
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
                int i = 0;
                test_data = (struct svm_node *) malloc(max_nr_attr*sizeof(struct svm_node));
                for( i = 1;i<=img_cols*img_rows;i++)
                {
                    test_data[i-1].index = i;
                    test_data[i-1].value = img_row.at<float>(0,i-1);
                }
                test_data[i].index=-1;

                double predict_class;
                predict_class = svm_predict(model,test_data);
                double predict_probability;
                predict_probability = svm_predict_probability(model,test_data,prob_estimates);
                int int_class = predict_class;
                double pro = *(prob_estimates + int_class - 1);
                sum = sum + pro ;
                if(predict_class == 1  && pro > 0.8){
                    correct_num ++ ;
                }
            }
            cout << "correct num is " <<  correct_num << endl;
            cout << "the probability is " << sum/200 << endl;
        }

    }


}

