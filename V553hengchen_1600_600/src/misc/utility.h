/*
 * utility.h
 *
 *  Created on: 2024年4月3日
 *      Author: pengzc
 */

#ifndef MISC_UTILITY_H_
#define MISC_UTILITY_H_

#include "system/setting.h"

/*
 * 摄像头策略
 * 在需要显示的界面onUI_show同步打开摄像头
 * 退出界面时不关闭摄像头
 *
 */

void restartRecorder(bool force);
void setupCamera(ViewType view, bool is_sync);

template <typename Func>
void async(const Func& func) {
  std::thread t(func);
  t.detach();
}

enum {
  AUTH_STATE_UNKNOWN,
  AUTH_STATE_AUTHORIZED,
  AUTH_STATE_UNAUTHORIZED,
};

//实现于 navibar.cc
void showAuthDialog(int carplay, int timeout);
bool isAuthDialogVisible();

void showDebug();
void hideDebug();

void setTesting(bool testing);
bool isTesting();

void resetAllProcess();

#endif /* MISC_UTILITY_H_ */
