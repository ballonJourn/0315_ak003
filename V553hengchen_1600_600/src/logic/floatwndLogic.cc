#pragma once
#include "uart/ProtocolSender.h"
/*
*此文件由GUI工具生成
*文件功能：用于处理用户的逻辑相应代码
*功能说明：
*========================onButtonClick_XXXX
当页面中的按键按下后系统会调用对应的函数，XXX代表GUI工具里面的[ID值]名称，
如Button1,当返回值为false的时候系统将不再处理这个按键，返回true的时候系统将会继续处理此按键。比如SYS_BACK.
*========================onSlideWindowItemClick_XXXX(int index) 
当页面中存在滑动窗口并且用户点击了滑动窗口的图标后系统会调用此函数,XXX代表GUI工具里面的[ID值]名称，
如slideWindow1;index 代表按下图标的偏移值
*========================onSeekBarChange_XXXX(int progress) 
当页面中存在滑动条并且用户改变了进度后系统会调用此函数,XXX代表GUI工具里面的[ID值]名称，
如SeekBar1;progress 代表当前的进度值
*========================ogetListItemCount_XXXX() 
当页面中存在滑动列表的时候，更新的时候系统会调用此接口获取列表的总数目,XXX代表GUI工具里面的[ID值]名称，
如List1;返回值为当前列表的总条数
*========================oobtainListItemData_XXXX(ZKListView::ZKListItem *pListItem, int index)
 当页面中存在滑动列表的时候，更新的时候系统会调用此接口获取列表当前条目下的内容信息,XXX代表GUI工具里面的[ID值]名称，
如List1;pListItem 是贴图中的单条目对象，index是列表总目的偏移量。具体见函数说明
*========================常用接口===============
*LOGD(...)  打印调试信息的接口
*mTextXXXPtr->setText("****") 在控件TextXXX上显示文字****
*mButton1Ptr->setSelected(true); 将控件mButton1设置为选中模式，图片会切换成选中图片，按钮文字会切换为选中后的颜色
*mSeekBarPtr->setProgress(12) 在控件mSeekBar上将进度调整到12
*mListView1Ptr->refreshListView() 让mListView1 重新刷新，当列表数据变化后调用
*mDashbroadView1Ptr->setTargetAngle(120) 在控件mDashbroadView1上指针显示角度调整到120度
*
* 在Eclipse编辑器中  使用 “alt + /”  快捷键可以打开智能提示
*/

#include "app/SysAppFactory.h"
#include "system/reverse.h"
#include "mode_observer.h"
#include "sysapp_context.h"
#include "common.h"

// 注册成系统界面
REGISTER_SYSAPP(APP_TYPE_SYS_FLOATWND, floatwndActivity);

namespace {
enum touchEvent_E {
	StartMoveTouchEvent,
	EndMoveTouchEvent,
	EnterWindowEvent,
	BackButtonEvent
};

enum windowMode_E {
	ButtonMode,
	WindowMode,
};


static LayoutPosition floatPosition;
static int floatMode = ButtonMode;
static bool hasMove = false;

static void updateWndPos(int mode, int x, int y) {
	LayoutPosition curPos = mPopButtonPtr->getPosition();
	switch (mode) {
	case StartMoveTouchEvent: {
		LayoutPosition pos(x - (curPos.mWidth / 2), y - (curPos.mHeight / 2), curPos.mWidth, curPos.mHeight);
		mActivityPtr->setPosition(pos);
		break;
	}
	case EndMoveTouchEvent: {
		// 此时传递的x, y是浮窗中心点的坐标
#if 0
		x = (x >= (SCREEN_WIDTH / 2)) ? (SCREEN_WIDTH - curPos.mWidth) : 0;
		y = y - curPos.mHeight / 2;
		y = (y >= (SCREEN_HEIGHT - curPos.mHeight)) ? (SCREEN_HEIGHT - curPos.mHeight) : ((y < 0) ? 0 : y);
#else
		x -= curPos.mWidth / 2;
		x = (x >= (SCREEN_WIDTH - curPos.mWidth)) ? (SCREEN_WIDTH - curPos.mWidth) : std::max(0, x);
		y -= curPos.mHeight / 2;
		y = (y >= (SCREEN_HEIGHT - curPos.mHeight)) ? (SCREEN_HEIGHT - curPos.mHeight) : std::max(0, y);
#endif
		floatPosition = LayoutPosition(x, y, curPos.mWidth, curPos.mHeight);
		mActivityPtr->setPosition(floatPosition);
		sys::setting::set_float_button_position(floatPosition);
		break;
	}
	case EnterWindowEvent: {
		mPopButtonPtr->setVisible(false);
		mPopWindowPtr->showWnd();

		LayoutPosition pos = mPopWindowPtr->getPosition();
		pos.mLeft = (SCREEN_WIDTH - pos.mWidth) / 2;
		pos.mTop = (SCREEN_HEIGHT - pos.mHeight) / 2;
		mActivityPtr->setPosition(pos);

		floatMode = WindowMode;
		break;
	}
	case BackButtonEvent: {
		mPopWindowPtr->hideWnd();
		mPopButtonPtr->setVisible(true);
		mActivityPtr->setPosition(floatPosition);
		floatMode = ButtonMode;
		break;
	}
	default:
		break;
	}
	return;
}

};

