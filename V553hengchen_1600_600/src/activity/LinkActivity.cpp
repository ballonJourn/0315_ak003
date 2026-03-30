/***********************************************
/gen auto by zuitools
***********************************************/
#include "LinkActivity.h"

/*TAG:GlobalVariable全局变量*/
static ZKWindow* mWindow3Ptr;
static ZKTextView* mTextView17Ptr;
static ZKTextView* mTextView16Ptr;
static ZKTextView* mcpbtTextviewPtr;
static ZKTextView* mbluemanageTextViewPtr;
static ZKTextView* mTextView15Ptr;
static ZKTextView* mTextView14Ptr;
static ZKTextView* mTextView13Ptr;
static ZKTextView* maccodeTextViewPtr;
static ZKButton* maicastButtonPtr;
static ZKTextView* macTipsTextViewPtr;
static ZKTextView* macTipsTitleTextPtr;
static ZKWindow* maicastWindowPtr;
static ZKTextView* mTextView11Ptr;
static ZKTextView* mTextView10Ptr;
static ZKTextView* mTextView2Ptr;
static ZKTextView* mTextView8Ptr;
static ZKTextView* mwifiSetupTitleViewPtr;
static ZKTextView* mmcTipsTitleViewPtr;
static ZKTextView* mapTipsTitleViewPtr;
static ZKTextView* maaTipsTitleViewPtr;
static ZKTextView* mcpTipsTitleViewPtr;
static ZKButton* mmusicZhenPtr;
static ZKTextView* malbumIconPtr;
static ZKTextView* martistIconPtr;
static ZKTextView* mline4Ptr;
static ZKTextView* mline3Ptr;
static ZKTextView* mline2Ptr;
static ZKTextView* mline1Ptr;
static ZKWindow* mWindow2Ptr;
static ZKTextView* mTextView12Ptr;
static ZKWindow* mWindow1Ptr;
static ZKTextView* mmusicPngPtr;
static ZKWindow* mWinMusicPngPtr;
static ZKButton* mwifiButtonPtr;
static ZKTextView* mwifiVideoTextViewPtr;
static ZKTextView* mwifiSetupTextViewPtr;
static ZKWindow* mwifiWindowPtr;
static ZKButton* mHornButtonPtr;
static ZKButton* mbtButtonPtr;
static ZKTextView* mmcStepTextViewPtr;
static ZKTextView* mapStepTextViewPtr;
static ZKTextView* maaStepTextViewPtr;
static ZKTextView* mcpStepTextViewPtr;
static ZKTextView* mconnectedNameTextViewPtr;
static ZKButton* mdisConnectButtonPtr;
static ZKTextView* mTextView9Ptr;
static ZKButton* mdisconCancelButtonPtr;
static ZKWindow* mdisConnectWindowPtr;
static ZKWindow* mnameLongTipsWindowPtr;
static ZKButton* mautoConnButtonPtr;
static ZKButton* mdisconnectBtnPtr;
static ZKButton* mconnDevTextPtr;
static ZKTextView* mTextView7Ptr;
static ZKButton* menableButtonPtr;
static ZKEditText* mbtNameEditTextPtr;
static ZKTextView* mTextView6Ptr;
static ZKTextView* mTextView5Ptr;
static ZKTextView* mTextView4Ptr;
static ZKTextView* mTextView3Ptr;
static ZKTextView* mTextView1Ptr;
static ZKWindow* mBtWndPtr;
static ZKButton* mchangecancelBtnPtr;
static ZKButton* mchangesureBtnPtr;
static ZKTextView* mTipinfoTextPtr;
static ZKWindow* mTipWndPtr;
static ZKButton* mairplayBtnPtr;
static ZKButton* mmiracastBtnPtr;
static ZKButton* mandroidautoBtnPtr;
static ZKButton* mbluemusicBtnPtr;
static ZKButton* mCarplayBtnPtr;
static ZKTextView* mmcTipsTextViewPtr;
static ZKWindow* mmiracastWindowPtr;
static ZKTextView* mapTipsTextViewPtr;
static ZKWindow* mapWindowPtr;
static ZKButton* mcpbtBtnPtr;
static ZKTextView* mcpTipsTextViewPtr;
static ZKWindow* mcpWindowPtr;
static ZKTextView* maaTipsTextViewPtr;
static ZKWindow* maaWindowPtr;
static ZKTextView* mtittleTextViewPtr;
static ZKTextView* martistTextViewPtr;
static ZKTextView* malbumTextViewPtr;
static ZKTextView* mcurposTextViewPtr;
static ZKTextView* mdurationTextViewPtr;
static ZKButton* mnextButtonPtr;
static ZKButton* mprevButtonPtr;
static ZKButton* mplayButtonPtr;
static ZKSeekBar* mmusicSeekBarPtr;
static ZKWindow* mbluemusicWndPtr;
static LinkActivity* mActivityPtr;

