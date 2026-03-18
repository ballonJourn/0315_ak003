/*
 * progress_dialog.cpp
 *
 *  Created on: 2021年10月30日
 *      Author: pengzc
 */

#include "popup_service.h"

#include <entry/EasyuiContext.h>
#include <utils/Log.h>
#include <base/os.hpp>
#include <thread>
#include <base/ui_handler.h>
#include <manager/LanguageManager.h>

void NavibarSetProgressMessage(const std::string& msg);
void NavibarSetProgressState(PopupProgressState state);
void NavibarSetProgressWindowVisible(bool visible);
void NavibarSetDialogWindowVisible(bool visible);
void NavibarShowDelayed(int delay);
void NavibarHideDelayed(int min_visible_duration);

void NavibarSetDialog(const std::string& title, const std::string& msg,
    std::vector<PopupButton>& buttons, const LayoutPosition& pos);

PopupButton::PopupButton(const std::string& text) {
  this->text = text;
}

PopupButton::PopupButton(const std::string& text,
    const base::function<void()>& click) {
  this->text = text;
  this->click = click;
}

void PopupButton::onClick(ZKBase *pBase) {
  if (click) {
    click();
  }
  PopupService::instance()->closeDialog();
}

PopupService::PopupService() {
  busy_ = false;
  mode_ = PopupMode::Transient;
}

PopupService::~PopupService() {
}

PopupService* PopupService::instance() {
  static PopupService singleton;
  return &singleton;
}

void PopupService::dialog(const std::string& title, const std::string& msg,
    const std::vector<PopupButton>& buttons, int timeout,
    const LayoutPosition& pos) {
  instance()->dialog2(title, msg, buttons, timeout, pos);
}

void PopupService::dialog(const std::string& title, const std::string& msg,
    int timeout) {
  std::vector<PopupButton> buttons;
  if (timeout <= 0) {
    buttons.push_back( { "" });
  }
  instance()->dialog2(title, msg, buttons, timeout, {0,0,0,0});
}

bool PopupService::isBusy() {
  return instance()->busy_;
}

void PopupService::setState(PopupProgressState state) {
  NavibarSetProgressState(state);
}

void PopupService::setMode(PopupMode mode) {
  mode_ = mode;
}

void PopupService::step(const std::string& name, PopupProgressState state,
    int display_time) {
  setState(state);
  setMessage(name);
  usleep(display_time < 0 ? 0 : (display_time * 1000));
  setState(PopupProgressState::Running);
}

void PopupService::failure(const std::string& name, int display_time) {
  step(name, PopupProgressState::Failure, display_time);
}

void PopupService::success(const std::string& name, int display_time) {
  step(name, PopupProgressState::Success, display_time);
}

bool PopupService::show2(const Task& func) {
  std::lock_guard<std::recursive_mutex> lock(mutex_);
  if (busy_) {
    LOGD("popup service is busy");
    return false;
  }
  busy_ = true;
  std::thread backend([this, func]() {
    base::screensaver_guard guard;
    mode_ = PopupMode::Transient;
    NavibarShowDelayed(500);
    NavibarSetProgressMessage("");
    NavibarSetProgressWindowVisible(true);
    NavibarSetDialogWindowVisible(false);
    NavibarSetProgressState(PopupProgressState::Running);
    int ret = func(this);
    NavibarSetProgressState(0 == ret ?
        PopupProgressState::Success : PopupProgressState::Failure);
    if (0 == ret) {
      if (mode_ == PopupMode::Longer) {
        usleep(1000 * 1500);
      }
      NavibarHideDelayed(700);
    } else {
      usleep(1000 * 2000);
      NavibarHideDelayed(0);
    }
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    busy_ = false;
  });
  backend.detach();
  return true;
}

void PopupService::dialog2(const std::string& title, const std::string& msg,
    const std::vector<PopupButton>& buttons, int timeout,
    const LayoutPosition& pos) {
  std::lock_guard<std::recursive_mutex> lock(mutex_);
  if (busy_) {
    return;
  }
  busy_ = true;
  buttons_.clear();
  for (auto btn : buttons) {
    buttons_.push_back({tr(btn.text), btn.click});
  }
  NavibarSetDialog(tr(title), tr(msg), buttons_, pos);
  NavibarSetDialogWindowVisible(true);
  NavibarSetProgressWindowVisible(false);
  NavibarShowDelayed(0);

  const char* close_dialog = "closeDialog";
  if (timeout > 0) {
    base::runInUiThreadUniqueDelayed(close_dialog, [this](){
      closeDialog();
    }, timeout);
  }
}

void PopupService::closeDialog() {
  std::lock_guard<std::recursive_mutex> lock(PopupService::instance()->mutex_);
  NavibarSetDialogWindowVisible(false);
  NavibarHideDelayed(0);
  PopupService::instance()->busy_ = false;
}

bool PopupService::show(Task task, PopupMode mode) {
  instance()->setMode(mode);
  return instance()->show2(task);
}

bool PopupService::show(Task task) {
  return instance()->show2(task);
}

void PopupService::setMessage(const std::string& msg) {
  NavibarSetProgressMessage(tr(msg));
}

std::string PopupService::tr(const std::string& at_str) {
  if (at_str.empty()) {
    return at_str;
  }
  if (at_str.front() != '@') {
    return at_str;
  }
  auto translate = LANGUAGEMANAGER->getValue(at_str.c_str() + 1);
  if (translate.empty()) {
    LOGE("not found i18n '%s' string in language '%s'", at_str.c_str() + 1,
        LANGUAGEMANAGER->getCurrentLanguage().c_str());
    return at_str;
  }
  return translate;
}
