#pragma once
#include "uart/ProtocolSender.h"
#include "utils/TimeHelper.h"
#include "link/context.h"
#include "media/audio_context.h"
#include "system/fm_emit.h"
#include "fy/date.hpp"
#include "sysapp_context.h"
#include "mode_observer.h"
#include <base/handler.hpp>
#include <base/strings.hpp>
#include "edge/popup_service.h"
#include <utils/ScreenHelper.h>
#include <memory>
#include <base/base.hpp>
#include <base/ui_utility.h>
#include "misc/utility.h"
#include "bt/context.h"
#include "system/hardware.h"
#include <app/Activity.h>
#include "uart/context.h"
#include "mode_observer.h"
#include <sys/sysinfo.h>
#include "fy/os.hpp"
#include <mpi/case/config.h>
#include <uart/context.h>


static bt_cb_t _s_bt_cb;

namespace {

enum {
  TIMER_PROGRESS = 1000,
  TIMER_DELAYED_SHOW,
  TIMER_ACTIVATION_COUNTDOWN,
  TIMER_SETBRIGHTNESS,
  TIMER_VIEW_BRIGHTNESS,
};

#define TEMP_PROTECT_DEBOUNCE	2
std::shared_ptr<base::handler<> > handler;
int loading_index = 0;
int64_t show_at = 0;
int64_t activate_dialog_deatline = 0;
static int brightness = 0;

LayoutPosition dialog_default_pos ;

void handleMessage(const base::message<>* msg, const void* user_data) {
  switch (msg->what) {
  case TIMER_PROGRESS: {
    loading_index = (loading_index + 1) % 8;
    mTextViewProgressStatePtr->setBackgroundPic(
      base::format("loading/%d.png", loading_index).c_str());
  }
    break;
  case TIMER_DELAYED_SHOW:
    show_at = base::uptime();
    mWindowPopupPtr->setVisible(true);
    break;
  default:
          break;
  }
}
//图标回调
// fm
static void _sound_mode_icon(sound_mode_e mode) {
    // 更新FM图标显示状态
    if (mTopFmBtnPtr != NULL) {
        mTopFmBtnPtr->setSelected(mode == E_SOUND_MODE_FM);
    }
}


static void _bt_connect_cb(bt_connect_state_e state) {
	switch (state) {
	case E_BT_CONNECT_STATE_IDLE:
		mblueBtnPtr->setVisible(false);
		break;
	case E_BT_CONNECT_STATE_CONNECTING:
		break;
	case E_BT_CONNECT_STATE_CONNECTED:
		mblueBtnPtr->setVisible(true);
		break;
	}
}

void showPopup() {
  //这里假设Navibar已经显示
  mnavibarPtr->mainWindow()->setPosition( { 0, 0,
      ScreenHelper::getScreenWidth(), ScreenHelper::getScreenHeight() });
}

void hidePopup() {
  if (!mWindowAuthPtr->isVisible()) {
    mnavibarPtr->mainWindow()->setPosition(mTopWindowPtr->getPosition());
  }
  mWindowPopupPtr->setVisible(false);
}

void hideAuth() {
  if (!mWindowPopupPtr->isVisible()) {
    mnavibarPtr->mainWindow()->setPosition(mTopWindowPtr->getPosition());
  }
  mWindowAuthPtr->setVisible(false);
}

void updateAuthState(int carplay) {
  mTextViewCarplayPtr->setTextTr(carplay ? "carplay_authorized" : "carplay_unauthorized");
  mTextViewCarplayPtr->setInvalid(!carplay);
}

}

void NavibarShowDelayed(int delay) {
  mWindowPopupPtr->setVisible(false);
  showPopup();
  if (delay <= 0) {
    show_at = base::uptime();
    mWindowPopupPtr->setVisible(true);
  } else {
    handler->send_message_delayed(TIMER_DELAYED_SHOW, delay);
  }
}

void NavibarHideDelayed(int min_visible_duration) {
  handler->remove_messages(TIMER_DELAYED_SHOW);
  if (min_visible_duration > 0) {
    auto duration = base::uptime() - show_at;
    if (duration > 0) {
      if (duration < min_visible_duration) {
        usleep(1000 * (min_visible_duration - duration));
      }
    }
  }
  handler->post([](const void* user_data) {
    hidePopup();
  }, NULL);
}

