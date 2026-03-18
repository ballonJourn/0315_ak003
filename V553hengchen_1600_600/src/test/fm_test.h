/*
 * fm_test.h
 *
 *  Created on: 2024年4月13日
 *      Author: pengzc
 */

#ifndef TEST_FM_TEST_H_
#define TEST_FM_TEST_H_

#include "test_item.h"

namespace test {

class FmTest: public TestItem {
public:
  FmTest();

  virtual bool test(Testing* t) override;
};

} /* namespace test */

#endif /* TEST_FM_TEST_H_ */
