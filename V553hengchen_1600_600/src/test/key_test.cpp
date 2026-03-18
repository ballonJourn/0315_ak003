/*
 * key_test.cpp
 *
 *  Created on: 2024年4月12日
 *      Author: pengzc
 */

#include <test/key_test.h>
#include <base/timer.hpp>
#include <entry/EasyUIContext.h>
#include <functional>

namespace test {

namespace {

class MyKeyListener: public EasyUIContext::IKeyListener {
public:
  typedef std::function<bool (const KeyEvent &ke)> EventListener;
  MyKeyListener(const EventListener& listener) : listener_(listener) {
    EASYUICONTEXT->registerKeyListener(this);
  }

  virtual ~MyKeyListener() {
    EASYUICONTEXT->unregisterKeyListener(this);
  }

  virtual bool onKeyEvent(const KeyEvent &ke) override {
    if (listener_) {
      return listener_(ke);
    }
    return false;
  }

private:
  EventListener listener_;
};

}


KeyTest::KeyTest() : TestItem("按键", NULL, true) {
}

KeyTest::~KeyTest() {
}

bool KeyTest::test(Testing* t) {
  std::string msg = "在5秒内点按电源键";
  t->setMessage(msg);
  bool down = false;
  bool up = false;
  MyKeyListener listener([t, msg, &down, &up](const KeyEvent &ke) -> bool {
    switch (ke.mKeyStatus) {
      case KeyEvent::E_KEY_DOWN:
        t->setMessage(msg + "\n已检测到按下");
        down = true;
        break;
      case KeyEvent::E_KEY_UP:
        t->setMessage(msg + "\n已检测到抬起");
        up = true;
        break;
      default:
        break;
    }
    return true;
  });
  base::timer timer;
  while (timer.elapsed() < 5000) {
    if (down && up) {
      break;
    }
    usleep(1000 * 100);
  }
  return down && up;
}

} /* namespace test */

