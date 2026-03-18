/*
 * carplay_auth_test.h
 *
 *  Created on: 2024年4月13日
 *      Author: pengzc
 */

#ifndef TEST_CARPLAY_AUTH_TEST_H_
#define TEST_CARPLAY_AUTH_TEST_H_

#include <test/test_item.h>

namespace test {

class CarplayAuthTest: public TestItem {
public:
  CarplayAuthTest();

  virtual bool test(Testing* t) override;
};

} /* namespace test */

#endif /* TEST_CARPLAY_AUTH_TEST_H_ */
