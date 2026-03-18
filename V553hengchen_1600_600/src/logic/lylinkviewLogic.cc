#pragma once
#include "uart/ProtocolSender.h"
#include "utils/ScreenHelper.h"
#include "manager/ConfigManager.h"
#include "link/context.h"
#include "system/reverse.h"
#include "system/setting.h"
#include "fy/time.hpp"
#include "misc/utility.h"
#include "sysapp_context.h"
#include "media/audio_context.h"
#include "link/lylinkapi.h"

#define TOUCH_MOVE_THRESHOLD 5

static int _s_touch_x_offset;
static float _s_x_scale = 1.0f;
static float _s_y_scale = 1.0f;

/**
 * 注册定时器
 * 填充数组用于注册定时器
 * 注意：id不能重复
 */
static S_ACTIVITY_TIMEER REGISTER_ACTIVITY_TIMER_TAB[] = {
	//{0,  6000}, //定时器id=0, 时间间隔6秒
	//{1,  1000},
};

static void _link_view_show(const LYLINKAPI_VIDEO_PARAM *param) {
	int w = param->width, h = param->height;
	int sw = LINK_VIEW_WIDTH, sh = LINK_VIEW_HEIGHT;
	int vx = 0, vy = 0, vw = sw, vh = sh;  // view pos
	int rot = CONFIGMANAGER->getScreenRotate();

	switch (lk::get_lylink_type()) {
	case LINK_TYPE_AIRPLAY:
	case LINK_TYPE_MIRACAST:
	case LINK_TYPE_WIFILY:
		if ((float) sw / w > (float) sh / h) {
			vw = sh * w / h;
			vx = (sw - vw) / 2;
		} else {
			vh = sw * h / w;
			vy = (sh - vh) / 2;
		}

		if ((rot == 90) || (rot == 270)) {
			std::swap(vx, vy);
			std::swap(vw, vh);
		}
		break;

	default:
		// 半屏模式
		if (!sys::setting::is_link_screen_full()) {
			vw /= 2;
		}

		_s_x_scale = (float) vw / w;
		_s_y_scale = (float) vh / h;

		if ((rot == 90) || (rot == 270)) {
			if (!sys::setting::is_driving_left()) {
				vy = sw - vw;
				_s_touch_x_offset = vy;
			}
			std::swap(vw, vh);
		} else {
			if (!sys::setting::is_driving_left()) {
				vx = sw - vw;
				_s_touch_x_offset = vx;
			}
		}
		break;
	}

	lk::video_show(vx, vy, vw, vh);
}

static void _link_view_hide() {
	lk::video_hide();
}

static void _link_touch(LYLINK_TOUCHMODE_E mode, int x, int y) {
	x -= _s_touch_x_offset;
	lylinkapi_touch(0, mode, x, y);
}

static void _lylink_video_callback(LYLINKAPI_EVENT evt, int para0, void *para1) {
	if (evt == LYLINK_VIDEO_START) {
		const LYLINKAPI_VIDEO_PARAM *param = (const LYLINKAPI_VIDEO_PARAM *) para1;
		LOGD("[lylinkview] LYLINK_VIDEO_START %d %d\n", param->width, param->height);
		_link_view_show(param);
	}
}

static void _link_start() {
	lk::add_lylink_callback(_lylink_video_callback);

	LYLINKAPI_VIDEO_PARAM param;
	if (lk::get_video_param(param)) {
		// 有视频信息直接显示
		_link_view_show(&param);
	} else {
		// 没有视频信息，启动下视频
		lk::video_start();
	}
//	if(lk::get_lylink_type() == LINK_TYPE_WIFILY || lk::get_lylink_type() == LINK_TYPE_USBLY) {
		lylinkapi_cmd(LYLINK_AUDIO_LYLINK);
		//audio::change_audio_type(E_AUDIO_TYPE_LYLINK_MUSIC);
//	}
}

static void _link_stop() {
	LOGD("[lylinkview] link stop");
	lk::remove_lylink_callback(_lylink_video_callback);
//	_link_view_hide();
	if ((lk::get_lylink_type() == LINK_TYPE_WIFILY) || (lk::get_lylink_type() == LINK_TYPE_USBLY)) {
		_link_view_hide();
	} else {
		lk::video_stop();
		lk::video_hide();
	}
}

