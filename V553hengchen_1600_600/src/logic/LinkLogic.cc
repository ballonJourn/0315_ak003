#pragma once
#include "uart/ProtocolSender.h"
#include "net/context.h"
#include "net/ap.h"
#include "bt/context.h"
#include "link/context.h"
#include "system/setting.h"
#include "media/audio_context.h"
#include "mode_observer.h"
#include "common.h"
#include "edge/popup_service.h"
#include <base/timer.hpp>
#include "sysapp_context.h"
#include "misc/app_server_impl.h"
#include "misc/utility.h"

#define LINK_RESTART_TIMER    1

static bt_cb_t _s_bt_cb;
static bt_dev_t _s_next_connect_bt_info;				// 要切换的连接设备信息
static link_mode_e _s_target_link_mode;


static void _update_layout() {
	std::string bt_name = sys::setting::get_bt_name();
	std::string Title = LTOV("Conn_step");
	std::string wfTitle = LTOV("wf_tips");
	std::string cpStep =
			   	   	     "\n\n"
					   + LTOV("cp_step1") +  "\n\n"
					   + LTOV("cp_step2") + "\n"
					   + bt_name + "\n\n"
					   + LTOV("cp_step3");

	std::string cpTips = LTOV("Conn_tips") + "\n"
					   + LTOV("cp_tips1") +  "\n"
					   + LTOV("cp_tips2") +  "\n"
					   + LTOV("cp_tips3");


	std::string aaStep = "\n\n\n"
					   + LTOV("aa_step1") +  "\n\n"
					   + LTOV("aa_step2") + "\n"
					   + bt_name;

	std::string aaTips = LTOV("Conn_tips") + "\n"
					   + LTOV("aa_mc_tips1") + "\n"
					   + LTOV("aa_tips2") +  "\n";


	std::string apStep = "\n\n"
					   + LTOV("ap_step1") + "\n" + sys::setting::get_carplay_ap_name() + "\n\n"
					   + LTOV("ap_step2") + std::string("12345678") + "\n\n"
					   + LTOV("ap_step3")  + "\n" + sys::setting::get_airplay_name();

	std::string apTips = LTOV("Conn_tips") + "\n"
					   + LTOV("ap_tips1") +  "\n";

	std::string mcStep = "\n\n"
					   + LTOV("mc_step1") +  "\n\n"
					   + LTOV("mc_step2") +  "\n\n"
					   + LTOV("mc_step3") + sys::setting::get_miracast_name();

	std::string mcTips = LTOV("Conn_tips") + "\n"
					   + LTOV("aa_mc_tips1") + "\n";

	std::string wfStep = LTOV("wf_step1") + "\n\n"
					   + LTOV("wf_step2") + "\n\n"
					   + LTOV("wf_step3") + sys::setting::get_wifivideo_ap_name() + "\n"
					   + LTOV("wf_step4") + std::string("12345678");

	std::string acStep = LTOV("ac_step1") +  "\n\n"
					   + LTOV("ac_step2") +  "\n\n"
					   + LTOV("ac_step3") + sys::setting::get_miracast_name();

//	mcpStepTextViewPtr->setText(cpStep);
	mcpTipsTitleViewPtr->setText(Title);
	mcpTipsTextViewPtr->setText(cpStep);

//	maaStepTextViewPtr->setText(aaStep);
	maaTipsTitleViewPtr->setText(Title);
	maaTipsTextViewPtr->setText(aaStep);
//	mapStepTextViewPtr->setText(apStep);
	mapTipsTitleViewPtr->setText(Title);
	mapTipsTextViewPtr->setText(apStep);
//	mmcStepTextViewPtr->setText(mcStep);
	mmcTipsTitleViewPtr->setText(Title);
	mmcTipsTextViewPtr->setText(mcStep);
	mwifiSetupTitleViewPtr->setText(wfTitle);
	mwifiSetupTextViewPtr->setText(wfStep);
	mTextView2Ptr->setTextTr("wf_step5");
	macTipsTitleTextPtr->setText(Title);
	macTipsTextViewPtr->setText(acStep);
}

