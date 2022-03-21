//
// Created by sun on 18-1-17.
//

#include "BuffHiter.h"
#include <boost/thread.hpp>
#include "serialapp.h"
#include "hitcrt_rm0422.pb-c.h"
#include "myVideoCap.h"

#include <boost/atomic/atomic.hpp>
#include <iomanip>
#include <boost/bind.hpp>
#define RecvBuf_Size 300
#define SendPeriod 2
#define RecvPeriod 1
using namespace boost::asio;

using namespace std;
using namespace cv;

//#define _DEBUG;
boost::thread grabBuffThread, processThread, communicationThread, manualThread;
boost::mutex frameMutex, pitchMutex, AssitMutex, BuffMutex, buffImgMutex, HitFlagMutex;
boost::mutex m_send_mutex;
hitcrt::SerialApp* pSerialApp;
ip::udp::socket* my_socket;
io_service my_io_service;
float pitchDegree = 0.0f;
int m_function_flag = 3;
char RecvBuf[RecvBuf_Size];

ip::udp::endpoint local_endpoint(ip::address_v4::from_string("192.168.1.110"),9999);//本地(电脑)端口信息
ip::udp::endpoint remote_endpoint(ip::address_v4::from_string("192.168.1.30"), 9999);


//buffHitingClass *buffHitingVar;
float pitchAngle = 0.0f;
bool flagHitBuff = true;
int FLAG_HIT_BUFF = 0;
int FLAG_ASSIST_AIM = 0; ///DEBUG

namespace hitcrt{
    struct Frame{
        Frame(){}
        Frame(cv::Mat _img, struct timeval _t) : m_img(_img), m_t(_t) {}
        Frame(cv::Mat _img) : m_img(_img) {
            gettimeofday(&m_t, NULL);
        }
        cv::Mat m_img;
        struct timeval m_t;
    };
}

hitcrt::Frame frameArmor;
hitcrt::Frame frameBuff;

void grabBuff()
{

//    VideoCapture cap;
//    if(!cap.open("/home/sun/Buff/VideoTest.avi")){
//        cout<<"cannot open video "<<endl;
//    }

//    VideoCapture cap(1);
//
//    cap.set(CV_CAP_PROP_FRAME_WIDTH,1920);
//    cap.set(CV_CAP_PROP_FRAME_HEIGHT,1080);
//    cap.set(CV_CAP_PROP_FPS,30);
//    if(!cap.isOpened()){
//        cout << "open camera failed" << endl;
//    }

    hitcrt::myVideoCap cap("/dev/video1", 1920, 1080, 30, 1, 103, 1);
    cv::Mat frame;
    for(int i = 0; i < 10; i ++) {
        if(buffImgMutex.try_lock()) {
            cap.VideoGrab(frame);

//            waitKey(1);
            frameBuff = hitcrt::Frame(frame);
            buffImgMutex.unlock();
        }
    }
    while(1) {

        boost::this_thread::interruption_point();
        if(m_function_flag==2||m_function_flag==3)
            if(buffImgMutex.try_lock()) {
                cap.VideoGrab(frame);
                cout << "grabBuff" << endl;
                cv::Mat imgShow=frame.clone();
//            cap >> frame;
//            cout << "111" << endl;
                resize(imgShow,imgShow,Size(0, 0), 0.3, 0.3);
                imshow("grabFrame", imgShow);

//                cap >> frame;
//                cout << "222" << endl;
//                imshow("rawFrame0", frame);
//                waitKey(1);
//                cap.VideoGrab(frame);
                frameBuff = hitcrt::Frame(frame);
                buffImgMutex.unlock();
            }
        boost::this_thread::sleep(boost::posix_time::milliseconds(5));
    }
}


