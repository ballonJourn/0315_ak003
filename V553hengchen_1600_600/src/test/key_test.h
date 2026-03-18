/*
 * key_test.h
 *
 *  Created on: 2024年4月12日
 *      Author: pengzc
 */

#ifndef TEST_KEY_TEST_H_
#define TEST_KEY_TEST_H_

#include "test_item.h"

namespace test {

class KeyTest : public TestItem {
public:
  KeyTest();
  virtual ~KeyTest();

  virtual bool test(Testing* t) override;

};

} /* namespace test */

#endif /* TEST_KEY_TEST_H_ */
