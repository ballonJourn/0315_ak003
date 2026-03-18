/*
 * test_item.h
 *
 *  Created on: 2024年4月11日
 *      Author: pengzc
 */

#ifndef TEST_TEST_ITEM_H_
#define TEST_TEST_ITEM_H_

#include <string>
#include <functional>
#include <window/ZKWindow.h>
#include "testing.h"

namespace test {

enum {
  STATE_NO_TEST,
  STATE_NOT_GOOD,
  STATE_PASSED,
};

class TestItem {
public:
  TestItem(const std::string& name, ZKWindow* window, bool automatic);
  virtual ~TestItem();

  std::string name() const;

  virtual bool test(Testing* t) = 0;

  int state() const;

  bool isAutomatic() const;

protected:
  bool awaitAndCountdown(Testing* t, const std::string& msg,
      const ConditionFunc& condition, int timeout, int interval = 50);

private:
  friend class Testing;

  void setState(int state);
  void setVisible(bool visible);

private:
  std::string name_;
  ZKWindow* window_;
  bool automatic_;

  int state_ = STATE_NO_TEST;
};



} /* namespace test */

#endif /* TEST_TEST_ITEM_H_ */
