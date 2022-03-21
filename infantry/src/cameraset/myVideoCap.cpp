#include"myVideoCap.h"
#include<iostream>

using namespace std;
namespace hitcrt
{


    myVideoCap::myVideoCap(char *filename,int width,int height ,int fps,int exposure_mode,int exposure_absolute, int change_mode)
    {
        width_=width;
        height_=height;
        fps_=fps;
        exposure_mode_=exposure_mode;
        exposure_absolute_=exposure_absolute;
        file = fopen(filename, "wb");
        vd = (struct vdIn *) calloc(1, sizeof(struct vdIn));
        EncodedGraph = Mat(Size(width_,height_),CV_64FC4);

        if(init_videoIn(vd, (char *) filename, width_, height_,fps_,format,grabmethod) < 0)
        {
          exit(1);
        }

        if (video_enable(vd))
        {
           exit(1);
        }

        memset(&vd->buf, 0, sizeof(struct v4l2_buffer));

        vd->buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        vd->buf.memory = V4L2_MEMORY_MMAP;
#ifdef DEBUG
        printf("recording to %s\n","TEST");
#endif // DEBUG
    //      return 0;
        if(change_mode == 1)
        {
            ///设置曝光模式
            ctrl.id = V4L2_CID_EXPOSURE_AUTO;
            ctrl.value =exposure_mode_;
            ret = ioctl(vd->fd, VIDIOC_S_CTRL, &ctrl);
            ///设置曝光值
            ctrl.id=V4L2_CID_EXPOSURE_ABSOLUTE;
            ctrl.value=exposure_absolute_;
            ret = ioctl(vd->fd, VIDIOC_S_CTRL, &ctrl);
        }
    }




bool myVideoCap::VideoGrab(Mat &dst)
    {

        ret = ioctl(vd->fd, VIDIOC_DQBUF, &vd->buf);
        if (ret < 0)
        {
          printf("Unable to dequeue buffer");
          exit(1);
        }
        memcpy((void *)EncodedGraph.data, vd->mem[vd->buf.index],vd->buf.bytesused);

        dst = imdecode(EncodedGraph,1);

        vd->framecount++;

        ret = ioctl(vd->fd, VIDIOC_QBUF, &vd->buf);
        if (ret < 0)
        {
          printf("Unable to requeue buffer");
          exit(1);
        }
    }


    myVideoCap::~myVideoCap()
    {
        fclose(file);
        close_v4l2(vd);
    }



}






