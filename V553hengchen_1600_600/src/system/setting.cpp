/*
 * setting.cpp
 *
 *  Created on: 2023年4月17日
 *      Author: ZKSWE Develop Team
 */

#include "link/context.h"
#include "misc/storage.h"
#include "setting.h"
#include "usb_monitor.h"
#include "fy/os.hpp"
#include "fy/files.hpp"
#include "entry/EasyUIContext.h"
#include "utils/TimeHelper.h"
#include "utils/BrightnessHelper.h"
#include "manager/LanguageManager.h"
#include "storage/StoragePreferences.h"
#include "control/ZKCameraView.h"
#include "misc/zkmisc.h"
#include <base/strings.hpp>
#include <mpi/case/shared_video_device.h>
#include <mpi/case/camera.h>
#include "ini.h"
#include "common.h"
#include "reverse.h"
#include "stdio.h"
#include <base/time.hpp>
#include "version.h"
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <mpi/case/text_watermark.h>

#define CAMERA_PROP_DEV            "persist.zkcamera.dev"
#define CAMERA_PROP_SIZE           "persist.zkcamera.size"
#define CAMERA_PROP_RATE           "persist.zkcamera.rate"
#define CAMERA_PROP_ROT            "persist.zkcamera.rot"
#define CAMERA_PROP_MIRROR         "persist.zkcamera.mirror"
#define CAMERA_PROP_CHN            "persist.zkcamera.chn"
#define CAMERA_PROP_SKIP           "persist.zkcamera.skip"
#define CAMERA_PROP_ENABLE         "persist.zkcamera.enable"

#define D_CAMERA_DEV               "/dev/video4"
#define D_CAMERA_SIZE_WIDTH        1920
#define D_CAMERA_SIZE_HEIGHT       1080
#define D_CAMERA_RATE              25
#define D_CAMERA_CHN               0
#define D_CAMERA_SKIP              14
#define D_CAMERA_MIRROR            E_MIRROR_NORMAL
#define D_CAMERA_ROT               E_ROTATION_270
#define D_CAMERA_ENABLE            0

#define BT_NAME                    "bt_name"
#define USB_MODE                   "usb_mode"
#define LINK_MODE                  "link_mode"
#define SOUND_MODE                 "sound_mode"
#define DRIVING_MODE               "driving_mode"
#define RESOLUTION                 "resolution"
#define DVR_RECORD_TIME            "dvr_record_time"
#define DVR_RECORD_SOUND           "dvr_record_sound"
#define SCREENSAVER_TIME           "screensaver_time"
#define LINK_SCREEN_MODE           "link_screen_mode"
#define TOUCH_SOUND                "touch_sound"
#define TIME_FORMAT_24H            "time_format_24h"
#define ASSIST_SWITCH              "assist_switch"
#define FLOAT_BUTTON_POS           "float_button_pos"
#define LINK_SOUND                 "link_sound"
#define AIRPLAY_NAME               "airplay_name"
#define MIRACAST_NAME              "miracast_name"
#define AICAST_NAME				   "aicast_name"
#define DEV_NAME				   "dev_name"
#define CARPLAY_AP_NAME            "carplay_ap_name"
#define AIRPLAY_AP_NAME            "airplay_ap_name"
#define WIFIVIDEO_AP_NAME          "wifivideo_ap_name"
#define REVERSE_LINE_POINT         "reverse_line_point"
#define REVERSE_VIEWBOX_Y            "reverse_viewbox_y"
#define DVR_VIEWBOX_FRONT_Y            "dvr_viewbox_front_y"
#define DVR_VIEWBOX_REAR_Y            "dvr_viewbox_rear_y"
#define LYLINK_AUDIO_MUTE			"lylink_audio_mute"

#define DVR_VIEW_TYPE              "dvr_view_type"
#define HALF_VIEW_TYPE              "half_view_type"
#define REAR_MIRROR              "rear_mirror"
#define REAR_RESOLUTION          "rear_resolution"
#define NEW_DEVICE         "new_device"
#define DVR_MARK           "dvr_mark"
#define RESOLUTION_4K				"resolution_4K"
#define APP_SOFTVER					"app_softver"
#define DVR_FLIP           "dvr_flip"
#define LIGHT_MODE		   "light_mode"
#define DEBUG_MODE			"debug_mode"
#define CAMERA_RATE			"camera_rate"

#define EYE_PROTECTION_MODE         "eye_protection_mode"
#define EYE_PROTECTION_START_TIME   "eye_protection_start_time"
#define EYE_PROTECTION_END_TIME     "eye_protection_end_time"
#define EYE_PROTECTION_BRIGHTNESS   "eye_protection_brightness"
#define EYE_PROTECTION_SAVE_BRIGHTNESS  "eye_protection_save_brightness"

#define SAVE_BRIGHTNESS    "save_brightness"

// 摄像头参数存储key (持久化到storage, 重启后恢复)
#define CAMERA_BRIGHTNESS              "camera_brightness"
#define CAMERA_CONTRAST                "camera_contrast"
#define CAMERA_SATURATION              "camera_saturation"
#define CAMERA_HUE                     "camera_hue"

#define FM_FREQUENCY               "fm_frequency"
#define D_SCREENSAVER_TIME         0
#define DEFAULT_FREQUENCY		   875

