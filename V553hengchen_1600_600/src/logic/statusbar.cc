#pragma once
#include "uart/ProtocolSender.h"
/*
*此文件由GUI工具生成
*文件功能：用于处理用户的逻辑相应代码
*功能说明：
*========================onButtonClick_XXXX
当页面中的按键按下后系统会调用对应的函数，XXX代表GUI工具里面的[标识]名称，
如Button1,当返回值为false的时候系统将不再处理这个按键，返回true的时候系统将会继续处理此按键。比如SYS_BACK.
*========================onSlideWindowItemClick_XXXX(int index) 
当页面中存在滑动窗口并且用户点击了滑动窗口的图标后系统会调用此函数,XXX代表GUI工具里面的[标识]名称，
如slideWindow1;index 代表按下图标的偏移值
*========================onSeekBarChange_XXXX(int progress)
当页面中存在滑动条并且用户改变了进度后系统会调用此函数,XXX代表GUI工具里面的[标识]名称，
如SeekBar1;progress 代表当前的进度值
*========================ogetListItemCount_XXXX() 
当页面中存在滑动列表的时候，更新的时候系统会调用此接口获取列表的总数目,XXX代表GUI工具里面的[标识]名称，
如List1;返回值为当前列表的总条数
*========================oobtainListItemData_XXXX(ZKListView::ZKListItem *pListItem, int index)
 当页面中存在滑动列表的时候，更新的时候系统会调用此接口获取列表当前条目下的内容信息,XXX代表GUI工具里面的[标识]名称，
如List1;pListItem 是贴图中的单条目对象，index是列表总目的偏移量。具体见函数说明
*========================常用接口===============
*LOGD(...)  打印调试信息的接口
*mTextXXX->setText("****") 在控件TextXXX上显示文字****
*mButton1->setSelected(true); 将控件mButton1设置为选中模式，图片会切换成选中图片，按钮文字会切换为选中后的颜色
*mSeekBar->setProgress(12) 在控件mSeekBar上将进度调整到12
*mListView1->refreshListView() 让mListView1 重新刷新，当列表数据变化后调用
*mDashbroadView1->setTargetAngle(120) 在控件mDashbroadView1上指针显示角度调整到120度
*/

#include "bt/context.h"
#include "link/context.h"
#include "common.h"
#include <utils/BrightnessHelper.h>


#define LYLINK_VIEW               "lylinkview"
#define TALK_TIMER			1

#define CALL_TIMER		9

static bt_cb_t _s_bt_cb;
static std::string call_num;

static void _update_talk_time() {
	uint32_t talk_time = bt::get_talk_time() / 1000;
	if (talk_time < 0 || talk_time >= 86400) {
		talk_time = 0;
	}
	char str[128] = { 0 };
	if (talk_time >= 3600) {
		sprintf(str, "%d:%02d:%02d", talk_time / 3600, talk_time / 60, talk_time % 60);
	} else {
		sprintf(str, "%02d:%02d", talk_time / 60, talk_time % 60);
	}

	mtimingTextViewPtr->setText(str);
}

static void _call_ui_reset() {
	mcallWindowPtr->hideWnd();
	manswerButtonPtr->setVisible(true);
	mincomingWindowPtr->showWnd();
//	mstatusbarPtr->unregisterUserTimer(TALK_TIMER);
	mstatusbarPtr->hide();
}

static void _bt_call_num_cb(bt_call_num_e num) {
	switch (num) {
	case E_BT_CALL_NUM:
		mtNameTextViewPtr->setText(call_num);
		break;
	default:
		break;
	}
}


