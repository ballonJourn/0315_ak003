/*
 * bluetooth_auth_test.h
 *
 *  Created on: 2024年4月13日
 *      Author: pengzc
 */

#ifndef LOGIC_BLUETOOTH_AUTH_TEST_H_
#define LOGIC_BLUETOOTH_AUTH_TEST_H_

#include <test/test_item.h>

namespace test {

class BluetoothAuthTest: public TestItem {
public:
  BluetoothAuthTest();

  virtual bool test(Testing* t) override;
};

} /* namespace test */

#endif /* LOGIC_BLUETOOTH_AUTH_TEST_H_ */