void process()
{
    cv::Mat frame;
    MessageToMCU msge = MESSAGE_TO__MCU__INIT;
//    hitcrt::ArmorDetect armorDetect;
//    hitcrt::Predictor predictor;
    hitcrt::BuffHiter buffHiter;
    cv::Mat R,new_Matrix;
    cv::initUndistortRectifyMap(hitcrt::BuffParam::KK,hitcrt::BuffParam::DISTORT,R,new_Matrix,Size(1920,1080),CV_32FC1,
                                hitcrt::BuffParam::MAPX,hitcrt::BuffParam::MAPY);

    struct timeval frame_t;
    double t1=0,t2=0;
    double fps = 0;
    int cntHeartBeat = 0;
    vector<float> vecEmpty{0,0,0};

    while(1) {
        boost::this_thread::interruption_point();

        cout << "process" << endl;
//        gettimeofday(&st,NULL);
//
//        if(checkAssistFlag == 0) {
        if(m_function_flag == 2 || m_function_flag == 3) {
            cout << "ENTER BUFF" << endl;
//            while(checkBuffFlag) {
//            while(checkBuffFlag) {
                bool checkFun = false;
                int cntFrameTrash = 0; //除去残余图像帧
                while(cntFrameTrash < 5) {
                    if(!frameBuff.m_img.empty()) {
                        frame = frameBuff.m_img.clone();
                        frameBuff.m_img.release();
                        buffImgMutex.unlock();
                    }else{
                        buffImgMutex.unlock();

                        continue;
                    }
                    cntFrameTrash++;
                }
            cout << "Get buffFrame1" << endl;
                ////old
//                int cntTemp = 0;
//                do
//                {
//                    boost::this_thread::interruption_point();
//                    buffImgMutex.lock();
//                    if(!frameBuff.m_img.empty()) {
//                        frame = frameBuff.m_img.clone();
//                        frameBuff.m_img.release();
//                        buffImgMutex.unlock();
//                    }else{
//                        buffImgMutex.unlock();
//                        continue;
//                    }
//                    cntTemp ++;
//                    if(cntTemp == 5)
//                    {
//                        cntTemp = 0;
//                        buffHiter.recordBuffImg(frame);
//                    }
//                    checkFun = buffHitingVar -> init(frame);
//                    HitFlagMutex.lock();
//                    checkHitFlag = flagHitBuff;
//                    HitFlagMutex.unlock();
//                }while(checkHitFlag&&(!checkFun));
//                checkFun = false;
            cout << "Get buffFrame2" << endl;

//                while(checkBuffFlag)
//                {
//                    boost::this_thread::interruption_point();
                    /*BuffMutex.lock();
                    checkBuffFlag = FLAG_HIT_BUFF;
                    BuffMutex.unlock();*/

                   /* buffImgMutex.lock();
                    if(!frameBuff.m_img.empty()) {
                        frame = frameBuff.m_img.clone();
                        frameBuff.m_img.release();
                        buffImgMutex.unlock();
                    }else{
                        buffImgMutex.unlock();
                        cout << "Get buffFrame3" << endl;
                        continue;
                    }*/

            cout << "Remap buffFrame" << endl;


                    remap(frame,frame,hitcrt::BuffParam::MAPX,hitcrt::BuffParam::MAPY,INTER_LINEAR);
                    bool hitbuffSuccess = true;
                    double start = cv::getTickCount();

            /*buffImgMutex.lock();
            cv::Mat imgShow=frame.clone();
//            cap >> frame;
//            cout << "111" << endl;
            resize(imgShow,imgShow,Size(0, 0), 0.3, 0.3);
            imshow("rawFrame", imgShow);
            buffImgMutex.unlock();*/
//            waitKey(0);
                    cout << "m_function_flag" << m_function_flag<< endl;
                    hitbuffSuccess = buffHiter.run(frame, m_function_flag);
                    cout << "total time is: " << (cv::getTickCount() - start)*1000/cv::getTickFrequency()<<"ms"<< endl;
                    if(!hitbuffSuccess) {
                        cout << "hitbuffSuccessFlag" << hitbuffSuccess << endl;
                        hitbuffSuccess= true;
                        continue;
                    }
//                    waitKey(0);

                    //old
                    /*vector<float> sendData;
                    sendData.push_back(buffHiter.stateCodeNum);
                    sendData.push_back(buffHiter.yaw);
                    sendData.push_back(buffHiter.pitch);
//                    sendData.push_back(buffHiter.yawCW);
//                    sendData.push_back(buffHiter.pitchCW);
                    pSerialApp->send(hitcrt::SerialApp::SEND_ERR_UV, sendData);*/

                    msge.target_flag=0;//给六个参数赋值
                    msge.aim_pitch=buffHiter.pitch;
                    msge.aim_yaw=buffHiter.yaw;
                    msge.distance=0;
                    msge.buff_state=buffHiter.stateCodeNum;
                    msge.heart_beat=0;
                    //计算数据包的大小，确定要发送的数据长度，注意数据有效位数不同时，包的大小会有变化，例如在21-29浮动
                    size_t len = message_to__mcu__get_packed_size(&msge);
                    char *SendBuf= (char *)malloc(50);//为发送缓冲区申请内存，100暂时够用，发送6个变量可能需要20-30字节
                    message_to__mcu__pack(&msge,(uint8_t *)SendBuf);//将存储参数，赋值好的结构体打包存进发送缓冲区，准备发送
                    //将打包好的数据，发送到电控的网口
//                  m_send_mutex.lock();
                    (*my_socket).send_to(boost::asio::buffer((char*)SendBuf, len), remote_endpoint);

//                  m_send_mutex.unlock();
                    free(SendBuf);


                    t2 = ((double)cv::getTickCount() - t1) / cv::getTickFrequency();
                    t1 = (double)cv::getTickCount();
                    fps = 1.0 / t2;

                    cout<<"-------------------------send yaw and pitch theta------------------------------"<<endl;
                    cout << "send state yaw and pitch are: " << "[" << buffHiter.stateCodeNum << buffHiter.yaw << "\t" <<buffHiter.pitch << "]" << endl;
                    cout << "fps is: " << fps << endl;
                    waitKey(10);
//                }
//            }
        }

        else if(m_function_flag == 1){
            cout << "send flag is AimAssistFlag" << endl;
        }
//        if(!checkBuffFlag)
//        {
//            if(checkAssistFlag==0)
//                continue;
//            else if(checkAssistFlag==1){
//                armorDetect.armor_type = true;
//            }
//            else if(checkAssistFlag==2){
//                armorDetect.armor_type = false;
//            }
//            //        cout << "checkAssistFlag " << checkAssistFlag <<endl;
//            frameMutex.lock();
//            if(!frameArmor.m_img.empty()) {
//                frame = frameArmor.m_img.clone();
//                frameArmor.m_img.release();
//                frame_t = frameArmor.m_t;
//                frameMutex.unlock();
//            }else{
//                frameMutex.unlock();
//                continue;
//            }
//
//            ++countNum;
//            //        cout << "countNum = " << countNum << endl;
//            ///去畸变
//            remap(frame,frame,hitcrt::Param::MAPX,hitcrt::Param::MAPY,INTER_LINEAR);
//
//            vector<hitcrt::Armor> armors;
//            pitchMutex.lock();
//            float m_pitchAngle = pitchAngle;
//            pitchMutex.unlock();
//            armorDetect.Apply(armors, frame,m_pitchAngle);
//            armorDetect.showImage(armors);
//            if(armors.size() < 1)
//                continue;
//
//            float min_distance = 100000;
//            int min_index = 0;
//            for (int j = 0; j < armors.size(); ++j) {
//                if(armors[j].m_distance < min_distance)
//                {
//                    min_distance = armors[j].m_distance;
//                    min_index = j;
//                }
//            }
//
//            double now_time = static_cast<double>(frame_t.tv_sec) + static_cast<double>(frame_t.tv_usec) / 1000000.0;
//            //        cout << "record = " << armors[min_index].m_center_uv.x-320 << endl;
//
//            //        cout << "output = " << output << endl;
//            //        cout << "distance = " << armors[min_index].m_distance << endl;
//            //        cout << "countNum" << countNum << endl;
//            float sendAngle = (armors[min_index].m_center_uv.x - 320) / (hitcrt::Param::DXDY) * 180/ CV_PI;
//
//            sendAngle += asin(46.5 / armors[min_index].m_distance) * 180 / CV_PI;
//
//            predictor.setRecord(sendAngle, now_time);
//            float output = predictor.predict(now_time+0.016*3);
//
//
//
//
//            vector<float> send_data;
//            send_data.push_back(armors[min_index].m_eject_theta * 180 / CV_PI);
//            send_data.push_back(armors[min_index].m_distance);
//            send_data.push_back(output - 0.5);
//            pSerialApp->send(hitcrt::SerialApp::SEND_ASSIST_AIM, send_data);
//
//            //        for (int i = 0; i < send_data.size(); ++i) {
//            //            cout << send_data[i] << endl;
//            //        }
//            t2 = ((double)cv::getTickCount() - t1) / cv::getTickFrequency();
//            t1 = (double)cv::getTickCount();
//            fps = 1.0 / t2;
//            cout << "sendAngle = " << sendAngle << endl;
//            cout << " X Y = " << armors[min_index].m_X << "  " << armors[min_index].m_Y << endl;
//            cout << "pitchAngle = " << m_pitchAngle * 180 / CV_PI << endl;
//            cout << " eject_data =" << armors[min_index].m_eject_theta * 180 / CV_PI << endl;
//            cout << "---------------------------------------------" << endl;
//            cout << "predict " << output - 0.5 <<endl;
//            cout << "normal  " << sendAngle - 0.5 <<endl;
//            cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~fps " << fps << endl;
//        }

        else if(m_function_flag == 0)
        {
            /*cntHeartBeat++;
            cout << "flag is not right, send heart beat" <<endl;
            if(cntHeartBeat > 250)
            {
                cntHeartBeat = 0;
                pSerialApp ->send(hitcrt::SerialApp::SEND_HEART_BEAT, vecEmpty);
            }
            boost::this_thread::sleep(boost::posix_time::milliseconds(2));*/

            cout << "-----------END-------------" <<endl;
            continue;
        }
        boost::this_thread::sleep(boost::posix_time::milliseconds(20));
    }
}

