#pragma once
#include "uart/ProtocolSender.h"
#include "system/setting.h"
#include "system/usb_monitor.h"
#include "link/context.h"
#include "manager/LanguageManager.h"
#include "utils/TimeHelper.h"
#include "fy/date.hpp"
#include "mode_observer.h"
#include "common.h"
#include "bt/context.h"
#include "mpi/case/camera.h"
#include <base/strings.hpp>
#include <base/time.hpp>
#include <base/timer.hpp>
#include "version.h"
#include "fy/os.hpp"
#include <base/files.hpp>
#include <edge/fat32/filesystem.h>
#include "edge/popup_service.h"
#include <base/ui_handler.h>
#include "misc/utility.h"
#include <utils/BrightnessHelper.h>
#include "misc/storage.h"
#include "sysapp_context.h"
#include "media/audio_context.h"
#include "uart/context.h"
#include "utils/ScreenHelper.h"
#include "net/ap.h"
using namespace sys::setting;

#define LINK_RESTART_TIMER    1

#define TIMEBIGSIZE 	35 	//时间设置中间字体大小
#define TIMESMALLSIZE 	22	//时间设置两边字体大小

#define START_YEAR      2010
#define YEAR_NUM        30

extern void NavibarSetMcuUpgradeMask(bool blocking);

