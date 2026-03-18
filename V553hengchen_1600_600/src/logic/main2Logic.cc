#pragma once
#include "uart/ProtocolSender.h"
#include "system/setting.h"
#include "net/context.h"
#include "link/context.h"
#include "bt/context.h"
#include "media/audio_context.h"
#include "media/media_context.h"
#include "fy/format.hpp"
#include "manager/LanguageManager.h"
#include "manager/ConfigManager.h"
#include "sysapp_context.h"
#include "mode_observer.h"
#include <mpi/case/camera.h>
#include <base/base.hpp>
#include "edge/popup_service.h"
#include "misc/app_server_impl.h"

/*
*此文件由GUI工具生成
*文件功能:用于处理用户的逻辑相应代码
*功能说明:
*========================onButtonClick_XXXX
当页面中的按键按下后系统会调用对应的函数,XXX代表GUI工具里面的[ID值]名称,
如Button1,当返回值为false的时候系统将不再处理这个按键,返回true的时候系统将会继续处理此按键。比如SYS_BACK.
*========================onSlideWindowItemClick_XXXX(int index) 
当页面中存在滑动窗口并且用户点击了滑动窗口的图标后系统会调用此函数,XXX代表GUI工具里面的[ID值]名称,
如slideWindow1;index 代表按下图标的偏移值
*========================onSeekBarChange_XXXX(int progress) 
当页面中存在滑动条并且用户改变了进度后系统会调用此函数,XXX代表GUI工具里面的[ID值]名称,
如SeekBar1;progress 代表当前的进度值
*========================ogetListItemCount_XXXX() 
当页面中存在滑动列表的时候,更新的时候系统会调用此接口获取列表的总数目,XXX代表GUI工具里面的[ID值]名称,
如List1;返回值为当前列表的总条数
*========================oobtainListItemData_XXXX(ZKListView::ZKListItem *pListItem, int index)
 当页面中存在滑动列表的时候,更新的时候系统会调用此接口获取列表当前条目下的内容信息,XXX代表GUI工具里面的[ID值]名称,
如List1;pListItem 是贴图中的单条目对象,index是列表总目的偏移量。具体见函数说明
*========================常用接口===============
*LOGD(...)  打印调试信息的接口
*mTextXXXPtr->setText("****") 在控件TextXXX上显示文字****
*mButton1Ptr->setSelected(true); 将控件mButton1设置为选中模式,图片会切换成选中图片,按钮文字会切换为选中后的颜色
*mSeekBarPtr->setProgress(12) 在控件mSeekBar上将进度调整到12
*mListView1Ptr->refreshListView() 让mListView1 重新刷新,当列表数据变化后调用
*mDashbroadView1Ptr->setTargetAngle(120) 在控件mDashbroadView1上指针显示角度调整到120度
*
* 在Eclipse编辑器中  使用 "alt + /"  快捷键可以打开智能提示
*/

#define LYLINK_VIEW               "lylinkview"

// 定义链接模式枚举(如果没有在其他头文件中定义)
// 这些定义应该已经在 mode_observer.h 或 system/setting.h 中
// 如果编译报错,请检查这些头文件

