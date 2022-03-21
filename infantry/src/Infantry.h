#ifndef INFANTRY_H
#define INFANTRY_H

#include <iomanip>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/atomic/atomic.hpp>

#include "assist/ArmorDetector.h"
#include "assist/AngleSolver.h"

#include "buff/BuffHiter.h"

#include "cameraset/myVideoCap.h"
#include "serial/serialapp.h"
#include "protobuf/protobuf-c.h"
#include "protobuf/hitcrt_rm0422.pb-c.h"

#define RecvBuf_Size 300

using namespace boost::asio;

namespace hitcrt {
class Infantry {
private:
    //mutex
    boost::mutex m_send_mutex;
    boost::mutex armorImgMutex;
    boost::mutex pitchMutex;
    boost::mutex buffImgMutex;
    boost::mutex functionMutex;
    boost::mutex velocityMutex;
    //thread
    boost::thread grabArmorThread;
    boost::thread grabBuffThread;
    boost::thread processThread;
    boost::thread manualThread;
    boost::thread thread_udp_receive;

    //serial vision
   /* boost::thread receiveThread;//serial communication
    boost::thread heartBeatThread;
    SerialApp* m_serial_vision;*/

    //net port
    char RecvBuf[RecvBuf_Size];
    ip::udp::socket* my_socket;
    boost::asio::io_service my_io_service;

    //common
    int m_function_flag=0;

    //assist
    cv::Mat frameArmor, grabframe, frame;
    hitcrt::myVideoCap* capArmor;
    hitcrt::AngleSolver* angleSolver;
    hitcrt::ArmorDetector armorDetector;
    std::vector<hitcrt::Armor> Armors;
    hitcrt::Armor TargetArmor;

    float sendEjectDegree;
    float sendYawDegree;
    float sendYawDegree_Pre;
    float send_distance;
    float velocity=25;
    float pitchDegree = 0.0f;
    float pitchDegree_grab=0.0f;
    float m_pitchDegree=0.0f;

    double t11 = 0, t22 = 0, fps = 0;
    bool target_flag = 0;
    int pre_count = 0;
    int find_flag = 0;
    int roi_flag = 0;
    int Miss_flag = 0;
    float compensate = 0;
    Point2f roi_center;
    Point2f roi_origin;
    cv::Rect2f rect;

    //buff
    hitcrt::myVideoCap* capBuffer;
    cv::Mat frameBuff,frame2;

    //thread function
    bool grabArmor();
    bool grabBuff();
    void process();
    void udp_receive();
    void manual();

    void heartBeat();
    bool roijudge();

    /*void receive();
    void createHeartBeatThread();*/
    float PitchAnglefilter(float angle);
public:

    Infantry();
    virtual ~Infantry();
    void run();
};
}
#endif // INFANTRY_H
