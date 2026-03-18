/*
 * reverse_test.cpp
 *
 *  Created on: 2024年4月12日
 *      Author: pengzc
 */

#include <test/reverse_test.h>
#include "system/reverse.h"
#include <base/timer.hpp>

namespace test {

ReverseTest::ReverseTest() : TestItem("倒车信号", NULL, true) {
}

bool ReverseTest::test(Testing* t) {
  auto in = []() {
    return sys::reverse_does_enter_status();
  };

  if (!awaitAndCountdown(t, "现在接入倒车信号", in, 5000)) {
    throw std::logic_error("未检测到倒车信号");
  }
  auto out = []() {
    return !sys::reverse_does_enter_status();
  };
  if (!awaitAndCountdown(t, "已检测到倒车信号，现在退出倒车信号", out, 5000)) {
    throw std::logic_error("未退出倒车信号");
  }
  return true;
}

} /* namespace test */