void NavibarSetProgressWindowVisible(bool visible) {
  if (mnavibarPtr == NULL) {
    return;
  }
  mWindowProgressPtr->setVisible(visible);
}

void NavibarSetProgressMessage(const std::string& msg) {
  if (mnavibarPtr == NULL) {
    return;
  }
  mTextViewProgressMessagePtr->setText(msg);
}

void NavibarSetProgressState(PopupProgressState state) {
  if (mnavibarPtr == NULL) {
    return;
  }
  switch (state) {
  case PopupProgressState::Running:
    loading_index = -1;
    mTextViewProgressStatePtr->setBackgroundPic("");
    handler->schedule(TIMER_PROGRESS, 100, 0);
    break;
  case PopupProgressState::Success:
    handler->remove_messages(TIMER_PROGRESS);
    handler->post([](const void* user_data) {
      mTextViewProgressStatePtr->setBackgroundPic("popup/success.png");
    }, NULL);
    break;
  case PopupProgressState::Failure:
    handler->remove_messages(TIMER_PROGRESS);
    handler->post([](const void* user_data) {
      mTextViewProgressStatePtr->setBackgroundPic("popup/failure.png");
    }, NULL);
    break;
  }
}

void NavibarSetDialogWindowVisible(bool visible) {
  if (mnavibarPtr == NULL) {
    return;
  }
  mWindowDialog1Ptr->setVisible(visible);
}

void NavibarSetDialog(const std::string& title, const std::string& msg,
    std::vector<PopupButton>& buttons, const LayoutPosition& pos) {
  if (mnavibarPtr == NULL) {
    return;
  }

  if (pos.mWidth > 0 && pos.mHeight > 0) {
    LOGE("%d, %d, %d, %d", pos.mLeft, pos.mTop, pos.mWidth, pos.mHeight);
    mWindowDialog1Ptr->setPosition(pos);
  } else {
    mWindowDialog1Ptr->setPosition(dialog_default_pos);
  }

  mTextViewDialogTitlePtr->setText(title);
  mTextViewDialogMessagePtr->setText(msg);
  auto msg_pos = mTextViewDialogMessagePtr->getPosition();
  msg_pos.mTop = buttons.empty() ? 110 : 70;
  mTextViewDialogMessagePtr->setPosition(msg_pos);

  auto button_views = base::getChildren(mWindowDialogButtonsPtr);
  for (int i = 0; i < (int)button_views.size(); ++i) {
    bool visible = i < (int)buttons.size();
    button_views[i]->setVisible(visible);
    ((ZKTextView*)button_views[i])->setText(visible ? buttons[i].text : "");
    button_views[i]->setClickListener(&buttons[i]);
  }
  base::tileAndCenterhorizontally(mWindowDialogButtonsPtr, 50, false);
}

void NavibarDialog1ClickOk() {
  while (PopupService::instance()->busy_) {
    PopupService::instance()->busy_ = false;
  }
  hidePopup();
}

void showDebug() {
	if(mnavibarPtr == NULL) {
		return;
	}
	mnavibarPtr->mainWindow()->setPosition( { 0, 0,
	      ScreenHelper::getScreenWidth(), 120});
	mnavibarPtr->registerUserTimer(TIMER_VIEW_BRIGHTNESS, 1000);
	mDebugWndPtr->setTouchPass(true);
	mDebugWndPtr->setVisible(true);
}

void hideDebug() {
	if(mnavibarPtr == NULL) {
		return;
	}
	mnavibarPtr->unregisterUserTimer(TIMER_VIEW_BRIGHTNESS);
	mDebugWndPtr->setVisible(false);
}