/*register activity*/
REGISTER_ACTIVITY(LinkActivity);

typedef struct {
	int id; // 定时器ID ， 不能重复
	int time; // 定时器  时间间隔  单位 毫秒
}S_ACTIVITY_TIMEER;

#include "logic/LinkLogic.cc"

/***********/
typedef struct {
    int id;
    const char *pApp;
} SAppInfo;

/**
 *点击跳转window
 */
static SAppInfo sAppInfoTab[] = {
//  { ID_MAIN_TEXT, "TextViewActivity" },
};

/***************/
typedef bool (*ButtonCallback)(ZKButton *pButton);
/**
 * button onclick表
 */
typedef struct {
    int id;
    ButtonCallback callback;
}S_ButtonCallback;

/*TAG:ButtonCallbackTab按键映射表*/
static S_ButtonCallback sButtonCallbackTab[] = {
    ID_LINK_aicastButton, onButtonClick_aicastButton,
    ID_LINK_musicZhen, onButtonClick_musicZhen,
    ID_LINK_wifiButton, onButtonClick_wifiButton,
    ID_LINK_HornButton, onButtonClick_HornButton,
    ID_LINK_btButton, onButtonClick_btButton,
    ID_LINK_disConnectButton, onButtonClick_disConnectButton,
    ID_LINK_disconCancelButton, onButtonClick_disconCancelButton,
    ID_LINK_autoConnButton, onButtonClick_autoConnButton,
    ID_LINK_disconnectBtn, onButtonClick_disconnectBtn,
    ID_LINK_connDevText, onButtonClick_connDevText,
    ID_LINK_enableButton, onButtonClick_enableButton,
    ID_LINK_changecancelBtn, onButtonClick_changecancelBtn,
    ID_LINK_changesureBtn, onButtonClick_changesureBtn,
    ID_LINK_airplayBtn, onButtonClick_airplayBtn,
    ID_LINK_miracastBtn, onButtonClick_miracastBtn,
    ID_LINK_androidautoBtn, onButtonClick_androidautoBtn,
    ID_LINK_bluemusicBtn, onButtonClick_bluemusicBtn,
    ID_LINK_CarplayBtn, onButtonClick_CarplayBtn,
    ID_LINK_cpbtBtn, onButtonClick_cpbtBtn,
    ID_LINK_nextButton, onButtonClick_nextButton,
    ID_LINK_prevButton, onButtonClick_prevButton,
    ID_LINK_playButton, onButtonClick_playButton,
};
/***************/


typedef void (*SeekBarCallback)(ZKSeekBar *pSeekBar, int progress);
typedef struct {
    int id;
    SeekBarCallback callback;
}S_ZKSeekBarCallback;
/*TAG:SeekBarCallbackTab*/
static S_ZKSeekBarCallback SZKSeekBarCallbackTab[] = {
    ID_LINK_musicSeekBar, onProgressChanged_musicSeekBar,
};


typedef int (*ListViewGetItemCountCallback)(const ZKListView *pListView);
typedef void (*ListViewobtainListItemDataCallback)(ZKListView *pListView,ZKListView::ZKListItem *pListItem, int index);
typedef void (*ListViewonItemClickCallback)(ZKListView *pListView, int index, int id);
typedef struct {
    int id;
    ListViewGetItemCountCallback getListItemCountCallback;
    ListViewobtainListItemDataCallback obtainListItemDataCallback;
    ListViewonItemClickCallback onItemClickCallback;
}S_ListViewFunctionsCallback;
/*TAG:ListViewFunctionsCallback*/
static S_ListViewFunctionsCallback SListViewFunctionsCallbackTab[] = {
};


