#ifndef HITCRT_ROBOMASTERS_MYVIDEOCAP_H_
#define HITCRT_ROBOMASTERS_MYVIDEOCAP_H_

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>


  #include <stdio.h>
  #include <stdlib.h>
  #include <unistd.h>
  #include <sys/types.h>
  #include <sys/stat.h>
  #include <sys/file.h>
  #include <string.h>
  #include <pthread.h>
  #include <linux/videodev2.h>
  #include <sys/ioctl.h>
  #include <sys/mman.h>
  #include <errno.h>
  #include <fcntl.h>
  #include <time.h>
  #include <sys/time.h>
  #include <signal.h>
//  #include <X11/Xlib.h>


  extern "C"
{
  #include "v4l2uvc.h"
}

using namespace cv;

///具体查一下v4l2功能集
///曝光模式 V4L2_EXPOSURE_AUTO = 0,
///V4L2_EXPOSURE_MANUAL = 1,
///V4L2_EXPOSURE_SHUTTER_PRIORITY = 2,
///V4L2_EXPOSURE_APERTURE_PRIORITY = 3

namespace hitcrt
{



  class myVideoCap
  {
  public:
      myVideoCap(char *filename,int width=640,int height=480,int fps=30,int exposure_mode =1 ,int exposure_absolute =22, int change_mode = 0 );
      ~myVideoCap();
      bool VideoGrab(Mat &dst);


  private:
      FILE *file = NULL;
      int format =V4L2_PIX_FMT_MJPEG;
      int ret;
      int grabmethod = 1;
      int width_ =640;
      int height_ =480;
      int fps_=30;
      int exposure_mode_=1;
      int exposure_absolute_=22;
      struct vdIn *vd;
      ///
      struct v4l2_control ctrl;
      Mat EncodedGraph;
  };




}



#endif // HITCRT_ROBOMASTERS_RUNE_H_
