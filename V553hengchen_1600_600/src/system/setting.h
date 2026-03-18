/*
 * setting.h
 *
 *  Created on: 2023年4月17日
 *      Author: ZKSWE Develop Team
 */

#ifndef _SYSTEM_SETTING_H_
#define _SYSTEM_SETTING_H_

#include "control/Common.h"
#include <mpi/case/recorder.h>
#include <mpi/case/camera.h>
#include "control/ZKTextView.h"
typedef enum {
	E_RESOLUTION_MODE_720P_720P,
	E_RESOLUTION_MODE_1080P_1080P,
	E_RESOLUTION_MODE_2_5K_720P,
} resolution_mode_e;

typedef enum {
	E_LINK_MODE_CARPLAY,       // carplay
	E_LINK_MODE_ANDROIDAUTO,   // android auto
	E_LINK_MODE_AIRPLAY,       // airplay
	E_LINK_MODE_MIRACAST,      // miracast
	E_LINK_MODE_BLUEMUSIC,     // 蓝牙音乐
	E_LINK_MODE_WIFIVIDEO,     // wifi视频
	E_LINK_MODE_AICAST,		   // aicast
} link_mode_e;

typedef enum {
	E_SOUND_MODE_SPK,     // 喇叭出声
	E_SOUND_MODE_LINK,    // 互联手机出声
	E_SOUND_MODE_FM,      // FM发射
	E_SOUND_MODE_AUX,     // aux
} sound_mode_e;

enum ViewType {
  VIEW_TYPE_FRONT = 0,
  VIEW_TYPE_REAR = 1,
  VIEW_TYPE_BOTH = 2,
  VIEW_TYPE_UNVISIBLE = 3,
  VIEW_TYPE_REVERSE,
  VIEW_TYPE_FRONT_MINI,
  VIEW_TYPE_REAR_MINI,
};