void manual() {
    char checkChar;
    while(1)
    {
        boost::this_thread::interruption_point();
        checkChar = getchar();
        if('q' == checkChar)
        {
            grabBuffThread.interrupt();
            cout <<"1" << endl;
            grabBuffThread.join();

//            grabArmorThread.interrupt();
//            cout <<"2" << endl;
//            grabArmorThread.join();

            processThread.interrupt();
            cout << "3" << endl;
            processThread.join();

            break;
        }
        else if('c' == checkChar)
        {
            HitFlagMutex.lock();
            flagHitBuff = !flagHitBuff;
            HitFlagMutex.unlock();
        }
        boost::this_thread::sleep(boost::posix_time::milliseconds(500));
    }
}

/*void communication() {
    pSerialApp = new hitcrt::SerialApp("/dev/ttyUSB0", 115200);
    while(1)
    {
        boost::this_thread::interruption_point();
        hitcrt::SerialApp::RECEIVE_FLAG eReceiveFlag;
        std::vector<float> data;
        pSerialApp->receive(eReceiveFlag, data);
//        for (int i = 0; i < data.size(); ++i) {
//            cout << data[i] << endl;
//        }
//        cout << "receiviqqng-------------------------------------------------------------------------" << endl;
//        cout << "eReceiveFlag " << eReceiveFlag <<endl;
//        exit(-2);
        if(eReceiveFlag == hitcrt::SerialApp::RECEIVE_ASSIST_AIM)
        {
            AssitMutex.lock();
            float tempFlag = data.front();
            //cout <<"temp flag " <<tempFlag<<"===================================="<<endl;
            if(fabs(tempFlag-1)<1e-6){
                FLAG_ASSIST_AIM = 1;
            }else if(fabs(tempFlag-2)<1e-6){
                FLAG_ASSIST_AIM = 2;
            }
            else if(fabs(tempFlag)<1e-6){
                FLAG_ASSIST_AIM = 0;
            }
            AssitMutex.unlock();
        }
        else if(eReceiveFlag == hitcrt::SerialApp::RECEIVE_PITCH_ANGLE)
        {
            pitchMutex.lock();
            float tempPitchAngle = data.front();
//            cout << "pitchAngle = " << tempPitchAngle << endl;
            if(tempPitchAngle < hitcrt::BuffParam::MAX_ROBOT_PITCH * 180 / CV_PI
               && tempPitchAngle > hitcrt::BuffParam::MIN_ROBOT_PITCH * 180 / CV_PI){
                pitchAngle = tempPitchAngle * CV_PI / 180;
            }
            pitchMutex.unlock();
        }
        else if(eReceiveFlag == hitcrt::SerialApp::RECEIVE_HIT_BUFF)
        {
            if(fabs(data[0] - 2) < 1e-6)
            {
                BuffMutex.lock();
                FLAG_HIT_BUFF = 2;
                cout << "Hit BigBuff" << endl;
                BuffMutex.unlock();
            }
            else if(fabs(data[0] - 1) < 1e-6)
            {
                BuffMutex.lock();
                FLAG_HIT_BUFF = 1;
                cout << "Hit SmallBuff" << endl;
                BuffMutex.unlock();

            } else if(fabs(data[0]) < 1e-6)
            {
                BuffMutex.lock();
                FLAG_HIT_BUFF = 0;
                cout << "Hit Buff End" << endl;
                BuffMutex.unlock();
            }
        }
        boost::this_thread::sleep(boost::posix_time::milliseconds(20));
    }
}*/


