#pragma once
#include "uart/ProtocolSender.h"
#include "system/setting.h"
#include "system/reverse.h"
#include "common.h"
#include "mpi/case/camera.h"
#include "system/setting.h"
#include <utils/BrightnessHelper.h>
#include <base/ui_handler.h>
#include "misc/utility.h"
#include <base/base.hpp>
#include "media/audio_context.h"
#include "link/context.h"
#include "bt/context.h"

using namespace sys::setting;
static float vol = 0.0;
extern void updateVolumeText();

namespace {

enum {
};

mpi::Rectangle rear_viewbox = { 0, 0, 1920, 720 };

#if 0
static int _s_signal_check_count;
class MyErrorCodeCallback : public ZKCameraView::IErrorCodeCallback {
private:
    virtual void onErrorCode(int error) {
//      LOGD("@@ onErrorCode error: %d\n", error);
        if (error == E_CAMERA_STATUS_CODE_NO_SIGNAL) {
            if (_s_signal_check_count < 2) {   // 检测几次无信号才出提示
                _s_signal_check_count++;
            } else {
//              mSignTextViewPtr->setVisible(true);
            }
        } else if (error == E_CAMERA_STATUS_CODE_HAS_SIGNAL) {
            _s_signal_check_count = 0;
        }
    }
};

static MyErrorCodeCallback sMyErrorCodeCallback;
#endif

static void _draw_reverse_line() {
    SZKPoint lt, rt, lb, rb;
    sys::setting::get_reverse_line_point(lt, rt, lb, rb);

    SZKPoint points[3];

    int h = lb.y - lt.y;  // 垂直高度, 限制不为0
    int gh = REVERSE_LINE_G_RATIO * h;
    int yh = REVERSE_LINE_Y_RATIO * h;

    mLinePainterPtr->setLineWidth(REVERSE_LINE_WIDTH);

    // draw left
    int w = lb.x - lt.x;
    float ratio = (float) w / h;

    points[0].x = lt.x + REVERSE_LINE_CORNER_LEN;
    points[0].y = lt.y;
    points[1].x = lt.x;
    points[1].y = lt.y;
    points[2].x = ratio * gh + lt.x;
    points[2].y = lt.y + gh;
    mLinePainterPtr->setSourceColor(REVERSE_LINE_G_COLOR);
    mLinePainterPtr->drawLines(points, 3);

    points[0].x = points[2].x + REVERSE_LINE_CORNER_LEN;
    points[0].y = points[2].y;
    points[1].x = points[2].x;
    points[1].y = points[2].y;
    points[2].x = ratio * (gh + yh) + lt.x;
    points[2].y = lt.y + (gh + yh);
    mLinePainterPtr->setSourceColor(REVERSE_LINE_Y_COLOR);
    mLinePainterPtr->drawLines(points, 3);

    points[0].x = points[2].x + REVERSE_LINE_CORNER_LEN;
    points[0].y = points[2].y;
    points[1].x = points[2].x;
    points[1].y = points[2].y;
    points[2].x = lb.x;
    points[2].y = lb.y;
    mLinePainterPtr->setSourceColor(REVERSE_LINE_R_COLOR);
    mLinePainterPtr->drawLines(points, 3);

    // draw right
    w = rb.x - rt.x;
    ratio = (float) w / h;

    points[0].x = rt.x - REVERSE_LINE_CORNER_LEN;
    points[0].y = rt.y;
    points[1].x = rt.x;
    points[1].y = rt.y;
    points[2].x = ratio * gh + rt.x;
    points[2].y = rt.y + gh;
    mLinePainterPtr->setSourceColor(REVERSE_LINE_G_COLOR);
    mLinePainterPtr->drawLines(points, 3);

    points[0].x = points[2].x - REVERSE_LINE_CORNER_LEN;
    points[0].y = points[2].y;
    points[1].x = points[2].x;
    points[1].y = points[2].y;
    points[2].x = ratio * (gh + yh) + rt.x;
    points[2].y = rt.y + (gh + yh);
    mLinePainterPtr->setSourceColor(REVERSE_LINE_Y_COLOR);
    mLinePainterPtr->drawLines(points, 3);

    points[0].x = points[2].x - REVERSE_LINE_CORNER_LEN;
    points[0].y = points[2].y;
    points[1].x = points[2].x;
    points[1].y = points[2].y;
    points[2].x = rb.x;
    points[2].y = rb.y;
    mLinePainterPtr->setSourceColor(REVERSE_LINE_R_COLOR);
    mLinePainterPtr->drawLines(points, 3);
}

void storeViewbox() {
  sys::setting::set_reverse_viewbox_y(rear_viewbox.y);
}

bool handleMotionEvent(const MotionEvent &ev) {
  if (!mWindowMotionPtr->getPosition().isHit(ev.mX, ev.mY)) {
    return false;
  }
  static MotionEvent last;
  switch (ev.mActionStatus) {
    case MotionEvent::E_ACTION_DOWN:
      last = ev;
      break;
    case MotionEvent::E_ACTION_MOVE:
      rear_viewbox = mpi::RearCamera::instance().moveViewbox(
          -(ev.mX - last.mX), -(ev.mY - last.mY));
      last = ev;
      base::runInUiThreadUniqueDelayed("set_reverse_viewbox", storeViewbox, 1000);
      break;
    case MotionEvent::E_ACTION_UP:
      break;
    default:
      break;
  }
  return false;
}

}

