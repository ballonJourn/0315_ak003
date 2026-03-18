/*
 * mode_observer.h
 *
 *  Created on: 2024年3月2日
 *      Author: guoxs
 */

#ifndef _LOGIC_MODE_OBSERVER_H_
#define _LOGIC_MODE_OBSERVER_H_

#include "system/setting.h"

/*
 * 事件模式
 */
typedef enum {
	E_EVENT_MODE_BRIGHTNESS,   // 亮度调节事件
	E_EVENT_MODE_VOICE,        // 声音调节事件
	E_EVENT_MODE_GOBACK,       // 返回事件
	E_EVENT_MODE_HOME,         // home事件
} event_mode_e;

/*
 * 切换模式
 */
typedef enum {
	E_SWITCH_MODE_SETUP,       // 设置模式
	E_SWITCH_MODE_DVR,         // dvr模式
	E_SWITCH_MODE_LINK,        // 互联模式
	E_SWITCH_MODE_GOBACK,      // 返回模式
} switch_mode_e;

typedef void (*event_mode_cb) (event_mode_e mode);
typedef void (*switch_mode_cb) (switch_mode_e mode);
typedef void (*sound_mode_cb) (sound_mode_e mode);

namespace mode {

// 以下接口非线程安全，请在ui线程中调用
void set_event_mode(event_mode_e mode);
event_mode_e get_event_mode();
void add_event_mode_cb(event_mode_cb cb);
void remove_event_mode_cb(event_mode_cb cb);

void set_switch_mode(switch_mode_e mode);
switch_mode_e get_switch_mode();
void add_switch_mode_cb(switch_mode_cb cb);
void remove_switch_mode_cb(switch_mode_cb cb);

void set_sound_mode(sound_mode_e mode);
sound_mode_e get_sound_mode();
void add_sound_mode_cb(sound_mode_cb cb);
void remove_sound_mode_cb(sound_mode_cb cb);

}

#endif /* _LOGIC_MODE_OBSERVER_H_ */
