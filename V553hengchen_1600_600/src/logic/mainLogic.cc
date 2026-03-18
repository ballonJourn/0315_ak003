#pragma once
#include "uart/ProtocolSender.h"
#include "system/setting.h"
#include "net/context.h"
#include "link/context.h"
#include "bt/context.h"
#include "media/audio_context.h"
#include "media/media_context.h"
#include "system/reverse.h"
#include "system/fm_emit.h"
#include "system/hardware.h"
#include "misc/storage.h"
#include "fy/format.hpp"
#include "fy/time.hpp"
#include "fy/thread.hpp"
#include "fy/os.hpp"
#include "manager/LanguageManager.h"
#include "manager/ConfigManager.h"
#include "sysapp_context.h"
#include "mode_observer.h"
#include <mpi/case/camera.h>
#include <base/base.hpp>
#include <base/files.hpp>
#include <base/mount_notification.h>
#include <base/ui_handler.h>
#include "edge/popup_service.h"
#include "edge/fat32/filesystem.h"
#include <mpi/case/shared_video_device.h>
#include <mpi/case/config.h>
#include <base/time.hpp>
#include <base/timer.hpp>
#include "misc/utility.h"
#include "misc/app_server_impl.h"
#include "mpi/case/audio_source.h"
#include "misc/rear_view_detection.h"
#include "utils/BitmapHelper.h"
#include "system/usb_monitor.h"
#include "uart/context.h"
#include "utils/TimeHelper.h"
#include "manager/LanguageManager.h"
using namespace sys::setting;

#define LYLINK_VIEW               "lylinkview"
#define LINK_CONNECT_TIP_TIMER    1

#define KEY_LONG_PRESS_TIMEOUT    3000

#define APP_USE_SLIDEWINDOW 0
#define APP_CP_INDEX        0
#define APP_AA_INDEX        1
#define BT_MUSIC_ID3		5

static bt_cb_t _s_bt_cb;

static int view_type = VIEW_TYPE_FRONT;
bool is_first_show = true;
extern void updateSdCardIcon(bool isMounted);

