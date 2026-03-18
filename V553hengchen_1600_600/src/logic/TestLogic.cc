#pragma once
#include "uart/ProtocolSender.h"

#include "config.h"
#include <net/NetManager.h>
#include <os/SystemProperties.h>
#include <memory>
#include <base/base.hpp>
#include <base/strings.hpp>
#include <base/files.hpp>
#include <base/time.hpp>
#include "misc/utility.h"
#include "version.h"
#include "ntp/ntp.h"

#include "test/testing.h"
#include "test/manual_test.h"
#include "test/mount_test.h"
#include "test/camera_test.h"
#include "test/audio_test.h"
#include "test/key_test.h"
#include "test/reverse_test.h"
#include "test/fm_test.h"
#include "test/aux_test.h"
#include "test/carplay_auth_test.h"
#include "test/bluetooth_auth_test.h"

//#undef LOG_TAG
//#define LOG_TAG "test"

namespace {

bool connectWifi(const std::string& name, const std::string& password) {
  auto wifi = NETMANAGER->getWifiManager();

  LOGD("wifi %s %s", name.c_str(), password.c_str());
#if 0
  if (wifi->isConnected() && wifi->getConnectionInfo()->getSsid() == name) {
    LOGD("already connected");
    wifi->disconnect();
    return true;
  }
#endif

  wifi->enableWifi(false);
  usleep(1000 * 2000); //must

  wifi->enableWifi(true);
  auto enabled = [wifi](){
    return wifi->isWifiEnable();
  };
  LOGD("await wifi enabled");
  if (!test::awaitTrue(enabled, 10 * 1000)) {
    LOGE("enable wifi error");
    return false;
  }
  wifi->startScan();
  auto scanded = [wifi, name](){
    std::vector<WifiInfo> infos;
    wifi->getWifiScanInfosLock(infos);
    for (auto info : infos) {
      if (info.getSsid() == name) {
        return true;
      }
    }
    return false;
  };
  if (!test::awaitTrue(scanded, 30 * 1000, 1000)) {
    LOGE("not found wifi which named of '%s'", name.c_str());
    return false;
  }
  wifi->connect(name, password);

  auto connected = [wifi]() {
    return wifi->isConnected();
  };

  LOGD("await connect");
  if (!test::awaitTrue(connected, 30 * 1000)) {
    LOGE("connect error");
    return false;
  }
  LOGD("wifi connect success");
  return true;
}

void updateVersions() {
  std::string txt;
  txt += "OS:  ";
  txt += sys::setting::get_os_version();
  txt += "\n\n";
  txt += "APP:  ";
  txt += sys::setting::get_app_softver();
  mTextViewVersionsPtr->setText(txt);
}

void updateTime() {
  auto now = base::DateTime::now();
  mTextViewTimePtr->setText(now.toString());
  mTextViewTimePtr->setBackgroundColor(
      now.year < 2024 ? 0xffff4734 : 0xff3aac54);
}

enum {
  TIMER_TIME,
};


} /* anonymous namespace */

/**
 * 注册定时器
 * 填充数组用于注册定时器
 * 注意：id不能重复
 */
static S_ACTIVITY_TIMEER REGISTER_ACTIVITY_TIMER_TAB[] = {
	//{0,  6000}, //定时器id=0, 时间间隔6秒
	{TIMER_TIME,  1000},
};

/**
 * 当界面构造时触发
 */
static void onUI_init() {
  setTesting(true);
  mpi::Recorder::instance().stop();

  updateVersions();
  updateTime();

  EASYUICONTEXT->hideNaviBar();
  EASYUICONTEXT->setScreensaverEnable(false);
  EASYUICONTEXT->screensaverOff();

  mWindowTestPtr->setVisible(false);
  mTextViewTestMessagePtr->setVisible(false);
  mTextViewTestTitlePtr->setVisible(false);
  mListViewTestPtr->setVisible(true);
  mButtonPassPtr->setVisible(false);
  mButtonPassPtr->setAlpha(220);
  mButtonNGPtr->setVisible(false);
  mButtonNGPtr->setAlpha(220);

  //TouchTest
  mPainterTouchPtr->setLineWidth(3);
  mPainterTouchPtr->setSourceColor(0xffff0000);

  auto& t = test::Testing::instance();
  auto config = t.load(
      base::Path::get(TFCARD_MOUNT_POINT, PRODUCT_TEST_FILE_NAME));

  async([config]() {
    while (true) {
      if(connectWifi(config.wifi.name, config.wifi.password)) {
        ntp::startSynchronizationTask(ntp::defaultServerList(),
            [](const std::string& server, const tm*) {
         //应用没有使用时区，这里固定设置为北京时间
         auto beijing_timestamp = time(NULL) + 60 * 60 * 8;
         base::setSystemDateTime(beijing_timestamp);
         updateTime();
         });
        break;
      }
    }
  });

  t.bind(mWindowTestPtr, mTextViewTestTitlePtr, mTextViewTestMessagePtr,
      mButtonPassPtr, mButtonNGPtr);
  t.clear();

  t.add(new test::ManualTest("触摸", mWindowTouchTestPtr));
  t.add(new test::ManualTest("屏幕", mWindowDisplayTestPtr));
  t.add(new test::MountTest());
  t.add(new test::CameraTest("前后摄像头", mWindowCameraTestPtr));
  t.add(new test::AudioTest("麦克风和喇叭"));
  t.add(new test::KeyTest());
  t.add(new test::ReverseTest());
  t.add(new test::FmTest());
  t.add(new test::AuxTest());
  t.add(new test::CarplayAuthTest());
  t.add(new test::BluetoothAuthTest());

  t.onAllPassed([]() {
    //测试完成后，删除配置
    LOGD("all passed");
//    system("rm -rf /data/*");
//    setTesting(false);
    NETMANAGER->getWifiManager()->disconnect();
  });
}