static void _enter_link() {
	mBtWndPtr->hideWnd();
//	EASYUICONTEXT->showNaviBar();
//	mode::set_switch_mode(E_SWITCH_MODE_DVR);
}

static void _event_mode_cb(event_mode_e mode) {
	if (E_EVENT_MODE_GOBACK == mode) {
		_enter_link();
	}
}

static std::string _time_format(int time) {
	char buf[32] = { 0 };
	sprintf(buf, "%02d:%02d", time/60, time%60);
	return buf;
}

static void _update_music_info() {
	bt_music_t music_info = bt::get_music_info();
	(strcmp(music_info.title.c_str(), "") == 0) ? mtittleTextViewPtr->setTextTr("Unknown") : mtittleTextViewPtr->setText(music_info.title);
	(strcmp(music_info.artist.c_str(), "") == 0) ? martistTextViewPtr->setTextTr("Unknown") : martistTextViewPtr->setText(music_info.artist);
	(strcmp(music_info.album.c_str(), "") == 0) ? malbumTextViewPtr->setTextTr("Unknown") : malbumTextViewPtr->setText(music_info.album);
//	mlyricTextViewPtr->setText(music_info.lyric);
}

static void _update_music_progress() {
	bt_music_t music_info = bt::get_music_info();

	mmusicSeekBarPtr->setMax(music_info.duration);
	mmusicSeekBarPtr->setProgress(music_info.curpos);
	mcurposTextViewPtr->setText(_time_format(music_info.curpos));
	mdurationTextViewPtr->setText(_time_format(music_info.duration));
}

static void _bt_music_cb(bt_music_state_e state) {
	if (bt::music_is_playing()) {
		_update_music_info();
		_update_music_progress();
		mtittleTextViewPtr->setTextColor(0xFF00FF40);
		mtittleTextViewPtr->setLongMode(ZKTextView::E_LONG_MODE_SCROLL);
		mplayButtonPtr->setSelected(true);
//		mmusicPngPtr->setBackgroundPic("new_linkhelp/icon_media_cover1_n.png");
	} else {
		mtittleTextViewPtr->setTextColor(0xFFFFFFFF);
		mplayButtonPtr->setSelected(false);
		mtittleTextViewPtr->setLongMode(ZKTextView::E_LONG_MODE_DOTS);
//		mmusicPngPtr->setBackgroundPic("new_linkhelp/icon_media_cover_n.png");
	}
}

static void _bt_power_cb(bt_power_state_e state) {
	switch (state) {
	case E_BT_POWER_STATE_OFF:
		mdisConnectWindowPtr->hideWnd();
		mconnDevTextPtr->setTextTr("no connect");
		menableButtonPtr->setInvalid(false);
		menableButtonPtr->setSelected(false);
		break;
	case E_BT_POWER_STATE_ON:
		menableButtonPtr->setInvalid(false);
		menableButtonPtr->setSelected(true);
		_update_layout();
		break;
	case E_BT_POWER_STATE_CHANGING:
		menableButtonPtr->setInvalid(true);
		break;
	}
}

static void _bt_connect_cb(bt_connect_state_e state) {
	switch (state) {
	case E_BT_CONNECT_STATE_IDLE:
		if (_s_next_connect_bt_info.addr != "") {
			bt::connect(_s_next_connect_bt_info.addr.c_str());
			_s_next_connect_bt_info.addr = "";
		} else {
			mconnDevTextPtr->setTextTr("no connect");
		}
		mdisconnectBtnPtr->setSelected(true);
		break;
	case E_BT_CONNECT_STATE_CONNECTING:
		mconnDevTextPtr->setTextTr("connecting");
		mdisconnectBtnPtr->setSelected(true);
		break;
	case E_BT_CONNECT_STATE_CONNECTED:
		mconnDevTextPtr->setText(bt::get_connect_dev().name);
		mdisconnectBtnPtr->setSelected(false);
		break;
	}
}

