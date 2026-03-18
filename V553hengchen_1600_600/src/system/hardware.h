/*
 * hardware.h
 *
 *  Created on: 2024年3月6日
 *      Author: guoxs
 */

#ifndef _SYSTEM_HARDWARE_H_
#define _SYSTEM_HARDWARE_H_
#include "utils/I2CHelper.h"

typedef enum {
	LIGHT_MODE_INVALID,
	LIGHT_MODE_DAY,
	LIGHT_MODE_MIDDLE,
	LIGHT_MODE_NIGHT
}LIGHT_MODE_E;

typedef void (*aux_det_cb)(bool insert);
typedef void (*light_det_cb)(LIGHT_MODE_E day);
typedef void (*auto_brightness_cb)(void);
typedef void (*temp_protect_cb)(int num);

namespace sys {
namespace hw {

void init();

LIGHT_MODE_E get_light_mode();
bool get_light_switch_status();
void set_light_auto_switch(bool enable);
void add_light_det_cb(light_det_cb cb);
void remove_light_det_cb(light_det_cb cb);

void add_aux_det_cb(aux_det_cb cb);
void remove_aux_det_cb(aux_det_cb cb);
bool aux_det_is_insert();

void add_temp_protect_cb(temp_protect_cb cb);
void remove_temp_protect_cb(temp_protect_cb cb);

bool _i2c_read_byte(I2CHelper &i2c, uint8_t read_addr, uint8_t *read_data);
bool _i2c_write_byte(I2CHelper &i2c, uint8_t write_addr, uint8_t write_data);

}
}

#endif /* _SYSTEM_HARDWARE_H_ */
