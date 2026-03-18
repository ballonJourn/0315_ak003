/*
 * aux_test.h
 *
 *  Created on: 2024年4月13日
 *      Author: pengzc
 */

#ifndef TEST_AUX_TEST_H_
#define TEST_AUX_TEST_H_

#include <test/test_item.h>

namespace test {

class AuxTest: public TestItem {
public:
  AuxTest();

  virtual bool test(Testing* t) override;
};

} /* namespace test */

#endif /* TEST_AUX_TEST_H_ */
