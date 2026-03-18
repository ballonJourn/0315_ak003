/*
 * manual_test_item.h
 *
 *  Created on: 2024年4月11日
 *      Author: pengzc
 */

#ifndef TEST_MANUAL_TEST_H_
#define TEST_MANUAL_TEST_H_

#include "test_item.h"
#include <window/ZKWindow.h>

namespace test {

class ManualTest : public TestItem {
public:
  explicit ManualTest(const std::string& name);
  explicit ManualTest(const std::string& name, ZKWindow* window);

  virtual ~ManualTest();

  virtual bool test(Testing* t) override;

};

} /* namespace test */

#endif /* TEST_MANUAL_TEST_H_ */
