#include <iostream>
#include <iomanip>
#include <sstream>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include "Infantry.h"
#include "assist/STF.h"
#include <stdio.h>
#include <time.h>
#include <sys/timeb.h>
//#define  _SAVEPICTURE
#define _DEBUG
#define TIME
using namespace std;
using namespace cv;
using namespace boost::asio;
namespace hitcrt
{
Infantry::Infantry()
{
    //m_serial_vision = new SerialApp("/dev/ttyUSB0", 115200);
    //指针用于向两个任务传递socket
    //ip::udp::endpoint local_endpoint(ip::address_v4::from_string("192.168.1.110"),9999);//本地(电脑)端口信息
    //my_socket = new ip::udp::socket(my_io_service,local_endpoint);
}
Infantry::~Infantry() {}

bool Infantry::grabArmor()
{
    int function_flag;
    cv::FileStorage fs;
    fs.open("/home/sun/HITCRT/Infantry/data/armor/cam20_6mm_480.yml",  cv::FileStorage::READ);
    assert(fs.isOpened());
    fs["exposure"] >> ArmorParam::EXPOSURE;
    fs["width"] >> ArmorParam::WIDHT;
    fs["height"] >> ArmorParam::HEIGHT;
    fs["fps"] >> ArmorParam::FPS;
    capArmor=new myVideoCap("/dev/videoA", ArmorParam::WIDHT, ArmorParam::HEIGHT, ArmorParam::FPS, 1, ArmorParam::EXPOSURE, 1);
    cv::Mat frame_armor;
    capArmor->VideoGrab(frame_armor);

    if(frame_armor.empty())
    {
        cout<<"Armor: Cannot open camera"<<endl;
        return false;
    }
    while(1)
    {

        boost::this_thread::interruption_point();
        functionMutex.lock();
        function_flag=m_function_flag;
        functionMutex.unlock();
        if(function_flag==1)   ///assist_aim function
        {
            if(armorImgMutex.try_lock())
            {
                capArmor->VideoGrab(frameArmor);
                armorImgMutex.unlock();
            }
            pitchMutex.lock();
            pitchDegree_grab = pitchDegree;
            pitchMutex.unlock();
        }
        boost::this_thread::sleep(boost::posix_time::milliseconds(1));
    }
}

bool Infantry::grabBuff()
{
    cv::FileStorage fs;
    int function_flag;
    fs.open("/home/sun/HITCRT/Infantry/data/buff/camera6_0504.yml", cv::FileStorage::READ);
    fs["exposure"] >> BuffParam::EXPOSURE;
    cout<<"BuffParam::EXPOSURE"<<BuffParam::EXPOSURE<<endl;
    capBuffer = new myVideoCap("/dev/videoB",1920, 1080, 30, 1, BuffParam::EXPOSURE, 1);
    cv::Mat frame_buffer;
    capBuffer->VideoGrab(frame_buffer);
    if(frame_buffer.empty())
    {
        cout<<"Buff: Cannot open camera"<<endl;
        return false;
    }

    while(1)
    {
        functionMutex.lock();
        function_flag=m_function_flag;
        functionMutex.unlock();

        boost::this_thread::interruption_point();
        //if(function_flag==2||function_flag==3)   ///buff function
        //{
            if(buffImgMutex.try_lock())
            {
                capBuffer->VideoGrab(frameBuff);
                buffImgMutex.unlock();
            }
        //}
        boost::this_thread::sleep(boost::posix_time::milliseconds(5));
    }
}

void Infantry::process()
{
//    ip::udp::endpoint remote_endpoint(ip::address_v4::from_string("192.168.1.30"), 9999);
//    MessageToMCU msge = MESSAGE_TO__MCU__INIT;

    STF KF;
    angleSolver=new AngleSolver;

    hitcrt::BuffHiter buffHiter;
    int cntTemp = 0;
    bool hitbuffSuccess = true;
    cv::Mat R,new_Matrix;
    cv::initUndistortRectifyMap(hitcrt::BuffParam::KK,hitcrt::BuffParam::DISTORT,R,new_Matrix,Size(1920,1080),CV_32FC1,
                                hitcrt::BuffParam::MAPX,hitcrt::BuffParam::MAPY);
    initUndistortRectifyMap(armorDetector.CAMERA_MATRIX, armorDetector.DISTORT, noArray()
                            ,noArray(), Size(ArmorParam::WIDHT,ArmorParam::HEIGHT),CV_32FC1,armorDetector.MAPX, armorDetector.MAPY);

    VideoCapture cap;
    if(!cap.open("/home/sun/HITCRT/Infantry/infantry_AE/car1/car1_30_000_2.mp4")){
        cout<<"cannot open video "<<endl;
   }

   cv::Mat video_frame;

    time_t log_time;
    int namelog = time(&log_time);
    ostringstream oslog;
    oslog <<"/home/sun/HITCRT/Infantry/data/logs/" << namelog <<"_send.txt";
    cout << "oslog" << oslog.str()<< endl;
    ofstream out1(oslog.str(),ios_base::out);
#ifdef _SAVEPICTURE
    string armorFrameSavePath = "/home/base2/data/armor/armor_img/";
    string buffFrameSavePath = "/home/base2/data/buff/buff_img/";
    unsigned long cntFrameArmorRecord = 0;
    unsigned long cntFrameBuffRecord = 0;
    VideoWriter writer("/home/base2/data/buff/buff_img/buff_0510.avi", CV_FOURCC('M', 'J', 'P', 'G'), 10, Size(1920,1080));
#endif


#ifdef TIME
    double tcon1 = (double)cv::getTickCount();
    double tcon2;
#endif TIME
    while(1)
    {
        cap >> video_frame;
        Mat video_img = video_frame.clone();
        resize(video_img, video_img, Size(0,0), 0.3, 0.3);
        imshow("raw", video_img);
        waitKey(0);
        if(video_frame.empty()){
            cout <<"no frame "<<endl;
            continue;
        }

        boost::this_thread::interruption_point();
         int function_flag;
        functionMutex.lock();
        function_flag=m_function_flag;
        functionMutex.unlock();
        function_flag = 2;

        QuadExtractor::functionFlag = function_flag;
//        cout << "process = " << function_flag <<endl;
        double tra1 = (double)cv::getTickCount();
        if(function_flag == 2 || function_flag == 3)
        {
//            vector<float> send_data;
//            int cntFrameTrash = 0; //除去残余图像帧
//            while(cntFrameTrash < 4)
//            {
//                if(!frameBuff.empty())
//                {
//                    frame2 = frameBuff.clone();
//                    frameBuff.release();
//                    buffImgMutex.unlock();
//                }
//                else
//                {
//                    buffImgMutex.unlock();
//                    continue;
//                }
//                cntFrameTrash++;
//            }
#ifdef TIME
            double tra2 = ((double)cv::getTickCount() - tra1)*1000 / cv::getTickFrequency();
            cout << "get img time is: " << tra2 << endl;
#endif TIME

#ifdef _SAVEPICTURE
        double saveImgTime1 = cv::getTickCount();
        ostringstream fileName;
        fileName << cntFrameBuffRecord << ".png";
        buffImgMutex.lock();
        if(cntFrameBuffRecord < 3000)
            cntFrameBuffRecord ++;
        else
            cntFrameBuffRecord = 0;
        imwrite(buffFrameSavePath + fileName.str(), frame2);
        writer << frame2;
        buffImgMutex.unlock();
        double saveImgTime2 = ((double)cv::getTickCount() - saveImgTime1)*1000/ cv::getTickFrequency();
        cout << "save img time is: " << saveImgTime2 << endl;

#endif // _SAVEPICTURE

            //remap(frame2,frame2,hitcrt::BuffParam::MAPX,hitcrt::BuffParam::MAPY,INTER_LINEAR);
            remap(video_frame,video_frame,hitcrt::BuffParam::MAPX,hitcrt::BuffParam::MAPY,INTER_LINEAR);

            double start = cv::getTickCount();
            hitbuffSuccess = buffHiter.run(video_frame, function_flag);
            waitKey(0);
            cout << "buffHiter run time is: " << (cv::getTickCount() - start)*1000/cv::getTickFrequency()<<"ms"<< endl;
            if(!hitbuffSuccess) continue;
//                waitKey(0);

            //old
            /*vector<float> sendData;
            sendData.push_back(buffHiter.stateCodeNum);
            sendData.push_back(buffHiter.yaw);
            sendData.push_back(buffHiter.pitch);
            pSerialApp->send(hitcrt::SerialApp::SEND_ERR_UV, sendData);*/
            double sendTime1 = cv::getTickCount();

            ///
//            msge.target_flag=buffHiter.stateflag;//给六个参数赋值
//            msge.aim_pitch=buffHiter.pitch;
//            msge.aim_yaw=buffHiter.yaw;
//            msge.distance=0;
//            msge.buff_state=buffHiter.stateCodeNum;
//            msge.heart_beat=0;
            if(buffHiter.stateCodeNum!=buffHiter.laststateCodeNum)
            {
            buffHiter.changestateCodeNum=buffHiter.changestateCodeNum+1;
            }
            buffHiter.laststateCodeNum=buffHiter.stateCodeNum;
            cout<<"buffHiter.laststateCodeNum = "<<buffHiter.laststateCodeNum<<endl;
            cout<<"buffHiter.stateCodeNum = "<<buffHiter.stateCodeNum<<endl;
            cout<<"buffHiter.changestateCodeNum = "<<buffHiter.changestateCodeNum<<endl;
            //计算数据包的大小，确定要发送的数据长度，注意数据有效位数不同时，包的大小会有变化，例如在21-29浮动
            if(buffHiter.changestateCodeNum>0){
                ///
//            size_t len = message_to__mcu__get_packed_size(&msge);
//            char *SendBuf= (char *)malloc(50);//为发送缓冲区申请内存，100暂时够用，发送6个变量可能需要20-30字节
//            message_to__mcu__pack(&msge,(uint8_t *)SendBuf);//将存储参数，赋值好的结构体打包存进发送缓冲区，准备发送
//            //将打包好的数据，发送到电控的网口
//            (*my_socket).send_to(boost::asio::buffer((char*)SendBuf, len), remote_endpoint);
//            free(SendBuf);
            double sendTime2 = ((double)cv::getTickCount() - sendTime1)*1000/ cv::getTickFrequency();
            cout << "send data time is: " << sendTime2 << endl;

#ifdef TIME
            tcon2 = ((double)cv::getTickCount() - tcon1)*1000 / cv::getTickFrequency();
            out1 <<"buffHiter.pitch: " << buffHiter.pitch<< "buffHiter.yaw: "<<buffHiter.yaw<<"buffHiter.stateCodeNum: "<<buffHiter.stateCodeNum<< endl;
            out1<<"time for send socket for buff: "<<tcon2<<endl;
            tcon1 = (double)cv::getTickCount();
#endif TIME
            t22 = ((double)cv::getTickCount() - t11) / cv::getTickFrequency()*1000;
            t11 = (double)cv::getTickCount();
            fps = 1000.0/ t22;

            cout<<"-------------------------send yaw and pitch theta------------------------------"<<endl;
            cout << "send state yaw and pitch are: " << "[" << buffHiter.stateCodeNum <<"\t"<< buffHiter.yaw << "\t" <<buffHiter.pitch << "]" << endl;
            cout <<"total time is: "<<t22<<endl;
            cout << "fps is: " << fps << endl;
            }
            waitKey(10);
        }
        else if(function_flag == 1)
        {
//            cout<<"asist mode"<<endl;
            buffHiter.stateCodeNum=-1;
            buffHiter.laststateCodeNum=-1;
            buffHiter.changestateCodeNum=-1;
            buffHiter.saveTime=1;
            armorImgMutex.lock();
            if(!frameArmor.empty())
            {
                grabframe = frameArmor.clone();
                pitchMutex.lock();
                m_pitchDegree = pitchDegree_grab;
                pitchMutex.unlock();
                frameArmor.release();
                armorImgMutex.unlock();
            }
            else
            {
                armorImgMutex.unlock();
                continue;
            }

#ifdef _SAVEPICTURE
            ostringstream fileName;
            fileName << cntFrameArmorRecord << ".png";
            if(fmod(cntFrameArmorRecord,30)==0)
            {
                imwrite(armorFrameSavePath + fileName.str(), grabframe);
            }
            cntFrameArmorRecord++;
#endif

            target_flag = false;
            vector<float> send_data;
                t22 = ((double)cv::getTickCount() - t11) / cv::getTickFrequency();
                t11 = (double)cv::getTickCount();
                fps = 1.0 / t22;
            if(roijudge())
            {
                target_flag=true;
                velocityMutex.lock();
                angleSolver->VELOCITY=velocity;
                velocityMutex.unlock();
                send_distance=TargetArmor.m_distance;
                angleSolver->calculateSendDegree(m_pitchDegree,TargetArmor);
                sendEjectDegree = angleSolver->Eject_Degree;
                //float pitch_eject=PitchAnglefilter(sendEjectDegree);
                sendYawDegree = angleSolver->Yaw_Degree;
                compensate = 0;
                if(sendYawDegree>0)
                    compensate = 3*pow(abs(sendYawDegree),1.3);
                else compensate = -3*pow(abs(sendYawDegree),1.3);
                KF.measurement.at<float>(0) = float(TargetArmor.Xc);
                Mat prediction = KF.predict(KF.measurement);
                TargetArmor.Xc = prediction.at<float>(0);
                angleSolver->calculateSendDegree(m_pitchDegree,TargetArmor);
                sendYawDegree_Pre = angleSolver->Yaw_Degree;
                if(pre_count >= 25)
                    sendYawDegree=sendYawDegree_Pre;
                else pre_count++;

                //old
                /*send_data.push_back(target_flag);
                send_data.push_back(sendEjectDegree);
                send_data.push_back(sendYawDegree);
                send_data.push_back(send_distance);
                m_serial_vision->send(hitcrt::SerialApp::SEND_ASSIST_AIM, send_data);
                cout << "pitchDegree = " << m_pitchDegree <<endl;*/

                cout << "Send Pitch = " << sendEjectDegree<< ",Send Yaw = " << sendYawDegree <<",Send distance = " << send_distance << "Target fps = " << fps<<endl;
                cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ " << endl;
            }
            else
            {
                pre_count = 0;
                target_flag=0;
                sendEjectDegree=0;
                sendYawDegree=0;
                send_distance=0;
                cout<<"fps= "<<fps<<"  no target~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<< endl;
//                continue;
            }
            //定义并初始化MessageToMCU类型的消息结构体，默认初始值都是0
//            msge.target_flag=target_flag;//给六个参数赋值
//            msge.aim_pitch=sendEjectDegree;
//            msge.aim_yaw=sendYawDegree;
//            msge.distance=send_distance;
//            msge.buff_state=0;
//            msge.heart_beat=0;
//
//
//            //计算数据包的大小，确定要发送的数据长度，注意数据有效位数不同时，包的大小会有变化，例如在21-29浮动
//            size_t len = message_to__mcu__get_packed_size(&msge);
//            char *SendBuf= (char *)malloc(50);//为发送缓冲区申请内存，100暂时够用，发送6个变量可能需要20-30字节
//            message_to__mcu__pack(&msge,(uint8_t *)SendBuf);//将存储参数，赋值好的结构体打包存进发送缓冲区，准备发送
//            //将打包好的数据，发送到电控的网口
////             m_send_mutex.lock();
//            (*my_socket).send_to(boost::asio::buffer((char*)SendBuf, len), remote_endpoint);
////            m_send_mutex.unlock();
//            free(SendBuf);
        }
        else if(function_flag == 0)
        {
//            cout << "-----------------END------------------" << endl;
            buffHiter.saveTime=1;
        }
        else
        {
            cout << "function flag is not right" << endl;
            buffHiter.saveTime=1;
        }
        QuadExtractor::lastFunctionFlag = function_flag;
    }
}

void Infantry::udp_receive()
{

    ip::udp::endpoint remote_endpoint(ip::address_v4::from_string("192.168.1.30"), 9999);
    size_t len=0;//记录接收到的字节数，解包时用到

    while(1)
    {
        //cout<<"receive thread..."<<endl;
        //从电控端口，接收数据包，存到RecvBuf中
        m_send_mutex.lock();
        len=(*my_socket).receive_from(boost::asio::buffer(RecvBuf, RecvBuf_Size), remote_endpoint);
        //cout<<"get received data"<<endl;
        m_send_mutex.unlock();
        //protobuf-c 解析数据
        MessageToCPU *msg;//定义一个MessageToCPU类型的消息指针
        msg = message_to__cpu__unpack(NULL, len, (uint8_t *) RecvBuf);//按.proto定义的数据格式解包，内部申请内存，返回结构体指针
        //接下来就可以用结构体的形式，调用接收到的参数啦
        //格式化输出接收到的参数信息，只用于调试
//        printf("\r\ncurrent_pitch=%f  function_select=%d  reserved_float1=%f  reserved_float2=%f  reserved_int2=%d  reserved_int3=%d len=%d",
//               msg->current_pitch, msg->function_select, msg->reserved_float1, msg->reserved_float2,msg->reserved_int2, msg->reserved_int3,len);

        pitchMutex.lock();
        if(msg->current_pitch < ArmorParam::MAX_ROBOT_PITCH&& msg->current_pitch > ArmorParam::MIN_ROBOT_PITCH)
        {
            pitchDegree = msg->current_pitch;
        }
        pitchMutex.unlock();

        functionMutex.lock();
        m_function_flag = msg->function_select;
        functionMutex.unlock();

        velocityMutex.lock();
        velocity=msg->reserved_float1*1000;
        if(velocity<10000) velocity=25000;
        velocityMutex.unlock();
        //cout<<"m_function_flag="<<m_function_flag<<endl;
        message_to__cpu__free_unpacked(msg, NULL);//释放数据结构体占用的内存
    }
}

void Infantry::manual()
{

    char keyCheck;
    while(1)
    {
        keyCheck = getchar();
        ///此处设置手动结束各个线程，在实际应用中推荐与下位机通信来改变线程，目的是为了调试方便
        boost::this_thread::interruption_point();
        if('q' == keyCheck)
        {
            grabArmorThread.interrupt();
            cout <<"1" << endl;
            grabArmorThread.join();

            grabBuffThread.interrupt();
            cout <<"2" << endl;
            grabBuffThread.join();

            processThread.interrupt();
            cout <<"3" << endl;
            processThread.join();

            thread_udp_receive.interrupt();
            cout <<"4" << endl;
//            thread_udp_receive.join();
//            receiveThread.interrupt();
            break;
        }
        else if('x' == keyCheck)
        {
            functionMutex.lock();
            m_function_flag = 2;
            functionMutex.unlock();
        }
        else if('d' == keyCheck)
        {
            functionMutex.lock();
            m_function_flag = 3;
            functionMutex.unlock();
        }
        else if('f' == keyCheck)
        {
            functionMutex.lock();
            m_function_flag = 1;
            functionMutex.unlock();
        }

        boost::this_thread::sleep(boost::posix_time::milliseconds(500));
    }
}

bool Infantry::roijudge()
{

    grabframe.copyTo(frame);
    if(find_flag>=3)
    {
        frame(rect).copyTo(frame);
        roi_flag = 1;
    }
    vector<hitcrt::Armor> armors;
    armorDetector.Apply(armors,frame,grabframe, roi_flag, roi_origin);
#ifdef _DEBUG
    armorDetector.showImage(armors,grabframe);
#endif // _DEBUG
if(armors.size() < 1)
    {
        if(roi_flag==0) find_flag=0;
        else
        {
            Miss_flag++;
            if(Miss_flag >= 5)
            {
                find_flag=0;
                Miss_flag = 0;
                roi_flag = 0;
                Armors.clear();
            }
        }
        return false;
    }
    if(roi_flag==0)  find_flag++;
    if(find_flag<3)  return false;
    float min_distance = 100000;
    int hereo_index = -1;
    int min_index = 0;
    for(int i = 0; i < armors.size(); ++i )
    {
        if(armors[i].m_type == Armor::HERO)  hereo_index = i;
        if(armors[i].m_distance < min_distance)
        {
            min_distance = armors[i].m_distance;
            min_index = i;
        }
    }
    if(hereo_index >= 0 && (armors[hereo_index].m_distance-armors[min_index].m_distance)<1000)
    {
        min_index = hereo_index;
    }
    roi_center=armors[min_index].m_center_uv;
//    float armor_height = fabs(armors[min_index].m_bottom_left.y-armors[min_index].m_upper_left.y);
    float armor_width = fabs(armors[min_index].m_upper_left.x-armors[min_index].m_upper_right.x);
    roi_origin = Point2f(max(roi_center.x-3*armor_width,0.0f),max(roi_center.y-3*armor_width,0.0f));
    Rect2f rect0(Point2f(max(roi_center.x-3*armor_width,0.0f),max(roi_center.y-3*armor_width,0.0f))
                 ,Point2f(min(roi_center.x+3*armor_width,640.0f),min(roi_center.y+3*armor_width,480.0f)));
    rect = rect0;
    TargetArmor=armors[min_index];
}

float Infantry::PitchAnglefilter(float angle)
{
    static std::vector<float> pitchAngleBuff;
    pitchAngleBuff.push_back(angle);
    if(pitchAngleBuff.size() < 10)
    {
        return 0;
    }
    float sum = 0;
    for(int i = 1; i < 10; i++)
    {
        sum+=pitchAngleBuff[i-1];
        pitchAngleBuff[i-1] = pitchAngleBuff[i];
    }
    pitchAngleBuff[9] = angle;
    sum+= angle;
    return (sum/10);
}

void Infantry::run()
{
    processThread = boost::thread(boost::bind(&Infantry::process, this));
    //grabArmorThread = boost::thread(boost::bind(&Infantry::grabArmor, this));
    //grabBuffThread = boost::thread(boost::bind(&Infantry::grabBuff, this));
    manualThread = boost::thread(boost::bind(&Infantry::manual,this));
    //thread_udp_receive = boost::thread(boost::bind(&Infantry::udp_receive,this));
    //receiveThread = boost::thread(boost::bind(&Infantry::receive,this));
    //thread_udp_receive.join();
    manualThread.join();
}

/*void Infantry::createHeartBeatThread() {
heartBeatThread = boost::thread(boost::bind(&Infantry::heartBeat,this));
}

void Infantry::receive() {
cout<<"receive "<<endl;
while(1)
{
    boost::this_thread::interruption_point();
    hitcrt::SerialApp::RECEIVE_FLAG eReceiveFlag;
    std::vector<float> data;
    m_serial_vision->receive(eReceiveFlag, data);
//
    if(eReceiveFlag == hitcrt::SerialApp::RECEIVE_PITCH_ANGLE)
    {
        pitchMutex.lock();
        float tempPitchDegree = data.front();

        if(tempPitchDegree <ArmorParam::MAX_ROBOT_PITCH&& tempPitchDegree >ArmorParam::MIN_ROBOT_PITCH)
        {
            pitchDegree = tempPitchDegree;
        }
        pitchMutex.unlock();
    }
//        cout << "pitchDegree = " << pitchDegree << endl;
    if(eReceiveFlag == hitcrt::SerialApp::RECEIVE_FUNCTION_SELECT)
    {
        //cout << "RECEIVE_FUNCTION_SELECT"<< endl;
        functionMutex.lock();
        float  function_flag = data.front();
        if(abs(function_flag)<1e-6)
        {
            m_function_flag = 0;
        }
        else if(abs(function_flag-1)<1e-6)
        {
            m_function_flag= 1;
        }
        else if(abs(function_flag-2)<1e-6)
        {
            m_function_flag = 2;
        }
        else if(abs(function_flag-3)<1e-6)
        {
            m_function_flag = 3;
        }
        functionMutex.unlock();
        cout<<"m_function_flag="<<m_function_flag<<endl;
    }

    boost::this_thread::sleep(boost::posix_time::milliseconds(15));
}
}

void Infantry::heartBeat() {
cout<<"heartBeat "<<endl;
while(1)
{
    boost::this_thread::interruption_point();
    boost::this_thread::sleep(boost::posix_time::milliseconds(500));
    vector<float> heartbeat= {0,0,0};
    m_serial_vision->send(hitcrt::SerialApp::SEND_HEART_BEAT,heartbeat);
}
}*/
} //hitcrt namespace
