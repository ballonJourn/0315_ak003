/*
 * reverse.h
 *
 *  Created on: Dec 7, 2022
 *      Author: ZKSWE Develop Team
 */

#ifndef _SYSTEM_REVERSE_H_
#define _SYSTEM_REVERSE_H_

#define REVERSE_STATUS_ENTER    1    // 进入倒车
#define REVERSE_STATUS_EXIT     0    // 退出倒车

typedef void (*reverse_status_cb)(int status);

enum ReverseCameraState {
  REVERSE_CAMERA_STATE_UNDETECTED = 0,
  REVERSE_CAMERA_STATE_DETECTED = 1,
};

typedef void (*reverse_camera_state_cb)(ReverseCameraState state);

namespace sys {

// 开启倒车检测
bool reverse_detect_start();
// 关闭倒车检测
void reverse_detect_stop();

// 添加倒车状态回调
void reverse_add_status_cb(reverse_status_cb cb);
// 移除倒车状态回调
void reverse_remove_status_cb(reverse_status_cb cb);

// 是否在倒车状态
bool reverse_does_enter_status();

ReverseCameraState reverse_camera_state();
bool has_reverse_camera();
void reverse_add_camera_state_cb(const reverse_camera_state_cb& cb);
void reverse_remove_camera_state_cb(reverse_camera_state_cb cb);

}

#endif /* _SYSTEM_REVERSE_H_ */
