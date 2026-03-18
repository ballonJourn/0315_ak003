/*
 * mount_test_item.h
 *
 *  Created on: 2024年4月11日
 *      Author: pengzc
 */

#ifndef TEST_MOUNT_TEST_H_
#define TEST_MOUNT_TEST_H_

#include "test_item.h"

namespace test {

class MountTest : public TestItem {
public:
  MountTest();

  virtual bool test(Testing* t) override;

};

} /* namespace test */

#endif /* TEST_MOUNT_TEST_H_ */
