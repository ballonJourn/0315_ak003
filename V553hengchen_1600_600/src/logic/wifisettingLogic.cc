#pragma once
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

#include "net/NetManager.h"
#include "net/context.h"
#include "system/reverse.h"
#include "system/setting.h"

#define WIFIMANAGER			NETMANAGER->getWifiManager()


static std::vector<WifiInfo> save_wifi_v;

static std::vector<WifiInfo> sWifiInfos;
static std::map<std::string, std::string> sWifiChangeAps;
static Mutex sLock;

static void removeItemWifiChangeAps(const char *bssid) {
	Mutex::Autolock _l(sLock);
	std::map<std::string, std::string>::iterator it;
	it = sWifiChangeAps.find(bssid);
	if (it != sWifiChangeAps.end()) {
		sWifiChangeAps.erase(it);
	}
}

// 连接设备位置调整为第一项
static void set_connect_info_to_first() {
	WifiInfo* conn_wifi = WIFIMANAGER->getConnectionInfo();
	for (size_t i=0; i<sWifiInfos.size(); i++) {
		LOGD(" --sWifiInfos[%d].Bssid:%s---conn_wifi.Bassid:%s--\n", i, sWifiInfos[i].getBssid().c_str(), conn_wifi->getBssid().c_str());
		if (sWifiInfos[i].getBssid() == conn_wifi->getBssid()) {
			LOGD("--%d-- --%s-- 更新WiFi信号强度! Level:%d\n", __LINE__, __FILE__, sWifiInfos[i].getLevel());
			conn_wifi->setLevel(sWifiInfos[i].getLevel());		//更新信号强度
			sWifiInfos.erase(sWifiInfos.begin() + i);
			sWifiInfos.insert(sWifiInfos.begin(), *conn_wifi);
			break;
		}
	}
}

static void net_mode_cb_(net_mode_e mode) {
	switch (mode) {
	case E_NET_MODE_NULL:
	case E_NET_MODE_AP:     // 热点
	case E_NET_MODE_P2P:    // wifi直连
		mButtonOnOffPtr->setInvalid(false);
		mButtonOnOffPtr->setSelected(false);
		break;
	case E_NET_MODE_WIFI:   // wifi sta
		mButtonOnOffPtr->setInvalid(false);
		mButtonOnOffPtr->setSelected(true);
		break;
	}
}

static void refreshWifiList() {
	if (EASYUICONTEXT->isIMEShow()) {
		return;
	}

	mListViewWifiInfoPtr->refreshListView();
}

class MyWifiListener : public WifiManager::IWifiListener {
public:
//	virtual void handleWifiEnable(E_WIFI_ENABLE event, int args) {
//		LOGD("MyWifiListener handleWifiEnable event: %d\n", event);
//		switch (event) {
//		case E_WIFI_ENABLE_ENABLE:
//			mButtonOnOffPtr->setInvalid(false);
//			mButtonOnOffPtr->setSelected(true);
//			break;
//		case E_WIFI_ENABLE_DISABLE:
//			mButtonOnOffPtr->setInvalid(false);
//			mButtonOnOffPtr->setSelected(false);
//			break;
//		case E_WIFI_ENABLE_DISABLEING:
//		case E_WIFI_ENABLE_ENABLEING:
//			mButtonOnOffPtr->setInvalid(true);
//			mButtonOnOffPtr->setSelected(false);
//			break;
//		case E_WIFI_ENABLE_UNKNOW:
//			mButtonOnOffPtr->setInvalid(false);
//			mButtonOnOffPtr->setSelected(false);
//			break;
//		}
//	}

	virtual void handleWifiConnect(E_WIFI_CONNECT event, int args) {
		LOGD("MyWifiListener handleWifiConnect event: %d\n", event);
		switch (event) {
		case E_WIFI_CONNECT_CONNECTED:
			LOGD("连接成功!");
			set_connect_info_to_first();
			break;
		case E_WIFI_CONNECT_DISCONNECT:
		case E_WIFI_CONNECT_CONNECTING:
		case E_WIFI_CONNECT_DICONNECTING:
		case E_WIFI_CONNECT_ERROR:
		case E_WIFI_CONNECT_UNKNOW:
			break;
		}

		refreshWifiList();
	}

