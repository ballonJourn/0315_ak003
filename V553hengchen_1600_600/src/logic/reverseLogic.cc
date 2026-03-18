#pragma once
#include "uart/ProtocolSender.h"
#include "system/setting.h"
#include "system/reverse.h"
#include "common.h"

using namespace sys::setting;

static int _s_signal_check_count;

namespace {

class MyErrorCodeCallback : public ZKCameraView::IErrorCodeCallback {
private:
	virtual void onErrorCode(int error) {
//		LOGD("@@ onErrorCode error: %d\n", error);
		if (error == E_CAMERA_STATUS_CODE_NO_SIGNAL) {
			if (_s_signal_check_count < 2) {   // 检测几次无信号才出提示
				_s_signal_check_count++;
			} else {
//				mSignTextViewPtr->setVisible(true);
			}
		} else if (error == E_CAMERA_STATUS_CODE_HAS_SIGNAL) {
			_s_signal_check_count = 0;
		}
	}
};

}

static MyErrorCodeCallback sMyErrorCodeCallback;

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

/**
 * 注册定时器
 * 填充数组用于注册定时器
 * 注意：id不能重复
 */
static S_ACTIVITY_TIMEER REGISTER_ACTIVITY_TIMER_TAB[] = {
	//{0,  6000}, //定时器id=0, 时间间隔6秒
};

/**
 * 当界面构造时触发
 */
static void onUI_init() {
	LOGD("[reserveLogic] onUI_init()--\n");
    //Tips :添加 UI初始化的显示代码到这里,如:mText1Ptr->setText("123");

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

	mCameraViewReversePtr->setCropPosition(cp);
	LOGD("[reserveLogic] camera crop(%d, %d, %d, %d)\n", cp.mLeft, cp.mTop, cp.mWidth, cp.mHeight);

	mCameraViewReversePtr->setChannel(get_camera_chn());

	_draw_reverse_line();
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
	LOGD("[reserveLogic] onUI_show()--\n");
	EASYUICONTEXT->hideNaviBar();
}

/*
 * 当界面隐藏时触发
 */
static void onUI_hide() {
	LOGD("[reserveLogic] onUI_hide()--\n");
	// 处理同时打开界面，不能进倒车问题
	WAIT(!mCameraViewReversePtr->isPreviewing(), 100, 30);
	EASYUICONTEXT->openActivity("reverseActivity");
}

/*
 * 当界面完全退出时触发
 */
static void onUI_quit() {
	LOGD("[reserveLogic] onUI_quit()--\n");
	mCameraViewReversePtr->setErrorCodeCallback(NULL);
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
static bool onreverseActivityTouchEvent(const MotionEvent &ev) {
    switch (ev.mActionStatus) {
	case MotionEvent::E_ACTION_DOWN://触摸按下
		//LOGD("时刻 = %ld 坐标  x = %d, y = %d", ev.mEventTime, ev.mX, ev.mY);
		break;
	case MotionEvent::E_ACTION_MOVE://触摸滑动
		break;
	case MotionEvent::E_ACTION_UP:  //触摸抬起
		break;
	default:
		break;
	}
	return false;
}