void showAuthDialog(int carplay, int timeout) {
  if(mnavibarPtr == NULL) {
    return;
  }
  mnavibarPtr->mainWindow()->setPosition( { 0, 0,
      ScreenHelper::getScreenWidth(), ScreenHelper::getScreenHeight() });
  updateAuthState(carplay);
  mWindowAuthPtr->setVisible(true);
  activate_dialog_deatline = base::uptime() + timeout * 1000;
  mnavibarPtr->unregisterUserTimer(TIMER_ACTIVATION_COUNTDOWN);
  mnavibarPtr->registerUserTimer(TIMER_ACTIVATION_COUNTDOWN, 200);
}

bool isAuthDialogVisible() {
  if (mnavibarPtr == NULL) {
    return false;
  }
  return mWindowAuthPtr->isVisible();
}

void updateVolumeText() {
	char buf[16];
	if ((lk::get_is_call_state() == CallState_Conn) || bt::is_calling()) {
		snprintf(buf, sizeof(buf), "%.0f", audio::get_call_vol() * 100);
		mvolumeTextPtr->setText(buf);
		mvolumeBtnPtr->setSelected(audio::get_call_vol() == 0.0f); // 音量大于0选中，否则不选中
	} else {
		snprintf(buf, sizeof(buf), "%.0f", audio::get_system_vol() * 100);
		mvolumeTextPtr->setText(buf);
		mvolumeBtnPtr->setSelected(audio::get_system_vol() == 0.0f); // 音量大于0选中，否则不选中
	}
}

void updateSdCardIcon(bool isMounted) {
    if (mTopSdBtnPtr != NULL) {
        mTopSdBtnPtr->setSelected(isMounted);
    }
}

static void _audio_output_mode_cb(audio_player_mode_e new_mode, audio_player_mode_e old_mode) {
	LOGD("[navibar] audio output mode %d ---> %d\n", old_mode, new_mode);
	switch (old_mode) {
	case E_AUDIO_PLAYER_MODE_FM:
		uart::fm_switch(false);
		break;
	default:
		break;
	}

	switch (new_mode) {
	case E_AUDIO_PLAYER_MODE_FM:
//		DELAY(100);
		uart::fm_switch(true);
		DELAY(120);
		uart::fm_frequency(sys::setting::get_fm_frequency());
		break;
	default:
		break;
	}
}

static void _sound_mode_cb(sound_mode_e mode) {
	_sound_mode_icon(mode);//fm图标
	switch (mode) {
	case E_SOUND_MODE_SPK:
//	    uart::set_sound_field(SOUND_FIELD_LU, 0);
//	    uart::set_sound_field(SOUND_FIELD_RU, 20);
		audio::change_output_mode(E_AUDIO_PLAYER_MODE_SPK);
//		DELAY(50);
//		uart::set_amplifier_mute(1);
		break;
	case E_SOUND_MODE_LINK:
		// do nothing
//		uart::set_sound_field(SOUND_FIELD_LU, 0);
//		uart::set_sound_field(SOUND_FIELD_RU, 20);
		break;
	case E_SOUND_MODE_FM:
//		uart::set_amplifier_mute(0);
		audio::change_output_mode(E_AUDIO_PLAYER_MODE_FM);
		break;
	case E_SOUND_MODE_AUX:
//		uart::set_amplifier_mute(0);
//		uart::set_sound_field(SOUND_FIELD_LU, 20);
//		uart::set_sound_field(SOUND_FIELD_RU, 0);
		audio::change_output_mode(E_AUDIO_PLAYER_MODE_AUX);
		break;
	default:
		break;
	}

}

static void _switch_mode_cb(switch_mode_e mode) {
	updateVolumeText();
	mnavibarPtr->show();
}

static void _update_date() {
	char str[20];
	struct tm *t = TimeHelper::getDateTime();
	int hour = t->tm_hour;

	if (!sys::setting::is_time_format_24h()) {
		mTopTimeAMTextPtr->setText((t->tm_hour >= 12) ? "PM" : "AM");
		hour = (t->tm_hour > 12) ? (t->tm_hour % 12) : t->tm_hour;
	} else {
		mTopTimeAMTextPtr->setText("");
	}
	sprintf(str, "%02d:%02d", hour, t->tm_min);
	mTopTimeTextViewPtr->setText(str);

//	const char *week[] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
//	mWeekTextViewPtr->setTextTr(week[t->tm_wday]);
}