static void _update_dev_name() {
	mbtNameEditTextPtr->setText(bt::get_name());
}

static void _bt_misc_info_cb(bt_misc_info_e info) {
	switch (info) {
	case E_BT_MISC_INFO_DEV_NAME:
		_update_dev_name();
		break;
	case E_BT_MISC_INFO_AUTO_CONNECT_ON:  // 自动连接打开
//		mautoConnButtonPtr->setInvalid(false);
		mautoConnButtonPtr->setSelected(true);
		break;
	case E_BT_MISC_INFO_AUTO_CONNECT_OFF: // 自动连接关闭
//		mautoConnButtonPtr->setInvalid(false);
		mautoConnButtonPtr->setSelected(false);
		break;
	case E_BT_MISC_INFO_AUTH:             // 授权
		break;
	default:
		break;
	}
}

static void _bt_add_cb() {
	_s_bt_cb.power_cb = _bt_power_cb;
	_s_bt_cb.music_cb = _bt_music_cb;
	_s_bt_cb.misc_info_cb = _bt_misc_info_cb;
	_s_bt_cb.connect_cb = _bt_connect_cb;
	bt::add_cb(&_s_bt_cb);
}

static void _bt_remove_cb() {
	bt::remove_cb(&_s_bt_cb);
}

static void _set_tip_status(link_mode_e mode) {
	ZKButton *bnttab[] = {
		mCarplayBtnPtr,
		mandroidautoBtnPtr,
		mairplayBtnPtr,
		mmiracastBtnPtr,
		mbluemusicBtnPtr,
		mwifiButtonPtr,
		maicastButtonPtr
	};
	ZKWindow *wndtab[] = {
		mcpWindowPtr,
		maaWindowPtr,
		mapWindowPtr,
		mmiracastWindowPtr,
		mbluemusicWndPtr,
		mwifiWindowPtr,
		maicastWindowPtr
	};

	for (size_t i = 0; i < TAB_SIZE(bnttab); i++) {
		if (i == mode) {
			mTextView16Ptr->setBackgroundPic(NULL);
			mTextView16Ptr->setVisible(false);
			if (mode == E_LINK_MODE_CARPLAY || mode == E_LINK_MODE_ANDROIDAUTO) {
				mcpbtBtnPtr->setVisible(true);
				mcpbtTextviewPtr->setVisible(true);
			}
			if (mode == E_LINK_MODE_BLUEMUSIC) {
				mBtWndPtr->hideWnd();
			    mTextView16Ptr->setBackgroundPic("/new_linkhelp/BTClipboard.png");
				mTextView16Ptr->setVisible(true);
			}
			bnttab[i]->setSelected(true);
			wndtab[i]->showWnd();
		} else {
			if (mode != E_LINK_MODE_CARPLAY && mode != E_LINK_MODE_ANDROIDAUTO) {
				mcpbtBtnPtr->setVisible(false);
				mcpbtTextviewPtr->setVisible(false);
			}
			if (mode != E_LINK_MODE_BLUEMUSIC) {
				mBtWndPtr->hideWnd();
			}
			bnttab[i]->setSelected(false);
			wndtab[i]->hideWnd();
		}
	}
}

