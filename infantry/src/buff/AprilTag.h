#ifndef QUADDETECT2_INCLUDED
#define QUADDETECT2_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

#include "common/matd.h"
#include "common/image_u8.h"
#include "common/zarray.h"
#include "common/workerpool.h"
#include "common/timeprofile.h"
#include <pthread.h>
#include "apriltag.h"

#define APRILTAG_TASKS_PER_THREAD_TARGET 10

// Detect tags from an image and return an array of
// apriltag_detection_t*. You can use apriltag_detections_destroy to
// free the array and the detections it contains, or call
// _detection_destroy and zarray_destroy yourself.
zarray_t *apriltag_detector_detect_Quads(apriltag_detector_t *td, image_u8_t *im_orig);


#ifdef __cplusplus
}
#endif

#endif