static void showSysInfo() {
	struct sysinfo info;
	sysinfo(&info);

	long freeram = info.freeram;
	char str[20] = {0};
	sprintf(str, "内存:%ld kb", freeram / 1000);
//	mTextView3Ptr->setText(str);
//	LOGE("[cache] ---%ld---", freeram / 1000);
//	if (sys::setting::is_debug_enabled()) {
		msysTextViewPtr->setText(str);
//	}
	if(freeram <= 5*1000*1000) {	//剩余内存小于5M时,释放缓存
		LOGE("[caches] freeram = %ld kb", freeram / 1000);
		fy::drop_caches();
	}
}

static void _temp_protect_cb(int val) {
	int num = val / 1000;
	char str[20] = {0};
	float temp_celsius = val / 1000.0;
	sprintf(str, "温度:%.2f", temp_celsius);
//	LOGE("[temp_celsius] ---%.2f---", temp_celsius);
	mTempTextViewPtr->setText(str);
#if 1
	static bool is_set_temp_protext_level1 = false;
	static bool is_set_temp_protext_level2 = false;
	if (!is_set_temp_protext_level1 && !is_set_temp_protext_level2 && (num >= sys::setting::get_temp_protect_level1())) {
		sys::setting::set_save_brightness(sys::setting::get_brightness());
	}
	if ((num >= sys::setting::get_temp_protect_level1()) && \
			(num < sys::setting::get_temp_protect_level2() - TEMP_PROTECT_DEBOUNCE)) {
		is_set_temp_protext_level1 = true;
		brightness = BRIGHTNESS_MAX_VAL * 0.4;
		if (!is_set_temp_protext_level2 && (brightness >= sys::setting::get_brightness())) return ;
		is_set_temp_protext_level2 = false;
//		mTempTextViewPtr->setTextColor(0xFFFF00);
//		sys::setting::set_camera_rate_mode(2);
//		mpi::RecorderParam param;
//		param = sys::setting::makeRecorderParam();
//		if (param.front.frame_rate != 20 && param.rear.frame_rate != 20) {
//			NO_EXCEPTION(setupCamera(VIEW_TYPE_BOTH, true));
//			NO_EXCEPTION(restartRecorder(true));
//		}
		mnavibarPtr->registerUserTimer(TIMER_SETBRIGHTNESS, 100);
	} else if (num >= sys::setting::get_temp_protect_level2()) {
		is_set_temp_protext_level2 = true;
		brightness = 0;
		mnavibarPtr->registerUserTimer(TIMER_SETBRIGHTNESS, 100);
//		mTempTextViewPtr->setTextColor(0xFF0000);
//		sys::setting::set_camera_rate_mode(3);
//		mpi::RecorderParam param;
//		param = sys::setting::makeRecorderParam();
//		if (param.front.frame_rate != 15 && param.rear.frame_rate != 15) {
//			NO_EXCEPTION(setupCamera(VIEW_TYPE_BOTH, true));
//			NO_EXCEPTION(restartRecorder(true));
//		}
	} else if (num < sys::setting::get_temp_protect_level1() - TEMP_PROTECT_DEBOUNCE) {
		if (is_set_temp_protext_level1 || is_set_temp_protext_level2) {
			brightness = sys::setting::get_save_brightness();
			mnavibarPtr->registerUserTimer(TIMER_SETBRIGHTNESS, 100);
			is_set_temp_protext_level1 = false;
			is_set_temp_protext_level2 = false;
			sys::setting::set_save_brightness(0);
			mTempTextViewPtr->setTextColor(0xFFFFFF);
		}
//		sys::setting::set_camera_rate_mode(1);
//		mpi::RecorderParam param;
//		param = sys::setting::makeRecorderParam();
//		if (param.front.frame_rate != 25 && param.rear.frame_rate != 25) {
//			NO_EXCEPTION(setupCamera(VIEW_TYPE_BOTH, true));
//			NO_EXCEPTION(restartRecorder(true));
//		}
	}
#endif
}