static void _set_tip_text(link_mode_e mode) {
	switch (mode) {
	case E_LINK_MODE_BLUEMUSIC:
		mTipinfoTextPtr->setText(LTOV("change_btmusic_tip"));
		break;
	case E_LINK_MODE_CARPLAY:
		mTipinfoTextPtr->setText(LTOV("change_carplay_tip"));
		break;
	case E_LINK_MODE_ANDROIDAUTO:
		mTipinfoTextPtr->setText(LTOV("change_auto_tip"));
		break;
	case E_LINK_MODE_AIRPLAY:
		mTipinfoTextPtr->setText(LTOV("change_airplay_tip"));
		break;
	case E_LINK_MODE_MIRACAST:
		mTipinfoTextPtr->setText(LTOV("change_miracast_tip"));
		break;
	case E_LINK_MODE_WIFIVIDEO:
		mTipinfoTextPtr->setText(LTOV("change_wifivideo_tip"));
		break;
	case E_LINK_MODE_AICAST:
		mTipinfoTextPtr->setText(LTOV("change_aicast_tip"));
		break;
	}
	if (mdisConnectWindowPtr->isWndShow()) {
		mdisConnectWindowPtr->hideWnd();
	}
	mTipWndPtr->showWnd();
}

static void _confirm_link_mode(link_mode_e mode) {
	link_mode_e last_mode = sys::setting::get_link_mode();
	if ((last_mode == E_LINK_MODE_WIFIVIDEO) || (mode == E_LINK_MODE_WIFIVIDEO)) {
		remove(NET_UDHCPD_LEASES_PATH);
	}
	sys::setting::set_link_mode(mode);
	if (mode != E_LINK_MODE_WIFIVIDEO) {
		stopAppServer();
	}
	switch (mode) {
	case E_LINK_MODE_CARPLAY:
	case E_LINK_MODE_ANDROIDAUTO:
	case E_LINK_MODE_BLUEMUSIC:
		if (!bt::is_on()) {
			bt::power_on();
		}
		if (E_LINK_MODE_BLUEMUSIC == mode) {
			mcpStepTextViewPtr->setBackgroundPic(NULL);
			maaStepTextViewPtr->setBackgroundPic(NULL);
			mapStepTextViewPtr->setBackgroundPic(NULL);
			mmcStepTextViewPtr->setBackgroundPic(NULL);
			audio::change_audio_type(E_AUDIO_TYPE_BT_MUSIC);
		} else if (E_LINK_MODE_CARPLAY == mode) {
			mcpStepTextViewPtr->setBackgroundPic("/new_linkhelp/carpaly_n.png");
			maaStepTextViewPtr->setBackgroundPic(NULL);
			mapStepTextViewPtr->setBackgroundPic(NULL);
			mmcStepTextViewPtr->setBackgroundPic(NULL);
		} else if (E_LINK_MODE_ANDROIDAUTO == mode) {
			mcpStepTextViewPtr->setBackgroundPic(NULL);
			maaStepTextViewPtr->setBackgroundPic("new_linkhelp/andoid_n.png");
			mapStepTextViewPtr->setBackgroundPic(NULL);
			mmcStepTextViewPtr->setBackgroundPic(NULL);
		}
		net::change_mode(E_NET_MODE_AP, true);
		break;
	case E_LINK_MODE_AIRPLAY:
		mcpStepTextViewPtr->setBackgroundPic(NULL);
		maaStepTextViewPtr->setBackgroundPic(NULL);
		mapStepTextViewPtr->setBackgroundPic("new_linkhelp/airpaly_n.png");
		mmcStepTextViewPtr->setBackgroundPic(NULL);
		if (bt::is_on()) {
			bt::power_off();
		}
		net::change_mode(E_NET_MODE_AP, true);
		break;
	case E_LINK_MODE_MIRACAST:
	case E_LINK_MODE_AICAST:
		if (E_LINK_MODE_MIRACAST == mode) {
			mcpStepTextViewPtr->setBackgroundPic(NULL);
			maaStepTextViewPtr->setBackgroundPic(NULL);
			mapStepTextViewPtr->setBackgroundPic(NULL);
			mmcStepTextViewPtr->setBackgroundPic("new_linkhelp/miracast_n.png");
		} else if (E_LINK_MODE_AICAST == mode) {
			mcpStepTextViewPtr->setBackgroundPic(NULL);
			maaStepTextViewPtr->setBackgroundPic(NULL);
			mapStepTextViewPtr->setBackgroundPic(NULL);
			mmcStepTextViewPtr->setBackgroundPic(NULL);
		}
		if (bt::is_on()) {
			bt::power_off();
		}
		net::change_mode(E_NET_MODE_P2P);
		break;
	case E_LINK_MODE_WIFIVIDEO:
		mcpStepTextViewPtr->setBackgroundPic(NULL);
		maaStepTextViewPtr->setBackgroundPic(NULL);
		mapStepTextViewPtr->setBackgroundPic(NULL);
		mmcStepTextViewPtr->setBackgroundPic(NULL);
		if (!bt::is_on()) {
			bt::power_on();
		}
		net::change_mode(E_NET_MODE_AP, true);
		startAppServer();
		LOGD("[lylink] wifi video start success");
		break;
	default:
		break;
	}

	mActivityPtr->registerUserTimer(LINK_RESTART_TIMER, 10);
}

