#ifndef _LIGHT_SERSOR_H_
#define _LIGHT_SERSOR_H_

#include <stdint.h>

namespace sys {
	void ls_init();
	bool get_light_sensor_data(uint16_t *value);
}

#endif /* _LIGHT_SERSOR_H_ */