static void _mcu_cb(MCU_EVENT event, void* data, uint32_t len) {

	switch (event) {
	case MCU_UPD_MODE_ENTRY:
		mTextView3Ptr->setVisible(true);
		break;
	case MCU_UPD_PRGGRESS:
		mTextView3Ptr->setVisible(true);
		break;
	case MCU_UPD_FINISHED:
		mTextView3Ptr->setVisible(false);
		break;
	}
}


/**
 * 注册定时器
 * 填充数组用于注册定时器
 * 注意：id不能重复
 */
static S_ACTIVITY_TIMEER REGISTER_ACTIVITY_TIMER_TAB[] = {
	//{0,  6000}, //定时器id=0, 时间间隔6秒
	{ 1, 1000 },
};

/**
 * 当界面构造时触发
 */
static void onUI_init() {
	//Tips :添加 UI初始化的显示代码到这里,如:mText1->setText("123");add_btn_mode_cb

	LOGD("[navibar] onUI_init\n");

	dialog_default_pos = mWindowDialog1Ptr->getPosition();

//	sys::setting::set_sound_mode(E_SOUND_MODE_SPK);
	audio::add_output_mode_cb(_audio_output_mode_cb);
	mode::add_sound_mode_cb(_sound_mode_cb);
	mode::add_switch_mode_cb(_switch_mode_cb);

	_update_date();
	updateVolumeText();
	//sys::setting::update_eye_protection_info();

    handler = std::make_shared<base::handler<>>();
    handler->set_lazy(true);
    handler->set_message_handler(handleMessage, NULL);
	sys::hw::add_temp_protect_cb(_temp_protect_cb);

    mWindowAuthPtr->setVisible(false);
    mTopWindowPtr->showWnd();
    mWindowProgressPtr->setVisible(false);
    mWindowDialog1Ptr->setVisible(false);
    mWindowPopupPtr->setVisible(false);
    //mTextViewMaskPtr->setAlpha(80);
    mTextViewMaskPtr->setVisible(false);
    mDebugWndPtr->setTouchPass(true);
    mDebugWndPtr->setVisible(false);
    mTextView3Ptr->setVisible(false);
    hidePopup();

    _s_bt_cb.connect_cb = _bt_connect_cb;
    bt::add_cb(&_s_bt_cb);
	uart::add_mcubt_cb(_mcu_cb);

}

/*
 * 当界面完全退出时触发
 */
static void onUI_quit() {
	handler.reset();
	sys::hw::remove_temp_protect_cb(_temp_protect_cb);
	audio::remove_output_mode_cb(_audio_output_mode_cb);
	mode::remove_sound_mode_cb(_sound_mode_cb);
	mode::remove_switch_mode_cb(_switch_mode_cb);
	bt::remove_cb(&_s_bt_cb);
	uart::remove_mcubt_cb(_mcu_cb);
}

/**
 * 串口数据回调接口
 */
