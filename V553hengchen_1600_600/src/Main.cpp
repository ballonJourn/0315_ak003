#include "entry/EasyUIContext.h"
#include "uart/UartContext.h"
#include "manager/ConfigManager.h"
#include "system/setting.h"
#include <mpi/module/sys.h>
#include <mpi/case/config.h>
#include "common.h"
#include <base/time.hpp>
#include <mpi/case/shared_video_device.h>
#include <mpi/case/config.h>
#include <utils/TimeHelper.h>

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

static bool _is_shutdown_state() {
	int val = 0;
	SystemProperties::getInt(SHUTDOWN_PROP_KEY, &val, 0);
	return val == 1;
}

void onEasyUIInit(EasyUIContext *pContext) {
  if (_is_shutdown_state()) {
    return;
  }

  //base::setBeijingTimeZone();

  // 设置alsa配置环境
  setenv("ALSA_CONFIG_DIR", "/res/ui/alsa", 1);
  // 升级不重启应用^M
  setenv("ZK_UPGRADE_RESTART", "0", 1);

  // 系统设置初始化
  sys::setting::init();

  if (sys::setting::isNewDevice()) {
      //默认2024年
	  if ((TimeHelper::getDateTime()->tm_year + 1900) < 2024) {
		TimeHelper::setDateTime("2024-01-01 09:00:00");
	  }
      sys::setting::setNewDevice(false);
    }

	mpi::initializeSystem();
	mpi::VideoDeviceRegistry::instance().add(mpi::VIDEO_DEVICE_FRONT)
		.setIsp(true)
	    .setFrameRate(25)
		.setPictureSize({1920, 1080})
		.setSkipStartFrames(14)
	    .setId(0);
  mpi::VideoDeviceRegistry::instance().add(mpi::VIDEO_DEVICE_FRONT_RECORD)
		.setIsp(true)
	    .setFrameRate(25)
		.setPictureSize({1920, 1080})
	    .setId(4);
	mpi::VideoDeviceRegistry::instance().add(mpi::VIDEO_DEVICE_REAR)
	    .setFrameRate(30)
		.setPictureSize(sys::setting::get_rear_video_resolution())
//		.setPictureSize({1280, 720})
	    .setId(8);
	mpi::config()
		.apply();

  static auto default_terminator = std::get_terminate();
  std::set_terminate([]() {
    LOGE("***************************************");
    LOGE("*                                     *");
    LOGE("*  unhandled c++ exception, abort!    *");
    LOGE("*                                     *");
    LOGE("***************************************");
    default_terminator();
  });
//  UartContext::init();
}

void onEasyUIDeinit(EasyUIContext *pContext) {
//	UartContext::destroy();
}

const char* onStartupApp(EasyUIContext *pContext) {
	return _is_shutdown_state() ? "shutdownActivity" : "mainActivity";
}

#ifdef __cplusplus
}
#endif  /* __cplusplus */
