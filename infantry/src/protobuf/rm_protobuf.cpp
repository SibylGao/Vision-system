//
// Created by yhp on 18-3-15.
//
#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

//protobuf-c包含的头文件，相应的功能函数在对应的“.c”文件中
//这里将它们编译生成“InfantryDebug/RM/lib/libmyprotobuf.so”再调用
#include "protobuf-c.h"
#include "hitcrt_rm0417.pb-c.h"

using namespace std;
using namespace boost::asio;

io_service my_io_service;
ip::udp::endpoint local_endpoint(ip::address_v4::from_string("192.168.1.110"),9999);//本地(电脑)端口信息
ip::udp::endpoint remote_endpoint(ip::address_v4::from_string("192.168.1.30"), 9999);//远端(单片机端)端口信息

//char *send_data = "this is CPU,can you hear me?";//测试用的字符串

ip::udp::socket* my_socket;//指针用于向两个任务传递socket
boost::mutex m_send_mutex;//互斥量，用于给socket加锁

#define RecvBuf_Size 300//实际30-40可能就够用
char RecvBuf[RecvBuf_Size];

//发送和接收周期(延时时间)，单位ms
#define SendPeriod 2
#define RecvPeriod 1

//发送线程
void udp_send()
{
    int TestCnt=0;
    while(1)
    {
        //cout<<"send thread..."<<endl;
        //产生变化的数据,只用于调试，测试通信效果
        if(TestCnt++>9999)   TestCnt=0;

        //发送前用protobuf-c将数据打包
        MessageToMCU msge = MESSAGE_TO__MCU__INIT;//定义并初始化MessageToMCU类型的消息结构体，默认初始值都是0

        msge.target_flag=13.56+(float)TestCnt;//给六个参数赋值
        msge.aim_pitch=24.67+(float)(TestCnt%9000)/100.0;
        msge.aim_yaw=45.78+(float)(TestCnt%9000)/100.0;
        msge.distance=56.78+(float)(TestCnt%1000)/100.0;
        msge.reserved_float1=12+TestCnt%1000;
        msge.heart_beat=34+TestCnt%1000;

        //计算数据包的大小，确定要发送的数据长度，注意数据有效位数不同时，包的大小会有变化，例如在21-29浮动
        size_t len = message_to__mcu__get_packed_size(&msge);

        char *SendBuf= (char *)malloc(100);//为发送缓冲区申请内存，100暂时够用，发送6个变量可能需要20-30字节
        message_to__mcu__pack(&msge,(uint8_t *)SendBuf);//将存储参数，赋值好的结构体打包存进发送缓冲区，准备发送

        //将打包好的数据，发送到电控的网口
        m_send_mutex.lock();
        (*my_socket).send_to(boost::asio::buffer((char*)SendBuf, len), remote_endpoint);
        m_send_mutex.unlock();

        free(SendBuf);//释放发送缓冲区内存

        //线程延时
        boost::this_thread::sleep(boost::posix_time::milliseconds(SendPeriod));
    }
}

//接收线程
void udp_receive()
{
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

        message_to__cpu__free_unpacked(msg, NULL);//释放数据结构体占用的内存

        //线程延时
        boost::this_thread::sleep(boost::posix_time::milliseconds(RecvPeriod));
    }
}

int main()
{
    // cout<<"main"<<endl;
    my_socket = new ip::udp::socket(my_io_service,local_endpoint);//创建socket

    //下面是两个主要线程
    boost::thread thread_udp_send(&udp_send);
    boost::thread thread_udp_receive(&udp_receive);

    thread_udp_receive.join();
    thread_udp_send.join();
    return 0;
}
