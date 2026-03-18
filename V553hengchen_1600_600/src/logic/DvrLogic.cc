#pragma once
#include "uart/ProtocolSender.h"

#include "mode_observer.h"
#include <mpi/case/recorder.h>
#include <base/base.hpp>
#include <base/time.hpp>
#include <base/strings.hpp>
#include "system/setting.h"
#include "edge/popup_service.h"
#include <base/ui_handler.h>
#include <base/timer.hpp>
#include "config.h"
#include "system/reverse.h"
#include <os/MountMonitor.h>
#include "edge/fat32/filesystem.h"
#include "misc/utility.h"
#include "misc/keytone.h"
#include <manager/ConfigManager.h>
#include "misc/app_server_impl.h"
#include <mpi/case/video_frame_interceptor.h>
#include <mpi/case/snapshot.h>
#include <base/lib.h>
#include "media/audio_context.h"
#include "bt/context.h"

namespace {

enum {
  TIMER_STATE = 100,
  TIMER_HIDE_BOTTOM_MENU,
};

const char* MSG_STORE_DVR_VIEWBOX = "setDvrViewbox";

mpi::Rectangle front_viewbox = {0, 0, 1920, 768};
mpi::Rectangle rear_viewbox = {0, 0, 1920, 768};
base::timer menu_timer;
std::shared_ptr<base::UiHandler> ui_handler;
ViewType view_type = VIEW_TYPE_FRONT;
base::timer take_picture_timer;

void updateRecordTime() {
  typedef mpi::Recorder::State State;
  switch (mpi::Recorder::instance().state()) {
    case State::UNSTARTED:
      mWindowTimePtr->setVisible(false);
      if (!mpi::Recorder::instance().isStarted()) {
        mButtonRecordPtr->setSelected(false);
      }
      break;
    case State::RECORDING: {
      auto sec = mpi::Recorder::instance().elapseTime() / 1000;
      mTextViewTimePtr->setText(
          base::format("%02lld:%02lld:%02lld", sec / 60 / 60, sec / 60 % 60,
              sec % 60));
      mWindowTimePtr->setVisible(true);
      mButtonRecordPtr->setSelected(true);
      const auto lock = mpi::Recorder::instance().isLocked();
      mTextViewDotPtr->setBackgroundPic(
          lock ? "recording/yellow.png" : "newdvr/icon_record.png");
      mlockBtnPtr->setSelected(lock);
    }
      break;
    case State::EXCEPTION:
      mTextViewTimePtr->setText("--:--:--");
      mWindowTimePtr->setVisible(true);
      break;
    default:
      break;
  }
}

void storeViewbox() {
  sys::setting::set_dvr_viewbox_front_y(front_viewbox.y);
  sys::setting::set_dvr_viewbox_rear_y(rear_viewbox.y);
}

void nextViewType() {
  view_type = ViewType(((int)view_type + 1) % (int)VIEW_TYPE_UNVISIBLE);
  sys::setting::set_dvr_view_type((int)view_type);
  NO_EXCEPTION(setupCamera(view_type, true));
}

void prevViewType() {
  view_type = ViewType(
      ((int) view_type + (int) VIEW_TYPE_UNVISIBLE - 1) % (int) VIEW_TYPE_UNVISIBLE);
  sys::setting::set_dvr_view_type((int)view_type);
  NO_EXCEPTION(setupCamera(view_type, true));
}

bool handleMotionEvent(const MotionEvent &ev) {
  if (!mWindowMotionPtr->getPosition().isHit(ev.mX, ev.mY)) {
    return false;
  }
  static MotionEvent down;
  static MotionEvent last;
  switch (ev.mActionStatus) {
    case MotionEvent::E_ACTION_DOWN:
      last = ev;
      down = ev;
      break;
    case MotionEvent::E_ACTION_MOVE: {
      if (view_type == VIEW_TYPE_BOTH) {
        break;
      }
      int x = -(ev.mX - last.mX);
      int y = -(ev.mY - last.mY);
      if (view_type == VIEW_TYPE_FRONT) {
        front_viewbox = mpi::FrontCamera::instance().moveViewbox(x, y);
      }
      if (view_type == VIEW_TYPE_REAR) {
        rear_viewbox = mpi::RearCamera::instance().moveViewbox(x, y);
      }
      last = ev;
      ui_handler->postUniqueDelayed(MSG_STORE_DVR_VIEWBOX, storeViewbox, 1000);
    }
      break;
    case MotionEvent::E_ACTION_UP:
      if (mWindowMotionPtr->getPosition().isHit(ev.mX, ev.mY) && (abs(ev.mX - down.mX) <= 20) && \
    		  (abs(ev.mY - down.mY) <= 20)) {
    	  if (!mWindowBottomMenuPtr->isVisible()) {
    		  mWindowBottomMenuPtr->setVisible(true);
    	  } else {
		  mWindowBottomMenuPtr->setVisible(false);
    	  }
    	  break;
      }
      if (!sys::has_reverse_camera()) {
        break;
      }
      // if (ev.mX - down.mX > 20) { //右滑动
      //   prevViewType();
      //   break;
      // }
      // if (ev.mX - down.mX < - 20) { //左滑动
      //   nextViewType();
      //   break;
      // }
      break;
    default:
      break;
  }
  return false;
}

} /* anonymous namespace */