	virtual void handleWifiErrorCode(E_WIFI_ERROR_CODE code) {
		mWindowPasswordErrorPtr->showWnd();
	}

	virtual void handleWifiScanResult(std::vector<WifiInfo>* wifiInfos) {
		if (wifiInfos) {		// 扫描结果
			Mutex::Autolock _l(sLock);
			sWifiInfos.assign(wifiInfos->begin(), wifiInfos->end());
			if (WIFIMANAGER->isConnected()) {
				LOGD("扫描完成!");
				set_connect_info_to_first();
			}
		}

		refreshWifiList();
	}

	virtual void notifySupplicantStateChange(int networkid,
			const char* ssid, const char* bssid,
			E_SUPPLICATION_STATE newState) {
		switch (newState) {
		case AUTHENTICATING: {
			Mutex::Autolock _l(sLock);
			sWifiChangeAps[bssid] = "认证中...";
			break;
		}
		case ASSOCIATING:
			break;
		case ASSOCIATED:
			break;
		case FOUR_WAY_HANDSHAKE:
			break;
		case GROUP_HANDSHAKE: {
			Mutex::Autolock _l(sLock);
			sWifiChangeAps[bssid] = "正在获取IP";
			break;
		}
		case COMPLETED: {
			Mutex::Autolock _l(sLock);
			sWifiChangeAps.clear();
			break;
		}

		case DISCONNECTED:
		case INTERFACE_DISABLED:
		case INACTIVE:
		case SCANNING:
		case DORMANT:
		case UNINITIALIZED:
		case INVALID:
			removeItemWifiChangeAps(bssid);
			break;
		}

		refreshWifiList();
	}
};

static MyWifiListener sMyWifiListener;


static std::string getEncryptionInfo(const WifiInfo &wi) {
	std::string info;

	const std::string &encryption = wi.getEncryption();

	if (encryption.find("WEP") != std::string::npos) {
		info = "WEP";
	}
	if (encryption.find("WPA") != std::string::npos) {
		info += info.empty() ? "WPA" : " WPA";
	}
	if (encryption.find("WPA2") != std::string::npos) {
		info += info.empty() ? "WPA2" : " WPA2";
	}
	if (encryption.find("WPS") != std::string::npos) {
		info += "(WPS)";
	}
	if (info.empty()) {
		info = "Openness";
	}

	return info;
}

/**
 * 注册定时器
 * 在此数组中添加即可
 */
static S_ACTIVITY_TIMEER REGISTER_ACTIVITY_TIMER_TAB[] = {
	//{0,  6000}, //定时器id=0, 时间间隔6秒
	{2,  10},
};

static void onUI_init() {
    //Tips :添加 UI初始化的显示代码到这里,如:mText1Ptr->setText("123");
	mButtonOnOffPtr->setSelected(WIFIMANAGER->isWifiEnable());

	mTextIPAddrPtr->setText(WIFIMANAGER->getIp());
	mTextMacAddrPtr->setText(WIFIMANAGER->getMacAddr());

	WIFIMANAGER->getWifiScanInfosLock(sWifiInfos);
	if (WIFIMANAGER->isConnected()) { LOGD("进入界面!"); set_connect_info_to_first(); }

	WIFIMANAGER->addWifiListener(&sMyWifiListener);
	net::add_mode_update_cb(net_mode_cb_);
}

/*
 * 当界面显示时触发
 */
static void onUI_show() {

}

static void onUI_quit() {
	WIFIMANAGER->removeWifiListener(&sMyWifiListener);
	net::remove_mode_update_cb(net_mode_cb_);

	sWifiInfos.clear();/*scan_wifi_v*/
	sWifiChangeAps.clear();
}

static void onProtocolDataUpdate(const SProtocolData &data) {
    // 串口数据回调接口
}