static void onProtocolDataUpdate(const SProtocolData &data) {
    //串口数据回调接口
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
	char str[20] = {0};

	switch (id) {
	case 1:
		showSysInfo();
		_update_date();
		break;
	case TIMER_ACTIVATION_COUNTDOWN: {
	  auto left = activate_dialog_deatline - base::uptime();
	  if (left < 0) {
	    hideAuth();
	    return false;
	  }
	  mTextViewCountdownPtr->setText(base::format("%lld S", left / 1000));
	  updateAuthState(lk::get_is_authorized());
	}
	  break;
	case TIMER_SETBRIGHTNESS: {
		int current_val = sys::setting::get_brightness();
		if (current_val > brightness) {
			sys::setting::set_brightness(max(brightness, current_val - 5));
		} else {
			sys::setting::set_brightness(min(brightness, current_val + 5));
		}
		if (sys::setting::get_brightness() == brightness) return false;
	}
	  break;
	case TIMER_VIEW_BRIGHTNESS:{
		sprintf(str, "背光:%d", sys::setting::get_brightness());
		mbriTextViewPtr->setText(str);
		char r[50] = {0};
		char f[50] = {0};
		mpi::RecorderParameters parameters;
		parameters = sys::setting::makeRecorderParam();
		sprintf(f, "前摄：%d %d 帧数：%d", parameters.settings[mpi::VIDEO_DEVICE_FRONT].size.h, parameters.settings[mpi::VIDEO_DEVICE_FRONT].size.w, parameters.settings[mpi::VIDEO_DEVICE_FRONT].frame_rate);
		sprintf(r, "后摄：%d %d 帧数：%d", parameters.settings[mpi::VIDEO_DEVICE_REAR].size.h, parameters.settings[mpi::VIDEO_DEVICE_REAR].size.w, parameters.settings[mpi::VIDEO_DEVICE_REAR].frame_rate);
		mTextView1Ptr->setText(f);
		mTextView4Ptr->setText(r);
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
static bool onnavibarActivityTouchEvent(const MotionEvent &ev) {
    switch (ev.mActionStatus) {
	case MotionEvent::E_ACTION_DOWN://触摸按下
		//LOGD("时刻 = %ld 坐标  x = %d, y = %d", ev.mEventTime, ev.mX, ev.mY);
		break;
	case MotionEvent::E_ACTION_MOVE://触摸滑动
		break;
	case MotionEvent::E_ACTION_UP:  //触摸抬起
		break;
	default:
		break;
	}
	return false;
}

static bool onButtonClick_sys_home(ZKButton *pButton) {
  if (PopupService::isBusy()) {
    return false;
  }
  LOGD(" ButtonClick sys_home !!!");
  mode::set_event_mode(E_EVENT_MODE_HOME);
  return false;
}

static bool onButtonClick_ButtonDialogButton1(ZKButton *pButton) {
  return false;
}

static bool onButtonClick_ButtonDialogButton2(ZKButton *pButton) {
  return false;
}

static bool onButtonClick_ButtonActivationYes(ZKButton *pButton) {
  hideAuth();
  mnavibarPtr->unregisterUserTimer(TIMER_ACTIVATION_COUNTDOWN);
  return false;
}

static bool onButtonClick_sys_back(ZKButton *pButton) {
    LOGD(" ButtonClick sys_back !!!\n");
    return false;
}
static bool onButtonClick_volumeBtn(ZKButton *pButton) {
    LOGD(" ButtonClick volumeBtn !!!\n");

    if (PopupService::isBusy()) {
      return false;
    }
    if (app::is_show_ctrlbar() && (mode::get_event_mode() == E_EVENT_MODE_VOICE)) {
      app::hide_ctrlbar();
    } else {
      mode::set_event_mode(E_EVENT_MODE_VOICE);
      app::show_ctrlbar();
    }
    return false;
}
static bool onButtonClick_briButton(ZKButton *pButton) {
    LOGD(" ButtonClick briButton !!!\n");
    if (PopupService::isBusy()) {
      return false;
    }
    LOGD(" ButtonClick brightnessBtn !!!\n");

    if (app::is_show_ctrlbar() && (mode::get_event_mode() == E_EVENT_MODE_BRIGHTNESS)) {
      app::hide_ctrlbar();
    } else {
      mode::set_event_mode(E_EVENT_MODE_BRIGHTNESS);
      app::show_ctrlbar();
    }

    return false;
}
static bool onButtonClick_TopWifiBtn(ZKButton *pButton) {
    LOGD(" ButtonClick TopWifiBtn !!!\n");
    return false;
}

static bool onButtonClick_TopFmBtn(ZKButton *pButton) {
    LOGD(" ButtonClick TopFmBtn !!!\n");
    return false;
}

static bool onButtonClick_TopUsbBtn(ZKButton *pButton) {
    LOGD(" ButtonClick TopUsbBtn !!!\n");
    return false;
}

static bool onButtonClick_TopSdBtn(ZKButton *pButton) {
    LOGD(" ButtonClick TopSdBtn !!!\n");
    return false;
}
static bool onButtonClick_blueBtn(ZKButton *pButton) {
    LOGD(" ButtonClick blueBtn !!!\n");
    return false;
}

void NavibarSetMcuUpgradeMask(bool visible) {
    if (mTextView3Ptr == NULL) {
        return;
    }
    mTextView3Ptr->setVisible(visible);
    LOGD("[navibar] MCU upgrade mask set to %s", visible ? "visible" : "hidden");
}