namespace sys {
namespace setting {

void init();

/**
 * @brief 设备名称、蓝牙名称
 */
void set_bt_name(const std::string &name);
std::string get_bt_name();

/**
 * @brief 设置摄像头节点 /dev/video*
 */
void set_camera_dev(const char *dev);

/**
 * @brief 获取摄像头节点
 */
const char* get_camera_dev();

/**
 * @brief 设置摄像头分辨率
 * @param width 宽     height 高
 */
void set_camera_size(int width, int height);

/**
 * @brief 获取摄像头图像宽
 */
int get_camera_width();

/**
 * @brief 获取摄像头图像高
 */
int get_camera_height();

/**
 * @brief 设置摄像头帧率
 * @param rate帧率
 */
void set_camera_rate(int rate);

/**
 * @brief 获取摄像头帧率
 */
int get_camera_rate();

/**
 * @brief 设置旋转角度，默认0不旋转，如果屏幕有旋转才需要设置
 */
void set_camera_rot(int rot);

/**
 * @brief 获取摄像头旋转角度
 */
int get_camera_rot();

/**
 * @brief 快速镜像设置
 */
void set_camera_mirror(int mirror);
int get_camera_mirror();

/**
 * @brief 设置摄像头通道
 * @param chn 范围：0 ~ 3
 */
void set_camera_chn(int chn);

/**
 * @brief 获取摄像头通道
 */
int get_camera_chn();

/**
 * @brief 设置跳过前面几帧图像，摄像头刚打开那会数据不稳定会有抖动的现象，可通过设置该参数来跳过
 * @param skip
 */
void set_camera_skip(int skip);

/**
 * @brief 获取跳过了前面多少帧图像
 */
int get_camera_skip();

/**
 * @brief 快速倒车开关
 */
void set_camera_enable(bool enable);
bool get_camera_enable();

/**
 * @brief 设置亮度值
 * @param brightness 范围：0 ~ 100
 */
void set_brightness(int brightness);

/**
 * @brief 获取亮度值
 */
int get_brightness();

/**
 * @brief 开背光
 */
void backlight_on();

/**
 * @brief 关背光
 */
void backlight_off();

/**
 * @brief 是否是开屏状态
 */
bool is_screenon();

/**
 * @brief 切换多国语言
 * @param code 语言编码
 */
void update_lang_code(const char *code);

/*
 * @brief 设置日期
 */
bool set_date_time(struct tm *ptm);

/*
 * @brief usb adb是否开启
 */
bool is_usb_adb_enabled();

/*
 * @brief 开关usb adb
 */
void enable_usb_adb(bool enabled);

std::string get_airplay_name();
std::string get_miracast_name();
std::string get_os_version();
std::string get_dev_name();

void set_link_mode(link_mode_e mode);
link_mode_e get_link_mode();

void set_sound_mode(sound_mode_e mode);
sound_mode_e get_sound_mode();

void set_resolution_mode(resolution_mode_e mode);
resolution_mode_e get_resolution_mode();

void set_camera_rate_mode(int rate);
int get_camera_rate_mode();

void set_dvr_record_time(int time);
int get_dvr_record_time();

void set_dvr_record_sound_enable(bool enabled);
bool is_dvr_record_sound_enabled();

void set_screensaver_time(int time);
int get_screensaver_time();

void set_driving_left(bool left);
bool is_driving_left();

void set_link_screen_full(bool full);
bool is_link_screen_full();

void set_touch_sound_enable(bool enabled);
bool is_touch_sound_enabled();

void set_time_format_24h(bool format_24h);
bool is_time_format_24h();

void set_assist_switch_enable(bool enabled);
bool is_assist_switch_enabled();

void set_float_button_position(const LayoutPosition &pos);
LayoutPosition get_float_button_position();

// left top, right top, left bottom, right bottom
void set_reverse_line_point(const SZKPoint &lt, const SZKPoint &rt, const SZKPoint &lb, const SZKPoint &rb);
void get_reverse_line_point(SZKPoint &lt, SZKPoint &rt, SZKPoint &lb, SZKPoint &rb);

void set_reverse_viewbox_y(int y);
int get_reverse_viewbox_y();

void set_dvr_viewbox_front_y(int y);
int get_dvr_viewbox_front_y();

void set_dvr_viewbox_rear_y(int y);
int get_dvr_viewbox_rear_y();

void set_dvr_view_type(int type);
int get_dvr_view_type();

void set_half_view_type(int type);
int get_half_view_type();

void set_rear_mirror(int mirror);
bool is_rear_mirror();

mpi::RecorderParam makeRecorderParam();
mpi::CameraParam makeReverseCameraParam();
void setFrontCameraChannel(int chn);
void setRearCameraChannel(int chn);


void setCameraBrightness(int brightness);
bool getCameraBrightness(int &brightness);

void setCameraContrast(int contrast);
bool getCameraContrast(int &contrast);

void setCameraSaturation(int saturation);
bool getCameraSaturation(int &saturation);

void setCameraHue(int hue);
bool getCameraHue(int &hue);

void setCameraSharpness(int sharpness);
bool getCameraSharpness(int &sharpness);

// 设置摄像头预览和录像是否显示isp
void set_isp_param_show(bool enable);
bool get_isp_param_show();


bool isNewDevice();
void setNewDevice(bool is_new);

void set_rear_video_resolution(const mpi::Size& re);
mpi::Size get_rear_video_resolution();

//dvr 水印显示
void set_dvr_mark_enable(bool enable);
bool is_dvr_mark_enable();

//设置界面显示分辨率2K还是2.5k
void set_resolution_text_show_2K(bool enable);
bool is_resolution_text_show_2K();

void set_resolution_show_4K(bool enable);
bool is_resolution_show_4K();

std::string get_app_softver();

bool is_dvr_flip_enable();

// 护眼模式
void set_eye_protection_enable(bool enable);
bool is_eye_protection_enable();

void set_eye_protection_start_time(const std::string &time);
std::string get_eye_protection_start_time();
void set_eye_protection_end_time(const std::string &time);
std::string get_eye_protection_end_time();

// 护眼模式亮度百分比
void set_protection_brightness(int brightness);
int get_protection_brightness();

// 护眼模式结束后恢复的亮度
void set_save_normal_brightness(int brightness);
int get_save_normal_brightness();

bool get_is_eye_protection_now();

// 更新护眼模式相关全局变量
void update_eye_protection_info();
void check_eye_protection_mode(struct tm *t);

std::string get_airplay_ap_name();
std::string get_wifivideo_ap_name();
std::string get_carplay_ap_name();

void set_light_state(bool res);
bool is_light_state();

// 获取fm频率
int get_fm_frequency();

// 设置fm频率
void set_fm_frequency(int freq);

int get_save_brightness();
void set_save_brightness(int val);

// 温控保护
bool is_temp_protect_enable();
int  get_temp_protect_level1();
int  get_temp_protect_level2();

void set_bt_state(bool state);
bool get_bt_state();

void set_reduce_resolution(bool enable);
bool get_reduce_resolution();
} /* namespace setting */

} /* namespace ssy */

#endif /* _SYSTEM_SETTING_H_ */