namespace {

enum {
	MPI_RECORDER_STOP = 15,
};

const char* TASK_HIDE_OPTION_LIST = "setting.hideOptionList";

enum {
  MODE_RESOLUTION_RATIO,
  MODE_LOOP_RECORD,
  MODE_SOUND_RECORD,
  MODE_ASSIST_SWITCH,
  MODE_MIRROR_IMAGE,
  MODE_SCREEN_SAVER,
  //MODE_EYE_PROTECTION,
  MODE_DRIVING_POSITION,
//  MODE_SPLIT_SCREEN,
  MODE_REVERSE_LINE_CAL,
  MODE_LANG_SETTING,
  MODE_TIME_SETTING,
  MODE_CLEAR_CONNECT_RECORD,
  MODE_TOUCH_SOUND,
  MODE_FORMAT_CARD,
  MODE_FACTORY_RESET,
  MODE_SOFTWARE_VERSION,
  MODE_MCU_UPGRADE,
//  MODE_DEBUG,
};

mpi::Rectangle reverse_viewbox = {0, 0, 1920, 720};
bool is_dvr_param_changed = false;
std::thread set_language_thread;

static const char *modetext[] = {
	"resolution ratio",
	"Loop recording",
	"sound recording",
	"assist switch",
	"mirror image",
	"screen saver",
	//"eyeprotect",
	"Driving Position",
	"Reverse line calibration",
	"Language settings",
	"time setting",
	"clear connect record",
	"touch sound",
	"Format memory card",
	"Factory Default",
	"Software version",
	"mcu Upgrade",
//	"Debug",
};
static const char *setmodetext[] = {
	"set_resolution ratio",
	"set_Loop recording",
	"set_sound recording",
	"set_assist switch",
	"set_mirror image",
	"set_screen saver",
	//"eyeprotect",
	"set_Driving Position",
	"set_Reverse line calibration",
	"set_Language settings",
	"set_time setting",
	"set_clear connect record",
	"set_touch sound",
	"set_Format memory card",
	"set_Factory Default",
	"set_Software version",
	"set_mcu Upgrade",
//	"Debug",
};

static const char *hdtext[] = { "1080P", "2.5K", "4K",};
// static const char *hdtext[] = { "1080P", "2.5K", };
//static const char *hdtext[] = { "720P_720P", "1080P_1080P", "2_5K_720P", };
static const char *recordingtext[] = { "1min", "3min", "5min", };
static const char *switchtext[] = { "close", "open", };
static const char *screentext[] = { "close", "1min", "2min", "3min", };
static const char *drivingtext[] = { "Left hand drive", "Right hand drive", };
static const char *splittext[] = { "Full Screen", "Half screen mode", };

static const char* lang_name[] = {
	"简体中文",
	"English(US)",
	"Français",
	"Español",
	"Deutsch",
	"日本語",
	"한국어",
	"中文（繁體）",
    "Polski",                    // pl_PL
    "Русский",                   // ru_RU
};
static const char* lang_code[] = {
	"zh_CN",
	"en_US",
	"fr_FR",
	"es_ES",
	"gm_GM",
	"ja_JP",
	"ko_KO",
	"zh_FT",
	"pl_PL",
	"ru_RU",

};

static int _s_option_sel_index;
static int _s_days;
static ZKBase *_s_hit_arrow;
static bool _s_mcu_file;
static bool _s_mcu_succes = false;
static bool _s_mcu_upgrade_state = false;
static const char* MCU_UPGRADE_STATE_FILE = "/data/.mcu_upgrade_state";

static bool get_mcu_upgrade_state() {
    return FILE_EXIST(MCU_UPGRADE_STATE_FILE);
}

static void set_mcu_upgrade_state(bool upgrading) {
    if (upgrading) {
        // 创建标志文件
        FILE* fp = fopen(MCU_UPGRADE_STATE_FILE, "w");
        if (fp) {
            fprintf(fp, "upgrading");
            fclose(fp);
            LOGD("[settings MCU] upgrade state flag created");
        }
    } else {
        // 删除
        if (FILE_EXIST(MCU_UPGRADE_STATE_FILE)) {
            remove(MCU_UPGRADE_STATE_FILE);
            LOGD("[settings MCU] upgrade state flag removed");
        }
    }
}

typedef struct {
	int listnum;
	const char **text;
	int (*get_select_index)();
	void (*set_select_index)(int index);
} option_info_t;

static int _get_resolution_index();
static void _set_resolution_index(int index);
static int _get_record_index();
static void _set_record_index(int index);
static int _get_record_sound_index();
static void _set_record_sound_index(int index);
static int _get_assist_switch_index();
static void _set_assist_switch_index(int index);
static int _get_back_camera_mirror_index();
static void _set_back_camera_mirror_index(int index);
static int _get_screensaver_index();
static void _set_screensaver_index(int index);
static int _get_driving_index();
static void _set_driving_index(int index);
static int _get_link_screen_index();
static void _set_link_screen_index(int index);
static int _get_lang_index();
static void _set_lang_index(int index);
static int _get_touch_sound_index();
static void _set_touch_sound_index(int index);

static option_info_t s_option_info[] = {
	{ TAB_SIZE(hdtext), hdtext, _get_resolution_index, _set_resolution_index },
	{ TAB_SIZE(recordingtext), recordingtext, _get_record_index, _set_record_index },
	{ TAB_SIZE(switchtext), switchtext, _get_record_sound_index, _set_record_sound_index },
	{ TAB_SIZE(switchtext), switchtext, _get_assist_switch_index, _set_assist_switch_index },
	{ TAB_SIZE(switchtext), switchtext, _get_back_camera_mirror_index, _set_back_camera_mirror_index },
	{ TAB_SIZE(screentext), screentext, _get_screensaver_index, _set_screensaver_index },
	//{ 0, NULL, NULL, NULL }, eye protection
	{ TAB_SIZE(drivingtext), drivingtext, _get_driving_index, _set_driving_index },
//	{ TAB_SIZE(splittext), splittext, _get_link_screen_index, _set_link_screen_index },
	{ 0, NULL, NULL, NULL },  // reverse line
	{ TAB_SIZE(lang_name), lang_name, _get_lang_index, _set_lang_index },
	{ 0, NULL, NULL, NULL },  // time setting
	{ 0, NULL, NULL, NULL },  // clear connect record
	{ TAB_SIZE(switchtext), switchtext, _get_touch_sound_index, _set_touch_sound_index },
	{ 0, NULL, NULL, NULL },  // format card
	{ 0, NULL, NULL, NULL },  // factory reset
	{ 0, NULL, NULL, NULL },  // software version
	{ 0, NULL, NULL, NULL },
	{ 0, NULL, NULL, NULL },
};

static void _start_camera() {
  setupCamera(VIEW_TYPE_REVERSE, false);
#if 0
	int w = get_camera_width(), h = get_camera_height();
	ERotation rot = (ERotation) get_camera_rot();

	mCameraViewReversePtr->setDevPath(get_camera_dev());
	mCameraViewReversePtr->setFormatSize(w, h);
	mCameraViewReversePtr->setFrameRate(get_camera_rate());
	mCameraViewReversePtr->setRotation(rot);
	mCameraViewReversePtr->setMirror((EMirror) get_camera_mirror());
	mCameraViewReversePtr->setChannel(get_camera_chn());

	// crop
	const LayoutPosition &vp = mCameraViewReversePtr->getPosition();
	LayoutPosition cp(0, 0, w, h);

	if ((float) w / vp.mWidth > (float) h / vp.mHeight) {
		cp.mWidth = h * vp.mWidth / vp.mHeight;
		cp.mLeft = (w - cp.mWidth) / 2;
	} else {
		cp.mHeight = w * vp.mHeight / vp.mWidth;
		cp.mTop = (h - cp.mHeight) / 2;
	}
	if ((rot == E_ROTATION_90) || (rot == E_ROTATION_270)) {
		std::swap(cp.mLeft, cp.mTop);
		std::swap(cp.mWidth, cp.mHeight);
	}

	mCameraViewReversePtr->setCropPosition(cp);
	LOGD("[settingsLogic] camera crop(%d, %d, %d, %d)\n", cp.mLeft, cp.mTop, cp.mWidth, cp.mHeight);

	mCameraViewReversePtr->startPreview();
#endif
}

static void _stop_camera() {
#if 0
	mCameraViewReversePtr->stopPreview();
	WAIT(!mCameraViewReversePtr->isPreviewing(), 100, 30);
#endif
}

static void _move_arrow(ZKBase *arrow, int x, int y) {
	LayoutPosition pos = arrow->getPosition();
	pos.mLeft = x - pos.mWidth / 2;
	pos.mTop = y - pos.mHeight / 2;
	arrow->setPosition(pos);
}

static void _get_arrow_center_point(ZKBase *arrow, SZKPoint &point) {
	LayoutPosition pos = arrow->getPosition();
	point.x = pos.mLeft + pos.mWidth / 2;
	point.y = pos.mTop + pos.mHeight / 2;
}

static void _draw_reverse_line() {
	SZKPoint lt, rt, lb, rb;
	_get_arrow_center_point(mLeftArrowButtonPtr, lt);
	_get_arrow_center_point(mRightArrowButtonPtr, rt);
	_get_arrow_center_point(mBottomLeftArrowButtonPtr, lb);
	_get_arrow_center_point(mBottomRightArrowButtonPtr, rb);

	mLinePainterPtr->erase(0, 0, 0, 0);

	SZKPoint points[3];

	int h = lb.y - lt.y;  // 垂直高度, 限制不为0
	int gh = REVERSE_LINE_G_RATIO * h;
	int yh = REVERSE_LINE_Y_RATIO * h;

	// draw left
	int w = lb.x - lt.x;
	float ratio = (float) w / h;

	points[0].x = lt.x + REVERSE_LINE_CORNER_LEN;
	points[0].y = lt.y;
	points[1].x = lt.x;
	points[1].y = lt.y;
	points[2].x = ratio * gh + lt.x;
	points[2].y = lt.y + gh;
	mLinePainterPtr->setSourceColor(REVERSE_LINE_G_COLOR);
	mLinePainterPtr->drawLines(points, 3);

	points[0].x = points[2].x + REVERSE_LINE_CORNER_LEN;
	points[0].y = points[2].y;
	points[1].x = points[2].x;
	points[1].y = points[2].y;
	points[2].x = ratio * (gh + yh) + lt.x;
	points[2].y = lt.y + (gh + yh);
	mLinePainterPtr->setSourceColor(REVERSE_LINE_Y_COLOR);
	mLinePainterPtr->drawLines(points, 3);

	points[0].x = points[2].x + REVERSE_LINE_CORNER_LEN;
	points[0].y = points[2].y;
	points[1].x = points[2].x;
	points[1].y = points[2].y;
	points[2].x = lb.x;
	points[2].y = lb.y;
	mLinePainterPtr->setSourceColor(REVERSE_LINE_R_COLOR);
	mLinePainterPtr->drawLines(points, 3);

	// draw right
	w = rb.x - rt.x;
	ratio = (float) w / h;

	points[0].x = rt.x - REVERSE_LINE_CORNER_LEN;
	points[0].y = rt.y;
	points[1].x = rt.x;
	points[1].y = rt.y;
	points[2].x = ratio * gh + rt.x;
	points[2].y = rt.y + gh;
	mLinePainterPtr->setSourceColor(REVERSE_LINE_G_COLOR);
	mLinePainterPtr->drawLines(points, 3);

	points[0].x = points[2].x - REVERSE_LINE_CORNER_LEN;
	points[0].y = points[2].y;
	points[1].x = points[2].x;
	points[1].y = points[2].y;
	points[2].x = ratio * (gh + yh) + rt.x;
	points[2].y = rt.y + (gh + yh);
	mLinePainterPtr->setSourceColor(REVERSE_LINE_Y_COLOR);
	mLinePainterPtr->drawLines(points, 3);

	points[0].x = points[2].x - REVERSE_LINE_CORNER_LEN;
	points[0].y = points[2].y;
	points[1].x = points[2].x;
	points[1].y = points[2].y;
	points[2].x = rb.x;
	points[2].y = rb.y;
	mLinePainterPtr->setSourceColor(REVERSE_LINE_R_COLOR);
	mLinePainterPtr->drawLines(points, 3);
}

static void _load_reverse_line() {
	SZKPoint lt, rt, lb, rb;
	get_reverse_line_point(lt, rt, lb, rb);

	_move_arrow(mLeftArrowButtonPtr, (int) lt.x, (int) lt.y);
	_move_arrow(mRightArrowButtonPtr, (int) rt.x, (int) rt.y);
	_move_arrow(mBottomLeftArrowButtonPtr, (int) lb.x, (int) lb.y);
	_move_arrow(mBottomRightArrowButtonPtr, (int) rb.x, (int) rb.y);

	_draw_reverse_line();
}

static void _save_reverse_line() {
	SZKPoint lt, rt, lb, rb;
	_get_arrow_center_point(mLeftArrowButtonPtr, lt);
	_get_arrow_center_point(mRightArrowButtonPtr, rt);
	_get_arrow_center_point(mBottomLeftArrowButtonPtr, lb);
	_get_arrow_center_point(mBottomRightArrowButtonPtr, rb);
	set_reverse_line_point(lt, rt, lb, rb);
}

static void _proc_drag_arrow(const MotionEvent &ev) {
	if (ev.mActionStatus == MotionEvent::E_ACTION_DOWN) {
		ZKBase *arrows[] = {
			mLeftArrowButtonPtr,
			mRightArrowButtonPtr,
			mBottomLeftArrowButtonPtr,
			mBottomRightArrowButtonPtr
		};

		_s_hit_arrow = NULL;
		for (size_t i = 0; i < TAB_SIZE(arrows); ++i) {
			if (arrows[i]->getPosition().isHit(ev.mX, ev.mY)) {
				_s_hit_arrow = arrows[i];
				break;
			}
		}
		return;
	}

	if (!_s_hit_arrow) {
		return;
	}

	if ((_s_hit_arrow == mLeftArrowButtonPtr) || (_s_hit_arrow == mRightArrowButtonPtr)) {
		int y = std::min(ev.mY, mBottomLeftArrowButtonPtr->getPosition().mTop);
		_move_arrow(_s_hit_arrow, ev.mX, y);

		ZKBase *other_arrow = (_s_hit_arrow == mLeftArrowButtonPtr) ? mRightArrowButtonPtr : mLeftArrowButtonPtr;
		SZKPoint point;
		_get_arrow_center_point(other_arrow, point);
		_move_arrow(other_arrow, (int) point.x, y);
	} else {
		SZKPoint point;
		_get_arrow_center_point(_s_hit_arrow, point);
		_move_arrow(_s_hit_arrow, ev.mX, (int) point.y);
	}

	_draw_reverse_line();
}

static int _get_days() {
	int year, month;
	if (myearListViewPtr->getFirstVisibleItemIndex() == (YEAR_NUM - 1)) {
		year = START_YEAR;
	} else {
		year = myearListViewPtr->getFirstVisibleItemIndex() + START_YEAR + 1;
	}
	if (mmonthListViewPtr->getFirstVisibleItemIndex() == 11) {
		month = 1;
	} else {
		month = mmonthListViewPtr->getFirstVisibleItemIndex() + 2;
	}

    return fy::date::get_days(year, month);
}

static void _update_time_info() {
	struct tm *t = TimeHelper::getDateTime();
	int year = t->tm_year + 1900;
	int month = t->tm_mon + 1;
	int day = t->tm_mday;
	int hour = t->tm_hour;
	int minute = t->tm_min;

//	LOGD("%d-%d-%d %d:%d\n", year, month, day, hour, minute);

	myearListViewPtr->setSelection((year <= START_YEAR) ? (YEAR_NUM - 1) : (year - START_YEAR - 1));
	mmonthListViewPtr->setSelection((month == 1) ? 11 : (month - 2));
	mdayListViewPtr->setSelection((day == 1) ? (day - 2 + fy::date::get_days(year, month)) : (day - 2));
	mhourListViewPtr->setSelection((hour == 0) ? 23 : (hour - 1));
	mminListViewPtr->setSelection((minute == 0) ? 59 : (minute - 1));
	mformatListViewPtr->setSelection(sys::setting::is_time_format_24h() ? 1 : 0);

	_s_days = _get_days();
}

static void _update_date() {
	int year, month, day, hour, min;

	if (myearListViewPtr->getFirstVisibleItemIndex() == (YEAR_NUM - 1)) {
		year = START_YEAR;
	} else {
		year = myearListViewPtr->getFirstVisibleItemIndex() + START_YEAR + 1;
	}
	if (mmonthListViewPtr->getFirstVisibleItemIndex() == 11) {
		month = 1;
	} else {
		month = mmonthListViewPtr->getFirstVisibleItemIndex() + 2;
	}
	if (mdayListViewPtr->getFirstVisibleItemIndex() == (fy::date::get_days(year, month) - 1)) {
		day = 1;
	} else {
		day = mdayListViewPtr->getFirstVisibleItemIndex() + 2;
	}
	if (mhourListViewPtr->getFirstVisibleItemIndex() == 23) {
		hour = 0;
	} else {
		hour = mhourListViewPtr->getFirstVisibleItemIndex() + 1;
	}
	if (mminListViewPtr->getFirstVisibleItemIndex() == 59) {
		min = 0;
	} else {
		min = mminListViewPtr->getFirstVisibleItemIndex() + 1;
	}
	if (mformatListViewPtr->getFirstVisibleItemIndex() == 1) {
		sys::setting::set_time_format_24h(true);
	} else {
		sys::setting::set_time_format_24h(false);
	}

	struct tm *pt = TimeHelper::getDateTime();

	if ((pt->tm_year != year - 1900) ||
		(pt->tm_mon != month - 1) ||
		(pt->tm_mday != day) ||
		(pt->tm_hour != hour) ||
		(pt->tm_min != min)) {
		struct tm t;

		t.tm_year = year - 1900;
		t.tm_mon = month - 1;
		t.tm_mday = day;
		t.tm_hour = hour;
		t.tm_min = min;
		t.tm_sec = 1;

		LOGD("[time] set time year = %d, mon = %d, day = %d, hour = %d, min = %d", t.tm_year + 1900, \
				t.tm_mon, t.tm_mday, t.tm_hour, t.tm_min);
		set_date_time(&t);
		_update_time_info();
	}
}

static void _refresh_day_list() {
	int days = _get_days();
	if (_s_days != days) {
		int index = mdayListViewPtr->getFirstVisibleItemIndex();
		if ((index < 0) || (index + 1 > days)) {
			mdayListViewPtr->setSelection(0);
		} else {
			mdayListViewPtr->refreshListView();
		}
	}
}

static void update_eye_protection_list() {
	mButton1Ptr->setSelected(sys::setting::is_eye_protection_enable());
	char str[20];
	sprintf(str, "%d%%      >", sys::setting::get_protection_brightness());
	mbrightnessButtonPtr->setText(str);
	mstartTimeButtonPtr->setText(sys::setting::get_eye_protection_start_time() + "      >");
	mendTimeButtonPtr->setText(sys::setting::get_eye_protection_end_time() + "      >");
	sprintf(str, "%d%%", sys::setting::get_protection_brightness());
	mbrigpercentTextViewPtr->setText(str);
	int start_hour = 0;
	int start_min = 0;
	int end_hour = 0;
	int end_min = 0;
	sscanf(sys::setting::get_eye_protection_start_time().c_str(), "%d:%d", &start_hour, &start_min);
	sscanf(sys::setting::get_eye_protection_end_time().c_str(), "%d:%d", &end_hour, &end_min);
	LOGD("[settinglogic] start_time = %d:%d, end_time = %d:%d", start_hour, start_min, end_hour, end_min);
	mstartHourListViewPtr->setSelection((start_hour == 0) ? 23 : (start_hour - 1));
	mstartMinListViewPtr->setSelection((start_min == 0) ? 59 : (start_min - 1));
	mendHourListViewPtr->setSelection((end_hour == 0) ? 23 : (end_hour - 1));
	mendMinListViewPtr->setSelection((end_min == 0) ? 59 : (end_min - 1));
	mSeekBar1Ptr->setProgress(sys::setting::get_protection_brightness());
	mstartTimeWindowPtr->hideWnd();
	mendTimeWindowPtr->hideWnd();
	mbrightnessWindowPtr->hideWnd();
}

static bool _probe_link_connected() {
	LYLINK_TYPE_E link_type = lk::get_lylink_type();
	if ((link_type == LINK_TYPE_WIFICP) || (link_type == LINK_TYPE_WIFIAUTO)) {
		char tip[256];
		sprintf(tip, "%s %s,%s",
				LANGUAGEMANAGER->getValue("connected").c_str(),
				(link_type == LINK_TYPE_WIFICP) ? "CarPlay" : "Android Auto",
				LANGUAGEMANAGER->getValue("confirm_switch_link_screen_mode").c_str());
		mLinkSwitchTipTextViewPtr->setText(tip);
		mLinkTipWindowPtr->showWnd();
		return true;
	}

	return false;
}

static void _handle_cli_modelist(int sel) {
	mmodeTextViewPtr->setTextTr(setmodetext[sel]);
	uart::query_mcubt_version();
	switch (sel) {
	case MODE_REVERSE_LINE_CAL:
		_start_camera();
		_load_reverse_line();
		EASYUICONTEXT->hideNaviBar();
		mrevLineWindowPtr->showWnd();
		break;
//	case MODE_EYE_PROTECTION:
//		update_eye_protection_list();
//		meyeProtectionWindowPtr->showWnd();
//		break;
	case MODE_TIME_SETTING:
		_update_time_info();
		msettimeWindowPtr->showWnd();
		break;
	case MODE_CLEAR_CONNECT_RECORD:
		mClearTipWindowPtr->showWnd();
		break;
	case MODE_FORMAT_CARD:
		mFormatWndPtr->showWnd();
		break;
	case MODE_FACTORY_RESET:
		mResetWindowPtr->showWnd();
		break;
	case MODE_SOFTWARE_VERSION: {
		std::string txt;
		txt += "OS:  ";
		txt += sys::setting::get_os_version();
		txt += "\n\n";
		txt += "APP:  ";
		txt += sys::setting::get_app_softver();
//		txt += "\n\n";
//		txt += "MCU:  AB5602F";
		mversionTextPtr->setText(txt);
		mversionWndPtr->showWnd();
		break;
	}
	case MODE_MCU_UPGRADE:
		if (get_mcu_upgrade_state()) {  
			LOGD("[settings MCU] resuming upgrade, showing progress window");
			mUpgradeWindowPtr->showWnd();
			mprogressWndPtr->showWnd();
			mActivityPtr->unregisterUserTimer(MPI_RECORDER_STOP);
			break;
		}
		if (uart::query_update_file()) {
			_s_mcu_file = true;
			mTextView13Ptr->setTextTr("mcu_upgrade_yes");
		} else {
			_s_mcu_file = false;
			mTextView13Ptr->setTextTr("mcu_upgrade_no");
		}
		mUpgradeWindowPtr->showWnd();
		break;
//	case MODE_DEBUG:
//		mDebugWndPtr->showWnd();
//		break;
	default:
		_s_option_sel_index = sel;
		mmodeWindowPtr->showWnd();
		break;
	}

  moptionListViewPtr->setSelection(
      s_option_info[_s_option_sel_index].get_select_index());
	moptionListViewPtr->refreshListView();
}

static int _get_resolution_index() {
	return get_resolution_mode();
}

static void _set_resolution_index(int index) {
  resolution_mode_e mode = (resolution_mode_e) index;
  int w = 1280, h = 720;
  if (mode == E_RESOLUTION_MODE_1080P_1080P ) {
    w = 2560;
    h = 1440;
	LOGD("[settings] camera size: w = %d, h = %d", w, h);
  }
  else if (mode == E_RESOLUTION_MODE_2_5K_720P) {
    w = 3840;
    h = 2160;
	LOGD("[settings] camera size: w = %d, h = %d", w, h);
  }
  LOGD("[settings] camera size: w = %d, h = %d", w, h);
  set_camera_size(w, h);
  set_resolution_mode(mode);
  is_dvr_param_changed = true;
}

static int _get_record_index() {
	return get_dvr_record_time() - 1;
}

static void _set_record_index(int index) {
  set_dvr_record_time(index + 1);
  is_dvr_param_changed = true;
}

static int _get_record_sound_index() {
	return is_dvr_record_sound_enabled() ? 1 : 0;
}

static void _set_record_sound_index(int index) {
  set_dvr_record_sound_enable(index == 1);
  is_dvr_param_changed = true;
}

static int _get_assist_switch_index() {
	return is_assist_switch_enabled() ? 1 : 0;
}

static void _set_assist_switch_index(int index) {
	set_assist_switch_enable(index == 1);
}

static int _get_back_camera_mirror_index() {
//	return (get_camera_mirror() == 0) ? 0 : 1;
	return is_rear_mirror() ? 1 : 0;
}

static void _set_back_camera_mirror_index(int index) {
  set_rear_mirror(index);
//	set_camera_mirror((index == 0) ? E_MIRROR_NORMAL : E_MIRROR_LEFT_RIGHT);
}

static int _get_screensaver_index() {
	return get_screensaver_time();
}

static void _set_screensaver_index(int index) {
	set_screensaver_time(index);
    EASYUICONTEXT->setScreensaverEnable(index > 0);
    if (index > 0) {
      EASYUICONTEXT->setScreensaverTimeOut(index * 60);
    }
}

static int _get_driving_index() {
	return is_driving_left() ? 0 : 1;
}

static void _set_driving_index(int index) {
	bool set_left = (index == 0);
	if (is_driving_left() == set_left) {
		return;
	}
	if (_probe_link_connected()) {
		return;
	}

	set_driving_left(set_left);

	link_mode_e mode = get_link_mode();
	if ((mode == E_LINK_MODE_CARPLAY) || (mode == E_LINK_MODE_ANDROIDAUTO)) {
		mActivityPtr->registerUserTimer(LINK_RESTART_TIMER, 0); // @suppress("无效参数")
	}
}

static int _get_link_screen_index() {
	return is_link_screen_full() ? 0 : 1;
}

static void _set_link_screen_index(int index) {
	bool set_full = (index == 0);
	if (is_link_screen_full() == set_full) {
		return;
	}
	if (_probe_link_connected()) {
		return;
	}

	set_link_screen_full(set_full);

	link_mode_e mode = get_link_mode();
	if ((mode == E_LINK_MODE_CARPLAY) || (mode == E_LINK_MODE_ANDROIDAUTO)) {
		mActivityPtr->registerUserTimer(LINK_RESTART_TIMER, 0);
	}
}

static int _get_lang_index() {
	const std::string &code = LANGUAGEMANAGER->getCurrentCode();
	for (size_t i = 0; i < TAB_SIZE(lang_code); ++i) {
		if (code == lang_code[i]) {
			return i;
		}
	}
	return 0;
}

static void _set_lang_index(int index) {
	if ((index >= 0) && (index < (int) TAB_SIZE(lang_code))) {
		update_lang_code(lang_code[index]);
	}
}

static int _get_touch_sound_index() {
	return is_touch_sound_enabled() ? 1 : 0;
}

static void _set_touch_sound_index(int index) {
	set_touch_sound_enable(index == 1);
}

static void _event_mode_cb(event_mode_e mode) {
	if (E_EVENT_MODE_GOBACK == mode) {
		EASYUICONTEXT->goBack();
	}
}

class MoreLongClickListener : public ZKBase::ILongClickListener {
protected:
	virtual void onLongClick(ZKBase *pBase) {
		mversionWndPtr->hideWnd();
		mMoreWindowPtr->showWnd();
	}
};

bool handleMotionEvent(const MotionEvent &ev) {
  if (!mrevLineWindowPtr->isVisible()) {
    return false;
  }
  if (msaveCalButtonPtr->getPosition().isHit(ev.mX, ev.mY)) {
    return false;
  }
  if (munCalButtonPtr->getPosition().isHit(ev.mX, ev.mY)) {
    return false;
  }
  if (mLeftArrowButtonPtr->getPosition().isHit(ev.mX, ev.mY)) {
    return false;
  }
  if (mRightArrowButtonPtr->getPosition().isHit(ev.mX, ev.mY)) {
    return false;
  }
  static MotionEvent last;
  switch (ev.mActionStatus) {
    case MotionEvent::E_ACTION_DOWN:
      last = ev;
      break;
    case MotionEvent::E_ACTION_MOVE:
      reverse_viewbox = mpi::RearCamera::instance().moveViewbox(
          -(ev.mX - last.mX), -(ev.mY - last.mY));
      last = ev;
      base::runInUiThreadUniqueDelayed("set_reverse_viewbox", [](){
        sys::setting::set_reverse_viewbox_y(reverse_viewbox.y);
      }, 1000);
      break;
    case MotionEvent::E_ACTION_UP:
      break;
    default:
      break;
  }
  return false;
}

void updateHornIcon(){
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
	mHornButtonPtr->setButtonStatusPic(ZK_CONTROL_STATUS_NORMAL, normal_icon[mode::get_sound_mode()]);
	mHornButtonPtr->setButtonStatusPic(ZK_CONTROL_STATUS_PRESSED, pressed_icon[mode::get_sound_mode()]);
}

} /* anonymous namespace */

