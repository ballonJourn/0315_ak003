/*
 * fm_emit.h
 *
 *  Created on: 2023年5月19日
 *      Author: ZKSWE Develop Team
 */

#ifndef _SYSTEM_FM_EMIT_H_
#define _SYSTEM_FM_EMIT_H_

#include <stdint.h>

namespace sys {

bool fm_start();
bool fm_set_freq(int freq);
int fm_get_freq();
bool fm_set_mute(bool mute);
void fm_dump();

}

#endif /* _SYSTEM_FM_EMIT_H_ */
