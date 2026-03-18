/*
 * fm_emit.cpp
 *
 *  Created on: 2023年5月19日
 *      Author: ZKSWE Develop Team
 */

#include "utils/I2CHelper.h"
#include "misc/storage.h"
#include "fm_emit.h"
#include "hardware.h"
#include "common.h"

#define I2C_DEV         1
#define I2C_SLAVE_ADDR  0x2c

#define CH_STEP         0x00
#define CH              0x01
#define CH_CH           0x03
#define FREQ2CHREG(freq)   ((freq-7600)/5)
#define CHREG2FREQ(ch)     (ch*5+7600)

#define FM_FREQ_KEY     "fm_freq"
#define DEFAULT_FREQ    9600

namespace sys {

bool fm_start() {
    I2CHelper i2c(I2C_DEV, I2C_SLAVE_ADDR, 1000, 5);
    uint8_t chipid0 = 0, chipid1 = 0;
    bool ret = false;

    do {
    	if (!sys::hw::_i2c_read_byte(i2c, 0x05, &chipid0)) {
    		break;
    	}
    	if (!sys::hw::_i2c_read_byte(i2c, 0x06, &chipid1)) {
    		break;
    	}

		LOGD("[fm] chipid: 0x%x%x\n", chipid0, chipid1);

		// Set the All the register to default values
		if (!sys::hw::_i2c_write_byte(i2c, 0x00, 0x81)) {
			break;
		}

		DELAY(20);

		// Using the default setting
		if (!sys::hw::_i2c_write_byte(i2c, 0x03, 0x10)) {
			break;
		}
		// Set the OSC frequency : 0x33 12MHz ;0xb3:24M
		if (!sys::hw::_i2c_write_byte(i2c, 0x04, 0xb3)) {
			break;
		}
		if (!sys::hw::_i2c_write_byte(i2c, 0x00, 0x41)) {
			break;
		}
		if (!sys::hw::_i2c_write_byte(i2c, 0x00, 0x1)) {
			break;
		}

		DELAY(20);

		//SNR
		if (!sys::hw::_i2c_write_byte(i2c, 0x18, 0xe4)) {
			break;
		}
		//freq
		if (!sys::hw::_i2c_write_byte(i2c, 0x01, 0x7e)) {
			break;
		}
		//cancel pa func
		if (!sys::hw::_i2c_write_byte(i2c, 0x02, 0xb9)) {
			break;
		}
		//start
		if (!sys::hw::_i2c_write_byte(i2c, 0x00, 0x02)) {
			break;
		}

		ret = true;

		LOGD("[fm] start ok\n");
    } while (0);

	return ret;
}

bool fm_set_freq(int freq) {
	// calculate ch parameter used for register setting
	uint8_t tStep;
	uint8_t tCh;
	uint16_t f;
	f = FREQ2CHREG(freq);

	// set to reg: CH
	tCh = (uint8_t) f;

	I2CHelper i2c(I2C_DEV, I2C_SLAVE_ADDR, 1000, 5);

	RETURN_VALUE_IF_FAIL(sys::hw::_i2c_write_byte(i2c, CH, tCh), false);
	RETURN_VALUE_IF_FAIL(sys::hw::_i2c_read_byte(i2c, CH_STEP, &tStep), false);

	// set to reg: CH_STEP
	tStep &= ~CH_CH;
	tStep |= ((uint8_t) (f >> 8) & CH_CH);
	RETURN_VALUE_IF_FAIL(sys::hw::_i2c_write_byte(i2c, CH_STEP, tStep), false);

	storage::put_int(FM_FREQ_KEY, freq);
	LOGD("[fm] set freq %.1f MHZ ok\n", freq / 100.f);

	return true;
}

int fm_get_freq() {
	return storage::get_int(FM_FREQ_KEY, DEFAULT_FREQ);
}

bool fm_set_mute(bool mute) {
	I2CHelper i2c(I2C_DEV, I2C_SLAVE_ADDR, 1000, 5);

	uint8_t tValue;
	RETURN_VALUE_IF_FAIL(sys::hw::_i2c_read_byte(i2c, 0x00, &tValue), false);

	/*modify start*/
	tValue &= 0x80;
	tValue |= 0x1;
	/*modify end*/

	if (mute) {
		tValue |= 0x08;
		tValue &= ~0x20;		// 关闭transmit
	} else {
		tValue &= ~0x08;
		tValue |= 0x20;			// 打开transmit
	}

	RETURN_VALUE_IF_FAIL(sys::hw::_i2c_write_byte(i2c, 0x00, tValue), false);
	LOGD("[fm] set mute %d ok\n", mute);

	return true;
}

void fm_dump() {
	uint8_t mem = 0;
	I2CHelper i2c(I2C_DEV, I2C_SLAVE_ADDR, 1000, 5);

	for (int i = 0; i < 0x13; i++) {
		RETURN_IF_FAIL(sys::hw::_i2c_read_byte(i2c, i, &mem));
		LOGD("[fm]: [0x%x]=0x%x\n", i, mem);
	}
}

}