static bool onUI_Timer(int id) {
    //Tips:添加定时器响应的代码到这里,但是需要在本文件的 REGISTER_ACTIVITY_TIMER_TAB 数组中 注册
    //id 是定时器设置时候的标签,这里不要写耗时的操作，否则影响UI刷新,ruturn:[true] 继续运行定时器;[false] 停止运行当前定时器
	switch (id) {
//	case 1:
//		if (mButtonOnOffPtr->isSelected() && !WIFIMANAGER->isWifiEnable()) {
//			WIFIMANAGER->enableWifi(true);
//		}
//		else if (!mButtonOnOffPtr->isSelected() && WIFIMANAGER->isWifiEnable()) {
//			WIFIMANAGER->enableWifi(false);
//		}
//		break;
	}
    return true;
}

static bool onwifisettingActivityTouchEvent(const MotionEvent &ev) {
    // 返回false触摸事件将继续传递到控件上，返回true表示该触摸事件在此被拦截了，不再传递到控件上
    return false;
}

static int getListItemCount_ListViewWifiInfo(const ZKListView *pListView) {
    //LOGD(" getListItemCount_ ListViewWifiInfo  !!!\n");
	Mutex::Autolock _l(sLock);
	return sWifiInfos.size(); /*save_wifi_v.size() + scan_wifi_v.size();*/
}

static void obtainListItemData_ListViewWifiInfo(ZKListView *pListView,ZKListView::ZKListItem *pListItem, int index) {
    //LOGD(" obtainListItemData_ ListViewWifiInfo  !!!\n");
	ZKListView::ZKListSubItem *pLevelItem = pListItem->findSubItemByID(ID_WIFISETTING_LISTSUBITEM_LEVEL);
	ZKListView::ZKListSubItem *pNameItem = pListItem->findSubItemByID(ID_WIFISETTING_LISTSUBITEM_NAME);
//	ZKListView::ZKListSubItem *pSubItem = pListItem->findSubItemByID(ID_WIFISETTING_LISTSUBITEM_SUB);
	ZKListView::ZKListSubItem *lockSubItem = pListItem->findSubItemByID(ID_WIFISETTING_lockSubItem);
	ZKListView::ZKListSubItem *SubTab = pListItem->findSubItemByID(ID_WIFISETTING_SubTab);

	Mutex::Autolock _l(sLock);
	bool isVisble = (WIFIMANAGER->isConnected() && index == 2) || index == 0 ;
	SubTab->setVisible(isVisble);
	pLevelItem->setVisible(!isVisble);
	pNameItem->setVisible(!isVisble);
//	pSubItem->setVisible(!isVisble);
	lockSubItem->setVisible(!isVisble);

	if (WIFIMANAGER->isConnected()) {
		if (index == 0) {
			SubTab->setTextTr("Connected to WIFI");
		}
		else if (index == 1) {
			pNameItem->setText(sWifiInfos.at(0).getSsid());
			pNameItem->setSelected(true);
//			pSubItem->setSelected(true);
			lockSubItem->setSelected(true);
//			pSubItem->setText("已连接");
			pLevelItem->setText(sWifiInfos.at(0).getLevel() + 5);
		}
		else if (index == 2) {
			SubTab->setTextTr("Available WIFI");
		}
		else {
			pNameItem->setText(sWifiInfos.at(index-2).getSsid());
			pNameItem->setSelected(false);
//			pSubItem->setSelected(false);
			lockSubItem->setSelected(false);
//			pSubItem->setText("");
			pLevelItem->setText(sWifiInfos.at(index-2).getLevel());
		}
	}
	else {
		if (index == 0) {
			SubTab->setTextTr("Available WIFI");
		}
		else {
			pNameItem->setText(sWifiInfos.at(index-1).getSsid());
			pNameItem->setSelected(false);
//			pSubItem->setSelected(false);
			lockSubItem->setSelected(false);
//			pSubItem->setText("");
			pLevelItem->setText(sWifiInfos.at(index-1).getLevel());
		}
	}

//	if (wi.getEncryption().length() < 5) {		//加密
//		lockSubItem->setVisible(true);
//
//	} else {
//		lockSubItem->setVisible(false);
//	}
//	pNameItem->setText(wi.getSsid());
//
//	if (WIFIMANAGER->isConnected() && (WIFIMANAGER->getConnectionInfo()->getBssid() == wi.getBssid())) {
//		pNameItem->setSelected(true);
//		pSubItem->setSelected(true);
//		lockSubItem->setSelected(true);
//		pSubItem->setText("已连接");
//		pLevelItem->setText(wi.getLevel() + 5);
//	} else {
//		pNameItem->setSelected(false);
//		pSubItem->setSelected(false);
//		lockSubItem->setSelected(false);
//		pSubItem->setText("");
//		pLevelItem->setText(wi.getLevel());
//	}
}

