/*
 * reverse_test.h
 *
 *  Created on: 2024年4月12日
 *      Author: pengzc
 */

#ifndef TEST_REVERSE_TEST_H_
#define TEST_REVERSE_TEST_H_

#include "test_item.h"

namespace test {

class ReverseTest : public TestItem {
public:
  ReverseTest();

  virtual bool test(Testing* t) override;
};

} /* namespace test */

#endif /* TEST_REVERSE_TEST_H_ */