/**
 * 注册定时器
 * 填充数组用于注册定时器
 * 注意：id不能重复
 */
static S_ACTIVITY_TIMEER REGISTER_ACTIVITY_TIMER_TAB[] = {
	{TIMER_STATE,  1000},
	{TIMER_HIDE_BOTTOM_MENU,  1000},
};

/**
 * 当界面构造时触发
 */
static void onUI_init() {
  LOGD_TRACE("");
  ui_handler = std::make_shared<base::UiHandler>(
      simple_class_name(mActivityPtr));
  menu_timer.reset();
  mWindowTimePtr->setBackgroundColor(0x00000000);
  mWindowTimePtr->setVisible(false);

  mWindowMotionPtr->setVisible(true);

  mmuteBtnPtr->setSelected(sys::setting::is_dvr_record_sound_enabled());

  front_viewbox.y = sys::setting::get_dvr_viewbox_front_y();
  rear_viewbox.y = sys::setting::get_dvr_viewbox_rear_y();

  view_type = sys::has_reverse_camera() ?
      (ViewType)sys::setting::get_dvr_view_type() : VIEW_TYPE_FRONT;

  updateRecordTime();
  if (sys::setting::is_dvr_mark_enable()) {
	  mdvrMarkTextViewPtr->setVisible(true);
	  mdvrMarkTextViewPtr->setTouchPass(true);
  }
  mButtonRecordPtr->setSelected(
      mpi::Recorder::instance().state() == mpi::Recorder::RECORDING);
//  if (!sys::setting::is_dvr_flip_enable()) {
//	  mpi::FrontVideoSource::setFlip(false);
//	  mpi::FrontVideoSource::setMirror(false);
//  }
}

/**
 * 当切换到该界面时触发
 */
static void onUI_intent(const Intent *intentPtr) {
	if (intentPtr == NULL) {
		return ;
	}
}

/*
 * 当界面显示时触发
 */
static void onUI_show() {
  LOGD_TRACE("");
  menu_timer.reset();
  mWindowBottomMenuPtr->setVisible(true);
  mode::set_switch_mode(E_SWITCH_MODE_DVR);

  NO_EXCEPTION(setupCamera(view_type, true));

  if (ENABLE_AUTO_RECORD) {
    NO_EXCEPTION(restartRecorder(false));
  }
}

/*
 * 当界面隐藏时触发
 */
static void onUI_hide() {
  ui_handler->erase(MSG_STORE_DVR_VIEWBOX);
}

/*
 * 当界面完全退出时触发
 */
