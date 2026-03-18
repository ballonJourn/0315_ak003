/*
 * progress_dialog.h
 *
 *  Created on: 2021年10月30日
 *      Author: pengzc
 */

#ifndef JNI_CORE_POPUP_SERVICE_H_
#define JNI_CORE_POPUP_SERVICE_H_

#include <atomic>
#include <string>
#include <base/base.h>
#include <control/ZKBase.h>
#include <mutex>

enum class PopupMode {
  Transient,
  Longer,
};

enum class PopupProgressState {
  Running = 1,
  Success,
  Failure,
};

class PopupButton : public ZKBase::IClickListener {
public:
  PopupButton(const std::string& text);
  PopupButton(const std::string& text, const base::function<void()>& click);

  std::string text;
  base::function<void()> click;

protected:
  virtual void onClick(ZKBase *pBase);
};

class PopupService {
public:
  virtual ~PopupService();

  typedef std::function<int (PopupService* srv)> Task;

  static bool show(Task task);
  static bool show(Task task, PopupMode mode);
  static void dialog(const std::string& title, const std::string& msg,
      const std::vector<PopupButton>& buttons, int timeout,
      const LayoutPosition& pos = {0,0,0,0});

  static void dialog(const std::string& title, const std::string& msg,
      int timeout);

  void setMessage(const std::string& msg);

  void setState(PopupProgressState state);
  void setMode(PopupMode mode);

  void failure(const std::string& name, int display_time = 2000);
  void success(const std::string& name, int display_time = 2000);

  static bool isBusy();

  static void closeDialog();

private:
  PopupService();
  static PopupService* instance();

  bool show2(const Task& func);
  void step(const std::string& name, PopupProgressState state,
      int display_time);

  void dialog2(const std::string& title, const std::string& msg,
      const std::vector<PopupButton>& buttons, int timeout,
      const LayoutPosition& pos);


  friend void NavibarDialog1ClickOk();
  friend class PopupButton;

  /**
   * 翻译@开头的字符串
   * @param at_str
   * @return
   */
  std::string tr(const std::string& at_str);

private:
  std::recursive_mutex mutex_;

  std::atomic<bool> busy_;
  PopupMode mode_;

  std::vector<PopupButton> buttons_;
};

#endif /* JNI_CORE_POPUP_SERVICE_H_ */