typedef void (*SlideWindowItemClickCallback)(ZKSlideWindow *pSlideWindow, int index);
typedef struct {
    int id;
    SlideWindowItemClickCallback onSlideItemClickCallback;
}S_SlideWindowItemClickCallback;
/*TAG:SlideWindowFunctionsCallbackTab*/
static S_SlideWindowItemClickCallback SSlideWindowItemClickCallbackTab[] = {
};


typedef void (*EditTextInputCallback)(const std::string &text);
typedef struct {
    int id;
    EditTextInputCallback onEditTextChangedCallback;
}S_EditTextInputCallback;
/*TAG:EditTextInputCallback*/
static S_EditTextInputCallback SEditTextInputCallbackTab[] = {
    ID_LINK_btNameEditText, onEditTextChanged_btNameEditText,
};

typedef void (*VideoViewCallback)(ZKVideoView *pVideoView, int msg);
typedef struct {
    int id; //VideoView ID
    bool loop; // 是否是轮播类型
    int defaultvolume;//轮播类型时,默认视频音量
    VideoViewCallback onVideoViewCallback;
}S_VideoViewCallback;
/*TAG:VideoViewCallback*/
static S_VideoViewCallback SVideoViewCallbackTab[] = {
};


LinkActivity::LinkActivity() {
	//todo add init code here
	mVideoLoopIndex = -1;
	mVideoLoopErrorCount = 0;
}

LinkActivity::~LinkActivity() {
  //todo add init file here
  // 退出应用时需要反注册
    EASYUICONTEXT->unregisterGlobalTouchListener(this);
    unregisterProtocolDataUpdateListener(onProtocolDataUpdate);
    onUI_quit();
    mActivityPtr = NULL;
    mWindow3Ptr = NULL;
    mTextView17Ptr = NULL;
    mTextView16Ptr = NULL;
    mcpbtTextviewPtr = NULL;
    mbluemanageTextViewPtr = NULL;
    mTextView15Ptr = NULL;
    mTextView14Ptr = NULL;
    mTextView13Ptr = NULL;
    maccodeTextViewPtr = NULL;
    maicastButtonPtr = NULL;
    macTipsTextViewPtr = NULL;
    macTipsTitleTextPtr = NULL;
    maicastWindowPtr = NULL;
    mTextView11Ptr = NULL;
    mTextView10Ptr = NULL;
    mTextView2Ptr = NULL;
    mTextView8Ptr = NULL;
    mwifiSetupTitleViewPtr = NULL;
    mmcTipsTitleViewPtr = NULL;
    mapTipsTitleViewPtr = NULL;
    maaTipsTitleViewPtr = NULL;
    mcpTipsTitleViewPtr = NULL;
    mmusicZhenPtr = NULL;
    malbumIconPtr = NULL;
    martistIconPtr = NULL;
    mline4Ptr = NULL;
    mline3Ptr = NULL;
    mline2Ptr = NULL;
    mline1Ptr = NULL;
    mWindow2Ptr = NULL;
    mTextView12Ptr = NULL;
    mWindow1Ptr = NULL;
    mmusicPngPtr = NULL;
    mWinMusicPngPtr = NULL;
    mwifiButtonPtr = NULL;
    mwifiVideoTextViewPtr = NULL;
    mwifiSetupTextViewPtr = NULL;
    mwifiWindowPtr = NULL;
    mHornButtonPtr = NULL;
    mbtButtonPtr = NULL;
    mmcStepTextViewPtr = NULL;
    mapStepTextViewPtr = NULL;
    maaStepTextViewPtr = NULL;
    mcpStepTextViewPtr = NULL;
    mconnectedNameTextViewPtr = NULL;
    mdisConnectButtonPtr = NULL;
    mTextView9Ptr = NULL;
    mdisconCancelButtonPtr = NULL;
    mdisConnectWindowPtr = NULL;
    mnameLongTipsWindowPtr = NULL;
    mautoConnButtonPtr = NULL;
    mdisconnectBtnPtr = NULL;
    mconnDevTextPtr = NULL;
    mTextView7Ptr = NULL;
    menableButtonPtr = NULL;
    mbtNameEditTextPtr = NULL;
    mTextView6Ptr = NULL;
    mTextView5Ptr = NULL;
    mTextView4Ptr = NULL;
    mTextView3Ptr = NULL;
    mTextView1Ptr = NULL;
    mBtWndPtr = NULL;
    mchangecancelBtnPtr = NULL;
    mchangesureBtnPtr = NULL;
    mTipinfoTextPtr = NULL;
    mTipWndPtr = NULL;
    mairplayBtnPtr = NULL;
    mmiracastBtnPtr = NULL;
    mandroidautoBtnPtr = NULL;
    mbluemusicBtnPtr = NULL;
    mCarplayBtnPtr = NULL;
    mmcTipsTextViewPtr = NULL;
    mmiracastWindowPtr = NULL;
    mapTipsTextViewPtr = NULL;
    mapWindowPtr = NULL;
    mcpbtBtnPtr = NULL;
    mcpTipsTextViewPtr = NULL;
    mcpWindowPtr = NULL;
    maaTipsTextViewPtr = NULL;
    maaWindowPtr = NULL;
    mtittleTextViewPtr = NULL;
    martistTextViewPtr = NULL;
    malbumTextViewPtr = NULL;
    mcurposTextViewPtr = NULL;
    mdurationTextViewPtr = NULL;
    mnextButtonPtr = NULL;
    mprevButtonPtr = NULL;
    mplayButtonPtr = NULL;
    mmusicSeekBarPtr = NULL;
    mbluemusicWndPtr = NULL;
}