static bool _s_isp_param_show = false;
#define DEVICE_TYPE				   "XTlink"
#define DEVIDE_BT_NAME			   "Car BT"
namespace sys {
namespace setting {

static struct {
	char dev[16];
	char size[16];
	int rate;
	int chn;
	int skip;
	int width;
	int height;
	int rot;
	int mirror;
	int enable;
} _s_camera_info;

static bool _s_time_format_24h = true;
static bool _s_temp_protect_enable = true;
static int  _s_temp_protect_level1 = 100;
static int  _s_temp_protect_level2 = 105;

static bool is_reduce_resolution = false;

// 护眼模式相关信息
static bool eye_protection_enable = false;
static std::string eye_protection_start_time = "00:00";
static std::string eye_protection_end_time = "00:00";
static bool is_eye_protection_now = false;

static void _oem_init() {
	uint32_t misc_oem_size = 0;
	uint8_t *misc_oem_data = zk_misc_load_data("oem.ini", &misc_oem_size);
	if (!misc_oem_data) {
		return;
	}

	const char *data_oem_ini = "/data/oem.ini";
	uint32_t data_oem_size = 0;
	uint8_t *data_oem_data = fy::files::load_data(data_oem_ini, data_oem_size);
	if (data_oem_data && (misc_oem_size == data_oem_size)
			&& (memcmp(misc_oem_data, data_oem_data, data_oem_size) == 0)) {
		free(misc_oem_data);
		free(data_oem_data);
		return;
	}

	fy::files::save_data(data_oem_ini, misc_oem_data, misc_oem_size);
	ini_t *ini = ini_load(data_oem_ini);
	if (!ini) {
		free(misc_oem_data);
		free(data_oem_data);
		return;
	}

	const char *lang = ini_get(ini, "system", "language");
	if (lang) {
		LOGD("[setting] language %s\n", lang);
		update_lang_code(lang);
	}
//
//	const char *rear_resolution = ini_get(ini, "system", "rear_resolution");
//	if (rear_resolution) {
//		if (strcmp(rear_resolution, "1080P") == 0) {
//			set_rear_video_resolution({1920, 1080});
//		} else {
//			set_rear_video_resolution({1280, 720});
//		}
//	}
//
//	const char *front_resolution = ini_get(ini, "system", "front_resolution");
//	if (front_resolution) {
//		if (strcmp(front_resolution, "1080P") == 0) {
//			set_resolution_mode(E_RESOLUTION_MODE_720P_720P);
//		} else if(front_resolution, "2.5K"){
//			set_resolution_mode(E_RESOLUTION_MODE_1080P_1080P);
//		}
//		else {
//			set_resolution_mode(E_RESOLUTION_MODE_2_5K_720P);
//		}
//	}
//
//	const char *sys_brightness_value = ini_get(ini, "system", "brightness");
//	if (sys_brightness_value) {
//		set_brightness(BRIGHTNESS_MAX_VAL * atof(sys_brightness_value));
//		StoragePreferences::putInt("sys_brightness_key", atof(sys_brightness_value) * BRIGHTNESS_MAX_VAL);
//	}
//
//	const char *call_value = ini_get(ini, "system", "call_vol");
//	if (call_value) {
//		storage::put_float("call_vol", atof(call_value));
//	}
//
//	const char *dvr_record = ini_get(ini, "system", DVR_RECORD_TIME);
//	if (dvr_record) {
//		set_dvr_record_time(atoi(dvr_record));
//	}
//
//	const char *int_key[] = {
//		SOUND_MODE,
//		REAR_MIRROR,
//		SCREENSAVER_TIME,
//		CAMERA_RATE,
//	};
//	for (size_t i = 0; i < TAB_SIZE(int_key); i++) {
//		const char *val = ini_get(ini, "system", int_key[i]);
//		if (val) {
//			storage::put_int(int_key[i], atoi(val));
//		}
//	}
//
//	const char *bool_key[] = {
//		DVR_RECORD_SOUND,
//		ASSIST_SWITCH,
//		DRIVING_MODE,
//		LINK_SCREEN_MODE,
//		TOUCH_SOUND,
//		DVR_MARK,
//		RESOLUTION_4K,
//		DVR_FLIP,
//	};
//	for (size_t i = 0; i < TAB_SIZE(bool_key); i++) {
//		const char *val = ini_get(ini, "system", bool_key[i]);
//		if (val) {
//			storage::put_bool(bool_key[i], atoi(val));
//		}
//	}
//
//	const char *str_key[] = {
//		AIRPLAY_NAME,
//		MIRACAST_NAME,
//		APP_SOFTVER,
//	};
//	for (size_t i = 0; i < TAB_SIZE(str_key); i ++) {
//		const char *val = ini_get(ini, "system", str_key[i]);
//		if (val) {
//			storage::put_string(str_key[i], val);
//		}
//	}
//
//	const char *str_uuid_key[] = {
//		BT_NAME,
//		CARPLAY_AP_NAME,
//		AIRPLAY_AP_NAME,
//		WIFIVIDEO_AP_NAME,
//	};
//	for (size_t i = 0; i < TAB_SIZE(str_uuid_key); i ++) {
//		const char *val = ini_get(ini, "system", str_uuid_key[i]);
//		if (val) {
//			storage::put_string(str_uuid_key[i], std::string(val) + fy::gen_uuid_str());
//		}
//	}

	free(misc_oem_data);
	free(data_oem_data);
	ini_free(ini);
}

static void init_default_language() {
    // 检查是否是首次启动
    const char* flag_file = "/data/.language_init_flag";
    
    if (!FILE_EXIST(flag_file)) {
        // 首次启动,设置英文为默认语言
        LOGD("[setting] First boot detected, setting default language to English");
        update_lang_code("en_US");  // 使用已有的 update_lang_code 函数
        
        // 创建标志文件
        FILE* fp = fopen(flag_file, "w");
        if (fp) {
            fclose(fp);
            LOGD("[setting] Language init flag created");
        }
    } else {
        LOGD("[setting] Language already initialized");
    }
}

static void _set_camera_skip_env(int skip) {
	char str[8];
	sprintf(str, "%d", skip);
	setenv("ZKCAMERA_SKIP_FRAMES", str, 1);
}

static void _camera_setting_init() {
	SystemProperties::getString(CAMERA_PROP_DEV, _s_camera_info.dev, "");
	SystemProperties::getString(CAMERA_PROP_SIZE, _s_camera_info.size, "");
	SystemProperties::getInt(CAMERA_PROP_RATE, &_s_camera_info.rate, -1);
	SystemProperties::getInt(CAMERA_PROP_SKIP, &_s_camera_info.skip, -1);
	SystemProperties::getInt(CAMERA_PROP_MIRROR, &_s_camera_info.mirror, -1);
	SystemProperties::getInt(CAMERA_PROP_ROT, &_s_camera_info.rot, -1);
	SystemProperties::getInt(CAMERA_PROP_CHN, &_s_camera_info.chn, -1);
	SystemProperties::getInt(CAMERA_PROP_ENABLE, &_s_camera_info.enable, -1);
	sscanf(_s_camera_info.size, "%dx%d", &_s_camera_info.width, &_s_camera_info.height);

	if (!_s_camera_info.dev[0]) {
		set_camera_dev(D_CAMERA_DEV);
	}
	if (!_s_camera_info.size[0]) {
		set_camera_size(D_CAMERA_SIZE_WIDTH, D_CAMERA_SIZE_HEIGHT);
	}
	if (_s_camera_info.rate == -1) {
		set_camera_rate(D_CAMERA_RATE);
	}
	if (_s_camera_info.skip == -1) {
		set_camera_skip(D_CAMERA_SKIP);
	} else {
		_set_camera_skip_env(_s_camera_info.skip);
	}
	if (_s_camera_info.mirror == -1) {
		set_camera_mirror(D_CAMERA_MIRROR);
	}
	if (_s_camera_info.rot == -1) {
		set_camera_rot(D_CAMERA_ROT);
	}
	if (_s_camera_info.chn == -1) {
		set_camera_chn(D_CAMERA_CHN);
	}
	if (_s_camera_info.enable == -1) {
		set_camera_enable(D_CAMERA_ENABLE);
	}
}



static void _set_isp_link() {
	if (FILE_EXIST(DEV_PATH_GC2093) && FILE_EXIST(ISP_PARAM_GC2093)) {
		mkdir(ISP_PARAM_PATH, 0666);
		symlink(ISP_PARAM_GC2093, ISP_PARAM_PATH "" ISP_PARAM_FILE);
	} else if (FILE_EXIST(DEV_PATH_GC4663) && FILE_EXIST(ISP_PARAM_GC4663)) {
		mkdir(ISP_PARAM_PATH, 0666);
		symlink(ISP_PARAM_GC4663, ISP_PARAM_PATH "" ISP_PARAM_FILE);
	} else if (FILE_EXIST(DEV_PATH_GC2083) && FILE_EXIST(ISP_PARAM_GC2083)) {
		mkdir(ISP_PARAM_PATH, 0666);
		symlink(ISP_PARAM_GC2083, ISP_PARAM_PATH "" ISP_PARAM_FILE);
	}
}

void init() {
	_oem_init();
	init_default_language();
	_camera_setting_init();

	// 设置ISP参数软链接
	_set_isp_link();

	_s_time_format_24h = storage::get_bool(TIME_FORMAT_24H, true);
	if (StoragePreferences::getInt("sys_brightness_key", -1) == -1) {
		// 设置默认亮度
		set_brightness(BRIGHTNESS_MAX_VAL * 0.72f);
	}

	if (is_usb_adb_enabled()) {
		if (get_usb_mode() != E_USB_MODE_DEVICE) {
			change_usb_mode(E_USB_MODE_DEVICE);
		}
	} else {
		set_usb_config(E_USB_MODE_HOST);
	}
}

void set_bt_name(const std::string &name) {
	storage::put_string(BT_NAME, name);
}

std::string get_bt_name() {
	return storage::get_string(BT_NAME, DEVIDE_BT_NAME + fy::gen_uuid_str());
}

void set_camera_dev(const char *dev) {
	strcpy(_s_camera_info.dev, dev);
	SystemProperties::setString(CAMERA_PROP_DEV, dev);
}

const char* get_camera_dev() {
	return _s_camera_info.dev;
}

int get_camera_width() {
	return _s_camera_info.width;
}

int get_camera_height() {
	return _s_camera_info.height;
}

void set_camera_size(int width, int height) {
	_s_camera_info.width = width;
	_s_camera_info.height = height;

	char size[16];
	sprintf(size, "%dx%d", width, height);
	SystemProperties::setString(CAMERA_PROP_SIZE, size);
}

void set_camera_rate(int rate) {
	_s_camera_info.rate = rate;
	SystemProperties::setInt(CAMERA_PROP_RATE, rate);
}

int get_camera_rate() {
	return _s_camera_info.rate;
}

void set_camera_rot(int rot) {
	SystemProperties::setInt(CAMERA_PROP_ROT, rot);
}

int get_camera_rot() {
	return _s_camera_info.rot;
}

void set_camera_mirror(int mirror) {
	_s_camera_info.mirror = mirror;
	SystemProperties::setInt(CAMERA_PROP_MIRROR, mirror);
}

int get_camera_mirror() {
	return _s_camera_info.mirror;
}

void set_camera_chn(int chn) {
	_s_camera_info.chn = chn;
	SystemProperties::setInt(CAMERA_PROP_CHN, chn);
}

int get_camera_chn() {
	return _s_camera_info.chn;
}

void set_camera_skip(int skip) {
	_s_camera_info.skip = skip;
	SystemProperties::setInt(CAMERA_PROP_SKIP, skip);
	_set_camera_skip_env(skip);
}

int get_camera_skip() {
	return _s_camera_info.skip;
}

void set_camera_enable(bool enable) {
	_s_camera_info.enable = enable;
	SystemProperties::setInt(CAMERA_PROP_ENABLE, enable);
}

bool get_camera_enable() {
	return _s_camera_info.enable;
}

void set_brightness(int brightness) {
	BRIGHTNESSHELPER->setBrightness(brightness);
}

int get_brightness() {
	return BRIGHTNESSHELPER->getBrightness();
}

void backlight_on() {
	BRIGHTNESSHELPER->backlightOn();
}

void backlight_off() {
	BRIGHTNESSHELPER->backlightOff();
}

bool is_screenon() {
	return BRIGHTNESSHELPER->isScreenOn();
}

void update_lang_code(const char *code) {
	EASYUICONTEXT->updateLocalesCode(code);
}

bool set_date_time(struct tm *ptm) {
	return TimeHelper::setDateTime(ptm);
}

bool is_usb_adb_enabled() {
	return storage::get_int(USB_MODE, E_USB_MODE_HOST) == E_USB_MODE_DEVICE;
}

void enable_usb_adb(bool enabled) {
	usb_mode_e mode = enabled ? E_USB_MODE_DEVICE : E_USB_MODE_HOST;
	change_usb_mode(mode);
	storage::put_int(USB_MODE, mode);
}

std::string get_airplay_name() {
	return storage::get_string(AIRPLAY_NAME, "XT LINK");
}

std::string get_miracast_name() {
	return storage::get_string(MIRACAST_NAME, "XT" + fy::gen_uuid_str());
}

std::string get_dev_name() {
	return storage::get_string(DEV_NAME, DEVICE_TYPE + fy::gen_uuid_str());
}

std::string get_os_version() {
	char version[32];
	SystemProperties::getString("ro.build.date", version, "");
	return base::format("V1.0.1.%s", version);
}

void set_link_mode(link_mode_e mode) {
	storage::put_int(LINK_MODE, mode);
}

link_mode_e get_link_mode() {
	return (link_mode_e) storage::get_int(LINK_MODE, E_LINK_MODE_CARPLAY);
}

void set_sound_mode(sound_mode_e mode) {
	storage::put_int(SOUND_MODE, mode);
}

sound_mode_e get_sound_mode() {
	return (sound_mode_e) storage::get_int(SOUND_MODE, E_SOUND_MODE_SPK);
}

void set_resolution_mode(resolution_mode_e mode) {
	storage::put_int(RESOLUTION, mode);
}

resolution_mode_e get_resolution_mode() {
	return (resolution_mode_e) storage::get_int(RESOLUTION, E_RESOLUTION_MODE_720P_720P);
}

void set_camera_rate_mode(int rate) {
	storage::put_int(CAMERA_RATE, rate);
}
int get_camera_rate_mode(){
	return storage::get_int(CAMERA_RATE, 25);
}

void set_dvr_record_time(int time) {
	storage::put_int(DVR_RECORD_TIME, time);
}

int get_dvr_record_time() {
	return storage::get_int(DVR_RECORD_TIME, 1);
}

void set_dvr_record_sound_enable(bool enabled) {
	storage::put_bool(DVR_RECORD_SOUND, enabled);
}

bool is_dvr_record_sound_enabled() {
	return storage::get_bool(DVR_RECORD_SOUND, true);
}

void set_screensaver_time(int time) {
	storage::put_int(SCREENSAVER_TIME, time);
}

int get_screensaver_time() {
	return storage::get_int(SCREENSAVER_TIME, D_SCREENSAVER_TIME);
}

void set_driving_left(bool left) {
	storage::put_bool(DRIVING_MODE, left);
}

bool is_driving_left() {
	return storage::get_bool(DRIVING_MODE, true);
}

void set_link_screen_full(bool full) {
	storage::put_bool(LINK_SCREEN_MODE, full);
}

bool is_link_screen_full() {
	return storage::get_bool(LINK_SCREEN_MODE, true);
}

void set_touch_sound_enable(bool enabled) {
	storage::put_bool(TOUCH_SOUND, enabled);
}

bool is_touch_sound_enabled() {
	return storage::get_bool(TOUCH_SOUND, true);
}

void set_time_format_24h(bool format_24h) {
	_s_time_format_24h = format_24h;
	storage::put_bool(TIME_FORMAT_24H, format_24h);
}

bool is_time_format_24h() {
	return _s_time_format_24h;
}

void set_assist_switch_enable(bool enabled) {
	storage::put_bool(ASSIST_SWITCH, enabled);
}

bool is_assist_switch_enabled() {
	return storage::get_bool(ASSIST_SWITCH, true);
}

void set_float_button_position(const LayoutPosition &pos) {
	char buf[128];
	sprintf(buf, "%d,%d,%d,%d", pos.mLeft, pos.mTop, pos.mWidth, pos.mHeight);
	storage::put_string(FLOAT_BUTTON_POS, buf);
}

LayoutPosition get_float_button_position() {
	LayoutPosition pos;
	std::string str = storage::get_string(FLOAT_BUTTON_POS, "");
	if (!str.empty()) {
		sscanf(str.c_str(), "%d,%d,%d,%d", &pos.mLeft, &pos.mTop, &pos.mWidth, &pos.mHeight);
	}
	return pos;
}

void set_reverse_line_point(const SZKPoint &lt, const SZKPoint &rt, const SZKPoint &lb, const SZKPoint &rb) {
	char buf[128];
	sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d",
			(int) lt.x, (int) lt.y, (int) rt.x, (int) rt.y,
			(int) lb.x, (int) lb.y, (int) rb.x, (int) rb.y);
	storage::put_string(REVERSE_LINE_POINT, buf);
}

void get_reverse_line_point(SZKPoint &lt, SZKPoint &rt, SZKPoint &lb, SZKPoint &rb) {
	std::string str = storage::get_string(REVERSE_LINE_POINT, "");
	if (!str.empty()) {
		int ltx = 0, lty = 0, rtx = 0, rty = 0, lbx = 0, lby = 0, rbx = 0, rby = 0;
		sscanf(str.c_str(), "%d,%d,%d,%d,%d,%d,%d,%d", &ltx, &lty, &rtx, &rty, &lbx, &lby, &rbx, &rby);
		lt.x = ltx;
		lt.y = lty;
		rt.x = rtx;
		rt.y = rty;
		lb.x = lbx;
		lb.y = lby;
		rb.x = rbx;
		rb.y = rby;
	} else {
		lt.x = REVERSE_LINE_DEF_LTX;
		lt.y = REVERSE_LINE_DEF_LTY;
		rt.x = REVERSE_LINE_DEF_RTX;
		rt.y = REVERSE_LINE_DEF_RTY;
		lb.x = REVERSE_LINE_DEF_LBX;
		lb.y = REVERSE_LINE_DEF_LBY;
		rb.x = REVERSE_LINE_DEF_RBX;
		rb.y = REVERSE_LINE_DEF_RBY;
	}
}

void set_reverse_viewbox_y(int y) {
  storage::put_int(REVERSE_VIEWBOX_Y, y);
}

int get_reverse_viewbox_y() {
  return storage::get_int(REVERSE_VIEWBOX_Y, 0);
}

void set_dvr_viewbox_front_y(int y) {
  storage::put_int(DVR_VIEWBOX_FRONT_Y, y);
}
int get_dvr_viewbox_front_y(){
  return storage::get_int(DVR_VIEWBOX_FRONT_Y, 0);
}

void set_dvr_viewbox_rear_y(int y) {
  storage::put_int(DVR_VIEWBOX_REAR_Y, y);
}
int get_dvr_viewbox_rear_y() {
  return storage::get_int(DVR_VIEWBOX_REAR_Y, 0);
}

void set_dvr_view_type(int type) {
  storage::put_int(DVR_VIEW_TYPE, type);
}
int get_dvr_view_type() {
  return storage::get_int(DVR_VIEW_TYPE, 0);
}

void set_half_view_type(int type) {
  storage::put_int(HALF_VIEW_TYPE, type);
}

int get_half_view_type() {
  return storage::get_int(HALF_VIEW_TYPE, 0);
}

void set_rear_mirror(int mirror) {
  storage::put_int(REAR_MIRROR, mirror ? 1 : 0);
}

bool is_rear_mirror() {
  return storage::get_int(REAR_MIRROR, 1) != 0;
}

inline int dvrVideoTime() {
  switch (get_dvr_record_time()) {
    case 1:
      return 60;
    case 2:
      return 60 * 3;
    case 3:
      return 60 * 5;
    default:
      return 60;
  }
}

// mpi::RecorderParam makeRecorderParam() {
//   const int duration = dvrVideoTime();

//   mpi::RecordingSettings front_parameters;
//   mpi::RecordingSettings rear_parameters;
//   front_parameters.duration = duration;
//   front_parameters.audio = is_dvr_record_sound_enabled();
//   front_parameters.audio_adjust_level = 2.0;
//   front_parameters.bitrate = 12 * 1024 * 1024;
//   front_parameters.frame_rate = get_camera_rate_mode();
//   front_parameters.thumbnail_size = {0,0};
//   rear_parameters = front_parameters;

//   switch (get_resolution_mode()) {
//     case E_RESOLUTION_MODE_720P_720P: //1080P
// #if 1
//     	front_parameters.size = {1920, 1080};
//     	rear_parameters.size = {1920, 1080};
// #else
//     	front_parameters.size = {1280, 720};
//     	rear_parameters.size = {1280, 720};
// #endif
//       break;
//     case E_RESOLUTION_MODE_1080P_1080P: //2.5k
// #if 1
// 		front_parameters.size = {2560, 1440};
// 		rear_parameters.size = {1920, 1080};
// #else
//       param.front.size = {1280, 720};
//       rear_parameters.size = {1280, 720};
// #endif
//       break;
//     case E_RESOLUTION_MODE_2_5K_720P: //4K
// #if 1
// 		front_parameters.size = {3840, 2160};
// 		rear_parameters.size = {1920, 1080};
// #else
// 		front_parameters.size = {3840, 2160};
// 		rear_parameters.size = {1920, 1080};
// #endif
//       break;
// // 	  case E_RESOLUTION_MODE_4K: //4K
// // #if 1
// // 		front_parameters.size = {3840, 2160};
// // 		rear_parameters.size = {1920, 1080};
// // #else
// // 		front_parameters.size = {2560, 1440};
// // 		rear_parameters.size = {1920, 1080};
// // #endif
// //       break;
//     default:
//       LOGE("invalid resolution mode");
//       front_parameters.size = {1080, 720};
//       rear_parameters.size = {1080, 720};
//       break;
//   }

//   mpi::RecorderParameters recorder_params;
//   recorder_params.settings[mpi::VIDEO_DEVICE_FRONT_RECORD] = front_parameters;
//   if(sys::has_reverse_camera()) {
// 	  recorder_params.settings[mpi::VIDEO_DEVICE_REAR] = rear_parameters;
//   }
//   recorder_params.sync_interval = 0;
//   return recorder_params;
// }

mpi::RecorderParam makeRecorderParam() {
  const int duration = dvrVideoTime();

  mpi::RecordingSettings front_parameters;
  mpi::RecordingSettings rear_parameters;
  front_parameters.duration = duration;
  front_parameters.audio = is_dvr_record_sound_enabled();
  front_parameters.audio_adjust_level = 2.0;
  front_parameters.bitrate = 8 * 1024 * 1024;
  front_parameters.frame_rate = get_camera_rate_mode();
  front_parameters.thumbnail_size = {0,0};
  rear_parameters = front_parameters;

  bool reduce_resolution = (lk::is_connected() && get_resolution_mode() != E_RESOLUTION_MODE_720P_720P);
  set_reduce_resolution(reduce_resolution);


  // 根据分辨率模式设置水印位置和大小
  // 基准：1600x600下 x=90, y=550, w=368, h=50
  // TextWatermark会根据target_rect的高度自动确定字体大小
  int watermark_x = 50;
  int watermark_y = 50;
  int watermark_width = 300;
  int watermark_height = 820;
  int watermark_fontsize = 50;
  switch (get_resolution_mode()) {
    case E_RESOLUTION_MODE_720P_720P: //1080P (1920x1080)
#if 1
      front_parameters.size = {1920, 1080};
      rear_parameters.size = {1920, 1080};
#else
      front_parameters.size = {1280, 720};
      rear_parameters.size = {1280, 720};
#endif
      // 1080P 水印参数 (按1600x600比例: x=90/1600, y=550/600, w=368/1600, h=50/600)
      watermark_x = 108;
      watermark_y = 990;
      watermark_width = 442;
      watermark_height = 160;
      break;

    case E_RESOLUTION_MODE_1080P_1080P: //2.5k (2560x1440)
#if 1
      front_parameters.size = {2560, 1440};
      rear_parameters.size = {1920, 1080};
#else
      front_parameters.size = {1280, 720};
      rear_parameters.size = {1280, 720};
#endif
      // 2.5K 水印参数
      watermark_x = 144;
      watermark_y = 1320;
      watermark_width = 589;
      watermark_height = 213;
      break;

    case E_RESOLUTION_MODE_2_5K_720P: //4K (3840x2160)
#if 1
      front_parameters.size = {3840, 2160};
      rear_parameters.size = {1920, 1080};
#else
      front_parameters.size = {3840, 2160};
      rear_parameters.size = {1920, 1080};
#endif
      // 4K 水印参数
//      watermark_x = 216;
//      watermark_y = 1980;
//      watermark_y = 990;
		watermark_x = 20;
		watermark_y = 100;
		watermark_width = 990;
		watermark_height = 600;
		watermark_fontsize = 65;
//      front_parameters.watermarks.push_back(
//          NewWatermark(mpi::Rectangle{watermark_x, watermark_y, watermark_width, watermark_height}, watermark_height,
//                       [](BITMAP_S& bitmap, int start_x, int start_y) {
//                           char txt[32];
//                           time_t now = time(NULL);
//                           struct tm *t = localtime(&now);
//                           sprintf(txt, "%04d/%02d/%02d %02d:%02d:%02d",
//                                   t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
//                                   t->tm_hour, t->tm_min, t->tm_sec);
//                           mpi::drawText(bitmap, watermark_x, watermark_y, txt, watermark_fontsize);
//                           return true;
//                       })
//      );
      front_parameters.watermarks.push_back(
          NewWatermark(mpi::Rectangle{watermark_x, watermark_y, watermark_width, watermark_height}, watermark_fontsize,
                       [=](BITMAP_S& bitmap, int start_x, int start_y) {  // [=] 值捕获
                           char txt[32];
                           time_t now = time(NULL);
                           struct tm *t = localtime(&now);
                           sprintf(txt, "%04d/%02d/%02d %02d:%02d:%02d",
                                   t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
                                   t->tm_hour, t->tm_min, t->tm_sec);
                           mpi::drawText(bitmap, watermark_x, watermark_y, txt, watermark_fontsize);
                           return true;
                       })
      );
      break;

    default:
      LOGE("invalid resolution mode");
      front_parameters.size = {1080, 720};
      rear_parameters.size = {1080, 720};
      // 默认水印参数 (1080x720)
      watermark_x = 61;
      watermark_y = 660;
      watermark_width = 248;
      watermark_height = 60;
      break;
  }
//
//  front_parameters.watermarks.push_back(
//	   NewWatermark(
//          mpi::Rectangle{watermark_x, watermark_y, watermark_width, watermark_height},
//		  80,
//          []() -> std::string {
//              char datetime_str[32];
//              time_t now = time(NULL);
//              struct tm *t = localtime(&now);
//              sprintf(datetime_str, "%04d/%02d/%02d %02d:%02d:%02d",
//                      t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
//                      t->tm_hour, t->tm_min, t->tm_sec);
//              return datetime_str;
//          }
//              )
//  );
//  auto time_watermark = [](BITMAP_S& bitmap, int start_x, int start_y) {
//      char txt[32];
//      time_t now = time(NULL);
//      struct tm *t = localtime(&now);
//      sprintf(txt,"%d",t->tm_hour);
//      mpi::drawText(bitmap, start_x, start_y, txt, 25);
//      return true;
//  };
//
//  front_parameters.watermarks.push_back(
//      NewWatermark(mpi::Rectangle{watermark_x, 550, watermark_width, watermark_height}, 80,
//              []() -> std::string {
//                  char datetime_str[32];
//                  time_t now = time(NULL);
//                  struct tm *t = localtime(&now);
//                  sprintf(datetime_str, "%04d/%02d/%02d %02d:%02d:%02d",
//                          t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
//                          t->tm_hour, t->tm_min, t->tm_sec);
//                  return datetime_str;
//              }
//      ));


  mpi::RecorderParameters recorder_params;
  recorder_params.settings[mpi::VIDEO_DEVICE_FRONT_RECORD] = front_parameters;
  if(sys::has_reverse_camera()) {
    recorder_params.settings[mpi::VIDEO_DEVICE_REAR] = rear_parameters;
  }
  recorder_params.sync_interval = 0;
  return recorder_params;
}

mpi::CameraParam makeReverseCameraParam() {
  mpi::CameraParam param;
  param.mirror = is_rear_mirror();
  param.viewbox = {0, 0, 1920, 1080};
  param.display = {0, 0, 1600, 600};
  param.visible = true;
  return param;
}

static bool _set_camera_channel(int fd, uint32_t id, int chn) {
	if(chn < 0 && chn > 4) {
		LOGE("Unsupported channel:%d", chn);
		return false;
	}
	if (fd < 0) {
		LOGE("camera fd open fail\n");
		return false;
	}
	struct v4l2_control ctrl;

	fcntl(fd, F_SETFD, FD_CLOEXEC);

	ctrl.id = id;
	ctrl.value = chn;

	if(ioctl(fd, VIDIOC_S_CTRL, &ctrl) != 0) {
		LOGE("ioctl camera fail %s\n", strerror(errno));
		return false;
	}
	return true;
}

void setFrontCameraChannel(int chn) {
	NO_EXCEPTION({
		mpi::SharedVideoDevice frontVideo(mpi::VIDEO_DEVICE_FRONT);
		_set_camera_channel(frontVideo.getFileDescriptor(), V4L2_CID_AUDIO_VOLUME, chn);
	});
}

void setRearCameraChannel(int chn) {
	NO_EXCEPTION({
//		mpi::FrontVideoSource frontVideo(VIDEO_IN_CHANNEL_RTSP);
		mpi::SharedVideoDevice rearVideo(mpi::VIDEO_DEVICE_REAR);
		_set_camera_channel(rearVideo.getFileDescriptor(), V4L2_CID_AUDIO_VOLUME, chn);
	});
}


// 设置倒车摄像头亮度 (同时写入V4L2设备和storage持久化)
void setCameraBrightness(int brightness) {
	storage::put_int(CAMERA_BRIGHTNESS, brightness);
	NO_EXCEPTION({
		mpi::SharedVideoDevice rearVideo(mpi::VIDEO_DEVICE_REAR);
		rearVideo.setBrightness(brightness);
	});
}
bool getCameraBrightness(int &brightness) {
	int val = storage::get_int(CAMERA_BRIGHTNESS, -1);
	if (val != -1) {
		brightness = val;
		return true;
	}
	bool ret = false;
	NO_EXCEPTION({
		mpi::SharedVideoDevice rearVideo(mpi::VIDEO_DEVICE_REAR);
		brightness = rearVideo.getBrightness();
		ret = true;
	});
	return ret;
}
// 设置对比度 (同时写入V4L2设备和storage持久化)
void setCameraContrast(int contrast) {
	storage::put_int(CAMERA_CONTRAST, contrast);
	NO_EXCEPTION({
		mpi::SharedVideoDevice rearVideo(mpi::VIDEO_DEVICE_REAR);
		rearVideo.setContrast(contrast);
	});
}
bool getCameraContrast(int &contrast) {
	int val = storage::get_int(CAMERA_CONTRAST, -1);
	if (val != -1) {
		contrast = val;
		return true;
	}
	bool ret = false;
	NO_EXCEPTION({
		mpi::SharedVideoDevice rearVideo(mpi::VIDEO_DEVICE_REAR);
		contrast = rearVideo.getContrast();
		ret = true;
	});
	return ret;
}
// 设置饱和度 (同时写入V4L2设备和storage持久化)
void setCameraSaturation(int saturation) {
	storage::put_int(CAMERA_SATURATION, saturation);
	NO_EXCEPTION({
		mpi::SharedVideoDevice rearVideo(mpi::VIDEO_DEVICE_REAR);
		rearVideo.setSaturation(saturation);
	});
}
bool getCameraSaturation(int &saturation) {
	int val = storage::get_int(CAMERA_SATURATION, -1);
	if (val != -1) {
		saturation = val;
		return true;
	}
	bool ret = false;
	NO_EXCEPTION({
		mpi::SharedVideoDevice rearVideo(mpi::VIDEO_DEVICE_REAR);
		saturation = rearVideo.getSaturation();
		ret = true;
	});
	return ret;
}
// 设置色度 (同时写入V4L2设备和storage持久化)
void setCameraHue(int hue) {
	storage::put_int(CAMERA_HUE, hue);
	NO_EXCEPTION({
		mpi::SharedVideoDevice rearVideo(mpi::VIDEO_DEVICE_REAR);
		rearVideo.setHue(hue);
	});
}
bool getCameraHue(int &hue) {
	int val = storage::get_int(CAMERA_HUE, -1);
	if (val != -1) {
		hue = val;
		return true;
	}
	bool ret = false;
	NO_EXCEPTION({
		mpi::SharedVideoDevice rearVideo(mpi::VIDEO_DEVICE_REAR);
		hue = rearVideo.getHue();
		ret = true;
	});
	return ret;
}
// 设置锐利度
void setCameraSharpness(int sharpness) {
	NO_EXCEPTION({
		mpi::SharedVideoDevice rearVideo(mpi::VIDEO_DEVICE_REAR);
		rearVideo.setSharpness(sharpness);
	});
}
bool getCameraSharpness(int &sharpness) {
	bool ret = false;
	NO_EXCEPTION({
		mpi::SharedVideoDevice rearVideo(mpi::VIDEO_DEVICE_REAR);
		sharpness = rearVideo.getSharpness();
		ret = true;
	});
	return ret;
}

void set_isp_param_show(bool enable) {
	_s_isp_param_show = enable;
//	storage::put_bool(ISP_PARAM_SHOW, enable);
}

bool get_isp_param_show() {
	return _s_isp_param_show;
}


bool isNewDevice() {
  return StoragePreferences::getBool(NEW_DEVICE, true);
}

void setNewDevice(bool is_new) {
  StoragePreferences::putBool(NEW_DEVICE, is_new);
}

void set_rear_video_resolution(const mpi::Size& s) {
	LOGE_TRACE("not implemented");
//  mpi::RearVideoSource::setResolution(s);
  auto last = get_rear_video_resolution();
  if (last.w == s.w && last.h == s.h) {
    return;
  }
  storage::put_string(REAR_RESOLUTION, base::format("%dx%d", s.w, s.h));
}

mpi::Size get_rear_video_resolution() {
  auto str = storage::get_string(REAR_RESOLUTION, "1920x1080");
  mpi::Size s;
  sscanf(str.c_str(), "%dx%d", &s.w, &s.h);
  return s;
}

void set_dvr_mark_enable(bool enable) {
	storage::put_bool(DVR_MARK, enable);
}

bool is_dvr_mark_enable() {
	return storage::get_bool(DVR_MARK, false);
}

//void set_resolution_show_4K(bool enable) {
//	storage::put_bool(RESOLUTION_4K, enable);
//}

bool is_resolution_show_4K() {
	return storage::get_bool(RESOLUTION_4K, true);
}

std::string get_app_softver() {
	auto dt = base::buildDateTime();
	std::string softver = base::format("V%d.%d.%d.%04d%02d%02d",
		    APP_MAJOR,
		    APP_MINOR,
		    APP_PATCH,
		    dt.year,dt.month,dt.day);
	return storage::get_string(APP_SOFTVER, softver);
}

bool is_dvr_flip_enable() {
	return storage::get_bool(DVR_FLIP, true);
}

void set_eye_protection_enable(bool enable) {
	storage::put_bool(EYE_PROTECTION_MODE, enable);
}

bool is_eye_protection_enable() {
	return storage::get_bool(EYE_PROTECTION_MODE, false);
}

void set_eye_protection_start_time(const std::string &time) {
	storage::put_string(EYE_PROTECTION_START_TIME, time);
}

std::string get_eye_protection_start_time() {
	return storage::get_string(EYE_PROTECTION_START_TIME, "00:00");
}

void set_eye_protection_end_time(const std::string &time) {
	storage::put_string(EYE_PROTECTION_END_TIME, time);
}

std::string get_eye_protection_end_time() {
	return storage::get_string(EYE_PROTECTION_END_TIME, "00:00");
}

void set_protection_brightness(int brightness) {
	storage::put_int(EYE_PROTECTION_BRIGHTNESS, brightness);
}

// 默认亮度百分比为72%
int get_protection_brightness() {
	return storage::get_int(EYE_PROTECTION_BRIGHTNESS, 72);
}

void set_save_normal_brightness(int brightness) {
	storage::put_int(EYE_PROTECTION_SAVE_BRIGHTNESS, brightness);
}

int get_save_normal_brightness() {
	return storage::get_int(EYE_PROTECTION_SAVE_BRIGHTNESS, BRIGHTNESS_MAX_VAL * 0.72f);
}

void update_eye_protection_info() {
	eye_protection_enable = sys::setting::is_eye_protection_enable();
	eye_protection_start_time = sys::setting::get_eye_protection_start_time();
	eye_protection_end_time = sys::setting::get_eye_protection_end_time();
}

bool get_is_eye_protection_now() {
	return is_eye_protection_now;
}

// 护眼模式检测切换
void check_eye_protection_mode(struct tm *t) {
	if (!eye_protection_enable) {
		if (is_eye_protection_now) {
			sys::setting::set_brightness(sys::setting::get_save_normal_brightness());
			is_eye_protection_now = false;
		}
		return ;
	}
	int start_hour = 0;
	int start_min = 0;
	sscanf(eye_protection_start_time.c_str(), "%d:%d", &start_hour, &start_min);
	int end_hour = 0;
	int end_min = 0;
	sscanf(eye_protection_end_time.c_str(), "%d:%d", &end_hour, &end_min);
//	LOGD("start_time = %d:%d, end_time = %d:%d", start_hour, start_min, end_hour, end_min);
	int time_info = t->tm_hour * 60 + t->tm_min;
	int start_info = start_hour * 60 + start_min;
	int end_info = end_hour * 60 + end_min;
	bool is_protection_range = ((time_info >= start_info) && (time_info <= end_info));
	if (end_info < start_info) {
		is_protection_range = !((time_info < start_info) && (time_info > end_info));
	}
	if (end_info == start_info) {
		is_protection_range = false;
	}
	if (!is_eye_protection_now && is_protection_range) {
		is_eye_protection_now = true;
		// 保存正常模式亮度
		sys::setting::set_save_normal_brightness(sys::setting::get_brightness());
		sys::setting::set_brightness(sys::setting::get_protection_brightness() * BRIGHTNESS_MAX_VAL / 100);
		LOGD("[setting] protection_light = %d", sys::setting::get_protection_brightness() * BRIGHTNESS_MAX_VAL / 100);
	} else {
		if (is_eye_protection_now && !is_protection_range) {
			is_eye_protection_now = false;
			sys::setting::set_brightness(sys::setting::get_save_normal_brightness());
			LOGD("[setting] normal_light = %d", sys::setting::get_save_normal_brightness());
		}
	}
}

std::string get_airplay_ap_name() {
       return storage::get_string(AIRPLAY_AP_NAME, DEVICE_TYPE + fy::gen_uuid_str()) + "A";
}

std::string get_wifivideo_ap_name() {
       return storage::get_string(WIFIVIDEO_AP_NAME, DEVICE_TYPE + fy::gen_uuid_str()) + "B";
}

std::string get_carplay_ap_name() {
		return storage::get_string(CARPLAY_AP_NAME, DEVICE_TYPE + fy::gen_uuid_str());
}

void set_light_state(bool res) {
	storage::put_bool(LIGHT_MODE, res);
}

bool is_light_state() {
	return storage::get_bool(LIGHT_MODE, false);
}

int get_fm_frequency() {
	return storage::get_int(FM_FREQUENCY, DEFAULT_FREQUENCY);
}

void set_fm_frequency(int freq) {
	storage::put_int(FM_FREQUENCY, freq);
}

int get_save_brightness() {
	return storage::get_int(SAVE_BRIGHTNESS, 0);
}

void set_save_brightness(int val) {
	storage::put_int(SAVE_BRIGHTNESS, val);
}

bool is_temp_protect_enable() {
	return _s_temp_protect_enable;
}
int  get_temp_protect_level1() {
	return _s_temp_protect_level1;
}
int  get_temp_protect_level2() {
	return _s_temp_protect_level2;
}

#define BT_STATE	"bt_state"
//用于长按关机记录状态
void set_bt_state(bool state){
	storage::put_bool(BT_STATE, state);
}
bool get_bt_state(){
	return storage::get_bool(BT_STATE, false);
}


void set_reduce_resolution(bool enable) {
	is_reduce_resolution = enable;
}
bool get_reduce_resolution() {
	return is_reduce_resolution;
}

}

}
