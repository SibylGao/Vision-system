//
// Created by sibylgao on 18-12-7.
//

#include <iostream>
#include <fstream>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/core/core.hpp>

using namespace cv;
using namespace std;

int main() {
    ofstream log_1;
    ofstream log_2;
    ofstream log_3;
    ofstream log_4;
    ofstream log_5;
    ofstream log_6;
    ofstream log_7;
    ofstream log_8;

    log_1.open("/home/sibylgao/CLionProjects/copy/rm_sentry_2018/uselibsvm/LibsvmData0725EP1000/traindata2/1/log.txt");
    log_2.open("/home/sibylgao/CLionProjects/copy/rm_sentry_2018/uselibsvm/LibsvmData0725EP1000/traindata2/2/log.txt");
    log_3.open("/home/sibylgao/CLionProjects/copy/rm_sentry_2018/uselibsvm/LibsvmData0725EP1000/traindata2/3/log.txt");
    log_4.open("/home/sibylgao/CLionProjects/copy/rm_sentry_2018/uselibsvm/LibsvmData0725EP1000/traindata2/4/log.txt");
    log_5.open("/home/sibylgao/CLionProjects/copy/rm_sentry_2018/uselibsvm/LibsvmData0725EP1000/traindata2/5/log.txt");
    log_6.open("/home/sibylgao/CLionProjects/copy/rm_sentry_2018/uselibsvm/LibsvmData0725EP1000/traindata2/6/log.txt");
    log_7.open("/home/sibylgao/CLionProjects/copy/rm_sentry_2018/uselibsvm/LibsvmData0725EP1000/traindata2/7/log.txt");
    log_8.open("/home/sibylgao/CLionProjects/copy/rm_sentry_2018/uselibsvm/LibsvmData0725EP1000/traindata2/8/log.txt");

    for(int i=1;i<=900;i++){

        log_1 << i << ".jpg" << "\n";
        log_2 << i << ".jpg" << "\n";
        log_3 << i << ".jpg" << "\n";
        log_4 << i << ".jpg" << "\n";
        log_5 << i << ".jpg" << "\n";
        log_6 << i << ".jpg" << "\n";
        log_7 << i << ".jpg" << "\n";
        log_8 << i << ".jpg" << "\n";
    }
    log_1.close();
    log_2.close();
    log_3.close();
    log_4.close();
    log_5.close();
    log_6.close();
    log_7.close();
    log_8.close();
    cout << "finished" << endl;
}