const char* LinkActivity::getAppName() const{
	return "Link.ftu";
}

//TAG:onCreate
void LinkActivity::onCreate() {
	Activity::onCreate();
    mWindow3Ptr = (ZKWindow*)findControlByID(ID_LINK_Window3);
    mTextView17Ptr = (ZKTextView*)findControlByID(ID_LINK_TextView17);
    mTextView16Ptr = (ZKTextView*)findControlByID(ID_LINK_TextView16);
    mcpbtTextviewPtr = (ZKTextView*)findControlByID(ID_LINK_cpbtTextview);
    mbluemanageTextViewPtr = (ZKTextView*)findControlByID(ID_LINK_bluemanageTextView);
    mTextView15Ptr = (ZKTextView*)findControlByID(ID_LINK_TextView15);
    mTextView14Ptr = (ZKTextView*)findControlByID(ID_LINK_TextView14);
    mTextView13Ptr = (ZKTextView*)findControlByID(ID_LINK_TextView13);
    maccodeTextViewPtr = (ZKTextView*)findControlByID(ID_LINK_accodeTextView);
    maicastButtonPtr = (ZKButton*)findControlByID(ID_LINK_aicastButton);
    macTipsTextViewPtr = (ZKTextView*)findControlByID(ID_LINK_acTipsTextView);
    macTipsTitleTextPtr = (ZKTextView*)findControlByID(ID_LINK_acTipsTitleText);
    maicastWindowPtr = (ZKWindow*)findControlByID(ID_LINK_aicastWindow);
    mTextView11Ptr = (ZKTextView*)findControlByID(ID_LINK_TextView11);
    mTextView10Ptr = (ZKTextView*)findControlByID(ID_LINK_TextView10);
    mTextView2Ptr = (ZKTextView*)findControlByID(ID_LINK_TextView2);
    mTextView8Ptr = (ZKTextView*)findControlByID(ID_LINK_TextView8);
    mwifiSetupTitleViewPtr = (ZKTextView*)findControlByID(ID_LINK_wifiSetupTitleView);
    mmcTipsTitleViewPtr = (ZKTextView*)findControlByID(ID_LINK_mcTipsTitleView);
    mapTipsTitleViewPtr = (ZKTextView*)findControlByID(ID_LINK_apTipsTitleView);
    maaTipsTitleViewPtr = (ZKTextView*)findControlByID(ID_LINK_aaTipsTitleView);
    mcpTipsTitleViewPtr = (ZKTextView*)findControlByID(ID_LINK_cpTipsTitleView);
    mmusicZhenPtr = (ZKButton*)findControlByID(ID_LINK_musicZhen);
    malbumIconPtr = (ZKTextView*)findControlByID(ID_LINK_albumIcon);
    martistIconPtr = (ZKTextView*)findControlByID(ID_LINK_artistIcon);
    mline4Ptr = (ZKTextView*)findControlByID(ID_LINK_line4);
    mline3Ptr = (ZKTextView*)findControlByID(ID_LINK_line3);
    mline2Ptr = (ZKTextView*)findControlByID(ID_LINK_line2);
    mline1Ptr = (ZKTextView*)findControlByID(ID_LINK_line1);
    mWindow2Ptr = (ZKWindow*)findControlByID(ID_LINK_Window2);
    mTextView12Ptr = (ZKTextView*)findControlByID(ID_LINK_TextView12);
    mWindow1Ptr = (ZKWindow*)findControlByID(ID_LINK_Window1);
    mmusicPngPtr = (ZKTextView*)findControlByID(ID_LINK_musicPng);
    mWinMusicPngPtr = (ZKWindow*)findControlByID(ID_LINK_WinMusicPng);
    mwifiButtonPtr = (ZKButton*)findControlByID(ID_LINK_wifiButton);
    mwifiVideoTextViewPtr = (ZKTextView*)findControlByID(ID_LINK_wifiVideoTextView);
    mwifiSetupTextViewPtr = (ZKTextView*)findControlByID(ID_LINK_wifiSetupTextView);
    mwifiWindowPtr = (ZKWindow*)findControlByID(ID_LINK_wifiWindow);
    mHornButtonPtr = (ZKButton*)findControlByID(ID_LINK_HornButton);
    mbtButtonPtr = (ZKButton*)findControlByID(ID_LINK_btButton);
    mmcStepTextViewPtr = (ZKTextView*)findControlByID(ID_LINK_mcStepTextView);
    mapStepTextViewPtr = (ZKTextView*)findControlByID(ID_LINK_apStepTextView);
    maaStepTextViewPtr = (ZKTextView*)findControlByID(ID_LINK_aaStepTextView);
    mcpStepTextViewPtr = (ZKTextView*)findControlByID(ID_LINK_cpStepTextView);
    mconnectedNameTextViewPtr = (ZKTextView*)findControlByID(ID_LINK_connectedNameTextView);
    mdisConnectButtonPtr = (ZKButton*)findControlByID(ID_LINK_disConnectButton);
    mTextView9Ptr = (ZKTextView*)findControlByID(ID_LINK_TextView9);
    mdisconCancelButtonPtr = (ZKButton*)findControlByID(ID_LINK_disconCancelButton);
    mdisConnectWindowPtr = (ZKWindow*)findControlByID(ID_LINK_disConnectWindow);
    mnameLongTipsWindowPtr = (ZKWindow*)findControlByID(ID_LINK_nameLongTipsWindow);
    mautoConnButtonPtr = (ZKButton*)findControlByID(ID_LINK_autoConnButton);
    mdisconnectBtnPtr = (ZKButton*)findControlByID(ID_LINK_disconnectBtn);
    mconnDevTextPtr = (ZKButton*)findControlByID(ID_LINK_connDevText);
    mTextView7Ptr = (ZKTextView*)findControlByID(ID_LINK_TextView7);
    menableButtonPtr = (ZKButton*)findControlByID(ID_LINK_enableButton);
    mbtNameEditTextPtr = (ZKEditText*)findControlByID(ID_LINK_btNameEditText);if(mbtNameEditTextPtr!= NULL){mbtNameEditTextPtr->setTextChangeListener(this);}
    mTextView6Ptr = (ZKTextView*)findControlByID(ID_LINK_TextView6);
    mTextView5Ptr = (ZKTextView*)findControlByID(ID_LINK_TextView5);
    mTextView4Ptr = (ZKTextView*)findControlByID(ID_LINK_TextView4);
    mTextView3Ptr = (ZKTextView*)findControlByID(ID_LINK_TextView3);
    mTextView1Ptr = (ZKTextView*)findControlByID(ID_LINK_TextView1);
    mBtWndPtr = (ZKWindow*)findControlByID(ID_LINK_BtWnd);
    mchangecancelBtnPtr = (ZKButton*)findControlByID(ID_LINK_changecancelBtn);
    mchangesureBtnPtr = (ZKButton*)findControlByID(ID_LINK_changesureBtn);
    mTipinfoTextPtr = (ZKTextView*)findControlByID(ID_LINK_TipinfoText);
    mTipWndPtr = (ZKWindow*)findControlByID(ID_LINK_TipWnd);
    mairplayBtnPtr = (ZKButton*)findControlByID(ID_LINK_airplayBtn);
    mmiracastBtnPtr = (ZKButton*)findControlByID(ID_LINK_miracastBtn);
    mandroidautoBtnPtr = (ZKButton*)findControlByID(ID_LINK_androidautoBtn);
    mbluemusicBtnPtr = (ZKButton*)findControlByID(ID_LINK_bluemusicBtn);
    mCarplayBtnPtr = (ZKButton*)findControlByID(ID_LINK_CarplayBtn);
    mmcTipsTextViewPtr = (ZKTextView*)findControlByID(ID_LINK_mcTipsTextView);
    mmiracastWindowPtr = (ZKWindow*)findControlByID(ID_LINK_miracastWindow);
    mapTipsTextViewPtr = (ZKTextView*)findControlByID(ID_LINK_apTipsTextView);
    mapWindowPtr = (ZKWindow*)findControlByID(ID_LINK_apWindow);
    mcpbtBtnPtr = (ZKButton*)findControlByID(ID_LINK_cpbtBtn);
    mcpTipsTextViewPtr = (ZKTextView*)findControlByID(ID_LINK_cpTipsTextView);
    mcpWindowPtr = (ZKWindow*)findControlByID(ID_LINK_cpWindow);
    maaTipsTextViewPtr = (ZKTextView*)findControlByID(ID_LINK_aaTipsTextView);
    maaWindowPtr = (ZKWindow*)findControlByID(ID_LINK_aaWindow);
    mtittleTextViewPtr = (ZKTextView*)findControlByID(ID_LINK_tittleTextView);
    martistTextViewPtr = (ZKTextView*)findControlByID(ID_LINK_artistTextView);
    malbumTextViewPtr = (ZKTextView*)findControlByID(ID_LINK_albumTextView);
    mcurposTextViewPtr = (ZKTextView*)findControlByID(ID_LINK_curposTextView);
    mdurationTextViewPtr = (ZKTextView*)findControlByID(ID_LINK_durationTextView);
    mnextButtonPtr = (ZKButton*)findControlByID(ID_LINK_nextButton);
    mprevButtonPtr = (ZKButton*)findControlByID(ID_LINK_prevButton);
    mplayButtonPtr = (ZKButton*)findControlByID(ID_LINK_playButton);
    mmusicSeekBarPtr = (ZKSeekBar*)findControlByID(ID_LINK_musicSeekBar);if(mmusicSeekBarPtr!= NULL){mmusicSeekBarPtr->setSeekBarChangeListener(this);}
    mbluemusicWndPtr = (ZKWindow*)findControlByID(ID_LINK_bluemusicWnd);
	mActivityPtr = this;
	onUI_init();
  registerProtocolDataUpdateListener(onProtocolDataUpdate);
  rigesterActivityTimer();
}

