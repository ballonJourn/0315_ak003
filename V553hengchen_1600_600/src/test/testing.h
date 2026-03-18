/*
 * testing.h
 *
 *  Created on: 2024年4月11日
 *      Author: pengzc
 */

#ifndef TEST_TESTING_H_
#define TEST_TESTING_H_

#include <vector>
#include <mutex>
#include <memory>
#include <condition_variable>
#include <control/ZKTextView.h>
#include <control/ZKButton.h>
#include <window/ZKWindow.h>

namespace test {

typedef std::function<bool ()> ConditionFunc;
bool awaitTrue(const ConditionFunc& condition, int timeout, int interval = 50);

class TestItem;

class Testing {
public:
  Testing();
  virtual ~Testing();

  Testing(const Testing&) = delete;
  Testing& operator=(const Testing&) = delete;

  static Testing& instance();

  void bind(ZKWindow* test_window,
      ZKTextView* title_text,
      ZKTextView* message_text,
      ZKButton* pass_button, ZKButton* ng_button);

  struct Config {
    struct {
      std::string name;
      std::string password;
    } wifi;
  };

  Config load(const std::string& ini_file);
  Config getConfig() const;

  bool await();
  bool await(int timeout);

  void pass();
  void ng();

  void setMessage(const std::string& str);

  void add(TestItem* item);
  void clear();

  int size() const;
  const TestItem* item(int index) const;

  void start(int index);

  typedef std::function<void ()> PassListener;
  void onAllPassed(const PassListener& listener);

private:
  void awake();
  bool isAllPassed();

private:
  std::mutex work_mutex_;
  std::condition_variable work_cv_;
  bool is_passed_;

  std::vector<std::shared_ptr<TestItem>> items_;

  ZKWindow* test_window_ = NULL;
  ZKTextView* title_text_ = NULL;
  ZKTextView* message_text_ = NULL;
  ZKButton* pass_button_ = NULL;
  ZKButton* ng_button_ = NULL;

  Config config_;
  PassListener all_passed_listener_;
};

} /* namespace test */

#endif /* TEST_TESTING_H_ */