/**
 * 当切换到该界面时触发
 */
static void onUI_intent(const Intent *intentPtr) {
    if (intentPtr != NULL) {
        //TODO
    }
}

/*
 * 当界面显示时触发
 */
static void onUI_show() {

}

/*
 * 当界面隐藏时触发
 */
static void onUI_hide() {
  //不允许退出
  EASYUICONTEXT->openActivity(class_name(mActivityPtr).c_str());
}

/*
 * 当界面完全退出时触发
 */
static void onUI_quit() {
  test::Testing::instance().clear();
  EASYUICONTEXT->showNaviBar();
}

/**
 * 串口数据回调接口
 */
static void onProtocolDataUpdate(const SProtocolData &data) {

}

/**
 * 定时器触发函数
 * 不建议在此函数中写耗时操作，否则将影响UI刷新
 * 参数： id
 *         当前所触发定时器的id，与注册时的id相同
 * 返回值: true
 *             继续运行当前定时器
 *         false
 *             停止运行当前定时器
 */
static bool onUI_Timer(int id) {
	switch (id) {
	  case TIMER_TIME:
	    updateTime();
	    break;
		default:
			break;
	}
    return true;
}

/**
 * 有新的触摸事件时触发
 * 参数：ev
 *         新的触摸事件
 * 返回值：true
 *            表示该触摸事件在此被拦截，系统不再将此触摸事件传递到控件上
 *         false
 *            触摸事件将继续传递到控件上
 */
static bool onTestActivityTouchEvent(const MotionEvent &ev) {
  switch (ev.mActionStatus) {
    case MotionEvent::E_ACTION_DOWN:    //触摸按下
      //LOGD("时刻 = %ld 坐标  x = %d, y = %d", ev.mEventTime, ev.mX, ev.mY);
      mPainterTouchPtr->drawArc(ev.mX, ev.mY, 1, 1, 0, 360);
      break;
    case MotionEvent::E_ACTION_MOVE:          //触摸滑动
      mPainterTouchPtr->drawArc(ev.mX, ev.mY, 1, 1, 0, 360);
      break;
    case MotionEvent::E_ACTION_UP:  //触摸抬起
      mPainterTouchPtr->drawArc(ev.mX, ev.mY, 1, 1, 0, 360);
      break;
    default:
      break;
  }
  return false;
}

static int getListItemCount_ListViewTest(const ZKListView *pListView) {
    return test::Testing::instance().size();
}

static void obtainListItemData_ListViewTest(ZKListView *pListView,ZKListView::ZKListItem *pListItem, int index) {
  ZKListView::ZKListSubItem* result_view = pListItem->findSubItemByID(
      ID_TEST_SubItemTestResult);
  if (!result_view) {
    LOGE("result view invalid");
    return;
  }
  auto item = test::Testing::instance().item(index);
  pListItem->setText(item->name());
  switch (item->state()) {
    case test::STATE_NO_TEST:
      pListItem->setBackgroundColor(0xff444444);
      result_view->setText("未测试");
      break;
    case test::STATE_NOT_GOOD:
      pListItem->setBackgroundColor(0xffff4734);
      result_view->setText("未通过 NG");
      break;
    case test::STATE_PASSED:
      pListItem->setBackgroundColor(0xff3aac54);
      result_view->setText("已通过 PASS");
      break;
    default:
      LOGE("invalid state %d", item->state());
      break;
  }
}

static void onListItemClick_ListViewTest(ZKListView *pListView, int index, int id) {
    //LOGD(" onListItemClick_ ListViewTest  !!!\n");
  auto pos = mPainterTouchPtr->getPosition();
  mPainterTouchPtr->erase(0,0, pos.mWidth, pos.mHeight);

  test::Testing::instance().start(index);
}

static bool onButtonClick_ButtonTestStart(ZKButton *pButton) {
  auto pos = mPainterTouchPtr->getPosition();
  mPainterTouchPtr->erase(0,0, pos.mWidth, pos.mHeight);

  test::Testing::instance().start(0);
  return false;
}
static bool onButtonClick_ButtonPass(ZKButton *pButton) {
  LOGD(" ButtonClick ButtonPass !!!\n");
  test::Testing::instance().pass();
  return false;
}

static bool onButtonClick_ButtonNG(ZKButton *pButton) {
  LOGD(" ButtonClick ButtonNG !!!\n");
  test::Testing::instance().ng();
  return false;
}

static void onVideoViewPlayerMessageListener_VideoView1(ZKVideoView *pVideoView, int msg) {
	switch (msg) {
	case ZKVideoView::E_MSGTYPE_VIDEO_PLAY_STARTED:
		break;
	case ZKVideoView::E_MSGTYPE_VIDEO_PLAY_COMPLETED:
		break;
	case ZKVideoView::E_MSGTYPE_VIDEO_PLAY_ERROR:
		break;
	}
}
static bool onButtonClick_ButtonExit(ZKButton *pButton) {
  LOGD(" ButtonClick ButtonExit !!!\n");
  CLOSE_ACTIVITY_WITH(mActivityPtr);
  return false;
}
static bool onButtonClick_Button1(ZKButton *pButton) {
  EASYUICONTEXT->openActivity("NetSettingActivity");
  return false;
}

static bool onButtonClick_ButtonDisplayColor(ZKButton *pButton) {
  const uint32_t colors[3] = { 0xffff0000,0xff00ff00,0xff0000ff };
  static int color_index = 0;
  mWindowDisplayTestPtr->setBackgroundColor(colors[color_index]);
  color_index = (color_index + 1) % array_length(colors);
  return false;
}