void LinkActivity::onClick(ZKBase *pBase) {
	//TODO: add widget onClik code 
    int buttonTablen = sizeof(sButtonCallbackTab) / sizeof(S_ButtonCallback);
    for (int i = 0; i < buttonTablen; ++i) {
        if (sButtonCallbackTab[i].id == pBase->getID()) {
            if (sButtonCallbackTab[i].callback((ZKButton*)pBase)) {
            	return;
            }
            break;
        }
    }


    int len = sizeof(sAppInfoTab) / sizeof(sAppInfoTab[0]);
    for (int i = 0; i < len; ++i) {
        if (sAppInfoTab[i].id == pBase->getID()) {
            EASYUICONTEXT->openActivity(sAppInfoTab[i].pApp);
            return;
        }
    }

	Activity::onClick(pBase);
}

void LinkActivity::onResume() {
	Activity::onResume();
	EASYUICONTEXT->registerGlobalTouchListener(this);
	startVideoLoopPlayback();
	onUI_show();
}

bool LinkActivity::onBack() {
	return onUI_back();
}

void LinkActivity::onPause() {
	Activity::onPause();
	EASYUICONTEXT->unregisterGlobalTouchListener(this);
	stopVideoLoopPlayback();
	onUI_hide();
}

void LinkActivity::onIntent(const Intent *intentPtr) {
	Activity::onIntent(intentPtr);
	onUI_intent(intentPtr);
}

