/*
 * test_item.cpp
 *
 *  Created on: 2024年4月11日
 *      Author: pengzc
 */

#include <test/test_item.h>
#include <base/timer.hpp>
#include <base/strings.hpp>

namespace test {

TestItem::TestItem(const std::string& name, ZKWindow* window, bool automatic) :
    name_(name), window_(window), automatic_(automatic) {
}

TestItem::~TestItem() {
}

std::string TestItem::name() const {
  return name_;
}

int TestItem::state() const {
  return state_;
}

void TestItem::setState(int state) {
  state_ = state;
}

void TestItem::setVisible(bool visible) {
  if (window_) {
    window_->setVisible(visible);
  }
}

bool TestItem::isAutomatic() const {
  return automatic_;
}

bool TestItem::awaitAndCountdown(Testing* t, const std::string& msg,
    const ConditionFunc& condition, int timeout, int interval) {
  base::timer timer;
  t->setMessage(msg);
  while (true) {
    if (condition()) {
      return true;
    }
    int64_t left = timeout - timer.elapsed();
    if (left < 0) {
      return false;
    }
    t->setMessage(base::format("%s\n%lld秒", msg.c_str(), left / 1000));
    usleep(1000 * interval);
  }
  return false;
}

} /* namespace test */