/**
 * 注册定时器
 * 填充数组用于注册定时器
 * 注意：id不能重复
 */
static S_ACTIVITY_TIMEER REGISTER_ACTIVITY_TIMER_TAB[] = {
//    {0,  50}, //定时器id=0, 时间间隔6秒
};

/**
 * 当界面构造时触发
 */
static void onUI_init() {
  LOGD_TRACE("");
    //Tips :添加 UI初始化的显示代码到这里,如:mText1Ptr->setText("123");

#if 0
    int w = get_camera_width(), h = get_camera_height();
    ERotation rot = (ERotation) get_camera_rot();

    mCameraViewReversePtr->setErrorCodeCallback(&sMyErrorCodeCallback);
    mCameraViewReversePtr->setDevPath(get_camera_dev());
    mCameraViewReversePtr->setFormatSize(w, h);
    mCameraViewReversePtr->setFrameRate(get_camera_rate());
    mCameraViewReversePtr->setRotation(rot);
    mCameraViewReversePtr->setMirror((EMirror) get_camera_mirror());
    mCameraViewReversePtr->setChannel(get_camera_chn());

    // crop
    const LayoutPosition &vp = mCameraViewReversePtr->getPosition();
    LayoutPosition cp(0, 0, w, h);

    if ((float) w / vp.mWidth > (float) h / vp.mHeight) {
        cp.mWidth = h * vp.mWidth / vp.mHeight;
        cp.mLeft = (w - cp.mWidth) / 2;
    } else {
        cp.mHeight = w * vp.mHeight / vp.mWidth;
        cp.mTop = (h - cp.mHeight) / 2;
    }
    if ((rot == E_ROTATION_90) || (rot == E_ROTATION_270)) {
        std::swap(cp.mLeft, cp.mTop);
        std::swap(cp.mWidth, cp.mHeight);
    }

//    mCameraViewReversePtr->setCropPosition(cp);
    LOGD("[reserveLogic] camera crop(%d, %d, %d, %d)\n", cp.mLeft, cp.mTop, cp.mWidth, cp.mHeight);
#endif

//    mCameraViewReversePtr->setChannel(get_camera_chn());

    rear_viewbox.y = sys::setting::get_reverse_viewbox_y();

    _draw_reverse_line();

    bool effect = bt::is_calling() || (lk::is_connected() && lk::get_is_call_state() != CallState_Hang);
//	vol = effect? (audio::get_call_vol()) : (audio::get_system_vol());
	vol = audio::get_system_vol();

	// [Fix B] 初始化摄像头参数SeekBar的国际化文本，使用LTOV宏从tr文件读取当前语言翻译
	mreversevcameraBrightnessPicPtr->setText(LTOV("CameraBrightness"));
	mreversevcameraContrastTextviewPtr->setText(LTOV("CameraContrast"));
	mreversevcameraSaturationTextViewPtr->setText(LTOV("CameraSaturation"));
	mreversevcameraHueTextViewPtr->setText(LTOV("CameraHue"));

	// [Fix C] 初始化摄像头参数SeekBar
	// 恢复出厂设置后摄像头驱动可能未就绪，getCameraXxx返回false时使用合理默认值
	const int DEFAULT_BRIGHTNESS = 25;
	const int DEFAULT_CONTRAST = 55;
	const int DEFAULT_SATURATION = 55;
	const int DEFAULT_HUE = 50;

	int brightness = DEFAULT_BRIGHTNESS, contrast = DEFAULT_CONTRAST;
	int saturation = DEFAULT_SATURATION, hue = DEFAULT_HUE;
	if (sys::setting::getCameraBrightness(brightness)) {
		mBrightnessSeekBarPtr->setProgress(brightness);
	} else {
		mBrightnessSeekBarPtr->setProgress(DEFAULT_BRIGHTNESS);
	}
	if (sys::setting::getCameraContrast(contrast)) {
		mContrastSeekBarPtr->setProgress(contrast);
	} else {
		mContrastSeekBarPtr->setProgress(DEFAULT_CONTRAST);
	}
	if (sys::setting::getCameraSaturation(saturation)) {
		mSaturationSeekBarPtr->setProgress(saturation);
	} else {
		mSaturationSeekBarPtr->setProgress(DEFAULT_SATURATION);
	}
	if (sys::setting::getCameraHue(hue)) {
		mHueSeekBarPtr->setProgress(hue);
	} else {
		mHueSeekBarPtr->setProgress(DEFAULT_HUE);
	}
}

