/*
 * camera_test.cpp
 *
 *  Created on: 2024年4月12日
 *      Author: pengzc
 */
#include "misc/utility.h"

#include <test/camera_test.h>

namespace test {

bool CameraTest::test(Testing* t) {
  setupCamera(VIEW_TYPE_BOTH, true);
  return t->await();
}

} /* namespace test */