static MoreLongClickListener _s_more_long_click_listener;

static void _mcu_cb(MCU_EVENT event, void* data, uint32_t len) {
	std::string txt;
	switch (event) {
	case MCU_UPD_MODE_ENTRY:
		mmcuUpdSeekBarPtr->setMax(0);
		mmcuUpdSeekBarPtr->setProgress(0);
		mTextView13Ptr->setVisible(false);
		set_mcu_upgrade_state(true); 
		mActivityPtr->unregisterUserTimer(MPI_RECORDER_STOP);
		break;
	case MCU_UPD_PRGGRESS:{
		mmcuSurePtr->setInvalid(true);
		mmcuCancelPtr->setInvalid(true);
		UpdProgress* updp =  (UpdProgress*)data;
		mmcuUpdSeekBarPtr->setMax(updp->total);
		if (updp->cur_pos > mmcuUpdSeekBarPtr->getProgress()) {
			mmcuUpdSeekBarPtr->setProgress(updp->cur_pos);
		}
		set_mcu_upgrade_state(true); 
		mActivityPtr->unregisterUserTimer(MPI_RECORDER_STOP);
	}
		break;
	case MCU_UPD_FINISHED:
	    mmcuSurePtr->setInvalid(false);
	    mmcuCancelPtr->setInvalid(false);
	    mmcuUpdSeekBarPtr->setProgress(mmcuUpdSeekBarPtr->getMax());
	    mprogressWndPtr->hideWnd();
	    mTextView13Ptr->setTextTr("mcu_upgrade_done");  // 这里设置文字
	    mTextView13Ptr->setVisible(true);
	    set_mcu_upgrade_state(false);
	    _s_mcu_succes = true;
	    mActivityPtr->registerUserTimer(MPI_RECORDER_STOP, 15 * 1000);
	    break;
	case MCU_VERSION:
		txt = std::string((char*)data, len);
		std::string mcu = mversionTextPtr->getText();
		mversionTextPtr->setText(mcu + "\n\n" + "MCU:  " + txt);
		break;
	}
}