static void _bt_call_cb(bt_call_state_e state) {
	LOGD("--%d-- --%s-- bt call state: %d \n", __LINE__, __FILE__, state);
	EASYUICONTEXT->screensaverOff();
	BRIGHTNESSHELPER->screenOn();
	call_num = bt::get_call_num();
	if (lk::is_connected()) return ;
//	if  (state == E_BT_CALL_STATE_IDLE) {    // 挂断
//		_call_ui_reset();
//	} else {
//		if (lk::get_lylink_type() != LINK_TYPE_WIFIAUTO) {
			switch (state) {
			case E_BT_CALL_STATE_IDLE:				 	 // 挂断
				_call_ui_reset();
				break;
			case E_BT_CALL_STATE_OUTGOING:               // 去电
				manswerButtonPtr->setVisible(false);
				mnumberTextViewPtr->setText(bt::get_call_num());
				mcallWindowPtr->showWnd();
				mstatusbarPtr->show();
				break;
			case E_BT_CALL_STATE_INCOMING:               // 来电
				mnumberTextViewPtr->setText(bt::get_call_num());
				mcallWindowPtr->showWnd();
				mstatusbarPtr->show();
				break;
			case E_BT_CALL_STATE_TALKING:                // 通话中
//				mstatusbarPtr->registerUserTimer(TALK_TIMER, 1000);
//				_update_talk_time();
//				LOGE("[statusbar] get_call_num %s", bt::get_call_num().c_str());
				mtNameTextViewPtr->setText(call_num);
				mcallWindowPtr->showWnd();
				mstatusbarPtr->show();
				mincomingWindowPtr->hideWnd();
				break;
			default:
				break;
			}
//		}
//	}
}

static void _bt_power_cb(bt_power_state_e state) {
	if (E_BT_POWER_STATE_OFF == state) {
		_call_ui_reset();
	}
}

static void _bt_connect_cb(bt_connect_state_e state) {
	if (E_BT_CONNECT_STATE_IDLE == state) {
		_call_ui_reset();
	}
}

static void _bt_add_cb() {
	_s_bt_cb.call_cb = _bt_call_cb;
	_s_bt_cb.call_num_cb = _bt_call_num_cb;
	_s_bt_cb.power_cb = _bt_power_cb;
	_s_bt_cb.connect_cb = _bt_connect_cb;
	bt::add_cb(&_s_bt_cb);
}

static void _bt_remove_cb() {
	bt::remove_cb(&_s_bt_cb);
}

/**
 * 注册定时器
 * 填充数组用于注册定时器
 * 注意：id不能重复
 */
static S_ACTIVITY_TIMEER REGISTER_ACTIVITY_TIMER_TAB[] = {
	//{0,  1000}, //定时器id=0, 时间间隔1秒
	//{1,  100},
		{CALL_TIMER,  1000},
};

/**
 * 当界面构造时触发
 */
static void onUI_init() {
	_bt_add_cb();
}

/*
 * 当界面完全退出时触发
 */
static void onUI_quit() {
	_bt_remove_cb();
}

/**
 * 串口数据回调接口
 */
static void onProtocolDataUpdate(const SProtocolData &data) {
    //串口数据回调接口
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
	case TALK_TIMER:
		_update_talk_time();
		break;
	case CALL_TIMER:
		_update_talk_time();
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
static bool onstatusbarActivityTouchEvent(const MotionEvent &ev) {
	switch (ev.mActionStatus) {
	case MotionEvent::E_ACTION_DOWN:	//触摸按下
		//LOGD("时刻 = %ld 坐标  x = %d, y = %d", ev.mEventTime, ev.mX, ev.mY);
		break;
	case MotionEvent::E_ACTION_MOVE:			//触摸滑动

		break;
	case MotionEvent::E_ACTION_UP:  //触摸抬起
		break;
	default:
		break;
	}
	return false;
}

static bool onButtonClick_answerButton(ZKButton *pButton) {
    LOGD(" ButtonClick answerButton !!!\n");
    bt::answer();
    return false;
}

static bool onButtonClick_rejectButton(ZKButton *pButton) {
    LOGD(" ButtonClick rejectButton !!!\n");
    bt::reject();
    return false;
}