void communication()
{
    my_socket = new ip::udp::socket(my_io_service,local_endpoint);
    int countnum=0;
    size_t len=0;//记录接收到的字节数，解包时用到

    while(1)
    {
        //cout<<"receive thread..."<<endl;
        //从电控端口，接收数据包，存到RecvBuf中
        m_send_mutex.lock();
        len=(*my_socket).receive_from(boost::asio::buffer(RecvBuf, RecvBuf_Size), remote_endpoint);
        m_send_mutex.unlock();
        //protobuf-c 解析数据
        MessageToCPU *msg;//定义一个MessageToCPU类型的消息指针
        msg = message_to__cpu__unpack(NULL, len, (uint8_t *) RecvBuf);//按.proto定义的数据格式解包，内部申请内存，返回结构体指针
        //接下来就可以用结构体的形式，调用接收到的参数啦
        //格式化输出接收到的参数信息，只用于调试
        printf("\r\ncurrent_pitch=%f  function_select=%d  reserved_float1=%f  reserved_float2=%f  reserved_int2=%d  reserved_int3=%d len=%d",
               msg->current_pitch, msg->function_select, msg->reserved_float1, msg->reserved_float2,msg->reserved_int2, msg->reserved_int3,len);

        if(msg->current_pitch < hitcrt::ArmorParam::MAX_ROBOT_PITCH&& msg->current_pitch >hitcrt::ArmorParam::MIN_ROBOT_PITCH)
        {
            pitchDegree = msg->current_pitch;
        }
        m_function_flag = msg->function_select;
//        cout << "receive m_function" << m_function_flag<< endl;
        countnum++;
//        cout<<"countnum="<<countnum<<endl;
//           cout<<"m_function_flag="<<m_function_flag<<endl;
        message_to__cpu__free_unpacked(msg, NULL);//释放数据结构体占用的内存
    }
}


int main()
{
//    grabArmorThread = boost::thread(grabArmor);
    grabBuffThread = boost::thread(grabBuff);
    processThread = boost::thread(process);
    manualThread = boost::thread(manual);
    communicationThread = boost::thread(communication);
    grabBuffThread.join();
    return 0;
}