static void _select_link_mode(link_mode_e mode) {
    if (sys::setting::get_link_mode() != mode) {
    	_s_target_link_mode = mode;
    	_set_tip_text(mode);
    } else {
    	if (mode == E_LINK_MODE_WIFIVIDEO) {
    		startAppServer();
    	}
    }
}

static void _init_bt_info() {
	_update_dev_name();
	menableButtonPtr->setSelected(bt::is_on());
	mautoConnButtonPtr->setSelected(bt::is_auto_connect());
	mconnDevTextPtr->setLongMode(ZKTextView::E_LONG_MODE_SCROLL_CIRCULAR);
	mconnectedNameTextViewPtr->setLongMode(ZKTextView::E_LONG_MODE_SCROLL_CIRCULAR);
	if (bt::get_connect_state() == E_BT_CONNECT_STATE_CONNECTED) {
		mconnDevTextPtr->setText(bt::get_connect_dev().name);
	}
}

static void _switch_link_screen_mode() {
	bool is_full = !sys::setting::is_link_screen_full();
	sys::setting::set_link_screen_full(is_full);
//	if (is_full) {
//		mcpScreenBtnPtr->setTextTr("Full Screen");
//	} else {
//		mcpScreenBtnPtr->setTextTr("Half screen mode");
//	}

	mActivityPtr->unregisterUserTimer(LINK_RESTART_TIMER);
	mActivityPtr->registerUserTimer(LINK_RESTART_TIMER, 500);
}

static void link_bg_show() {
	switch (sys::setting::get_link_mode()) {
	case E_LINK_MODE_CARPLAY:
	case E_LINK_MODE_ANDROIDAUTO:
	case E_LINK_MODE_AIRPLAY:
	case E_LINK_MODE_MIRACAST:
		if (E_LINK_MODE_CARPLAY == sys::setting::get_link_mode()) {
			mcpStepTextViewPtr->setBackgroundPic("/new_linkhelp/carpaly_n.png");
			maaStepTextViewPtr->setBackgroundPic(NULL);
			mapStepTextViewPtr->setBackgroundPic(NULL);
			mmcStepTextViewPtr->setBackgroundPic(NULL);
		} else if (E_LINK_MODE_ANDROIDAUTO == sys::setting::get_link_mode()) {
			mcpStepTextViewPtr->setBackgroundPic(NULL);
			maaStepTextViewPtr->setBackgroundPic("/new_linkhelp/andoid_n.png");
			mapStepTextViewPtr->setBackgroundPic(NULL);
			mmcStepTextViewPtr->setBackgroundPic(NULL);
		} else if (E_LINK_MODE_AIRPLAY == sys::setting::get_link_mode()){
			mcpStepTextViewPtr->setBackgroundPic(NULL);
			maaStepTextViewPtr->setBackgroundPic(NULL);
			mapStepTextViewPtr->setBackgroundPic("/new_linkhelp/airpaly_n.png");
			mmcStepTextViewPtr->setBackgroundPic(NULL);
		} else if (E_LINK_MODE_MIRACAST == sys::setting::get_link_mode()) {
			mcpStepTextViewPtr->setBackgroundPic(NULL);
			maaStepTextViewPtr->setBackgroundPic(NULL);
			mapStepTextViewPtr->setBackgroundPic(NULL);
			mmcStepTextViewPtr->setBackgroundPic("/new_linkhelp/miracast_n.png");
		}
		break;
	default:
		mcpStepTextViewPtr->setBackgroundPic(NULL);
		maaStepTextViewPtr->setBackgroundPic(NULL);
		mapStepTextViewPtr->setBackgroundPic(NULL);
		mmcStepTextViewPtr->setBackgroundPic(NULL);
		break;
	}
}