bool LinkActivity::onTimer(int id) {
	return onUI_Timer(id);
}

void LinkActivity::onProgressChanged(ZKSeekBar *pSeekBar, int progress){

    int seekBarTablen = sizeof(SZKSeekBarCallbackTab) / sizeof(S_ZKSeekBarCallback);
    for (int i = 0; i < seekBarTablen; ++i) {
        if (SZKSeekBarCallbackTab[i].id == pSeekBar->getID()) {
            SZKSeekBarCallbackTab[i].callback(pSeekBar, progress);
            break;
        }
    }
}

int LinkActivity::getListItemCount(const ZKListView *pListView) const{
    int tablen = sizeof(SListViewFunctionsCallbackTab) / sizeof(S_ListViewFunctionsCallback);
    for (int i = 0; i < tablen; ++i) {
        if (SListViewFunctionsCallbackTab[i].id == pListView->getID()) {
            return SListViewFunctionsCallbackTab[i].getListItemCountCallback(pListView);
            break;
        }
    }
    return 0;
}

void LinkActivity::obtainListItemData(ZKListView *pListView,ZKListView::ZKListItem *pListItem, int index){
    int tablen = sizeof(SListViewFunctionsCallbackTab) / sizeof(S_ListViewFunctionsCallback);
    for (int i = 0; i < tablen; ++i) {
        if (SListViewFunctionsCallbackTab[i].id == pListView->getID()) {
            SListViewFunctionsCallbackTab[i].obtainListItemDataCallback(pListView, pListItem, index);
            break;
        }
    }
}