/**
 * 注册定时器
 * 填充数组用于注册定时器
 * 注意：id不能重复
 */
static S_ACTIVITY_TIMEER REGISTER_ACTIVITY_TIMER_TAB[] = {
	//{1,  1000},
};

/**
 * 当界面构造时触发
 */
static void onUI_init() {
	uart::add_mcubt_cb(_mcu_cb);
	uart::query_mcubt_version();
	is_dvr_param_changed = false;

    reverse_viewbox.y = sys::setting::get_reverse_viewbox_y();

    mversionWndPtr->setVisible(false);
    mmodeListViewPtr->setVisible(true);
	mode::add_event_mode_cb(_event_mode_cb);
	mLinePainterPtr->setLineWidth(REVERSE_LINE_WIDTH);
	mMoreButtonPtr->setLongClickListener(&_s_more_long_click_listener);
	_s_option_sel_index = 0;
	mMoreWindowPtr->setVisible(false);
	mrevLineWindowPtr->setVisible(false);
	mResetWindowPtr->setVisible(false);
	mmodeWindowPtr->setVisible(false);
	mOtpWindowPtr->setVisible(false);

	mActivityPtr->registerUserTimer(MPI_RECORDER_STOP, 15 * 1000);
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
    if (!EASYUICONTEXT->isNaviBarShow()) {
        EASYUICONTEXT->showNaviBar();
    }
	// 进入设置界面停止预览停止录像，避归倒车辅助线设置卡顿问题
//	mpi::RearCamera::instance().stop();
//	mpi::FrontCamera::instance().stop();
    NO_EXCEPTION(mpi::Recorder::instance().stop());

    mSettingButtonPtr->setSelected(true);
    updateHornIcon();
    if (!get_mcu_upgrade_state()) {
        _s_mcu_succes = false;
    } else {
// 		后台正在升级,恢复升级窗口
        LOGD("[settings MCU] resuming upgrade UI");
        mUpgradeWindowPtr->showWnd();
        if (!_s_mcu_succes) {  
            mprogressWndPtr->showWnd();
            mActivityPtr->unregisterUserTimer(MPI_RECORDER_STOP);
        }
    }
    mTextView1Ptr->setLongMode(ZKTextView::E_LONG_MODE_SCROLL_CIRCULAR);
    mTextView9Ptr->setLongMode(ZKTextView::E_LONG_MODE_SCROLL_CIRCULAR);
    LOGD("[settings MCU] upgrade_state=%d, success=%d", 
         get_mcu_upgrade_state(), _s_mcu_succes);
}

