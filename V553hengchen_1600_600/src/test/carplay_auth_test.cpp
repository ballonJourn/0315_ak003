/*
 * carplay_auth_test.cpp
 *
 *  Created on: 2024年4月13日
 *      Author: pengzc
 */

#include <test/carplay_auth_test.h>
#include <base/timer.hpp>
#include <base/strings.hpp>
#include "link/context.h"

namespace test {

CarplayAuthTest::CarplayAuthTest(): TestItem("Carplay授权", NULL, true) {

}

bool CarplayAuthTest::test(Testing* t) {
  auto auth = []() {
    return lk::get_is_authorized();
  };
  if (!awaitAndCountdown(t, "等待Carplay授权", auth, 30 * 1000)) {
    throw std::logic_error("Carplay未授权");
  }
  return true;
}

} /* namespace test */
