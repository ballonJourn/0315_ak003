#pragma once
#include "uart/ProtocolSender.h"

#include "app/SysAppFactory.h"
#include "system/setting.h"
#include "link/context.h"
#include "bt/context.h"
#include "media/audio_context.h"
#include "mode_observer.h"
#include "sysapp_context.h"
#include "common.h"
#include <base/ui_handler.h>
#include "system/hardware.h"

#define BAR_HIDE_TIMER       1
#define BAR_HIDE_TIMEOUT     5000

// 注册成系统界面
REGISTER_SYSAPP(APP_TYPE_SYS_CTRLBAR, ctrlbarActivity);
extern void updateVolumeText();

static bool _is_calling() {
	return (lk::get_is_call_state() == CallState_Conn) || bt::is_calling();
}

static void _update_voice_icon(int progress) {
	const char *icon[] = {
		"main/voice_no.png",
		"main/voice_small.png",
		"main/voice_middle.png",
		"main/voice_big.png",
	};

	int max = mVoiceSeekBarPtr->getMax();
	int level = 0;

	if (progress > (max / 3 * 2)) {
		level = 3;
	} else if (progress > (max / 3)) {
		level = 2;
	} else if (progress > 0) {
		level = 1;
	}

	mvoicepicPtr->setBackgroundPic(icon[level]);
}

static void _event_mode_cb(event_mode_e mode) {
	if (mode == E_EVENT_MODE_BRIGHTNESS) {
		mbrightnessSeekBarPtr->setProgress(sys::setting::get_brightness());
		mBrightnessWndPtr->showWnd();
		mvoiceWndPtr->hideWnd();
		mActivityPtr->unregisterUserTimer(BAR_HIDE_TIMER);
		mActivityPtr->registerUserTimer(BAR_HIDE_TIMER, BAR_HIDE_TIMEOUT);
	} else if (mode == E_EVENT_MODE_VOICE) {
		int progress;
		if (lk::get_is_call_state() == CallState_Conn) {
			progress = audio::get_call_vol() * mVoiceSeekBarPtr->getMax();
		} else if (bt::is_calling()) {
			progress = audio::get_call_vol() * mVoiceSeekBarPtr->getMax();
			bt::call_vol(progress / 10.f);
		} else {
			progress = audio::get_system_vol() * mVoiceSeekBarPtr->getMax();
		}
		mVoiceSeekBarPtr->setProgress(progress);
		_update_voice_icon(progress);

		mvoiceWndPtr->showWnd();
		mBrightnessWndPtr->hideWnd();
		mActivityPtr->unregisterUserTimer(BAR_HIDE_TIMER);
		mActivityPtr->registerUserTimer(BAR_HIDE_TIMER, BAR_HIDE_TIMEOUT);
	}
}

static void _auto_light_cb(LIGHT_MODE_E mode) {
	if(mode != LIGHT_MODE_INVALID) {
		mbrightnessSeekBarPtr->setProgress(sys::setting::get_brightness());
	}
}

/**
 * 注册定时器
 * 填充数组用于注册定时器
 * 注意：id不能重复
 */
static S_ACTIVITY_TIMEER REGISTER_ACTIVITY_TIMER_TAB[] = {
	//{0,  6000}, //定时器id=0, 时间间隔6秒
	//{1,  1000},
};

/**
 * 当界面构造时触发
 */
static void onUI_init() {
    //Tips :添加 UI初始化的显示代码到这里,如:mText1Ptr->setText("123");
	mbrightnessSeekBarPtr->setMax(BRIGHTNESS_MAX_VAL);

	_event_mode_cb(mode::get_event_mode());
	mode::add_event_mode_cb(_event_mode_cb);
	sys::hw::add_light_det_cb(_auto_light_cb);
	mTextViewBgPtr->setAlpha(150);
}

/**
 * 当切换到该界面时触发
 */
static void onUI_intent(const Intent *intentPtr) {
    if (intentPtr != NULL) {
        //TODO
    }
}

/*
 * 当界面显示时触发
 */
static void onUI_show() {

}

/*
 * 当界面隐藏时触发
 */
static void onUI_hide() {

}

/*
 * 当界面完全退出时触发
 */
static void onUI_quit() {
	mode::remove_event_mode_cb(_event_mode_cb);
	sys::hw::remove_light_det_cb(_auto_light_cb);
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
	case BAR_HIDE_TIMER:
		app::hide_ctrlbar();
		return false;
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
static bool onctrlbarActivityTouchEvent(const MotionEvent &ev) {
	static bool isHit;

    switch (ev.mActionStatus) {
	case MotionEvent::E_ACTION_DOWN://触摸按下
		//LOGD("时刻 = %ld 坐标  x = %d, y = %d", ev.mEventTime, ev.mX, ev.mY);
		if (mActivityPtr->isShow()) {
			if (mActivityPtr->getPosition().isHit(ev.mX, ev.mY)) {
				isHit = true;
				mActivityPtr->resetUserTimer(BAR_HIDE_TIMER, BAR_HIDE_TIMEOUT);
				break;
			}
		}

		isHit = false;
		break;
	case MotionEvent::E_ACTION_MOVE://触摸滑动
		if (isHit) {
			mActivityPtr->resetUserTimer(BAR_HIDE_TIMER, BAR_HIDE_TIMEOUT);
		}
		break;
	case MotionEvent::E_ACTION_UP:  //触摸抬起
		if (!isHit) {
			mActivityPtr->resetUserTimer(BAR_HIDE_TIMER, 10);
		}
		break;
	default:
		break;
	}
	return false;
}

static void onProgressChanged_VoiceSeekBar(ZKSeekBar *pSeekBar, int progress) {
  auto vol = (float) progress / pSeekBar->getMax();
  _update_voice_icon(progress);

  if (_is_calling()) {
    audio::set_call_vol(vol, true, false);
    base::runInUiThreadUniqueDelayed("ctrlbarl_set_call_vol", [vol]() {
    	audio::set_call_vol(vol, true, true);
    }, 1000);
    updateVolumeText();
  } else {
    audio::set_system_vol(vol, true, false);
    base::runInUiThreadUniqueDelayed("ctrlbarl_set_system_vol", [vol]() {
      audio::set_system_vol(vol, true, true);
    }, 1000);
    //extern函数代替回调更新系统音量
    updateVolumeText();
  }
}

static void onProgressChanged_brightnessSeekBar(ZKSeekBar *pSeekBar, int progress) {
	sys::setting::set_brightness(progress);
}
