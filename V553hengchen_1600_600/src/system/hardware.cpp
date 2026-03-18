/*
 * hardware.cpp
 *
 *  Created on: 2024年3月6日
 *      Author: guoxs
 */

#include <fcntl.h>
#include <sys/ioctl.h>
#include <vo/hwdisplay.h>
#include "utils/GpioHelper.h"
#include "utils/AdcHelper.h"
#include "hw/HardwareManager.h"
#include "system/setting.h"
#include "uart/context.h"
#include "fy/thread.hpp"
#include "hardware.h"
#include "common.h"
#include <manager/ConfigManager.h>
#include "misc/keytone.h"
#include <mpi/case/shared_video_device.h>
#include "utils/I2CHelper.h"
#include "light_sersor.h"
#include <base/timer.hpp>
#include <mpi/case/recorder.h>
#include <mpi/module/isp.h>

#define ENABLE_AUX_DET     0
#define CAMERA_POWER_IO_19 "GPIO_19"
#define CAMERA_POWER_IO    "GPIO_239"
#define AUX_DET_IO         "GPIO_235"

#define LDR_READ_TIMES 	   10  //光照传感器ADC循环读取次数
#define ADC_CHANNEL0       0
#define ADC_CHANNEL1       1

namespace sys {
namespace hw {

static bool _s_aux_is_insert;
static std::set<aux_det_cb> _s_cb_set;
static Mutex _s_mutex;

static Mutex _s_light_mutex;
static std::set<light_det_cb> _s_light_cb_set;
static LIGHT_MODE_E _s_light_mode = LIGHT_MODE_INVALID;
static bool _enadble_auto_set_light = false;
static Mutex _s_auto_brightness_mutex;
static std::set<auto_brightness_cb> _s_auto_brightness_cb_set;
base::timer auto_brightness_timer;
static Mutex _s_temp_mutex;
static std::set<temp_protect_cb> _s_temp_cb_set;
base::timer temp_protect_timer;

static int _layer_config(int fd, int cmd, disp_layer_config *cfg) {
	unsigned long args[4] = { 0 };
	args[1] = (unsigned long) cfg;
	args[2] = 1;
    return ioctl(fd, cmd, args);
}

bool _i2c_read_byte(I2CHelper &i2c, uint8_t read_addr, uint8_t *read_data) {
    if (!i2c.write(&read_addr, 1)) {
		LOGE("i2c write addr %x error!\n", read_addr);
		return false;
	}

    if (!i2c.read(read_data, 1)) {
    	LOGE("i2c read addr %x error!\n", read_addr);
		return false;
	}

    return true;
}

bool _i2c_write_byte(I2CHelper &i2c, uint8_t write_addr, uint8_t write_data) {
	uint8_t buf[2];

	buf[0] = write_addr;
	buf[1] = write_data;
	if (!i2c.write(buf, 2)) {
		LOGE("i2c write addr %x, data %x error!\n", write_addr, write_data);
		return false;
	}
    return true;
}

static void _release_layer() {
	RETURN_IF_FAIL(!FILE_EXIST("/tmp/zk_boot_anim"));

	int fd = open("/dev/disp", O_RDWR);
	if (fd < 0) {
		LOGE("Failed to open disp device, errno: %d\n", errno);
		return;
	}

	for (int ch = 0; ch < CHN_NUM - 1; ++ch) {
		for (int lyl = 0; lyl < LYL_NUM; ++lyl) {
			disp_layer_config config;
			memset(&config, 0, sizeof(disp_layer_config));
			config.channel = ch;
			config.layer_id = lyl;

			_layer_config(fd, DISP_LAYER_GET_CONFIG, &config);
			if (!config.enable) {
				continue;
			}
			if ((config.info.fb.format >= DISP_FORMAT_ARGB_8888) &&
				(config.info.fb.format <= DISP_FORMAT_BGRA_5551)) {
				continue;
			}

			config.enable = 0;
			_layer_config(fd, DISP_LAYER_SET_CONFIG, &config);
			LOGD("[hw] close channel[%d] layer_id[%d]\n", ch, lyl);
		}
	}

	close(fd);
}

typedef struct BrightnessLevel {
	int low;
	int high;
	int backlight;
} _s_brightness_level;

static _s_brightness_level brightnessLevelTab[] = {
	{0,    0,     0},
	{0,    2500,  0},
	{1500, 4500,  25},
	{3500, 6500,  50},
	{5500, 8500,  75},
	{7500, 65536, 100},
};

static void _set_automatic_brightness() {
	static int level = 0;
	if (!sys::setting::is_light_state()) {
		level = 0;
		return ;
	}
	uint16_t value = 0;
	if (!sys::get_light_sensor_data(&value)) {
		return ;
	}
	if ((value < brightnessLevelTab[level].high) && (value > brightnessLevelTab[level].low)) {
		return ;
	}
	for (int i = 0; i < 6; i ++) {
		if (value < brightnessLevelTab[i].high) {
			level = i;
			sys::setting::set_brightness((BRIGHTNESS_MAX_VAL - BRIGHTNESS_NIGHT) * \
					brightnessLevelTab[i].backlight / 100.0);
			Mutex::Autolock _l(_s_auto_brightness_mutex);
			SET_FOREACH(_s_auto_brightness_cb_set, auto_brightness_cb, it) {
				(*it)();
			}
			return ;
		}
	}
}

static void _adc_auto_switch_light(int chn) {
	AdcHelper::setChannel(chn);
	int val = AdcHelper::getVal();
	static int nightCount = 0;
	static int middleCount = 0;
	static int dayCount = 0;
	static LIGHT_MODE_E mode  = LIGHT_MODE_INVALID;
//	LOGE("_adc_auto_switch_light  val = %d", val);
	if(val > ADC_DAY_MODE_THRESHOLD) {
		if(++dayCount >= GET_ISP_COUNT){
			dayCount = 0;
			mode = LIGHT_MODE_DAY;
			sys::setting::set_brightness(BRIGHTNESS_DAY);
		}
		nightCount = 0;
		middleCount = 0;
	} else if((val > (ADC_NIGHT_MODE_THRESHOLD + 100)) && (val < (ADC_DAY_MODE_THRESHOLD - 100))) {
		if(++middleCount >= GET_ISP_COUNT){
			middleCount = 0;
			mode = LIGHT_MODE_MIDDLE;
			sys::setting::set_brightness(BRIGHTNESS_MIDDLE);
		}
		dayCount = 0;
		nightCount = 0;
	}  else if((val < ADC_NIGHT_MODE_THRESHOLD) && (val > 0)) {
		if(++nightCount >= GET_ISP_COUNT){
			nightCount = 0;
			mode = LIGHT_MODE_NIGHT;
			sys::setting::set_brightness(BRIGHTNESS_NIGHT);
		}
		dayCount = 0;
		middleCount = 0;
	} else {
		mode = LIGHT_MODE_INVALID;
		dayCount = 0;
		nightCount = 0;
	}
	if(mode != get_light_mode()){
		Mutex::Autolock _l(_s_light_mutex);
		_s_light_mode = mode;
		SET_FOREACH(_s_light_cb_set, light_det_cb, it) {
			(*it)(_s_light_mode);
		}
	}
}

static void _isp_auto_switch_light() {
	static int nightCount = 0;
	static int middleCount = 0;
	static int dayCount = 0;
	static LIGHT_MODE_E mode  = LIGHT_MODE_INVALID;
	int lightLevel = AW_MPI_ISP_GetEvLvAdj(0);
	if(lightLevel > DAY_MODE_THRESHOLD) {
		if(++dayCount >= GET_ISP_COUNT){
			dayCount = 0;
			mode = LIGHT_MODE_DAY;
			sys::setting::set_brightness(BRIGHTNESS_DAY);
		}
		nightCount = 0;
		middleCount = 0;
	} else if((lightLevel > (NIGHT_MODE_THRESHOLD + 80)) && (lightLevel < (DAY_MODE_THRESHOLD - 80))) {
		if(++middleCount >= GET_ISP_COUNT){
			middleCount = 0;
			mode = LIGHT_MODE_MIDDLE;
			sys::setting::set_brightness(BRIGHTNESS_MIDDLE);
		}
		dayCount = 0;
		nightCount = 0;
	}  else if((lightLevel < NIGHT_MODE_THRESHOLD) && (lightLevel > 0)) {
		if(++nightCount >= GET_ISP_COUNT){
			nightCount = 0;
			mode = LIGHT_MODE_NIGHT;
			sys::setting::set_brightness(BRIGHTNESS_NIGHT);
		}
		dayCount = 0;
		middleCount = 0;
	} else {
		mode = LIGHT_MODE_INVALID;
		dayCount = 0;
		nightCount = 0;
	}
	if(mode != get_light_mode()){
		Mutex::Autolock _l(_s_light_mutex);
		_s_light_mode = mode;
		SET_FOREACH(_s_light_cb_set, light_det_cb, it) {
			(*it)(_s_light_mode);
		}
	}
}

static int _read_temp() {
	char str[32] = {0};
	FILE *temp_value = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
	if (!temp_value) {
		return 0;
	}
	fread(str, 1, 8, temp_value);
	fclose(temp_value);
//	LOGD("str %s\n", str);
	return strtol(str, NULL, 10);
}

static void _temp_protect() {
	if (!sys::setting::is_temp_protect_enable()) return ;
	int temp_num = _read_temp();
	if (temp_num == 0) return ;
	Mutex::Autolock _l(_s_temp_mutex);
	SET_FOREACH(_s_temp_cb_set, temp_protect_cb, it) {
		(*it)(temp_num);
	}
}

#if ENABLE_AUX_DET
static bool _aux_is_insert() {
	return (GpioHelper::input(AUX_DET_IO) == 1);
}
#endif

static void* _det_thread(void *args) {
	LOGD("[hw] det thread start...\n");
//	auto_brightness_timer.reset();
	while (true) {
		DELAY(100);
#if ENABLE_AUX_DET
		if (_s_aux_is_insert != _aux_is_insert()) {
			_s_aux_is_insert = !_s_aux_is_insert;
			Mutex::Autolock _l(_s_mutex);
			SET_FOREACH(_s_cb_set, aux_det_cb, it) {
				(*it)(_s_aux_is_insert);
			}
		}
#endif
//		if(get_light_switch_status()) {
//			_isp_auto_switch_light();
//			_adc_auto_switch_light(ADC_CHANNEL1);
//		}
//		if ((auto_brightness_timer.elapsed() > 1000)) {
//			_set_automatic_brightness();
//			auto_brightness_timer.reset();
//		}
		if (temp_protect_timer.elapsed() > 5000) {
			_temp_protect();
			temp_protect_timer.reset();
		}
	}

	return NULL;
}


static void _custom_beep_cb() {
  if (!sys::setting::is_touch_sound_enabled()) {
    return;
  }
  static base::timer take_key_timer;
  if (take_key_timer.elapsed() < 300) {
    return ;
  }
  uart::set_touch_sound(true);
//  Keytone::instance().click();
  take_key_timer.reset();
}

void init() {
	_release_layer();
//	sys::ls_init();
	// 后拉摄像头上电
//	GpioHelper::output(CAMERA_POWER_IO, 1);
//	倒车脚的io  没用到
	GpioHelper::output(CAMERA_POWER_IO_19, 1);
	// 自定义按键音
	HARDWAREMANAGER->setCustomBeep(_custom_beep_cb);
#if ENABLE_AUX_DET
	_s_aux_is_insert = _aux_is_insert();
#endif
	fy::run_thread(_det_thread, NULL);
}

LIGHT_MODE_E get_light_mode() {
	Mutex::Autolock _l(_s_light_mutex);
	return _s_light_mode;
}

bool get_light_switch_status() {
	Mutex::Autolock _l(_s_light_mutex);
	return _enadble_auto_set_light;
}
void set_light_auto_switch(bool enable) {
	Mutex::Autolock _l(_s_light_mutex);
	_enadble_auto_set_light = enable;
}

void add_light_det_cb(light_det_cb cb) {
	Mutex::Autolock _l(_s_light_mutex);
	_s_light_cb_set.insert(cb);
}

void remove_light_det_cb(light_det_cb cb) {
	Mutex::Autolock _l(_s_light_mutex);
	_s_light_cb_set.erase(cb);
}

void add_aux_det_cb(aux_det_cb cb) {
	Mutex::Autolock _l(_s_mutex);
	_s_cb_set.insert(cb);
}

void remove_aux_det_cb(aux_det_cb cb) {
	Mutex::Autolock _l(_s_mutex);
	_s_cb_set.erase(cb);
}

bool aux_det_is_insert() {
	return _s_aux_is_insert;
}

void add_temp_protect_cb(temp_protect_cb cb) {
	Mutex::Autolock _l(_s_temp_mutex);
	_s_temp_cb_set.insert(cb);
}
void remove_temp_protect_cb(temp_protect_cb cb) {
	Mutex::Autolock _l(_s_temp_mutex);
	_s_temp_cb_set.erase(cb);
}

}
}