namespace {

enum {
  TIMER_RECORDING = 300,
  TIMER_CHECK_TFCARD,
  TIMER_START_CAMERA,
  TIMER_POWERKEY_EVENT,
  TIMER_CHECK_AUTH,
  TIMER_SCREENSAVER_START,
};

#define LTOV(s) LANGUAGEMANAGER->getValue(s)

// 录频时间
void updateRecordTime() {
  typedef mpi::Recorder::State State;
  switch (mpi::Recorder::instance().state()) {
    case State::UNSTARTED:
    	mDvrTimeWindowPtr->hideWnd();
      break;
    case State::RECORDING: {
      auto sec = mpi::Recorder::instance().elapseTime() / 1000;
      mdvrTimeTextViewPtr->setText(
          base::format("%02lld:%02lld:%02lld", sec / 60 / 60, sec / 60 % 60,
              sec % 60));
      mDvrTimeWindowPtr->showWnd();
      mdvrTimeTextViewPtr->setVisible(true);
    }
      break;
    case State::EXCEPTION:
      mdvrTimeTextViewPtr->setText("--:--:--");
      mDvrTimeWindowPtr->showWnd();
      break;
    default:
      break;
  }
}

static bt_cb_t _s_bt_cb;
static bool _s_need_reopen_linkview;

static void _register_timer_fun(int id, int time) {
	mActivityPtr->registerUserTimer(id, time); // @suppress("无效参数")
}

static void _unregister_timer_fun(int id) {
	mActivityPtr->unregisterUserTimer(id); // @suppress("无效参数")
}

static void _update_time_display() {
    const char* week[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
    char buf[128] = {0};
    char time[128] = {0};

    struct tm *t = TimeHelper::getDateTime();
    int hour = t->tm_hour;

    // 处理12小时制和24小时制
    if (!sys::setting::is_time_format_24h()) {
        // 12小时制
        std::string ampmStr = (hour >= 12) ? "PM" : "AM";
        hour = (hour % 12 == 0) ? 12 : (hour % 12);  // 修正：0点显示为12，12点也显示为12
        sprintf(time, "%02d:%02d %s", hour, t->tm_min, ampmStr.c_str());
    } else {
        // 24小时制
        sprintf(time, "%02d:%02d", hour, t->tm_min);
    }

    mtimeTextViewPtr->setText(time);
    sprintf(buf, "%04d/%02d/%02d", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
    mdateTextViewPtr->setText(buf);
    mweekTextViewPtr->setText(LTOV(week[t->tm_wday % 7]));
}


static void _enter_lylinkview() {
	if (!sys::reverse_does_enter_status()) {
		OPEN_APP(LYLINK_VIEW);
		_s_need_reopen_linkview = false;
	} else {
		LOGD("[main] is reverse status !!!\n");
		lk::video_stop();
		_s_need_reopen_linkview = true;
	}
}

static void _lylink_callback(LYLINKAPI_EVENT evt, int para0, void *para1) {
	switch (evt) {
	case LYLINK_LINK_ESTABLISH:
		LOGD("[main] LYLINK_LINK_ESTABLISH %s", lk::_link_type_to_str((LYLINK_TYPE_E) para0));
		EASYUICONTEXT->hideStatusBar();		//隐藏蓝牙来电界面
		if ((LINK_TYPE_AIRPLAY == para0) || (LINK_TYPE_MIRACAST == para0)) {
			_enter_lylinkview();
		}
		mActivityPtr->registerUserTimer(LINK_CONNECT_TIP_TIMER, 0); // @suppress("无效参数")
		base::runInUiThreadUniqueDelayed("restart record", []() {
			if (sys::setting::get_resolution_mode() == E_RESOLUTION_MODE_2_5K_720P) {
				restartRecorder(true);
			}
		}, 1000);
		break;
	case LYLINK_LINK_DISCONN:
		LOGD("[main] LYLINK_LINK_DISCONN........... %s", lk::_link_type_to_str((LYLINK_TYPE_E) para0));
		bt::query_state();
		CLOSE_APP(LYLINK_VIEW);
		mActivityPtr->registerUserTimer(LINK_CONNECT_TIP_TIMER, 0); // @suppress("无效参数")
		base::runInUiThreadUniqueDelayed("restart record", []() {
			if (sys::setting::get_reduce_resolution()) {
				restartRecorder(true);
			}
		}, 1000);
		break;
	case LYLINK_PHONE_CONNECT:
		LOGD("[main] LYLINK_PHONE_CONNECT %s", lk::_link_type_to_str((LYLINK_TYPE_E) para0));
		if ((para0 == LINK_TYPE_WIFIAUTO) || (para0 == LINK_TYPE_WIFICP)) {
			LOGD("[main] You should open AP now.");
		}
		break;
	case LYLINK_FOREGROUND:
		LOGD("[main] LYLINK_FOREGROUND");
		_enter_lylinkview();
		break;
	case LYLINK_BACKGROUND:
	case LYLINK_HID_COMMAND: {
		if (evt == LYLINK_BACKGROUND) {
			LOGD("[main] LYLINK_BACKGROUND\n");
		} else {
			LOGD("[main] LYLINK_HID_COMMAND");
		}

		const char *app = EASYUICONTEXT->currentAppName();
		if (app && (strcmp(app, LYLINK_VIEW"Activity") == 0)) {
			EASYUICONTEXT->goHome();
		} else {
			CLOSE_APP(LYLINK_VIEW);
		}

		_s_need_reopen_linkview = false;
		break;
	}
	case LYLINK_VERSION_CODE:
		LOGD("[main] errcode: %d, version: %s", (LYLINK_ERRCODE) para0, (const char *) para1);
		if (((LYLINK_ERRCODE) para0) == LINK_ERRCODE_NONE) {
//          showActivationDialog(bluetooth, carplay, timeout)
		}
		break;
	case LYLINK_PHONE_DISCONN: // 蓝牙断开,不用处理
		LOGD("[main] LYLINK_PHONE_DISCONN............. %s", lk::_link_type_to_str((LYLINK_TYPE_E) para0));
		break;
	case LYLINK_CALL_STATE:
		if ((para0 != CallState_Hang) && EASYUICONTEXT->isScreensaverEnable()) {
			EASYUICONTEXT->screensaverOff();
		}
		break;
	default:
		break;
	}
}

static void _bt_call_cb(bt_call_state_e state) {
	if (state != E_BT_CALL_STATE_IDLE) {
		if (lk::get_lylink_type() == LINK_TYPE_WIFIAUTO) {
			const char *app = EASYUICONTEXT->currentAppName();
			if (app) {
				if (strcmp(app, LYLINK_VIEW"Activity") != 0) {
					if (strcmp(app, "reverse2Activity") != 0) {
						OPEN_APP(LYLINK_VIEW);
					} else {
						_s_need_reopen_linkview = true;
					}
				}
			}
		}

		if (EASYUICONTEXT->isScreensaverEnable()) {
			EASYUICONTEXT->screensaverOff();
		}
	}
}

static void _bt_add_cb() {
	_s_bt_cb.call_cb = _bt_call_cb;
	bt::add_cb(&_s_bt_cb);
}

static void _bt_remove_cb() {
	bt::remove_cb(&_s_bt_cb);
}

static void _show_link_tips(LYLINK_TYPE_E link_type) {
	switch (link_type) {
	case LINK_TYPE_WIFICP:
	case LINK_TYPE_USBCP:
		mlinkTipsTextViewPtr->setTextTr("Connected CarPlay device");
		break;
	case LINK_TYPE_USBAUTO:
	case LINK_TYPE_WIFIAUTO:
		mlinkTipsTextViewPtr->setTextTr("Connected Android Auto device");
		break;
	case LINK_TYPE_AIRPLAY:
		mlinkTipsTextViewPtr->setTextTr("Connected AirPlay device");
		break;
	case LINK_TYPE_MIRACAST:
		mlinkTipsTextViewPtr->setTextTr("Connected Miracast device");
		break;
	case LINK_TYPE_WIFILY:
		mlinkTipsTextViewPtr->setTextTr("Connected Aicast device");
		break;
	default:
		LOGD("--%d-- --%s-- type = %d   连接类型错误! \n", __LINE__, __FILE__, link_type);
		break;
	}
	mlinkTipsWindowPtr->showWnd();
}

static void _switch_app_window() {
//	if (mWindow1Ptr->isWndShow()) {
//		mWindow1Ptr->hideWnd();
//		mWindow2Ptr->showWnd();
//		mStatusRadioGroupPtr->setCheckedID(ID_MAIN_RadioButton1);
//	} else {
//		mWindow2Ptr->hideWnd();
//		mWindow1Ptr->showWnd();
//		mStatusRadioGroupPtr->setCheckedID(ID_MAIN_RadioButton0);
//	}
}

static void _update_sound_mode_icon(sound_mode_e mode) {
	const char *normal_icon[] = {
		"new_status/icon_volume_n.png",
		"newFm/icon_carpaly_n.png",
		"newFm/icon_fm_n.png",
		"newFm/icon_aux_n.png"
	};
	const char *pressed_icon[] = {
		"new_status/icon_volume_p.png",
		"newFm/icon_carpaly_p.png",
		"newFm/icon_fm_p.png",
		"newFm/icon_aux_p.png"
	};

	mHornButtonPtr->setButtonStatusPic(ZK_CONTROL_STATUS_NORMAL, normal_icon[mode]);
	mHornButtonPtr->setButtonStatusPic(ZK_CONTROL_STATUS_PRESSED, pressed_icon[mode]);
}

static void _sound_mode_cb(sound_mode_e mode) {
	_update_sound_mode_icon(mode);
}

static void _enter_link_app(link_mode_e mode) {
	LYLINK_TYPE_E link_type = lk::get_lylink_type();
	switch (mode) {
	case E_LINK_MODE_CARPLAY:
	    if ((link_type == LINK_TYPE_WIFICP) || (link_type == LINK_TYPE_USBCP)) {
	    	OPEN_APP(LYLINK_VIEW);
	    	return;
	    }
		break;

	case E_LINK_MODE_ANDROIDAUTO:
	    if ((link_type == LINK_TYPE_WIFIAUTO) || (link_type == LINK_TYPE_USBAUTO)) {
	    	OPEN_APP(LYLINK_VIEW);
			return;
	    }
		break;

	case E_LINK_MODE_AIRPLAY:
	    if (link_type == LINK_TYPE_AIRPLAY) {
	    	OPEN_APP(LYLINK_VIEW);
			return;
	    }
		break;

	case E_LINK_MODE_MIRACAST:
	    if (link_type == LINK_TYPE_MIRACAST) {
	    	OPEN_APP(LYLINK_VIEW);
			return;
	    }
		break;

	case E_LINK_MODE_BLUEMUSIC:
		break;

	case E_LINK_MODE_WIFIVIDEO:
		break;

	case E_LINK_MODE_AICAST:
		if (link_type == LINK_TYPE_WIFILY) {
			OPEN_APP(LYLINK_VIEW);
			return;
		}
		break;
	}

	if (lk::is_connected()) {
		_show_link_tips(link_type);
		return;
	}

	Intent *i = new Intent;
	i->putExtra("link_mode", fy::format("%d", mode));
	EASYUICONTEXT->openActivity("LinkActivity", i);
}

static void _event_mode_cb(event_mode_e mode) {
	if (E_EVENT_MODE_HOME == mode) {
		if (mActivityPtr->isShow()) {
#if !APP_USE_SLIDEWINDOW
			_switch_app_window();
#endif
		}
	}
}

static void _reverse_status_cb(int status) {
	LOGD("[main] reverse status %d\n", status);
	if (isTesting()) {
	  return;
	}
	base::runInUiThreadUniqueDelayed("rear_view_detection", [](){
			int status = sys::reverse_does_enter_status();
			if (status == REVERSE_STATUS_ENTER) {   // 进入倒车
			EASYUICONTEXT->closeActivity("DvrPlayActivity");
			OPEN_APP("reverse2");
			} else {    							// 退出倒车
				CLOSE_APP("reverse2");

				if (_s_need_reopen_linkview) {
					_s_need_reopen_linkview = false;
					if (lk::is_connected()) {
						OPEN_APP(LYLINK_VIEW);
					}
				}
			}
		}, 500);
}

// 耳机线接入拔出回调
static void _aux_det_cb(bool insert) {
	if (insert) {
		if (mode::get_sound_mode() == E_SOUND_MODE_LINK) {
			lk::restart_lylink();
		}
		mode::set_sound_mode(E_SOUND_MODE_AUX);
	}
}

static void* _preload_resources(void *arg) {
	const char *pic_tab[] = {
//		"/res/font/sans.ttf",
		"newsetting/icon_display_n.png",
		"newsetting/icon_record_n.png",
		"newsetting/icon_sound_n.png",
		"newsetting/icon_assit_n.png",
		"newsetting/icon_mirror_n.png",
		"newsetting/icon_protect_n.png",
		"newsetting/icon_seat_n.png",
		"newsetting/icon_reverse_n.png",
		"newsetting/icon_language_n.png",
		"newsetting/icon_time_n.png",
		"newsetting/icon_clean_n.png",
		"newsetting/icon_key_n.png",
		"newsetting/icon_delete_n.png",
		"newsetting/icon_restore_n.png",
		"newsetting/icon_version_n.png",
		"newsetting/pull_bg.png",

		"new_linkhelp/bg.png",
		"new_linkhelp/nav_bg.png",
		"new_linkhelp/bg.png",
		"new_linkhelp/airpaly_n.png",
		"new_linkhelp/andoid_n.png",
		"new_linkhelp/carpaly_n.png",
		"new_linkhelp/miracast_n.png",
		"new_linkhelp/icon_airplay_n.png",
		"new_linkhelp/icon_android_n.png",
		"new_linkhelp/icon_bt_n.png",
		"new_linkhelp/icon_carpaly_n.png",
		"new_linkhelp/icon_miracast_n.png",
		"new_linkhelp/icon_aicast_n.png",

		"newFm/icon_aux_n.png",
		"newFm/icon_carpaly_n.png",
		"newFm/icon_fm_n.png",
		"newFm/icon_sound_n.png",
		"newFm/radio_aux_bg.png",
		"newFm/radio_carpaly_bg.png",
		"newFm/radio_fm_bg.png"
		"newFm/radio_bg.png"
	};

	LOGD("[main] preload resources start\n");

	size_t size = TAB_SIZE(pic_tab);
	for (size_t i = 0; i < size; ++i) {
		struct _bitmap_t *bmp = NULL;
		BitmapHelper::loadBitmapFromFile(bmp, CONFIGMANAGER->getResFilePath(pic_tab[i]).c_str());
	}

	LOGD("[main] preload resources end\n");

	return NULL;
}

class AppSlidePageChangeListener : public ZKSlideWindow::ISlidePageChangeListener {
protected:
	virtual void onSlidePageChange(ZKSlideWindow *pSlideWindow, int page) {
		mStatusRadioGroupPtr->setCheckedID((page == 0) ? ID_MAIN_RadioButton0 : ID_MAIN_RadioButton1);
	}
};

void showFormatDialog() {
  std::vector<PopupButton> buttons;
  buttons.push_back({ "@confirm", []() {
    base::runInUiThread("format", []() {
          formatTfcardProcess(true);
        });
  } });
  buttons.push_back({ "@cancel" });
  PopupService::dialog("@Tips", "@invalid_tfcard", buttons, 0);
}

class KeyListener : public EasyUIContext::IKeyListener {
public:
	KeyListener() : mHasDown(false), mDownTime(0) {

	}

protected:
	virtual bool onKeyEvent(const KeyEvent &ke) {
	  if (isTesting()) {
	    return true;
	  }
		switch (ke.mKeyStatus) {
		case KeyEvent::E_KEY_DOWN:
			mHasDown = true;
			mDownTime = (uint32_t) fy::time::uptime();
			mActivityPtr->unregisterUserTimer(TIMER_POWERKEY_EVENT);
			break;
		case KeyEvent::E_KEY_LONG_PRESS:
			if (mHasDown) {
				// 长按关机
				if ((uint32_t) fy::time::uptime() - mDownTime >= KEY_LONG_PRESS_TIMEOUT) {
					if (sys::reverse_does_enter_status()) {
						LOGD("[main] is reverse status, don't proc key long press\n");
						break;
					}

					if(bt::is_on()){
						bt::power_off();
						sys::setting::set_bt_state(true);
					}
					mpi::FrontCamera::instance().stop();
					mpi::RearCamera::instance().stop();
					NO_EXCEPTION(mpi::Recorder::instance().stop());
					OPEN_APP("shutdown");
					mHasDown = false;
				}
			}
			break;
		case KeyEvent::E_KEY_UP:
			if (mHasDown) {
				if (sys::reverse_does_enter_status()) {
					LOGD("[main] is reverse status, don't proc screensaver\n");
					break;
				}

				mActivityPtr->registerUserTimer(TIMER_POWERKEY_EVENT, 100);
			}
			break;
		default:
			break;
		}
		return true;
	}

private:
	bool mHasDown;
	uint32_t mDownTime;
};

void onExtsdMounted(int status,
    const std::string& mount_point) {
  switch (status) {
    case MountMonitor::E_MOUNT_STATUS_MOUNTED:
    	   updateSdCardIcon(true);//sd图标
      if (ENABLE_AUTO_RECORD && !isTesting()) {
    	  try {
    		  if (base::fat32::getBlockSize(mount_point) != 65536) {
    			  showFormatDialog();
    			  break;
    		  }
    		  NO_EXCEPTION(
    				  mpi::Recorder::instance().start(sys::setting::makeRecorderParam()))
    	  } catch (const base::Exception& e) {
    		  LOGE_TRACE("e.what() = %s", e.what());
    		  break;
    	  } catch (...) {
    		  LOGE_TRACE("unknown error !!!");
    		  break;
    	  }
      }
      /*if (base::exists(
          base::Path::get(mount_point, "isp_param_config.bin").c_str())) {
        LOGE("isp_param_config.bin detect");
        PopupService::dialog("@Tips", "检测到ISP参数文件", 0);
        mpi::FrontVideoSource::loadIspConfigBin(mount_point);
        EASYUICONTEXT->goHome();
      }*/
      if (base::exists(
          base::Path::get(mount_point, PRODUCT_TEST_FILE_NAME).c_str())) {
        EASYUICONTEXT->openActivity("TestActivity");
      }
      mpi::AppServer::instance().notifySDCardChanged();
      break;
    case MountMonitor::E_MOUNT_STATUS_REMOVE:
    	   updateSdCardIcon(false);
      NO_EXCEPTION(mpi::Recorder::instance().stop());
      PopupService::dialog("@Tips", "@tfcard_not_found", 2000);
      mpi::AppServer::instance().notifySDCardChanged();
      base::fat32::umount(TFCARD_MOUNT_POINT); // 防止下次挂载异常
      break;
    case MountMonitor::E_MOUNT_STATUS_CHECKING:
//    	   updateSdCardIcon(false);
      base::runInUiThreadUniqueDelayed("checking_tfcard", [mount_point](){
        if(MOUNTMONITOR->isMounted(mount_point.c_str())) {
          return;
        }
        if (!base::exists(TFCARD_BLOCK)) {
          return;
        }
        showFormatDialog();
      }, 3000);
      break;
    default:
    	   updateSdCardIcon(false);
    	   break;
  }
}

void onReverseCameraChanged(ReverseCameraState state) {
  LOGD("rear camera state %d", state);
  if (isTesting()) {
    return;
  }
  NO_EXCEPTION(restartRecorder(true));
  if (state == REVERSE_CAMERA_STATE_UNDETECTED) {
    EASYUICONTEXT->closeActivity("DvrActivity");
    PopupService::dialog("@Tips", "@rear_camera_detach", 2000);
    if (strcmp(EASYUICONTEXT->currentAppName(), "mainActivity") == 0) {
		setupCamera(VIEW_TYPE_FRONT_MINI, false);
	}
  }
  if (sys::has_reverse_camera()) {
    mpi::AppServer::instance().notifyCameraChanged(true);
  } else {
    auto& srv = mpi::RealTimeStreamingProtocolServer::instance();
    if (srv.isStarted() && srv.view() == mpi::VIEW_REAR) {
      srv.stop();
      mpi::RealTimeStreamingProtocolServerParams params;
      params.video_device = mpi::VIDEO_DEVICE_FRONT;
      params.mirror = false;
      srv.start(params);
    }
    mpi::AppServer::instance().notifyCameraChanged(false);

  }
}

void readFrontVideo(int milliseconds) {
  try {
	PRINT_SCOPE_EXEC_TIME(__FUNCTION__);
	const auto deadline = base::uptime() + milliseconds;
	mpi::SharedVideoDevice daemon(mpi::VIDEO_DEVICE_FRONT);
//    mpi::FrontVideoSource daemon(VIDEO_IN_CHANNEL_CAMERA);
//    daemon.channel().enable();

    while (base::uptime() <= deadline) {
      daemon.getFrame(3000);
    }
  } catch (mpi::Exception& e) {
    LOGE_TRACE("%s", e.what());
  }
}

void resetFrontVideo() {
  //开机多开几次摄像头
  readFrontVideo(200);
  Thread::sleep(100);
  readFrontVideo(200);
}

} /* anonymous namespace */

static AppSlidePageChangeListener _s_app_slide_page_change_listener;
static KeyListener _s_key_listener;

/**
 * 注册定时器
 * 填充数组用于注册定时器
 * 注意：id不能重复
 */
static S_ACTIVITY_TIMEER REGISTER_ACTIVITY_TIMER_TAB[] = {
    {TIMER_RECORDING, 1000},
    {TIMER_CHECK_TFCARD, 3000},
    {TIMER_CHECK_AUTH, 10* 1000},
};

/**
 * 当界面构造时触发
 */
static void onUI_init() {
	LOGE("[main] onUI_init");
  // 预加载图片资源，加快界面打开速度
  fy::run_thread(_preload_resources);

  // 硬件模块初始化
  sys::hw::add_aux_det_cb(_aux_det_cb);
  sys::hw::init();

  // 串口初始化
  uart::init();
  // 蓝牙初始化
  if (base::exists("/mnt/extsd/pengzc_debug_disable_bluetooth")) {
    LOGE("调试禁用蓝牙");
  } else {
    bt::init();
    _bt_add_cb();
    mActivityPtr->registerUserTimer(BT_MUSIC_ID3, 0);
  }

  // 媒体初始化
  media::init();

  // 启动手机互联
  lk::add_lylink_callback(_lylink_callback);

  // 通过线程启动网络初始化，减少开机时间
  {
    std::thread net_init_thread([]() {
		net::init();
		lk::start_lylink();
		// 放在线程中打开，缩短开机时间
//		resetFrontVideo(); // 规避摄像头异常
    });
    net_init_thread.detach();
    // TODO
  }

  // 网络初始化
//  net::init();


//  lk::start_lylink();

  // 启动倒车检测
  sys::reverse_add_status_cb(_reverse_status_cb);
  sys::reverse_detect_start();

  _aux_det_cb(sys::hw::aux_det_is_insert());
//  sys::setting::set_sound_mode(E_SOUND_MODE_SPK);
  mode::add_event_mode_cb(_event_mode_cb);
  mode::add_sound_mode_cb(_sound_mode_cb);
  mode::set_sound_mode(sys::setting::get_sound_mode());
  app::attach_timer(_register_timer_fun, _unregister_timer_fun);
  //app::show_floatwnd();
  EASYUICONTEXT->hideStatusBar();

//  mappSlideWindowPtr->setSlidePageChangeListener(
//      &_s_app_slide_page_change_listener);
  mStatusRadioGroupPtr->setCheckedID(ID_MAIN_RadioButton0);

  const int minutes = sys::setting::get_screensaver_time();
  EASYUICONTEXT->setScreensaverEnable(minutes > 0);
  if (minutes > 0) {
    EASYUICONTEXT->setScreensaverTimeOut(minutes * 60);
  }

  //system("mount -t debugfs none /sys/kernel/debug");
  EASYUICONTEXT->registerKeyListener(&_s_key_listener);

  _update_sound_mode_icon(mode::get_sound_mode());

//  mWindow1Ptr->setVisible(true);
//  mAppWindow1Ptr->setVisible(true);
//  sys::setting::enable_usb_adb(true);

  base::UiHandler::implementTimerRegistration([]() {
    mActivityPtr->registerUserTimer(base::TIMER_UI_HANDLER, 0); // @suppress("无效参数")
  });
  resetFrontVideo(); // 规避摄像头异常
//  setupCamera(VIEW_TYPE_UNVISIBLE, false);

  RearViewDetection::instance().start();
    // auto state_listener = [](mpi::View view, bool is_streaming) {
    //   if (view == mpi::VIEW_REAR) {
    //     if (is_streaming) {
    //       RearViewDetection::instance().stop();
    //     } else {
    //       RearViewDetection::instance().start();
    //     }
    //   }
    // };
    // mpi::RealTimeStreamingProtocolServer::instance().setStateListener(
    //     state_listener);

  NO_EXCEPTION(setupCamera(VIEW_TYPE_UNVISIBLE, false));
  if (isTesting()) {
    resetAllProcess();
    return;
  }

  base::runInUiThreadDelayed("delayInit", [](){
    static base::MountNotification mount_extsd(TFCARD_MOUNT_POINT,
        onExtsdMounted,
        MountMonitor::E_MOUNT_STATUS_MOUNTED,
        MountMonitor::E_MOUNT_STATUS_REMOVE,
        MountMonitor::E_MOUNT_STATUS_CHECKING);
    sys::reverse_add_camera_state_cb(onReverseCameraChanged);
  }, 3000);
#if 0
  async([](){
    sleep(2);
    showAuthDialog(false, false, 10);
  });
#endif

//  mButton2Ptr->setSelected(sys::setting::is_light_state());
//  sys::hw::set_light_auto_switch(sys::setting::is_light_state());

  bt::query_state();

  if(sys::setting::get_bt_state()){
	  bt::power_on();
	  sys::setting::set_bt_state(false);
  }
}

/**
 * 当切换到该界面时触发
 */
static void onUI_intent(const Intent *intentPtr) {
}

/*
 * 当界面显示时触发
 */
static void onUI_show() {
	LOGE("[main] onUI_show");
  mode::set_switch_mode(E_SWITCH_MODE_SETUP);
//  setupCamera(VIEW_TYPE_REAR, true);
  mActivityPtr->registerUserTimer(TIMER_START_CAMERA, 0); // @suppress("无效参数")
#if 0
  {
	  mpi::CameraParam param;
	  param.mirror = false;
	  param.viewbox = {0, sys::setting::get_dvr_viewbox_rear_y(), 1920, 768};
	  param.display = {76, 158, 300, 196};
	  view_type = sys::has_reverse_camera() ?
			  (ViewType)sys::setting::get_dvr_view_type() : VIEW_TYPE_FRONT;
	  if (view_type != VIEW_TYPE_REAR) {
		  mpi::FrontCamera::instance().setParam(param, true);
	  	  mpi::RearCamera::instance().setVisible(false, false);
	  } else {
		  param.mirror = sys::setting::is_rear_mirror();
		  mpi::RearCamera::instance().setParam(param, true);
		  mpi::FrontCamera::instance().setVisible(false, false);
	  }
  }
#endif
  if (ENABLE_AUTO_RECORD) {
    if (!is_first_show) {
      NO_EXCEPTION(restartRecorder(false));
    }
  }

  is_first_show = false;
#if 0
  {
    LOGE("调试打开测试界面");
    EASYUICONTEXT->openActivity("TestActivity");
  }
#endif
  mPlaybackButtonPtr->setLongMode(ZKTextView::E_LONG_MODE_SCROLL_CIRCULAR);
  _update_time_display();
}

/*
 * 当界面隐藏时触发
 */
static void onUI_hide() {
  LOGD_TRACE("%s", __FUNCTION__);
  mActivityPtr->unregisterUserTimer(TIMER_START_CAMERA);
}

static bool onUI_back() {
	if (PopupService::isBusy()) return false;
//	if (mWindow2Ptr->isWndShow()) {
//		mWindow2Ptr->hideWnd();
//		mWindow1Ptr->showWnd();
//		mStatusRadioGroupPtr->setCheckedID(ID_MAIN_RadioButton0);
//	} else {
//		mWindow1Ptr->hideWnd();
//		mWindow2Ptr->showWnd();
//		mStatusRadioGroupPtr->setCheckedID(ID_MAIN_RadioButton1);
//	}
	return false;

}

/*
 * 当界面完全退出时触发
 */
static void onUI_quit() {
  EASYUICONTEXT->unregisterKeyListener(&_s_key_listener);
  sys::reverse_remove_status_cb(_reverse_status_cb);
  lk::remove_lylink_callback(_lylink_callback);
  mode::remove_event_mode_cb(_event_mode_cb);
  sys::hw::remove_aux_det_cb(_aux_det_cb);
  sys::reverse_remove_camera_state_cb(onReverseCameraChanged);
  mode::remove_sound_mode_cb(_sound_mode_cb);
  _bt_remove_cb();
//  bt::remove_cb(&_s_bt_cb);
}

/**
 * 串口数据回调接口
 */
static void onProtocolDataUpdate(const SProtocolData &data) {

}

/**
 * 定时器触发函数
 * 不建议在此函数中写耗时操作，否则将影响UI刷新
 * 参数： id
 *         当前所触发定时器的id，与注册时的id相同
 * 返回值: true
 *             继续运行当前定时器
 *         false
 *             停止运行当前定时器
 */
static bool onUI_Timer(int id) {
	if (app::on_timer(id)) {
		return false;
	}

	switch (id) {
	case LINK_CONNECT_TIP_TIMER: {
		const char *cp_tip = "CarPlay";
		const char *aa_tip = "Android Auto";

		switch (lk::get_lylink_type()) {
		case LINK_TYPE_WIFICP:
		case LINK_TYPE_USBCP:
			cp_tip = "@connected_carplay_tip";
			break;
		case LINK_TYPE_USBAUTO:
		case LINK_TYPE_WIFIAUTO:
			aa_tip = "@connected_android_auto_tip";
			break;
		default:
			break;
		}

#if APP_USE_SLIDEWINDOW
		mappSlideWindowPtr->setItemText(APP_CP_INDEX, cp_tip);
		mappSlideWindowPtr->setItemText(APP_AA_INDEX, aa_tip);
#else
//		if (cp_tip[0] == '@') {
//			mCPButtonPtr->setTextTr(cp_tip + 1);
//		} else {
//			mCPButtonPtr->setTextTr("");
//			mCPButtonPtr->setText(cp_tip);
//		}
//		if (aa_tip[0] == '@') {
//			mAAButtonPtr->setTextTr(aa_tip + 1);
//		} else {
//			mAAButtonPtr->setTextTr("");
//			mAAButtonPtr->setText(aa_tip);
//		}
#endif
		return false;
	}
	case BT_MUSIC_ID3:
		if(bt::music_is_playing()){
			bt::query_bt_music_info();
		}
		return false;
	case TIMER_RECORDING:
      updateRecordTime();
	  _update_time_display();
	  break;
    case base::TIMER_UI_HANDLER:
      return base::UiHandler::onTimer();
      break;
    case TIMER_CHECK_TFCARD:
      if (!MOUNTMONITOR->isMounted(TFCARD_MOUNT_POINT)) {
        if (base::exists(TFCARD_BLOCK)) {
          showFormatDialog();
        } else {
          PopupService::dialog("@Tips", "@tfcard_not_found", 2000);
        }
        return false;
      }
      break;
    case TIMER_START_CAMERA:
      if (sys::reverse_does_enter_status())
    	return false;
      try {
        auto view = sys::has_reverse_camera() ?
            (ViewType)sys::setting::get_dvr_view_type() : VIEW_TYPE_FRONT;
        setupCamera(VIEW_TYPE_FRONT_MINI, true);
		// 解决dvr回到主界面黑屏的问题
      } catch (mpi::Exception& e) {
        LOGE_TRACE("%s", e.what());
      }
      return false;
      break;
	case TIMER_POWERKEY_EVENT:
		if (EASYUICONTEXT->isScreensaverOn()) {
			EASYUICONTEXT->screensaverOff();
		} else {
			EASYUICONTEXT->screensaverOn();
		}
		return false;
	case TIMER_CHECK_AUTH: {
	  if(!lk::get_is_authorized()) {
	    showAuthDialog(lk::get_is_authorized(), 300);
	  }
	  return false;
	}
	case TIMER_SCREENSAVER_START: {
		EASYUICONTEXT->screensaverOn();
		return false;
	}
	default:
		break;
	}
    return true;
}

/**
 * 有新的触摸事件时触发
 * 参数：ev
 *         新的触摸事件
 * 返回值：true
 *            表示该触摸事件在此被拦截，系统不再将此触摸事件传递到控件上
 *         false
 *            触摸事件将继续传递到控件上
 */
static bool onmainActivityTouchEvent(const MotionEvent &ev) {
	static MotionEvent down_ev;
	static bool allow_switch;

	switch (ev.mActionStatus) {
	case MotionEvent::E_ACTION_DOWN:	//触摸按下
		//LOGD("时刻 = %ld 坐标  x = %d, y = %d", ev.mEventTime, ev.mX, ev.mY);
		if (!(app::is_hit_ctrlbar(ev.mX, ev.mY) || app::is_hit_floatwnd(ev.mX, ev.mY)))	{
			allow_switch = true;
			down_ev = ev;
		}
		break;
	case MotionEvent::E_ACTION_MOVE:			//触摸滑动
		//LOGD("时刻 = %ld 坐标  x = %d, y = %d", ev.mEventTime, ev.mX, ev.mY);
		break;
	case MotionEvent::E_ACTION_UP:  //触摸抬起
//#if !APP_USE_SLIDEWINDOW
//		if (allow_switch && (abs(ev.mX - down_ev.mX) >= SCREEN_WIDTH / 10)) {
//			_switch_app_window();
//		}
//
//		allow_switch = false;
//#endif
		break;
	default:
		break;
	}
    return false;
}

static void onCheckedChanged_StatusRadioGroup(ZKRadioGroup* pRadioGroup, int checkedID) {
    LOGD(" RadioGroup StatusRadioGroup checked %d", checkedID);
}

static bool onButtonClick_CPButton(ZKButton *pButton) {
    LOGD(" ButtonClick CPButton !!!\n");
    _enter_link_app(E_LINK_MODE_CARPLAY);
    return false;
}

static bool onButtonClick_AAButton(ZKButton *pButton) {
    LOGD(" ButtonClick AAButton !!!\n");
    _enter_link_app(E_LINK_MODE_ANDROIDAUTO);
    return false;
}

static bool onButtonClick_PlaybackButton(ZKButton *pButton) {
    LOGD(" ButtonClick PlaybackButton !!!\n");
	if (sys::setting::get_link_mode() == E_LINK_MODE_WIFIVIDEO) {
		if (mpi::AppServer::instance().isEnter()) {
			PopupService::dialog("@Tips", "@Connected Wifi Video", 2000);
			return false;
		}
	}
    OPEN_APP("DvrPlay");
    return false;
}

static bool onButtonClick_AudioOutputButton(ZKButton *pButton) {
    LOGD(" ButtonClick AudioOutputButton !!!\n");
    OPEN_APP("AudioOutput");
    return false;
}

static bool onButtonClick_APButton(ZKButton *pButton) {
    LOGD(" ButtonClick APButton !!!\n");
    _enter_link_app(E_LINK_MODE_AIRPLAY);
    return false;
}

static bool onButtonClick_ACButton(ZKButton *pButton) {
    LOGD(" ButtonClick ACButton !!!\n");
    _enter_link_app(E_LINK_MODE_MIRACAST);
    return false;
}

static bool onButtonClick_BtMusicButton(ZKButton *pButton) {
    LOGD(" ButtonClick BtMusicButton !!!\n");
    _enter_link_app(E_LINK_MODE_BLUEMUSIC);
    return false;
}
static bool onButtonClick_PowerButton(ZKButton *pButton) {
    LOGD(" ButtonClick PowerButton !!!\n");
//    mActivityPtr->registerUserTimer(TIMER_SCREENSAVER_START, 500);
    EASYUICONTEXT->screensaverOn();
    return false;
}

static bool onButtonClick_LinkButton(ZKButton *pButton) {
    LOGD(" ButtonClick LinkButton !!!\n");
    if (lk::is_connected()) {
		OPEN_APP("lylinkview");
	} else {
		if (E_LINK_MODE_BLUEMUSIC == sys::setting::get_link_mode()) {
			audio::change_audio_type(E_AUDIO_TYPE_BT_MUSIC);
			if(bt::music_is_playing()){
				bt::query_bt_music_info();
			}
		}
		OPEN_APP("Link");
	}
    return false;
}

static bool onButtonClick_LightButton(ZKButton *pButton) {
    if (PopupService::isBusy()) {
    	return false;
    }
    LOGD(" ButtonClick LightButton !!!\n");
    if (app::is_show_ctrlbar() && (mode::get_event_mode() == E_EVENT_MODE_BRIGHTNESS)) {
    	app::hide_ctrlbar();
    } else {
    	mode::set_event_mode(E_EVENT_MODE_BRIGHTNESS);
    	app::show_ctrlbar();
    }
    return false;
}

static bool onButtonClick_HornButton(ZKButton *pButton) {
	if (PopupService::isBusy()) {
		return false;
	}
    LOGD(" ButtonClick HornButton !!!\n");
    if (mode::get_sound_mode() == E_SOUND_MODE_SPK) {
    	if (app::is_show_ctrlbar() && (mode::get_event_mode() == E_EVENT_MODE_VOICE)) {
    		app::hide_ctrlbar();
    	} else {
    		mode::set_event_mode(E_EVENT_MODE_VOICE);
    		app::show_ctrlbar();
    	}
    } else {
    	OPEN_APP("AudioOutput");
    }
    return false;
}
static bool onButtonClick_Button1(ZKButton *pButton) {
    LOGD(" ButtonClick Button1 !!!\n");
    OPEN_APP("Dvr");
    return false;
}

static bool onButtonClick_videoCheckButton(ZKButton *pButton) {
  LOGD(" ButtonClick videoCheckButton !!!\n");
  if (!sys::has_reverse_camera()) {
  	  return false;
  }
  view_type = (view_type == VIEW_TYPE_REAR) ? VIEW_TYPE_FRONT : VIEW_TYPE_REAR;
  sys::setting::set_dvr_view_type(view_type);
  ViewType view_type_mini = (view_type == VIEW_TYPE_REAR) ? VIEW_TYPE_REAR_MINI : VIEW_TYPE_FRONT_MINI;
  setupCamera(view_type_mini, true);
#if 0
  {
    mpi::CameraParam param;
    param.mirror = false;
    param.viewbox = {0, sys::setting::get_dvr_viewbox_rear_y(), 1920, 768};
    param.display = {76, 158, 300, 196};
    if (view_type != VIEW_TYPE_REAR) {
      mpi::FrontCamera::instance().setParam(param, true);
      mpi::RearCamera::instance().setVisible(false, false);
    } else {
      param.mirror = sys::setting::is_rear_mirror();
      mpi::RearCamera::instance().setParam(param, true);
      mpi::FrontCamera::instance().setVisible(false, false);
    }
  }
#endif
  return false;
}

static bool onButtonClick_wifiVideoButton(ZKButton *pButton) {
    LOGD(" ButtonClick wifiVideoButton !!!\n");
    _enter_link_app(E_LINK_MODE_WIFIVIDEO);
    return false;
}
//static bool onButtonClick_Button2(ZKButton *pButton) {
//    LOGD(" ButtonClick Button2 !!!\n");
//    if (sys::setting::is_light_state()) {
//    	sys::setting::set_light_state(false);
//    } else {
//    	sys::setting::set_light_state(true);
//    }
//
//    mButton2Ptr->setSelected(sys::setting::is_light_state());
//    sys::hw::set_light_auto_switch(sys::setting::is_light_state());
//    return false;
//}
static bool onButtonClick_SettingButton(ZKButton *pButton) {
    LOGD(" ButtonClick SettingButton !!!\n");
    OPEN_APP("settings");
    return false;
}

static bool onButtonClick_SETButton(ZKButton *pButton) {
    LOGD(" ButtonClick SETButton !!!\n");
    OPEN_APP("settings");
    return false;
}
static bool onButtonClick_MCButton(ZKButton *pButton) {
    LOGD(" ButtonClick MCButton !!!\n");
    _enter_link_app(E_LINK_MODE_AICAST);
    return false;
}

static void onVideoViewPlayerMessageListener_VideoView1(ZKVideoView *pVideoView, int msg) {
	switch (msg) {
	case ZKVideoView::E_MSGTYPE_VIDEO_PLAY_STARTED:
		break;
	case ZKVideoView::E_MSGTYPE_VIDEO_PLAY_COMPLETED:
		break;
	case ZKVideoView::E_MSGTYPE_VIDEO_PLAY_ERROR:
		break;
	}
}

static bool onButtonClick_maincarplayButton(ZKButton *pButton) {
    LOGD(" ButtonClick maincarplayButton !!!\n");
    _enter_link_app(E_LINK_MODE_CARPLAY);
    return false;
}

static bool onButtonClick_mainandroidautoButton(ZKButton *pButton) {
    LOGD(" ButtonClick mainandroidautoButton !!!\n");
    _enter_link_app(E_LINK_MODE_ANDROIDAUTO);
    return false;
}

static bool onButtonClick_dockLinkButton(ZKButton *pButton) {
    LOGD(" ButtonClick LinkButton !!!\n");
    if (lk::is_connected()) {
		OPEN_APP("lylinkview");
	} else {
		OPEN_APP("Link");
	}
    return false;
}

static bool onButtonClick_dockVolumeoutputButton(ZKButton *pButton) {
    LOGD(" ButtonClick dockVolumeoutputButton !!!\n");
    OPEN_APP("AudioOutput");
    return false;
}

static bool onButtonClick_dockSettingButton(ZKButton *pButton) {
    LOGD(" ButtonClick SettingButton !!!\n");
    OPEN_APP("settings");
    return false;
}

static bool onButtonClick_dockPowerButton(ZKButton *pButton) {
    LOGD(" ButtonClick dockPowerButton !!!\n");
    EASYUICONTEXT->screensaverOn();
    return false;
}

static bool onButtonClick_dockBtButton(ZKButton *pButton) {
    LOGD(" ButtonClick dockBtButton !!!\n");
    _enter_link_app(E_LINK_MODE_BLUEMUSIC);
    return false;
}

static bool onButtonClick_appsButton(ZKButton *pButton) {
    LOGD(" ButtonClick appsButton !!!\n");
    OPEN_APP("main2");
    return false;
}

static bool onButtonClick_dockVideoButton(ZKButton *pButton) {
    LOGD(" ButtonClick dockVideoButton !!!\n");
	if (sys::setting::get_link_mode() == E_LINK_MODE_WIFIVIDEO) {
		if (mpi::AppServer::instance().isEnter()) {
			PopupService::dialog("@Tips", "@Connected Wifi Video", 2000);
			return false;
		}
	}
    OPEN_APP("DvrPlay");
    return false;
}
static bool onButtonClick_BluetoothappButton(ZKButton *pButton) {
    LOGD(" ButtonClick BluetoothappButton !!!\n");
    _enter_link_app(E_LINK_MODE_BLUEMUSIC);
    return false;
}
