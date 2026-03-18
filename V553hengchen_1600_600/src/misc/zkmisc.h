/*
 * zkmisc.h
 *
 *  Created on: 2023年8月19日
 *      Author: ZKSWE Develop Team
 */

#ifndef _ZK_MISC_H_
#define _ZK_MISC_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

uint8_t* zk_misc_load_data(const char *name, uint32_t *size);
int zk_misc_save_file(const char *name, const char *file);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _ZK_MISC_H_ */
