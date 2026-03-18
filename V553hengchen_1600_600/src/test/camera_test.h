/*
 * camera_test.h
 *
 *  Created on: 2024年4月12日
 *      Author: pengzc
 */

#ifndef TEST_CAMERA_TEST_H_
#define TEST_CAMERA_TEST_H_

#include "manual_test.h"

namespace test {

class CameraTest : public ManualTest {
public:
  using ManualTest::ManualTest;

  virtual bool test(Testing* t) override;
};

} /* namespace test */

#endif /* TEST_CAMERA_TEST_H_ */