void lanyaConnectIcon(){
	martistTextViewPtr->setLongMode(ZKTextView::E_LONG_MODE_SCROLL_CIRCULAR);
	malbumTextViewPtr->setLongMode(ZKTextView::E_LONG_MODE_SCROLL_CIRCULAR);
	if (bt::get_connect_state() != E_BT_CONNECT_STATE_CONNECTED) {
		mdisconnectBtnPtr->setSelected(true);
	}
	else{
		mdisconnectBtnPtr->setSelected(false);
	}
}
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
static void onUI_init() {
//    mmusicPngPtr->setBackgroundPic("new_linkhelp/icon_media_cover_n.png");
	mnameLongTipsWindowPtr->setVisible(false);
    mBtWndPtr->setVisible(false);
    mbluemusicWndPtr->setVisible(false);
    mTextView16Ptr->setBackgroundPic(NULL);
    mTextView16Ptr->setVisible(false);
    //mcpScreenBtnPtr->setLongMode(ZKButton::E_LONG_MODE_SCROLL);
//    mcpbtBtnPtr->setLongMode(ZKButton::E_LONG_MODE_SCROLL);
    mbluemusicBtnPtr->setLongMode(ZKButton::E_LONG_MODE_SCROLL_CIRCULAR);
    //Tips :添加 UI初始化的显示代码到这里,如:mText1Ptr->setText("123");
	_bt_add_cb();
	_init_bt_info();
	_update_music_info();		// 音乐信息
	_update_music_progress();	// 进度信息

	_update_layout();
	_set_tip_status(sys::setting::get_link_mode());

	bool music_is_play = bt::music_is_playing();

	mtittleTextViewPtr->setLongMode(music_is_play ? ZKTextView::E_LONG_MODE_SCROLL : ZKTextView::E_LONG_MODE_DOTS);
	mplayButtonPtr->setSelected(music_is_play);
	mtittleTextViewPtr->setTextColor(music_is_play ? 0xFF00FF40 : 0xFFFFFFFF);
//	if (sys::setting::is_link_screen_full()) {
//		mcpScreenBtnPtr->setTextTr("Full Screen");
//	} else {
//		mcpScreenBtnPtr->setTextTr("Half screen mode");
//	}
	lanyaConnectIcon();
	mode::add_event_mode_cb(_event_mode_cb);
}

/**
 * 当切换到该界面时触发
 */
static void onUI_intent(const Intent *intentPtr) {
    if (intentPtr != NULL) {
    	_select_link_mode((link_mode_e) atoi(intentPtr->getExtra("link_mode").c_str()));
    }
}

/*
 * 当界面显示时触发
 */