static void onUI_quit() {
  LOGD_TRACE("");
  if (ENABLE_AUTO_RECORD) {
    NO_EXCEPTION(restartRecorder(false));
  }
  ui_handler.reset();
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
  switch (id) {
    case TIMER_STATE:
      updateRecordTime();
      mmuteBtnPtr->setSelected(sys::setting::is_dvr_record_sound_enabled());
      break;
    case TIMER_HIDE_BOTTOM_MENU:
      if (PopupService::isBusy()) {
        menu_timer.reset();
        break;
      }
      if (menu_timer.elapsed() > 5000) {
        mWindowBottomMenuPtr->setVisible(false);
      }
      break;
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
static bool onDvrActivityTouchEvent(const MotionEvent &ev) {
  if (PopupService::isBusy()) {
    return false;
  }
  menu_timer.reset();

  if (handleMotionEvent(ev)) {
    return true;
  }
  switch (ev.mActionStatus) {
    case MotionEvent::E_ACTION_DOWN: //触摸按下
      //LOGD("时刻 = %ld 坐标  x = %d, y = %d", ev.mEventTime, ev.mX, ev.mY);
      break;
    case MotionEvent::E_ACTION_MOVE:			//触摸滑动
      break;
    case MotionEvent::E_ACTION_UP:  //触摸抬起
      break;
    default:
      break;
  }
  return false;
}

static bool onButtonClick_ButtonRecord(ZKButton *pButton) {
  try {
    if (mpi::Recorder::instance().isStarted()) {
      mpi::Recorder::instance().stop();
    } else {
      mpi::Recorder::instance().start(sys::setting::makeRecorderParam());
    }
    pButton->setSelected(mpi::Recorder::instance().isStarted());
  } catch (mpi::Exception& e) {
    LOGE_TRACE("%s", e.what());
	PopupService::dialog("@Tips", e.localizedMessage("error"), 2000);
    return -1;
  }
  return false;
}

static bool onButtonClick_lockBtn(ZKButton *pButton) {
  if (!mpi::Recorder::instance().isStarted()) {
    return false;
  }
  auto is_lock = !mpi::Recorder::instance().isLocked();
  NO_EXCEPTION(mpi::Recorder::instance().lockNext(is_lock));
  pButton->setSelected(is_lock);
  updateRecordTime();
  mpi::AppServer::instance().notifyLockVideo(is_lock);
  return false;
}

static bool onButtonClick_playBtn(ZKButton *pButton) {
    try {
    	if (sys::setting::get_link_mode() == E_LINK_MODE_WIFIVIDEO) {
    		if (mpi::AppServer::instance().isEnter()) {
    			PopupService::dialog("@Tips", "@Connected Wifi Video", 2000);
    			return false;
    		}
    	}
      mpi::Recorder::instance().stop();
      EASYUICONTEXT->openActivity("DvrPlayActivity");
      return 0;
    } catch (mpi::Exception& e) {
      LOGE_TRACE("%s", e.what());
      PopupService::dialog("@Tips", e.localizedMessage("error"), 2000);
      return -1;
    }
  return false;
}

static bool onButtonClick_muteBtn(ZKButton *pButton) {
  try {
    sys::setting::set_dvr_record_sound_enable(!pButton->isSelected());
    auto conf = sys::setting::makeRecorderParam();
    restartRecorder(true);
    pButton->setSelected(!pButton->isSelected());
    mpi::AppServer::instance().notifyMicChanged(
    sys::setting::is_dvr_record_sound_enabled());
  } catch (mpi::Exception& e) {
    LOGE_TRACE("%s", e.what());
  }
  return false;
}

static void onVideoViewPlayerMessageListener_VideoViewPreview(ZKVideoView *pVideoView, int msg) {
	switch (msg) {
	case ZKVideoView::E_MSGTYPE_VIDEO_PLAY_STARTED:
		break;
	case ZKVideoView::E_MSGTYPE_VIDEO_PLAY_COMPLETED:
		break;
	case ZKVideoView::E_MSGTYPE_VIDEO_PLAY_ERROR:
		break;
	}
}

static bool onButtonClick_ButtonTakePicture(ZKButton *pButton) {
  if (take_picture_timer.elapsed() < 200) {
    return false;
  }
  try {
//	  if (!bt::music_is_playing() && !bt::is_calling()) {	// 蓝牙播放音乐以及通话时，dvr拍照不出提示声
//		  audio::change_audio_type(E_AUDIO_TYPE_MUSIC);		// 拍照声音需要切换为arm出声
////		  DELAY(100);	// 不加延时，点击拍照快的情况下未获取到状态可能会出现几下没有声音的情况；添加延时最高100毫秒的延时，也会出现1-2下没有声音
//		  Keytone::instance().play(CONFIGMANAGER->getResFilePath("takepicture.wav"));
//	  }
	std::vector<mpi::VideoDeviceName> names;
	names.push_back(mpi::VIDEO_DEVICE_FRONT);
	if(sys::has_reverse_camera()) {
		names.push_back(mpi::VIDEO_DEVICE_REAR);
	}
	NO_EXCEPTION(mpi::Snapshot::instance().takePicture(names, {}));
//    mpi::Recorder::instance().takePicture(
//        sys::reverse_camera_state() == REVERSE_CAMERA_STATE_DETECTED);
	take_picture_timer.reset();
  } catch (mpi::Exception& e) {
	LOGE_TRACE("%s", e.what());
	PopupService::dialog("@Tips", e.localizedMessage("error"), 0);
  }
  return false;
}

static bool onButtonClick_ButtonView(ZKButton *pButton) {
//  if (!sys::has_reverse_camera()) {
//	  return false;
//  }
  nextViewType();
  return false;
}