/**
 * 注册定时器
 * 填充数组用于注册定时器
 * 注意：id不能重复
 */
static S_ACTIVITY_TIMEER REGISTER_ACTIVITY_TIMER_TAB[] = {
	//{0,  6000}, //定时器id=0, 时间间隔6秒
	//{1,  1000},
};

/**
 * 当界面构造时触发
 */
static void onUI_init(){
    //Tips :添加 UI初始化的显示代码到这里,如:mText1Ptr->setText("123");
	floatPosition = sys::setting::get_float_button_position();
	if ((floatPosition.mWidth == 0) || (floatPosition.mHeight == 0)) {
		floatPosition = mPopButtonPtr->getPosition();
		floatPosition.mLeft = SCREEN_WIDTH - floatPosition.mWidth;
		floatPosition.mTop = SCREEN_HEIGHT - floatPosition.mHeight;
	}

	mActivityPtr->setPosition(floatPosition);
	mPopWindowPtr->hideWnd();
	mPopButtonPtr->setVisible(true);

	floatMode = ButtonMode;
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

}

/*
 * 当界面完全退出时触发
 */
static void onUI_quit() {

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
static bool onUI_Timer(int id){
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
static bool onfloatwndActivityTouchEvent(const MotionEvent &ev) {
	static bool isEffective = false;
	static MotionEvent downEv = ev;

	// 检测浮动窗口是否需要响应
	switch (ev.mActionStatus) {
	case MotionEvent::E_ACTION_DOWN:	//触摸按下
		downEv = ev;
		if (floatMode == WindowMode || mActivityPtr->getPosition().isHit(ev.mX, ev.mY)) {
			isEffective = true;
		} else {
			isEffective = false;
		}
		break;
	case MotionEvent::E_ACTION_MOVE:	//触摸滑动
		if (floatMode == WindowMode || !isEffective)
			break;
		if (abs(downEv.mX - ev.mX) > 10 || abs(downEv.mY - ev.mY) > 10) {
			updateWndPos(StartMoveTouchEvent, ev.mX, ev.mY);
			hasMove = true;
		}
		break;
	case MotionEvent::E_ACTION_UP:  //触摸抬起
		if (!isEffective)
			break;
		if ((floatMode == WindowMode) && mActivityPtr->getPosition().isHit(ev.mX, ev.mY))
			break;
		if (!hasMove) {
			// 倒车状态
			if (sys::reverse_does_enter_status()) {
				if (floatMode == WindowMode) {
					updateWndPos(BackButtonEvent, ev.mX, ev.mY);
				}
				break;
			}

			int mode = (floatMode == WindowMode) ? BackButtonEvent : EnterWindowEvent;
			updateWndPos(mode, ev.mX, ev.mY);
		} else {
			updateWndPos(EndMoveTouchEvent, ev.mX, ev.mY);
		}
		hasMove = false;
		break;
	default:
		break;
	}
	return false;
}

static bool onButtonClick_PopButton(ZKButton *pButton) {
    LOGD(" ButtonClick PopButton !!!\n");
    return false;
}

static bool onButtonClick_VolButton(ZKButton *pButton) {
	LOGD(" ButtonClick VolButton !!!\n");
	mode::set_event_mode(E_EVENT_MODE_VOICE);
	app::show_ctrlbar();
	updateWndPos(BackButtonEvent, 0, 0);
	return false;
}

static bool onButtonClick_BrightnessButton(ZKButton *pButton) {
	LOGD(" ButtonClick BrightnessButton !!!\n");
	mode::set_event_mode(E_EVENT_MODE_BRIGHTNESS);
	app::show_ctrlbar();
	updateWndPos(BackButtonEvent, 0, 0);
	return false;
}

static bool onButtonClick_RecordButton(ZKButton *pButton) {
    LOGD(" ButtonClick RecordButton !!!\n");
    OPEN_APP("Dvr");
    updateWndPos(BackButtonEvent, 0, 0);
    return false;
}

static bool onButtonClick_sys_home(ZKButton *pButton) {
    LOGD(" ButtonClick sys_home !!!\n");
    mode::set_event_mode(E_EVENT_MODE_HOME);
    updateWndPos(BackButtonEvent, 0, 0);
    return false;
}