static void onUI_show() {
	link_bg_show();
//	mpi::RearCamera::instance().stop();
//	mpi::FrontCamera::instance().stop();
	NO_EXCEPTION(setupCamera(VIEW_TYPE_UNVISIBLE, false));
	if (mBtWndPtr->isWndShow()) {
		_enter_link();
	}
	if (!EASYUICONTEXT->isNaviBarShow()) {
		EASYUICONTEXT->showNaviBar();
	}
	mbluemanageTextViewPtr->setTouchPass(true);
	mcpbtTextviewPtr->setTouchPass(true);
	mbluemanageTextViewPtr->setLongMode(ZKTextView::E_LONG_MODE_SCROLL_CIRCULAR);
	mcpbtTextviewPtr->setLongMode(ZKTextView::E_LONG_MODE_SCROLL_CIRCULAR);
}

static bool onUI_back() {
	if (mBtWndPtr->isWndShow()) {
		mBtWndPtr->hideWnd();
		return false;
	}
	return true;

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
	mode::remove_event_mode_cb(_event_mode_cb);
	_bt_remove_cb();
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
	case LINK_RESTART_TIMER:
		lk::restart_lylink();
		return false;
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
static bool onLinkActivityTouchEvent(const MotionEvent &ev) {
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

static void onProgressChanged_musicSeekBar(ZKSeekBar *pSeekBar, int progress) {
    //LOGD(" ProgressChanged musicSeekBar %d !!!\n", progress);
}

static bool onButtonClick_playButton(ZKButton *pButton) {
    LOGD(" ButtonClick playButton !!!\n");
    if (bt::music_is_playing()) {
    	bt::music_pause();
    } else {
    	bt::music_play();
    }
    return false;
}

static bool onButtonClick_prevButton(ZKButton *pButton) {
    LOGD(" ButtonClick prevButton !!!\n");
    bt::music_prev();
    return false;
}

static bool onButtonClick_nextButton(ZKButton *pButton) {
    LOGD(" ButtonClick nextButton !!!\n");
    bt::music_next();
    return false;
}

static bool onButtonClick_cpbtBtn(ZKButton *pButton) {
    LOGD(" ButtonClick cpbtBtn !!!\n");
    mBtWndPtr->showWnd();
//	mode::set_switch_mode(E_SWITCH_MODE_GOBACK);
    return false;
}

static bool onButtonClick_cpScreenBtn(ZKButton *pButton) {
    LOGD(" ButtonClick cpScreenBtn !!!\n");
    _switch_link_screen_mode();
    return false;
}

static bool onButtonClick_CarplayBtn(ZKButton *pButton) {
    LOGD(" ButtonClick CarplayBtn !!!\n");
    _select_link_mode(E_LINK_MODE_CARPLAY);
    return false;
}

static bool onButtonClick_bluemusicBtn(ZKButton *pButton) {
    LOGD(" ButtonClick bluemusicBtn !!!\n");
    _select_link_mode(E_LINK_MODE_BLUEMUSIC);
    return false;
}

static bool onButtonClick_androidautoBtn(ZKButton *pButton) {
    LOGD(" ButtonClick androidautoBtn !!!\n");
    _select_link_mode(E_LINK_MODE_ANDROIDAUTO);
    return false;
}

static bool onButtonClick_miracastBtn(ZKButton *pButton) {
    LOGD(" ButtonClick miracastBtn !!!\n");
    _select_link_mode(E_LINK_MODE_MIRACAST);
    return false;
}

static bool onButtonClick_airplayBtn(ZKButton *pButton) {
    LOGD(" ButtonClick airplayBtn !!!\n");
    _select_link_mode(E_LINK_MODE_AIRPLAY);
    return false;
}

static bool onButtonClick_changesureBtn(ZKButton *pButton) {
    LOGD(" ButtonClick changesureBtn !!!\n");
	_confirm_link_mode(_s_target_link_mode);
	_set_tip_status(_s_target_link_mode);
	mTipWndPtr->hideWnd();
    return false;
}

static bool onButtonClick_changecancelBtn(ZKButton *pButton) {
    LOGD(" ButtonClick changecancelBtn !!!\n");
    mTipWndPtr->hideWnd();
    return false;
}

static void onEditTextChanged_btNameEditText(const std::string &text) {
    //LOGD(" onEditTextChanged_ btNameEditText %s !!!\n", text.c_str());
	if (text != bt::get_name()) {
		if (text.length() > 26) {
			sys::setting::set_bt_name(text.substr(0, 26).c_str());
			mbtNameEditTextPtr->setText(text.substr(0, 26));
			mnameLongTipsWindowPtr->showWnd();
		}
		else {
			sys::setting::set_bt_name(text);
		}
		bt::modify_name(sys::setting::get_bt_name().c_str());
	}
}

static bool onButtonClick_enableButton(ZKButton *pButton) {
  LOGD(" ButtonClick enableButton !!!\n");
  PopupService::show([](PopupService* srv) {
    if (bt::is_on()) {
      srv->setMessage("@waiting_for_bt_off");
      bt::power_off();
    } else {
      srv->setMessage("@waiting_for_bt_on");
      bt::power_on();
    }
    base::timer t;
    while (true) {
      if (t.elapsed() > 5000) {
        break;
      }
      if (bt::is_on()) {
        break;
      }
      Thread::sleep(100);
    }
    if (t.elapsed() < 1000) {
      //至少等待一秒
      Thread::sleep(abs(1000 - t.elapsed()));
    }
    return 0;
  });
  return false;
}

static bool onButtonClick_connDevText(ZKButton *pButton) {
    LOGD(" ButtonClick connDevText !!!\n");
    return false;
}

static bool onButtonClick_disconnectBtn(ZKButton *pButton) {
    LOGD(" ButtonClick disconnectBtn !!!\n");
    if (bt::get_connect_state() != E_BT_CONNECT_STATE_CONNECTED) {
    	mdisconnectBtnPtr->setSelected(true);
    	return false;
    }
    mdisconnectBtnPtr->setSelected(false);
    mconnectedNameTextViewPtr->setText(mconnDevTextPtr->getText());
    mdisConnectWindowPtr->showWnd();
    return false;
}

static bool onButtonClick_autoConnButton(ZKButton *pButton) {
    LOGD(" ButtonClick autoConnButton !!!\n");
    bt::set_auto_connect(!pButton->isSelected());
    bt::query_mf();
    return false;
}

static bool onButtonClick_disconCancelButton(ZKButton *pButton) {
    LOGD(" ButtonClick disconCancelButton !!!\n");
    mdisConnectWindowPtr->hideWnd();
    return false;
}

static bool onButtonClick_disConnectButton(ZKButton *pButton) {
    LOGD(" ButtonClick disConnectButton !!!\n");
    bt::disconnect();
    mdisConnectWindowPtr->hideWnd();
    return false;
}

static bool onButtonClick_btButton(ZKButton *pButton) {
    LOGD(" ButtonClick btButton !!!\n");
    mBtWndPtr->showWnd();
//	mode::set_switch_mode(E_SWITCH_MODE_GOBACK);
    return false;
}
static bool onButtonClick_HornButton(ZKButton *pButton) {
    LOGD(" ButtonClick HornButton !!!\n");
    if (app::is_show_ctrlbar() && (mode::get_event_mode() == E_EVENT_MODE_VOICE)) {
      app::hide_ctrlbar();
    } else {
      mode::set_event_mode(E_EVENT_MODE_VOICE);
      app::show_ctrlbar();
    }
    return false;
}
static bool onButtonClick_wifiButton(ZKButton *pButton) {
    LOGD(" ButtonClick wifiButton !!!\n");
    _select_link_mode(E_LINK_MODE_WIFIVIDEO);
    return false;
}
static bool onButtonClick_musicZhen(ZKButton *pButton) {
    LOGD(" ButtonClick musicZhen !!!\n");
    return false;
}
static bool onButtonClick_aicastButton(ZKButton *pButton) {
    LOGD(" ButtonClick aicastButton !!!\n");
    _select_link_mode(E_LINK_MODE_AICAST);
    return false;
}
