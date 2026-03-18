/*
 * rear_camera_detect_task.cpp
 *
 *  Created on: 2024年4月29日
 *      Author: pengzc
 */

#include <misc/rear_view_detection.h>
#include <mpi/case/shared_video_device.h>
#include <mpi/case/config.h>

RearViewDetection::~RearViewDetection() {
}

void RearViewDetection::doTask() {
  while (isStarted()) {
    try {
      mpi::SharedVideoDevice device1(mpi::VIDEO_DEVICE_FRONT);
      mpi::SharedVideoDevice device(mpi::VIDEO_DEVICE_REAR);
      while (isStarted()) {
    	  wait();
      }
    } catch (mpi::Exception& e) {
      LOGE_TRACE("%s", e.what());
      Thread::sleep(1000);
    }
  }
}