/*
 * 当界面隐藏时触发
 */
static void onUI_hide() {
	mSettingButtonPtr->setSelected(false);
	_stop_camera();
	mrevLineWindowPtr->hideWnd();
// 隐藏设置界面打开录像
    NO_EXCEPTION(restartRecorder(true));

	mActivityPtr->unregisterUserTimer(MPI_RECORDER_STOP);
}

/*
 * 当界面完全退出时触发
 */
static void onUI_quit() {
	uart::remove_mcubt_cb(_mcu_cb);
	mSettingButtonPtr->setSelected(false);
  if (set_language_thread.joinable()) {
    set_language_thread.join();
  }
  base::eraseTask(TASK_HIDE_OPTION_LIST);
  _stop_camera();
	mode::remove_event_mode_cb(_event_mode_cb);
  if (is_dvr_param_changed) {
    NO_EXCEPTION(restartRecorder(true));
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
static bool onUI_Timer(int id) {
	switch (id) {
	case LINK_RESTART_TIMER:
		lk::restart_lylink();
		return false;
	  break;
	case MPI_RECORDER_STOP:
		OPEN_APP("main");
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
static bool onsettingsActivityTouchEvent(const MotionEvent &ev) {
	_proc_drag_arrow(ev);
	handleMotionEvent(ev);

	switch (ev.mActionStatus) {
	case MotionEvent::E_ACTION_DOWN:  //触摸按下
		//LOGD("时刻 = %ld 坐标  x = %d, y = %d", ev.mEventTime, ev.mX, ev.mY);
		mActivityPtr->resetUserTimer(MPI_RECORDER_STOP, 15*1000);
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

static int getListItemCount_modeListView(const ZKListView *pListView) {
    //LOGD("getListItemCount_modeListView !\n");
    return TAB_SIZE(modetext);
}

static void obtainListItemData_modeListView(ZKListView *pListView,ZKListView::ZKListItem *pListItem, int index) {
    //LOGD(" obtainListItemData_ modeListView  !!!\n");
	ZKListView::ZKListSubItem* pic = pListItem->findSubItemByID(ID_SETTINGS_PicSubItem);
	ZKListView::ZKListSubItem* text = pListItem->findSubItemByID(ID_SETTINGS_textSubItem);

	const char *pictext[] = {
		"0","1","2","3","4","5","6","7","8","9","a","b","c","d","e","f",//"g",
	};

	pic->setText(pictext[index]);
	text->setTextTr(modetext[index]);
//	mmodeListViewPtr->refreshListView();
//	text->setLongMode(ZKTextView::E_LONG_MODE_SCROLL_CIRCULAR);
}

static void onListItemClick_modeListView(ZKListView *pListView, int index, int id) {
    //LOGD(" onListItemClick_ modeListView  !!!\n");
	_handle_cli_modelist(index);
}

static int getListItemCount_optionListView(const ZKListView *pListView) {
    //LOGD("getListItemCount_optionListView !\n");
	if (_s_option_sel_index == MODE_RESOLUTION_RATIO && !sys::setting::is_resolution_show_4K()) {
		return s_option_info[_s_option_sel_index].listnum - 1;
	}
    return s_option_info[_s_option_sel_index].listnum;
}

static void obtainListItemData_optionListView(ZKListView *pListView,ZKListView::ZKListItem *pListItem, int index) {
    //LOGD(" obtainListItemData_ optionListView  !!!\n");
	ZKListView::ZKListSubItem* subItemText = pListItem->findSubItemByID(ID_SETTINGS_TextSubItem);
	ZKListView::ZKListSubItem* subItemSelect = pListItem->findSubItemByID(ID_SETTINGS_selectedSubItem);
	if (_s_option_sel_index == MODE_LANG_SETTING) {    // language
		subItemText->setText(s_option_info[_s_option_sel_index].text[index]);
	} else {
		subItemText->setTextTr(s_option_info[_s_option_sel_index].text[index]);
	}

	subItemSelect->setSelected((index == s_option_info[_s_option_sel_index].get_select_index()) ? true : false);
}

static void onListItemClick_optionListView(ZKListView *pListView, int index, int id) {
    //LOGD(" onListItemClick_ optionListView  !!!\n");
	s_option_info[_s_option_sel_index].set_select_index(index);
    base::runInUiThreadUniqueDelayed(TASK_HIDE_OPTION_LIST, [](){
      mmodeWindowPtr->hideWnd();
    }, 100);
}

static bool onButtonClick_resetSure(ZKButton *pButton) {
  PopupService::show([](PopupService* srv) {
    srv->setMessage("@wait a moment");
    NO_EXCEPTION(mpi::Recorder::instance().stop());
    bt::del_match_alldevice();
    bt::disconnect();
    audio::change_output_mode(E_AUDIO_PLAYER_MODE_SPK);
    system("rm -rf /data");
    Thread::sleep(1000);
    sys::setting::backlight_off();
    net::ap::restart_hostapd();
    storage::clear();
    system("reboot");
    return 0;
  }, PopupMode::Longer);
  mResetWindowPtr->hideWnd();
  return false;
}

static bool onButtonClick_resetCancel(ZKButton *pButton) {
  LOGD(" ButtonClick resetCancel !!!\n");
  mResetWindowPtr->hideWnd();
  return false;
}

static int getListItemCount_minListView(const ZKListView *pListView) {
    //LOGD("getListItemCount_minListView !\n");
	return 60;
}

static void obtainListItemData_minListView(ZKListView *pListView,ZKListView::ZKListItem *pListItem, int index) {
    //LOGD(" obtainListItemData_ minListView  !!!\n");
	char str[3];
	sprintf(str, "%02d", index);
	pListItem->setText(str);
	pListItem->setTextColor(index == (pListView->getFirstVisibleItemIndex()+1)%pListView->getListItemCount() ? 0x00DDFF : 0xC0C0C0);
}

static void onListItemClick_minListView(ZKListView *pListView, int index, int id) {
    //LOGD(" onListItemClick_ minListView  !!!\n");
	pListView->setSelection(((0 == index) ? pListView->getListItemCount() : index)-1);
}

static int getListItemCount_monthListView(const ZKListView *pListView) {
    //LOGD("getListItemCount_monthListView !\n");
	return 12;
}

static void obtainListItemData_monthListView(ZKListView *pListView,ZKListView::ZKListItem *pListItem, int index) {
    //LOGD(" obtainListItemData_ monthListView  !!!\n");
	char str[3];
	sprintf(str, "%02d", index + 1);
	pListItem->setText(str);
	pListItem->setTextColor(index == (pListView->getFirstVisibleItemIndex()+1)%pListView->getListItemCount() ? 0x00DDFF : 0xC0C0C0);
	_refresh_day_list();

}

static void onListItemClick_monthListView(ZKListView *pListView, int index, int id) {
    //LOGD(" onListItemClick_ monthListView  !!!\n");
	pListView->setSelection(((0 == index) ? pListView->getListItemCount() : index) - 1);
}

static int getListItemCount_dayListView(const ZKListView *pListView) {
    //LOGD("getListItemCount_dayListView !\n");
	_s_days = _get_days();
	return _s_days;
}

static void obtainListItemData_dayListView(ZKListView *pListView,ZKListView::ZKListItem *pListItem, int index) {
    //LOGD(" obtainListItemData_ dayListView  !!!\n");
	char str[3];
	sprintf(str, "%02d", index + 1);
	pListItem->setText(str);
	pListItem->setTextColor(index == (pListView->getFirstVisibleItemIndex()+1)%pListView->getListItemCount() ? 0x00DDFF : 0xC0C0C0);
}

static void onListItemClick_dayListView(ZKListView *pListView, int index, int id) {
    //LOGD(" onListItemClick_ dayListView  !!!\n");
	pListView->setSelection(((0 == index) ? pListView->getListItemCount() : index) - 1);
}

static int getListItemCount_hourListView(const ZKListView *pListView) {
    //LOGD("getListItemCount_hourListView !\n");
	return 24;
}

static void obtainListItemData_hourListView(ZKListView *pListView,ZKListView::ZKListItem *pListItem, int index) {
    //LOGD(" obtainListItemData_ hourListView  !!!\n");
	char str[3];
	sprintf(str, "%02d", index);
	pListItem->setText(str);
	pListItem->setTextColor(index == (pListView->getFirstVisibleItemIndex()+1)%pListView->getListItemCount() ? 0x00DDFF : 0xC0C0C0);
}

static void onListItemClick_hourListView(ZKListView *pListView, int index, int id) {
    //LOGD(" onListItemClick_ hourListView  !!!\n");
	pListView->setSelection(((0 == index) ? pListView->getListItemCount() : index) - 1);
}

static int getListItemCount_yearListView(const ZKListView *pListView) {
    //LOGD("getListItemCount_yearListView !\n");
	return YEAR_NUM;
}

static void obtainListItemData_yearListView(ZKListView *pListView,ZKListView::ZKListItem *pListItem, int index) {
    //LOGD(" obtainListItemData_ yearListView  !!!\n");
	pListItem->setText(index + START_YEAR);
	pListItem->setTextColor(index == (pListView->getFirstVisibleItemIndex()+1)%pListView->getListItemCount() ? 0x00DDFF : 0xC0C0C0);
	_refresh_day_list();
}

static void onListItemClick_yearListView(ZKListView *pListView, int index, int id) {
    //LOGD(" onListItemClick_ yearListView  !!!\n");
	pListView->setSelection(((0 == index) ? pListView->getListItemCount() : index) - 1);
}

static bool onButtonClick_settimesure(ZKButton *pButton) {
    LOGD(" ButtonClick settimesure !!!\n");
    _update_date();
    msettimeWindowPtr->hideWnd();
    return false;
}

static bool onButtonClick_settimecancel(ZKButton *pButton) {
    LOGD(" ButtonClick settimecancel !!!\n");
    msettimeWindowPtr->hideWnd();
    return false;
}

static bool onButtonClick_FormatSureBtn(ZKButton *pButton) {
  formatTfcardProcess(false);
  mFormatWndPtr->hideWnd();
  return false;
}

static bool onButtonClick_FormatCancelBtn(ZKButton *pButton) {
    LOGD(" ButtonClick FormatCancelBtn !!!\n");
    mFormatWndPtr->hideWnd();
    return false;
}

static bool onButtonClick_tipSure(ZKButton *pButton) {
    LOGD(" ButtonClick tipSure !!!\n");
    if (_s_option_sel_index == MODE_DRIVING_POSITION) {
		set_driving_left(!is_driving_left());
    }
//    if (_s_option_sel_index == MODE_SPLIT_SCREEN) {
//		set_link_screen_full(!is_link_screen_full());
//    }
    mLinkTipWindowPtr->hideWnd();
    mActivityPtr->registerUserTimer(LINK_RESTART_TIMER, 0);
    return false;
}

static bool onButtonClick_tipCancel(ZKButton *pButton) {
    LOGD(" ButtonClick tipCancel !!!\n");
    mLinkTipWindowPtr->hideWnd();
    return false;
}

static bool onButtonClick_MoreButton(ZKButton *pButton) {
    LOGD(" ButtonClick MoreButton !!!\n");
    //快速点击版本号10次，进入调试模式
    static base::timer timer;
    static int counter = 0;
    if (timer.elapsed() > 2000) {
      counter = 0;
      timer.reset();
      return false;
    }
    ++counter;
    timer.reset();
    if (counter > 10) {
      mversionWndPtr->setVisible(false);
      mMoreWindowPtr->setVisible(true);
    }
    return false;
}

static bool onButtonClick_AdbButton(ZKButton *pButton) {
    LOGD(" ButtonClick AdbButton !!!\n");
    enable_usb_adb(true);
    return false;
}

static bool onButtonClick_unCalButton(ZKButton *pButton) {
    LOGD(" ButtonClick unCalButton !!!\n");
    _stop_camera();
    EASYUICONTEXT->showNaviBar();
    mrevLineWindowPtr->hideWnd();
    return false;
}

static bool onButtonClick_saveCalButton(ZKButton *pButton) {
    LOGD(" ButtonClick saveCalButton !!!\n");
    _stop_camera();
    _save_reverse_line();
    EASYUICONTEXT->showNaviBar();
    mrevLineWindowPtr->hideWnd();
    return false;
}

static bool onButtonClick_BottomRightArrowButton(ZKButton *pButton) {
    LOGD(" ButtonClick BottomRightArrowButton !!!\n");
    return false;
}

static bool onButtonClick_BottomLeftArrowButton(ZKButton *pButton) {
    LOGD(" ButtonClick BottomLeftArrowButton !!!\n");
    return false;
}

static bool onButtonClick_RightArrowButton(ZKButton *pButton) {
    LOGD(" ButtonClick RightArrowButton !!!\n");
    return false;
}

static bool onButtonClick_LeftArrowButton(ZKButton *pButton) {
    LOGD(" ButtonClick LeftArrowButton !!!\n");
    return false;
}

static bool onButtonClick_ButtonNet(ZKButton *pButton) {
  EASYUICONTEXT->openActivity("wifisettingActivity");
//    LOGD(" ButtonClick debugButton !!!\n");
//    if (pButton->isSelected()) {
//    	hideDebug();
//    } else {
//    	showDebug();
//    }
//	pButton->setSelected(!pButton->isSelected());
//    return false;
  return false;
}

static bool onButtonClick_clearSure(ZKButton *pButton) {
    LOGD(" ButtonClick clearSure !!!\n");
	bt::del_match_alldevice();
    bt::disconnect();

	LYLINK_TYPE_E link_type = lk::get_lylink_type();
	if (link_type == LINK_TYPE_NULL) {
		// do nothing
	} else if ((link_type == LINK_TYPE_WIFICP) || (link_type == LINK_TYPE_WIFIAUTO)) {
		LOGD("wifi kill hostapd\n");
		fy::kill_process(NET_AP_SUPP);
	} else {
		LOGD("will restart link service\n");
		RESTART_SERVICE(LINK_SERVICE);
	}

	mWaitMomentWindowPtr->showWnd();
    mClearTipWindowPtr->hideWnd();
    return false;
}

static bool onButtonClick_clearCancel(ZKButton *pButton) {
    LOGD(" ButtonClick clearCancel !!!\n");
    mClearTipWindowPtr->hideWnd();
    return false;
}

static int getListItemCount_formatListView(const ZKListView *pListView) {
    //LOGD("getListItemCount_formatListView !\n");
    return 4;
}

static void obtainListItemData_formatListView(ZKListView *pListView,ZKListView::ZKListItem *pListItem, int index) {
    //LOGD(" obtainListItemData_ formatListView  !!!\n");
	switch (index) {
	case 1:
		pListItem->setText("12H");
		break;
	case 2:
		pListItem->setText("24H");
		break;
	default:
		pListItem->setText("");
		break;
	}
	pListItem->setTextColor(index == (pListView->getFirstVisibleItemIndex()+1)%pListView->getListItemCount() ? 0x00DDFF : 0xC0C0C0);
}

static void onListItemClick_formatListView(ZKListView *pListView, int index, int id) {
    //LOGD(" onListItemClick_ formatListView  !!!\n");
//	pListView->setSelected(((1 == index) ? index : 0));
}
static bool onButtonClick_brightnessButton(ZKButton *pButton) {
    LOGD(" ButtonClick brightnessButton !!!\n");
    mbrightnessWindowPtr->showWnd();
    return false;
}

static bool onButtonClick_startTimeButton(ZKButton *pButton) {
    LOGD(" ButtonClick startTimeButton !!!\n");
    mstartTimeWindowPtr->showWnd();
    return false;
}

static bool onButtonClick_endTimeButton(ZKButton *pButton) {
    LOGD(" ButtonClick endTimeButton !!!\n");
    mendTimeWindowPtr->showWnd();
    return false;
}

static bool onButtonClick_Button1(ZKButton *pButton) {
    LOGD(" ButtonClick Button1 !!!\n");
    if (sys::setting::is_eye_protection_enable()) {
    	sys::setting::set_eye_protection_enable(false);
    }
    else {
    	sys::setting::set_eye_protection_enable(true);
    }
    sys::setting::update_eye_protection_info();
    update_eye_protection_list();
    return false;
}

static void onProgressChanged_SeekBar1(ZKSeekBar *pSeekBar, int progress) {
    //LOGD(" ProgressChanged SeekBar1 %d !!!\n", progress);
	char str[20];
	sprintf(str, "%d%%", progress);
	mbrigpercentTextViewPtr->setText(str);

}

static bool onButtonClick_brigCancelButton(ZKButton *pButton) {
    LOGD(" ButtonClick brigCancelButton !!!\n");
    mbrightnessWindowPtr->hideWnd();
    return false;
}

static bool onButtonClick_brigSureButton(ZKButton *pButton) {
    LOGD(" ButtonClick brigSureButton !!!\n");
    sys::setting::set_protection_brightness(mSeekBar1Ptr->getProgress());
    if (sys::setting::get_is_eye_protection_now()) {
    	sys::setting::set_brightness(sys::setting::get_protection_brightness() * BRIGHTNESS_MAX_VAL / 100);
    }
    sys::setting::update_eye_protection_info();
    update_eye_protection_list();
    mbrightnessWindowPtr->hideWnd();
    return false;
}

static int getListItemCount_startHourListView(const ZKListView *pListView) {
    //LOGD("getListItemCount_startHourListView !\n");
    return 24;
}

static void obtainListItemData_startHourListView(ZKListView *pListView,ZKListView::ZKListItem *pListItem, int index) {
    //LOGD(" obtainListItemData_ startHourListView  !!!\n");
	char str[3];
	sprintf(str, "%02d", index);
	pListItem->setText(str);
}

static void onListItemClick_startHourListView(ZKListView *pListView, int index, int id) {
    //LOGD(" onListItemClick_ startHourListView  !!!\n");
	pListView->setSelection(((0 == index) ? pListView->getListItemCount() : index) - 1);
}

static int getListItemCount_startMinListView(const ZKListView *pListView) {
    //LOGD("getListItemCount_startMinListView !\n");
    return 60;
}

static void obtainListItemData_startMinListView(ZKListView *pListView,ZKListView::ZKListItem *pListItem, int index) {
    //LOGD(" obtainListItemData_ startMinListView  !!!\n");
	char str[3];
	sprintf(str, "%02d", index);
	pListItem->setText(str);
}

static void onListItemClick_startMinListView(ZKListView *pListView, int index, int id) {
    //LOGD(" onListItemClick_ startMinListView  !!!\n");
	pListView->setSelection(((0 == index) ? pListView->getListItemCount() : index) - 1);
}

static bool onButtonClick_startCancelButton(ZKButton *pButton) {
    LOGD(" ButtonClick startCancelButton !!!\n");
    mstartTimeWindowPtr->hideWnd();
    return false;
}

static bool onButtonClick_startSureButton(ZKButton *pButton) {
    LOGD(" ButtonClick startSureButton !!!\n");
    char str[20];
    int hour = (mstartHourListViewPtr->getFirstVisibleItemIndex() == 23) ? 0 : mstartHourListViewPtr->getFirstVisibleItemIndex() + 1;
    int min = (mstartMinListViewPtr->getFirstVisibleItemIndex() == 59) ? 0 : mstartMinListViewPtr->getFirstVisibleItemIndex() + 1;
    sprintf(str, "%02d:%02d", hour, min);
    sys::setting::set_eye_protection_start_time(str);
    sys::setting::update_eye_protection_info();
	update_eye_protection_list();
    mstartTimeWindowPtr->hideWnd();
    return false;
}

static int getListItemCount_endHourListView(const ZKListView *pListView) {
    //LOGD("getListItemCount_endHourListView !\n");
    return 24;
}

static void obtainListItemData_endHourListView(ZKListView *pListView,ZKListView::ZKListItem *pListItem, int index) {
    //LOGD(" obtainListItemData_ endHourListView  !!!\n");
	char str[3];
	sprintf(str, "%02d", index);
	pListItem->setText(str);
}

static void onListItemClick_endHourListView(ZKListView *pListView, int index, int id) {
    //LOGD(" onListItemClick_ endHourListView  !!!\n");
	pListView->setSelection(((0 == index) ? pListView->getListItemCount() : index) - 1);
}

static int getListItemCount_endMinListView(const ZKListView *pListView) {
    //LOGD("getListItemCount_endMinListView !\n");
    return 60;
}

static void obtainListItemData_endMinListView(ZKListView *pListView,ZKListView::ZKListItem *pListItem, int index) {
    //LOGD(" obtainListItemData_ endMinListView  !!!\n");
	char str[3];
	sprintf(str, "%02d", index);
	pListItem->setText(str);
}

static void onListItemClick_endMinListView(ZKListView *pListView, int index, int id) {
    //LOGD(" onListItemClick_ endMinListView  !!!\n");
	pListView->setSelection(((0 == index) ? pListView->getListItemCount() : index) - 1);
}

static bool onButtonClick_endCancelButton(ZKButton *pButton) {
    LOGD(" ButtonClick endCancelButton !!!\n");
    mendTimeWindowPtr->hideWnd();
    return false;
}

static bool onButtonClick_endSureButton(ZKButton *pButton) {
    LOGD(" ButtonClick endSureButton !!!\n");
    char str[20];
	int hour = (mendHourListViewPtr->getFirstVisibleItemIndex() == 23) ? 0 : mendHourListViewPtr->getFirstVisibleItemIndex() + 1;
	int min = (mendMinListViewPtr->getFirstVisibleItemIndex() == 59) ? 0 : mendMinListViewPtr->getFirstVisibleItemIndex() + 1;
	sprintf(str, "%02d:%02d", hour, min);
	sys::setting::set_eye_protection_end_time(str);
    sys::setting::update_eye_protection_info();
	update_eye_protection_list();
	mendTimeWindowPtr->hideWnd();
    return false;
}

const int LEN = 255;
const int OFFSET = 0;		//lyink (0 + 32), blink(32 + 220)
static void read_otp_len(int offset, int len) {
	uint8_t otp_data[len];
	if (!storage::otp_read_data(offset, otp_data, len)) {
		LOGE("[bt] otp read data err !!!\n");
		return ;
	}

	std::string otpstr;
	char buf[64];
	for (int i=0; i<len; i++) {
		if (i == 32) {
			snprintf(buf, TABLESIZE(buf), "\n%x ", otp_data[i]);
		} else {
			snprintf(buf, TABLESIZE(buf), "%x ", otp_data[i]);
		}
		otpstr += buf;
	}

	mTextView11Ptr->setText(otpstr);
}

static bool onButtonClick_OtpButton(ZKButton *pButton) {
    LOGD(" ButtonClick OtpButton !!!\n");
    read_otp_len(OFFSET, LEN);
    mMoreWindowPtr->setVisible(false);
    mOtpWindowPtr->showWnd();
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

static bool onButtonClick_LinkButton(ZKButton *pButton) {
	LOGD(" ButtonClick LinkButton !!!\n");
	if (lk::is_connected()) {
		OPEN_APP("lylinkview");
	} else {
		OPEN_APP("Link");
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

static bool onButtonClick_SettingButton(ZKButton *pButton) {
    LOGD(" ButtonClick SettingButton !!!\n");
    //OPEN_APP("settings");
    return false;
}

static bool onButtonClick_PowerButton(ZKButton *pButton) {
    LOGD(" ButtonClick PowerButton !!!\n");
    EASYUICONTEXT->screensaverOn();
    return false;
}

static bool onButtonClick_mcuSure(ZKButton *pButton) {
    LOGD(" ButtonClick mcuSure !!!\n");
    if (_s_mcu_file) {
        if (_s_mcu_succes) {
            mUpgradeWindowPtr->hideWnd();
            _s_mcu_succes = false;
            return false;
        }

        // 立即显示遮挡层，防止用户快速点击返回
        NavibarSetMcuUpgradeMask(true);

        // 禁用按钮，防止重复点击
        mmcuSurePtr->setInvalid(true);
        mmcuCancelPtr->setInvalid(true);

        if (uart::entry_update_mode()) {
            DELAY(100);
            uart::start_mcu_update();
            mmcuUpdSeekBarPtr->setMax(0);
            mmcuUpdSeekBarPtr->setProgress(0);
            mTextView13Ptr->setVisible(false);
            set_mcu_upgrade_state(true);
            mActivityPtr->unregisterUserTimer(MPI_RECORDER_STOP);

            // 0.9秒后隐藏遮挡层（此时MCU应该已经识别到升级命令）
            base::runInUiThreadDelayed("hide_mcu_block", [](){
            	NavibarSetMcuUpgradeMask(false);
            }, 900);

            mprogressWndPtr->showWnd();
        } else {
        	NavibarSetMcuUpgradeMask(false);
            mmcuSurePtr->setInvalid(false);
            mmcuCancelPtr->setInvalid(false);
        }
    } else {
        mUpgradeWindowPtr->hideWnd();
        _s_mcu_succes = false;
    }
    return false;
}


static bool onButtonClick_mcuCancel(ZKButton *pButton) {
    LOGD(" ButtonClick mcuCancel !!!\n");
    // 如果在升级过程中取消，也清除状态
    if (get_mcu_upgrade_state() && !_s_mcu_succes) {
        set_mcu_upgrade_state(false);
    }
    mUpgradeWindowPtr->hideWnd();
    _s_mcu_succes = false;
    return false;
}
static void onProgressChanged_mcuUpdSeekBar(ZKSeekBar *pSeekBar, int progress) {
    //LOGD(" ProgressChanged mcuUpdSeekBar %d !!!\n", progress);
}
static bool onButtonClick_debugButton(ZKButton *pButton) {
    LOGD(" ButtonClick debugButton !!!\n");
    if (pButton->isSelected()) {
    	hideDebug();
    } else {
    	showDebug();
    }
	pButton->setSelected(!pButton->isSelected());
    return false;
}
