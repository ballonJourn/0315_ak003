/*
 * ntp_test.h
 *
 *  Created on: 2024年4月13日
 *      Author: pengzc
 */

#ifndef TEST_NTP_TEST_H_
#define TEST_NTP_TEST_H_

#include <test/test_item.h>

namespace test {

class NtpTest: public TestItem {
public:
  NtpTest();

  virtual bool test(Testing* t) override;

};

} /* namespace test */

#endif /* TEST_NTP_TEST_H_ */
