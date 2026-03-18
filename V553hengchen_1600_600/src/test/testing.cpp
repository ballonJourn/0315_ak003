/*
 * testing.cpp
 *
 *  Created on: 2024年4月11日
 *      Author: pengzc
 */

#include <test/testing.h>
#include <thread>
#include "utils/Log.h"
#include <base/ui_utility.h>
#include "test_item.h"
#include <base/timer.hpp>
#include <base/strings.hpp>
#include <ini.h>

namespace test {

bool awaitTrue(const ConditionFunc& condition, int timeout, int interval) {
  base::timer timer;
  while (timer.elapsed() < timeout) {
    if (condition()) {
      return true;
    }
    usleep(1000 * interval);
  }
  return false;
}

Testing::Testing(): is_passed_(false) {

}

Testing::~Testing() {
  clear();
}

Testing& test::Testing::instance() {
  static Testing singleton;
  return singleton;
}

void Testing::bind(ZKWindow* test_window, ZKTextView* title_text,
    ZKTextView* message_text, ZKButton* pass_button, ZKButton* ng_button) {
  test_window_ = test_window;
  title_text_ = title_text;
  message_text_ = message_text;
  pass_button_ = pass_button;
  ng_button_ = ng_button;
}

inline std::string iniGet(ini_t* ini, const std::string& section,
    const std::string& key, const std::string& def) {
  auto ptr = ini_get(ini, section.c_str(), key.c_str());
  if (ptr) {
    return ptr;
  }
  return def;
}

Testing::Config Testing::load(const std::string& ini_file) {
  auto ini = ini_load(ini_file.c_str());
  if (!ini) {
    LOGE("load ini error");
    return config_;
  }
  config_.wifi.name = base::trim(iniGet(ini, "wifi", "name", ""));
  config_.wifi.password = base::trim(iniGet(ini, "wifi", "password", ""));

  ini_free(ini);
  return config_;
}

Testing::Config Testing::getConfig() const {
  return config_;
}

void Testing::pass() {
  is_passed_ = true;
  awake();
}

void Testing::ng() {
  is_passed_ = false;
  awake();
}

void Testing::setMessage(const std::string& str) {
  if (message_text_) {
    message_text_->setText(str);
  }
}

void Testing::add(TestItem* item) {
  items_.push_back(std::shared_ptr<TestItem>(item));
}

void Testing::clear() {
  items_.clear();
}

int Testing::size() const {
  return items_.size();
}

const TestItem* Testing::item(int index) const {
  if (index < 0 || index >= (int)items_.size()) {
    LOGE("index out of range: index = %d", index);
  }
  return items_.at(index).get();
}

bool Testing::await(int timeout) {
  std::unique_lock<std::mutex> lock(work_mutex_);
  auto status = work_cv_.wait_for(lock, std::chrono::milliseconds(timeout));
  if (status == std::cv_status::timeout) {
    return false;
  }
  return is_passed_;
}

bool Testing::await() {
  std::unique_lock<std::mutex> lock(work_mutex_);
  work_cv_.wait(lock);
  return is_passed_;
}

void Testing::awake() {
  std::unique_lock<std::mutex> lock_running(work_mutex_);
  work_cv_.notify_all();
}

bool Testing::isAllPassed() {
  for (auto item : items_) {
    if (item->state() != STATE_PASSED) {
      return false;
    }
  }
  return true;
}

void Testing::onAllPassed(const PassListener& listener) {
  //TODO NON THREAD-SAFE
  all_passed_listener_ = listener;
}

void Testing::start(int index) {
  std::thread backend([this, index]() {
    if (test_window_ == NULL
        || message_text_ == NULL
        || ng_button_ == NULL
        || pass_button_ == NULL) {
      LOGE("invalid views, you must bind first");
      return;
    }

    //先隐藏所有测试项的界面
    for (auto item : items_) {
      item->setVisible(false);
    }
    test_window_->setVisible(true);
    message_text_->setVisible(true);
    message_text_->setTouchPass(true);
    title_text_->setVisible(true);
    title_text_->setTouchPass(true);

    int counter = -1;
    for (auto& item : items_) {
      ++counter;
      if (counter < index) {
        continue;
      }
      title_text_->setText(std::string("正在测试 ") + item->name());
      message_text_->setText("");
      message_text_->setTextColor(0xffdddddd);
      item->setVisible(true);
      pass_button_->setVisible(!item->isAutomatic());
      ng_button_->setVisible(!item->isAutomatic());
      try {
        bool passed = item->test(this);
        item->setState(passed ? STATE_PASSED : STATE_NOT_GOOD);
        if (!passed) {
          break;
        }
      } catch (std::exception& e) {
        LOGE("Testing exception: %s", e.what());
        message_text_->setTextColor(0xffff4734);
        message_text_->setText(e.what());
        item->setState(STATE_NOT_GOOD);
        usleep(1000 * 1000);
        break;
      }
      item->setVisible(false);
      pass_button_->setVisible(false);
      ng_button_->setVisible(false);
    }
    pass_button_->setVisible(false);
    ng_button_->setVisible(false);
    test_window_->setVisible(false);
    message_text_->setVisible(false);
    title_text_->setVisible(false);

    //检查是否已经全部通过
    if (isAllPassed() && all_passed_listener_) {
      all_passed_listener_();
    }
  });
  backend.detach();
}

} /* namespace test */