void LinkActivity::onItemClick(ZKListView *pListView, int index, int id){
    int tablen = sizeof(SListViewFunctionsCallbackTab) / sizeof(S_ListViewFunctionsCallback);
    for (int i = 0; i < tablen; ++i) {
        if (SListViewFunctionsCallbackTab[i].id == pListView->getID()) {
            SListViewFunctionsCallbackTab[i].onItemClickCallback(pListView, index, id);
            break;
        }
    }
}

void LinkActivity::onSlideItemClick(ZKSlideWindow *pSlideWindow, int index) {
    int tablen = sizeof(SSlideWindowItemClickCallbackTab) / sizeof(S_SlideWindowItemClickCallback);
    for (int i = 0; i < tablen; ++i) {
        if (SSlideWindowItemClickCallbackTab[i].id == pSlideWindow->getID()) {
            SSlideWindowItemClickCallbackTab[i].onSlideItemClickCallback(pSlideWindow, index);
            break;
        }
    }
}

bool LinkActivity::onTouchEvent(const MotionEvent &ev) {
    return onLinkActivityTouchEvent(ev);
}

void LinkActivity::onTextChanged(ZKTextView *pTextView, const std::string &text) {
    int tablen = sizeof(SEditTextInputCallbackTab) / sizeof(S_EditTextInputCallback);
    for (int i = 0; i < tablen; ++i) {
        if (SEditTextInputCallbackTab[i].id == pTextView->getID()) {
            SEditTextInputCallbackTab[i].onEditTextChangedCallback(text);
            break;
        }
    }
}

void LinkActivity::rigesterActivityTimer() {
    int tablen = sizeof(REGISTER_ACTIVITY_TIMER_TAB) / sizeof(S_ACTIVITY_TIMEER);
    for (int i = 0; i < tablen; ++i) {
        S_ACTIVITY_TIMEER temp = REGISTER_ACTIVITY_TIMER_TAB[i];
        registerTimer(temp.id, temp.time);
    }
}


void LinkActivity::onVideoPlayerMessage(ZKVideoView *pVideoView, int msg) {
    int tablen = sizeof(SVideoViewCallbackTab) / sizeof(S_VideoViewCallback);
    for (int i = 0; i < tablen; ++i) {
        if (SVideoViewCallbackTab[i].id == pVideoView->getID()) {
        	if (SVideoViewCallbackTab[i].loop) {
                //循环播放
        		videoLoopPlayback(pVideoView, msg, i);
        	} else if (SVideoViewCallbackTab[i].onVideoViewCallback != NULL){
        	    SVideoViewCallbackTab[i].onVideoViewCallback(pVideoView, msg);
        	}
            break;
        }
    }
}