/**
 * 当切换到该界面时触发
 */
static void onUI_intent(const Intent *intentPtr) {

}

/*
 * 当界面显示时触发
 */
static void onUI_show() {
  LOGD_TRACE("");
  bool effect = bt::is_calling() || (lk::is_connected() && lk::get_is_call_state() != CallState_Hang);
//  if (effect) {
//	  audio::set_call_vol(0, effect);
//  } else {
	  audio::set_system_vol(0, !effect);
//  }

  EASYUICONTEXT->hideNaviBar();
  EASYUICONTEXT->screensaverOff();
  BRIGHTNESSHELPER->screenOn();

  setupCamera(VIEW_TYPE_REVERSE, true);
}

/*
 * 当界面隐藏时触发
 */
static void onUI_hide() {
  LOGD_TRACE("");
  // 处理同时打开界面，不能进倒车问题
  EASYUICONTEXT->openActivity(class_name(mActivityPtr).c_str());
}

/*
 * 当界面完全退出时触发
 */
static void onUI_quit() {
  LOGD_TRACE("");
//  bool effect = bt::is_calling() || (lk::is_connected() && lk::get_is_call_state() != CallState_Hang);
//  if (effect) {
//	  audio::set_call_vol(vol, effect);
//  } else {
	  audio::set_system_vol(vol, true);
//  }
    updateVolumeText();
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
static bool onreverse2ActivityTouchEvent(const MotionEvent &ev) {
  if (handleMotionEvent(ev)) {
    return true;
  }
    switch (ev.mActionStatus) {
    case MotionEvent::E_ACTION_DOWN://触摸按下
        //LOGD("时刻 = %ld 坐标  x = %d, y = %d", ev.mEventTime, ev.mX, ev.mY);
        break;
    case MotionEvent::E_ACTION_MOVE://触摸滑动
        break;
    case MotionEvent::E_ACTION_UP:  //触摸抬起
        // 点击其他区域时隐藏设置窗口
        if (mreversevcameraViewnessWndPtr->isVisible()) {
            // 检查点击位置是否在设置窗口和设置按钮之外
            if (!mreversevcameraViewnessWndPtr->getPosition().isHit(ev.mX, ev.mY) &&
                !mreversevcameraSetButtonPtr->getPosition().isHit(ev.mX, ev.mY)) {
                mreversevcameraViewnessWndPtr->hideWnd();
            }
        }
        break;
    default:
        break;
    }
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
static bool onButtonClick_reversevcameraSetButton(ZKButton *pButton) {
    LOGD(" ButtonClick reversevcameraSetButton !!!\n");
    // 点击按钮时切换窗口的显示状态
    if (mreversevcameraViewnessWndPtr->isVisible()) {
        mreversevcameraViewnessWndPtr->hideWnd();
    } else {
        mreversevcameraViewnessWndPtr->showWnd();
    }
    return false;
}

static void onProgressChanged_BrightnessSeekBar(ZKSeekBar *pSeekBar, int progress) {
    //LOGD(" ProgressChanged BrightnessSeekBar %d !!!\n", progress);
    sys::setting::setCameraBrightness(progress);
}

static void onProgressChanged_ContrastSeekBar(ZKSeekBar *pSeekBar, int progress) {
    //LOGD(" ProgressChanged ContrastSeekBar %d !!!\n", progress);
    sys::setting::setCameraContrast(progress);
}

static void onProgressChanged_SaturationSeekBar(ZKSeekBar *pSeekBar, int progress) {
    //LOGD(" ProgressChanged SaturationSeekBar %d !!!\n", progress);
    sys::setting::setCameraSaturation(progress);
}

static void onProgressChanged_HueSeekBar(ZKSeekBar *pSeekBar, int progress) {
    //LOGD(" ProgressChanged HueSeekBar %d !!!\n", progress);
    sys::setting::setCameraHue(progress);
}
static bool onButtonClick_defaultButton(ZKButton *pButton) {
    const int DEFAULT_BRIGHTNESS = 25;
    const int DEFAULT_CONTRAST = 55;
    const int DEFAULT_SATURATION = 55;
    const int DEFAULT_HUE = 50;

    mBrightnessSeekBarPtr->setProgress(DEFAULT_BRIGHTNESS);
    mContrastSeekBarPtr->setProgress(DEFAULT_CONTRAST);
    mSaturationSeekBarPtr->setProgress(DEFAULT_SATURATION);
    mHueSeekBarPtr->setProgress(DEFAULT_HUE);

    sys::setting::setCameraBrightness(DEFAULT_BRIGHTNESS);
    sys::setting::setCameraContrast(DEFAULT_CONTRAST);
    sys::setting::setCameraSaturation(DEFAULT_SATURATION);
    sys::setting::setCameraHue(DEFAULT_HUE);
    return false;
}
