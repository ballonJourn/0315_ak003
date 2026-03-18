/*
 * utility.cpp
 *
 *  Created on: 2024年4月3日
 *      Author: pengzc
 */

#include "utility.h"
#include <base/files.hpp>
#include <manager/ConfigManager.h>
#include <base/strings.hpp>
#include <mpi/case/app_server.h>
#include <Json/value.h>
#include <net/NetUtils.h>
#include "version.h"
#include "edge/popup_service.h"
#include <system/Thread.h>
#include <storage/StoragePreferences.h>

void restartRecorder(bool force) {
  if (!mpi::Recorder::instance().isStarted() || force) {
    mpi::Recorder::instance().stop();
    mpi::Recorder::instance().start(sys::setting::makeRecorderParam());
  }
}

void setupCamera(ViewType view, bool is_sync) {
  switch (view) {
    case VIEW_TYPE_FRONT: {
      mpi::RearCamera::instance().setVisible(false, true);
      mpi::CameraParam param;
      param.fps = sys::setting::get_camera_rate_mode();
      param.mirror = false;
      param.viewbox = {0, 0, 1920, 1080};
      param.display = {0, 0, 1600, 600};
      mpi::FrontCamera::instance().setParam(param, is_sync);
      // mpi::RearCamera::instance().setVisible(false, false);
    }
      break;
    case VIEW_TYPE_REAR: {
      mpi::FrontCamera::instance().setVisible(false, true);
      mpi::CameraParam param;
      param.fps = sys::setting::get_camera_rate_mode();
      param.mirror = sys::setting::is_rear_mirror();
      param.viewbox = {0, 0, 1920, 1080};
//      param.viewbox = {0, 0, 1280, 720};
//      param.display = {76, 141, 300, 196};
      param.display = {0, 0, 1600, 600};
      mpi::RearCamera::instance().setParam(param, is_sync);
      // mpi::FrontCamera::instance().setVisible(false, false);
    }
      break;
    case VIEW_TYPE_BOTH: {
      mpi::RearCamera::instance().setVisible(false, true);
      mpi::CameraParam param;
      param.fps = sys::setting::get_camera_rate_mode();
      {
        param.mirror = false;
        param.viewbox = {0, 0, 1920, 1080};
        param.display = {0, 0, 800, 600};
        param.visible = true;
//        param.layer = 0;
        mpi::FrontCamera::instance().setParam(param, is_sync);
      }
      {
        param.mirror = sys::setting::is_rear_mirror();
        param.viewbox = {0, 0, 1920, 1080};
//        param.viewbox = {0, 0, 1280, 720};
        param.display = {800, 0, 800, 600};
        param.visible = true;
//        param.layer = 4;
        mpi::RearCamera::instance().setParam(param, is_sync);
      }
    }
      break;
    case VIEW_TYPE_UNVISIBLE: {
      mpi::FrontCamera::instance().setVisible(false, is_sync);
      mpi::RearCamera::instance().setVisible(false, is_sync);
    }
      break;
    case VIEW_TYPE_REVERSE: {
      auto param = sys::setting::makeReverseCameraParam();
      mpi::FrontCamera::instance().setVisible(false, is_sync);
      mpi::RearCamera::instance().setParam(param, is_sync);
    }
      break;
    case VIEW_TYPE_FRONT_MINI: {
      mpi::CameraParam param;
      param.viewbox = {0, 0, 1920, 1080};
      param.display = {78, 132, 400, 240};
      mpi::RearCamera::instance().setVisible(false, is_sync);
      mpi::FrontCamera::instance().setParam(param, is_sync);
    }
    break;
    case VIEW_TYPE_REAR_MINI: {
      mpi::CameraParam param;
      param.viewbox = {0, 0, 1920, 1080};
      param.display = {78, 132, 400, 240};
      param.mirror = sys::setting::is_rear_mirror();
      mpi::FrontCamera::instance().setVisible(false, is_sync);
      mpi::RearCamera::instance().setParam(param, is_sync);
    }
    break;
    default:
      break;
  }
}

void resetAllProcess() {
  PopupService::show([](PopupService* srv) {
    srv->setMessage("@wait a moment");
    NO_EXCEPTION(mpi::Recorder::instance().stop());
    system("rm -rf /data");
    Thread::sleep(1000);
    system("reboot");
    return 0;
  }, PopupMode::Longer);
}

#define KEY_TESTING "TESTING"
void setTesting(bool testing) {
	if (testing) {
	  StoragePreferences::putBool(KEY_TESTING, testing);
	} else {
	  StoragePreferences::remove(KEY_TESTING);
	}
	sync();
}

bool isTesting() {
	return StoragePreferences::getBool(KEY_TESTING, false);
}
