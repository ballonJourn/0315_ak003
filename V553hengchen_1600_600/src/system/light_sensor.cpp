#include "utils/I2CHelper.h"
#include "misc/storage.h"
#include "light_sersor.h"
#include "hardware.h"
#include "common.h"

#define I2C_DEV       0
#define I2C_DEV_LEFT        3
#define I2C_LSENSOR_SLAVE_ADDR      0x49

namespace sys {

static I2CHelper i2c_right(I2C_DEV, I2C_LSENSOR_SLAVE_ADDR, 1000, 5);
//static I2CHelper i2c_left(I2C_DEV_LEFT, I2C_LSENSOR_SLAVE_ADDR, 1000, 5);

static bool right_init_status = false;
static bool left_init_status = false;

static bool _ls_get_data(I2CHelper &i2c_dev, uint16_t *IR_channel, uint16_t *ambient_channel, uint16_t *dark_channel) {
	bool ret = true;
	unsigned char data[6] = {0};
	unsigned char addr = 0x39;
	for (int i = 0; i < 6; i ++) {
		if (!sys::hw::_i2c_read_byte(i2c_dev, addr + i, data + i)) {
			ret = false;
			break;
		}
	}
	*IR_channel = (data[1] << 8) | data[0];
	*ambient_channel = (data[3] << 8) | data[2];
	*dark_channel = (data[5] << 8) | data[4];
	return ret;
}

void ls_init() {
	right_init_status = true;
//	left_init_status = true;
	unsigned char revision = 0;
//	sys::hw::_i2c_read_byte(i2c_left, 0x03, &revision);
	sys::hw::_i2c_read_byte(i2c_right, 0x03, &revision);
//	if (!sys::hw::_i2c_write_byte(i2c_left, 0x02, 0x01)) {
//		left_init_status = false;
//	}
	if (!sys::hw::_i2c_write_byte(i2c_right, 0x02, 0x01)) {
		right_init_status = false;
	}
	DELAY(100);
//	if (!sys::hw::_i2c_write_byte(i2c_left, 0x00, 0x01)) {
//		left_init_status = false;
//	}
	if (!sys::hw::_i2c_write_byte(i2c_right, 0x00, 0x01)) {
		right_init_status = false;
	}
//	if (!sys::hw::_i2c_write_byte(i2c_left, 0x02, 0x02)) {
//		left_init_status = false;
//	}
	if (!sys::hw::_i2c_write_byte(i2c_right, 0x02, 0x02)) {
		right_init_status = false;
	}
//	LOGD("[light_sensor] left init status = %d", left_init_status);
	LOGD("[light_sensor] right init status = %d", right_init_status);
}

bool get_light_sensor_data(uint16_t *value) {
	uint16_t left_data[3] = {0};
	uint16_t right_data[3] = {0};
	int effective_num = 2; // 参与返回值计算的参数数量
//	if (!left_init_status || !_ls_get_data(i2c_left, left_data, left_data + 1, left_data + 2))  {
//		left_data[1] = 0;
//		effective_num --;
//	}
	if (!right_init_status || !_ls_get_data(i2c_right, right_data, right_data + 1, right_data + 2)) {
		right_data[1] = 0;
		effective_num --;
	}
	if (effective_num == 0) {
		return false;
	}
	// 以ambient_channel的值作为光感检测的值
	*value = (left_data[1] + right_data[1]) / effective_num;
	return true;
}

}
