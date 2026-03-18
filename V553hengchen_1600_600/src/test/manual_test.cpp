/*
 * manual_test_item.cpp
 *
 *  Created on: 2024年4月11日
 *      Author: pengzc
 */

#include <test/manual_test.h>
#include "testing.h"

namespace test {

ManualTest::ManualTest(const std::string& name) :
    TestItem(name, NULL, false) {
}

ManualTest::ManualTest(const std::string& name, ZKWindow* window) :
    TestItem(name, window, false) {
}

ManualTest::~ManualTest() {
}

bool ManualTest::test(Testing* t) {
  return t->await();
}

} /* namespace test */