void LinkActivity::videoLoopPlayback(ZKVideoView *pVideoView, int msg, size_t callbackTabIndex) {

	switch (msg) {
	case ZKVideoView::E_MSGTYPE_VIDEO_PLAY_STARTED:
		LOGD("ZKVideoView::E_MSGTYPE_VIDEO_PLAY_STARTED\n");
    if (callbackTabIndex >= (sizeof(SVideoViewCallbackTab)/sizeof(S_VideoViewCallback))) {
      break;
    }
		pVideoView->setVolume(SVideoViewCallbackTab[callbackTabIndex].defaultvolume / 10.0);
		mVideoLoopErrorCount = 0;
		break;
	case ZKVideoView::E_MSGTYPE_VIDEO_PLAY_ERROR:
		/**错误处理 */
		++mVideoLoopErrorCount;
		if (mVideoLoopErrorCount > 100) {
			LOGD("video loop error counts > 100, quit loop playback !");
            break;
		} //不用break, 继续尝试播放下一个
	case ZKVideoView::E_MSGTYPE_VIDEO_PLAY_COMPLETED:
		LOGD("ZKVideoView::E_MSGTYPE_VIDEO_PLAY_COMPLETED\n");
        std::vector<std::string> videolist;
        std::string fileName(getAppName());
        if (fileName.size() < 4) {
             LOGD("getAppName size < 4, ignore!");
             break;
        }
        fileName = fileName.substr(0, fileName.length() - 4) + "_video_list.txt";
        fileName = "/mnt/extsd/" + fileName;
        if (!parseVideoFileList(fileName.c_str(), videolist)) {
            LOGD("parseVideoFileList failed !");
		    break;
        }
		if (pVideoView && !videolist.empty()) {
			mVideoLoopIndex = (mVideoLoopIndex + 1) % videolist.size();
			pVideoView->play(videolist[mVideoLoopIndex].c_str());
		}
		break;
	}
}

void LinkActivity::startVideoLoopPlayback() {
    int tablen = sizeof(SVideoViewCallbackTab) / sizeof(S_VideoViewCallback);
    for (int i = 0; i < tablen; ++i) {
    	if (SVideoViewCallbackTab[i].loop) {
    		ZKVideoView* videoView = (ZKVideoView*)findControlByID(SVideoViewCallbackTab[i].id);
    		if (!videoView) {
    			return;
    		}
    		//循环播放
    		videoLoopPlayback(videoView, ZKVideoView::E_MSGTYPE_VIDEO_PLAY_COMPLETED, i);
    		return;
    	}
    }
}

void LinkActivity::stopVideoLoopPlayback() {
    int tablen = sizeof(SVideoViewCallbackTab) / sizeof(S_VideoViewCallback);
    for (int i = 0; i < tablen; ++i) {
    	if (SVideoViewCallbackTab[i].loop) {
    		ZKVideoView* videoView = (ZKVideoView*)findControlByID(SVideoViewCallbackTab[i].id);
    		if (!videoView) {
    			return;
    		}
    		if (videoView->isPlaying()) {
    		    videoView->stop();
    		}
    		return;
    	}
    }
}

bool LinkActivity::parseVideoFileList(const char *pFileListPath, std::vector<string>& mediaFileList) {
	mediaFileList.clear();
	if (NULL == pFileListPath || 0 == strlen(pFileListPath)) {
        LOGD("video file list is null!");
		return false;
	}

	ifstream is(pFileListPath, ios_base::in);
	if (!is.is_open()) {
		LOGD("cann't open file %s \n", pFileListPath);
		return false;
	}
	char tmp[1024] = {0};
	while (is.getline(tmp, sizeof(tmp))) {
		string str = tmp;
		removeCharFromString(str, '\"');
		removeCharFromString(str, '\r');
		removeCharFromString(str, '\n');
		if (str.size() > 1) {
     		mediaFileList.push_back(str.c_str());
		}
	}
  LOGD("(f:%s, l:%d) parse fileList[%s], get [%d]files", __FUNCTION__,
      __LINE__, pFileListPath, int(mediaFileList.size()));
  for (std::vector<string>::size_type i = 0; i < mediaFileList.size(); i++) {
    LOGD("file[%u]:[%s]", ::size_t(i), mediaFileList[i].c_str());
  }
	is.close();

	return true;
}

int LinkActivity::removeCharFromString(string& nString, char c) {
    string::size_type   pos;
    while(1) {
        pos = nString.find(c);
        if(pos != string::npos) {
            nString.erase(pos, 1);
        } else {
            break;
        }
    }
    return (int)nString.size();
}

void LinkActivity::registerUserTimer(int id, int time) {
	registerTimer(id, time);
}

void LinkActivity::unregisterUserTimer(int id) {
	unregisterTimer(id);
}

void LinkActivity::resetUserTimer(int id, int time) {
	resetTimer(id, time);
}