namespace {

class ViewTouchListener: public ZKBase::ITouchListener {
public:
	virtual void onTouchEvent(ZKBase *pBase, const MotionEvent &ev) {
		if (lk::get_lylink_type() == LINK_TYPE_MIRACAST) {
			return;
		}

		switch (ev.mActionStatus) {
		case MotionEvent::E_ACTION_DOWN:
			_link_touch(TouchMode_Down, ev.mX, ev.mY);
			mLastEv = ev;
			break;
		case MotionEvent::E_ACTION_MOVE:	// 触摸滑动
			if ((abs(ev.mX - mLastEv.mX) >= TOUCH_MOVE_THRESHOLD) ||
					(abs(ev.mY - mLastEv.mY) >= TOUCH_MOVE_THRESHOLD)) {
				mLastEv = ev;
				_link_touch(TouchMode_Move, ev.mX , ev.mY);
			}
			break;
		default:
			_link_touch(TouchMode_Up, ev.mX, ev.mY);
			break;
		}
	}

private:
	MotionEvent mLastEv;
};

int view_type = VIEW_TYPE_FRONT;

void changeView(int view) {
  mpi::CameraParam param;
  param.mirror = false;
  param.viewbox = {0, 0, 1920, 1080};
  if (sys::setting::is_driving_left()) {
    param.display = {800, 0, 800, 600};
  } else {
    param.display = {0, 0, 800, 600};
  }
  if (view == VIEW_TYPE_FRONT) {
    mpi::RearCamera::instance().setVisible(false, false);
    NO_EXCEPTION(mpi::FrontCamera::instance().setParam(param, true));
  }
  if (view == VIEW_TYPE_REAR) {
    mpi::FrontCamera::instance().setVisible(false, false);
    NO_EXCEPTION(mpi::RearCamera::instance().setParam(param, true));
  }
  mWindowCameraPtr->setPosition({param.display.x, 0, 800, 600});
}

void prevViewType() {
  view_type = view_type == VIEW_TYPE_FRONT ? VIEW_TYPE_REAR : VIEW_TYPE_FRONT;
  sys::setting::set_half_view_type(view_type);
  changeView(view_type);
}

void nextViewType() {
  view_type = view_type == VIEW_TYPE_FRONT ? VIEW_TYPE_REAR : VIEW_TYPE_FRONT;
  sys::setting::set_half_view_type(view_type);
  changeView(view_type);
}

class CameraWindowTouchListener : public ZKBase::ITouchListener {
public:
  virtual void onTouchEvent(ZKBase* pBase, const MotionEvent& ev) {
    static MotionEvent down;
    switch (ev.mActionStatus) {
      case MotionEvent::E_ACTION_DOWN:
        down = ev;
        break;
      case MotionEvent::E_ACTION_MOVE:
        break;
      case MotionEvent::E_ACTION_UP:
        if (!sys::has_reverse_camera()) {
          break;
        }
        if (ev.mX - down.mX > 100) { //右滑动
          prevViewType();
          break;
        }
        if (ev.mX - down.mX < -100) { //左滑动
          nextViewType();
          break;
        }
        break;
      default:
        break;
    }
  }
};
CameraWindowTouchListener camera_window_listener;

}

static ViewTouchListener _s_view_touch_listener;

/**
 * 当界面构造时触发
 */
static void onUI_init() {
    //Tips :添加 UI初始化的显示代码到这里,如:mText1Ptr->setText("123");
  view_type =
      sys::has_reverse_camera() ?
          (ViewType)sys::setting::get_half_view_type() : VIEW_TYPE_FRONT;

	LayoutPosition pos = LayoutPosition(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	mVideoView1Ptr->setPosition(pos);
	mVideoView1Ptr->setTouchListener(&_s_view_touch_listener);
	mActivityPtr->setPosition(pos);

	_s_touch_x_offset = 0;
	_s_x_scale = 1.0f;
	_s_y_scale = 1.0f;
}

/**
 * 当切换到该界面时触发
 */
static void onUI_intent(const Intent *intentPtr) {
  if (intentPtr != NULL) {
  }
}

/*
 * 当界面显示时触发
 */
static void onUI_show() {
  // 推迟检测播放，处理倒车界面未完全退出，播放异常问题
  //mActivityPtr->registerUserTimer(DELAY_PLAY_TIMER, 0); // @suppress("无效参数")
  EASYUICONTEXT->hideNaviBar();
  mWindowCameraPtr->setVisible(false);

  const auto type = lk::get_lylink_type();
  if ((type == LINK_TYPE_WIFIAUTO || type == LINK_TYPE_WIFICP)
      && !sys::setting::is_link_screen_full()) {
//    changeView(view_type);
//    mWindowCameraPtr->setTouchListener(&camera_window_listener);
//    mWindowCameraPtr->setVisible(true);
  } else {
	  NO_EXCEPTION(setupCamera(VIEW_TYPE_UNVISIBLE, true));
  }

  if (sys::setting::is_assist_switch_enabled()) {
    app::show_floatwnd();
  }
  _link_start();
}

/*
 * 当界面隐藏时触发
 */
static void onUI_hide() {
  //mActivityPtr->unregisterUserTimer(DELAY_PLAY_TIMER); // @suppress("无效参数")
	_link_stop();
	mWindowCameraPtr->setTouchListener(NULL);

	if (sys::setting::is_assist_switch_enabled()) {
		app::hide_floatwnd();
	}
}

/*
 * 当界面完全退出时触发
 */
static void onUI_quit() {
  _link_stop();

  if (sys::setting::is_assist_switch_enabled()) {
    app::hide_floatwnd();
  }
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
static bool onUI_Timer(int id){
	switch (id) {
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
static bool onlylinkviewActivityTouchEvent(const MotionEvent &ev) {
	switch (ev.mActionStatus) {
	case MotionEvent::E_ACTION_DOWN:	//触摸按下
		//LOGD("时刻 = %ld 坐标  x = %d, y = %d", ev.mEventTime, ev.mX, ev.mY);
		break;
	case MotionEvent::E_ACTION_MOVE:	        //触摸滑动
		break;
	case MotionEvent::E_ACTION_UP:  //触摸抬起
		break;
	default:
		break;
	}
	return false;
}
