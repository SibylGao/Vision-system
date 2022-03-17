#include <iostream>
#include <fstream>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/core/core.hpp>
#include <iostream>
#include "Armor.h"
#include "LightBar.h"

using namespace std;
using namespace cv;

int main() {
    string frameSavePath = "/media/sibylgao/新加卷/相机8/ ";
    for(int i =0; i<3000;i++){
        ostringstream imgName;
        imgName << i << ".jpg";
        string filename = "/home/sibylgao/CLionProjects/img/8/" + imgName.str();
        Mat img_temp = imread(filename);
        Mat img2= img_temp.clone();
//        Mat img_temp = imread("/home/sibylgao/CLionProjects/img/1/ 0.jpg");
        Armor armor_tmp;
        cout<< 1<<endl;
        armor_tmp.LightBarDetect(img_temp,armor_tmp);
        cout<<2<<endl;
//        imwrite(frameSavePath + imgName.str(), armor_tmp.ROI_img);
//        armor_tmp.remap(img2,armor_tmp,i);

        stringstream imagename;
        string imagename1;
        imagename <<i;
        imagename >> imagename1;
        string filename2 = "/home/sibylgao/数据集/数据集3/8/" + imagename1 + ".jpg";
        imwrite(filename2, armor_tmp.ROI_img);
        cout<<3<<endl;
    }
}