namespace {

static void _show_link_tips(LYLINK_TYPE_E link_type) {
	switch (link_type) {
	case LINK_TYPE_WIFICP:
	case LINK_TYPE_USBCP:
		mlinkTipsTextViewPtr->setTextTr("Connected CarPlay device");
		break;
	case LINK_TYPE_USBAUTO:
	case LINK_TYPE_WIFIAUTO:
		mlinkTipsTextViewPtr->setTextTr("Connected Android Auto device");
		break;
	case LINK_TYPE_AIRPLAY:
		mlinkTipsTextViewPtr->setTextTr("Connected AirPlay device");
		break;
	case LINK_TYPE_MIRACAST:
		mlinkTipsTextViewPtr->setTextTr("Connected Miracast device");
		break;
	case LINK_TYPE_WIFILY:
		mlinkTipsTextViewPtr->setTextTr("Connected Aicast device");
		break;
	default:
		LOGD("--%d-- --%s-- type = %d   连接类型错误! \n", __LINE__, __FILE__, link_type);
		break;
	}
	mlinkTipsWindowPtr->showWnd();
}

static void _enter_link_app(link_mode_e mode) {
    LYLINK_TYPE_E link_type = lk::get_lylink_type();

    switch (mode) {
    case E_LINK_MODE_CARPLAY:
        if ((link_type == LINK_TYPE_WIFICP) || (link_type == LINK_TYPE_USBCP)) {
            OPEN_APP(LYLINK_VIEW);
            return;
        }
        break;

    case E_LINK_MODE_ANDROIDAUTO:
        if ((link_type == LINK_TYPE_WIFIAUTO) || (link_type == LINK_TYPE_USBAUTO)) {
            OPEN_APP(LYLINK_VIEW);
            return;
        }
        break;

    case E_LINK_MODE_AIRPLAY:
        if (link_type == LINK_TYPE_AIRPLAY) {
            OPEN_APP(LYLINK_VIEW);
            return;
        }
        break;

    case E_LINK_MODE_MIRACAST:
        if (link_type == LINK_TYPE_MIRACAST) {
            OPEN_APP(LYLINK_VIEW);
            return;
        }
        break;

    case E_LINK_MODE_BLUEMUSIC:
        break;

    case E_LINK_MODE_WIFIVIDEO:
        break;

    case E_LINK_MODE_AICAST:
        if (link_type == LINK_TYPE_WIFILY) {
            OPEN_APP(LYLINK_VIEW);
            return;
        }
        break;
    }

    if (lk::is_connected()) {
        _show_link_tips(link_type);
        return;
    }

    Intent *i = new Intent;
    i->putExtra("link_mode", fy::format("%d", mode));
    EASYUICONTEXT->openActivity("LinkActivity", i);
}

} // namespace

/**
 * 注册定时器
 * 填充数组用于注册定时器
 * 注意:id不能重复
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
	if (!EASYUICONTEXT->isNaviBarShow()) {
		EASYUICONTEXT->showNaviBar();
	}
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
 * 不建议在此函数中写耗时操作,否则将影响UI刷新
 * 参数: id
 *         当前所触发定时器的id,与注册时的id相同
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
 * 参数:ev
 *         新的触摸事件
 * 返回值:true
 *            表示该触摸事件在此被拦截,系统不再将此触摸事件传递到控件上
 *         false
 *            触摸事件将继续传递到控件上
 */
static bool onmain2ActivityTouchEvent(const MotionEvent &ev) {
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

static bool onButtonClick_CPButton(ZKButton *pButton) {
    LOGD(" ButtonClick CPButton !!!\n");
    _enter_link_app(E_LINK_MODE_CARPLAY);
    return false;
}

static bool onButtonClick_AAButton(ZKButton *pButton) {
    LOGD(" ButtonClick AAButton !!!\n");
    _enter_link_app(E_LINK_MODE_ANDROIDAUTO);
    return false;
}

static bool onButtonClick_SETButton(ZKButton *pButton) {
    LOGD(" ButtonClick SETButton !!!\n");
    OPEN_APP("settings");
    return false;
}

static bool onButtonClick_APButton(ZKButton *pButton) {
    LOGD(" ButtonClick APButton !!!\n");
    _enter_link_app(E_LINK_MODE_AIRPLAY);
    return false;
}

static bool onButtonClick_wifiVideoButton(ZKButton *pButton) {
    LOGD(" ButtonClick wifiVideoButton !!!\n");
    _enter_link_app(E_LINK_MODE_WIFIVIDEO);
    return false;
}

static bool onButtonClick_ACButton(ZKButton *pButton) {
    LOGD(" ButtonClick ACButton !!!\n");
    _enter_link_app(E_LINK_MODE_MIRACAST);
    return false;
}

static bool onButtonClick_AudioOutputButton(ZKButton *pButton) {
    LOGD(" ButtonClick AudioOutputButton !!!\n");
    OPEN_APP("AudioOutput");
    return false;
}

static bool onButtonClick_MCButton(ZKButton *pButton) {
    LOGD(" ButtonClick MCButton !!!\n");
    _enter_link_app(E_LINK_MODE_AICAST);
    return false;
}

static bool onButtonClick_PlaybackButton(ZKButton *pButton) {
    LOGD(" ButtonClick PlaybackButton !!!\n");
    if (sys::setting::get_link_mode() == E_LINK_MODE_WIFIVIDEO) {
        if (mpi::AppServer::instance().isEnter()) {
            PopupService::dialog("@Tips", "@Connected Wifi Video", 2000);
            return false;
        }
    }
    OPEN_APP("DvrPlay");
    return false;
}

static bool onButtonClick_BluetoothappButton(ZKButton *pButton) {
    LOGD(" ButtonClick BluetoothappButton !!!\n");
    _enter_link_app(E_LINK_MODE_BLUEMUSIC);
    return false;
}