static void onListItemClick_ListViewWifiInfo(ZKListView *pListView, int index, int id) {
    //LOGD(" onListItemClick_ ListViewWifiInfo  !!!\n");
	Mutex::Autolock _l(sLock);

	if ((WIFIMANAGER->isConnected() && index == 2) || index == 0) {
		return;
	}

	const WifiInfo& wi = sWifiInfos.at(WIFIMANAGER->isConnected() ? (index == 1 ? 0 : index-2) : index-1);

	if (WIFIMANAGER->isConnected() && (WIFIMANAGER->getConnectionInfo()->getBssid() == wi.getBssid())) {
		mTextConnectSsidPtr->setText(wi.getSsid());
		if ("Openness" == getEncryptionInfo(wi)) {
			mTextConnectSecTypePtr->setTextTr("Openness");
		} else {
			mTextConnectSecTypePtr->setText(getEncryptionInfo(wi));
		}

		mWindowDisconnectPtr->showWnd();
		mTextIPAddrPtr->setText(WIFIMANAGER->getIp());
		mTextMacAddrPtr->setText(WIFIMANAGER->getMacAddr());
	} else {
		mTextSsidPtr->setText(wi.getSsid());

		if ("Openness" == getEncryptionInfo(wi)) {
			mTextSecTypePtr->setTextTr("Openness");
		} else {
			mTextSecTypePtr->setText(getEncryptionInfo(wi));
		}

		mEdittextPwdPtr->setText("");
		mWindowSetPtr->showWnd();
	}
}

static bool onButtonClick_ButtonOnOff(ZKButton *pButton) {
    //LOGD(" ButtonClick ButtonOnOff !!!\n");
    if (!pButton->isInvalid()) {
    	net::change_mode(net::get_mode() == E_NET_MODE_WIFI ? E_NET_MODE_AP : E_NET_MODE_WIFI);
    	pButton->setInvalid(true);
    }
    return false;
}

static bool onButtonClick_ButtonConnect(ZKButton *pButton) {
    //LOGD(" ButtonClick ButtonConnect !!!\n");
	WIFIMANAGER->connect(mTextSsidPtr->getText(), mEdittextPwdPtr->getText());
	mWindowSetPtr->hideWnd();
    return false;
}

static void onEditTextChanged_EdittextPwd(const std::string &text) {
    //LOGD(" onEditTextChanged_ EdittextPwd %s !!!\n", text.c_str());
}

static bool onButtonClick_ButtonShowPwd(ZKButton *pButton) {
    //LOGD(" ButtonClick ButtonShowPwd !!!\n");
	pButton->setSelected(!pButton->isSelected());
	mEdittextPwdPtr->setPassword(!pButton->isSelected());
    return false;
}

static bool onButtonClick_ButtonDisconnect(ZKButton *pButton) {
    //LOGD(" ButtonClick ButtonDisconnect !!!\n");
	WIFIMANAGER->disconnect();
	mWindowDisconnectPtr->hideWnd();
    return false;
}
static bool onButtonClick_sys_home(ZKButton *pButton) {
    LOGD(" ButtonClick sys_home !!!\n");
	if (sys::reverse_does_enter_status()) {
		return true;
	}
    return false;
}

static bool onButtonClick_settingButton(ZKButton *pButton) {
    LOGD(" ButtonClick settingButton !!!\n");
    EASYUICONTEXT->openActivity("settingsActivity");
    return false;
}

static bool onButtonClick_sys_back(ZKButton *pButton) {
    LOGD(" ButtonClick sys_back !!!\n");
    return false;
}
