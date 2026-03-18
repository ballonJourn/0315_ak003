/***********************************************
/gen auto by zuitools
***********************************************/
#include "AudioOutputActivity.h"

/*TAG:GlobalVariable全局变量*/
static ZKTextView* mfmSeekTextViewPtr;
static ZKTextView* mThemeBackPtr;
static ZKWindow* mWindow1Ptr;
static ZKWindow* mWindow2Ptr;
static ZKTextView* mTextView2Ptr;
static ZKTextView* mFMTextViewPtr;
static ZKButton* mtipCancelPtr;
static ZKButton* mtipSurePtr;
static ZKTextView* mLinkSwitchTipTextViewPtr;
static ZKTextView* mTextView7Ptr;
static ZKWindow* mLinkTipWindowPtr;
static ZKTextView* mfmTipsTextViewPtr;
static ZKButton* mAuxBtnPtr;
static ZKButton* mFMBtnPtr;
static ZKButton* mlinkvoiceBtnPtr;
static ZKButton* mhornBtnPtr;
static ZKTextView* mTextView11Ptr;
static ZKTextView* mTextView9Ptr;
static ZKWindow* mAUXWndPtr;
static ZKTextView* mTextView10Ptr;
static ZKTextView* mTextView6Ptr;
static ZKWindow* mhornWndPtr;
static ZKTextView* mTextView12Ptr;
static ZKTextView* mTextView5Ptr;
static ZKWindow* mlinkvoiceWndPtr;
static ZKSeekBar* mfmSeekBarPtr;
static ZKButton* maddButtonPtr;
static ZKButton* mdecButtonPtr;
static ZKEditText* mEditTextFmFreqPtr;
static ZKTextView* mTextView3Ptr;
static ZKWindow* mFmTransmitWndPtr;
static AudioOutputActivity* mActivityPtr;

/*register activity*/
REGISTER_ACTIVITY(AudioOutputActivity);

typedef struct {
	int id; // 定时器ID ， 不能重复
	int time; // 定时器  时间间隔  单位 毫秒
}S_ACTIVITY_TIMEER;

#include "logic/AudioOutputLogic.cc"

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
    ID_AUDIOOUTPUT_tipCancel, onButtonClick_tipCancel,
    ID_AUDIOOUTPUT_tipSure, onButtonClick_tipSure,
    ID_AUDIOOUTPUT_AuxBtn, onButtonClick_AuxBtn,
    ID_AUDIOOUTPUT_FMBtn, onButtonClick_FMBtn,
    ID_AUDIOOUTPUT_linkvoiceBtn, onButtonClick_linkvoiceBtn,
    ID_AUDIOOUTPUT_hornBtn, onButtonClick_hornBtn,
    ID_AUDIOOUTPUT_addButton, onButtonClick_addButton,
    ID_AUDIOOUTPUT_decButton, onButtonClick_decButton,
};
/***************/


typedef void (*SeekBarCallback)(ZKSeekBar *pSeekBar, int progress);
typedef struct {
    int id;
    SeekBarCallback callback;
}S_ZKSeekBarCallback;
/*TAG:SeekBarCallbackTab*/
static S_ZKSeekBarCallback SZKSeekBarCallbackTab[] = {
    ID_AUDIOOUTPUT_fmSeekBar, onProgressChanged_fmSeekBar,
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
    ID_AUDIOOUTPUT_EditTextFmFreq, onEditTextChanged_EditTextFmFreq,
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


AudioOutputActivity::AudioOutputActivity() {
	//todo add init code here
	mVideoLoopIndex = -1;
	mVideoLoopErrorCount = 0;
}

AudioOutputActivity::~AudioOutputActivity() {
  //todo add init file here
  // 退出应用时需要反注册
    EASYUICONTEXT->unregisterGlobalTouchListener(this);
    unregisterProtocolDataUpdateListener(onProtocolDataUpdate);
    onUI_quit();
    mActivityPtr = NULL;
    mfmSeekTextViewPtr = NULL;
    mThemeBackPtr = NULL;
    mWindow1Ptr = NULL;
    mWindow2Ptr = NULL;
    mTextView2Ptr = NULL;
    mFMTextViewPtr = NULL;
    mtipCancelPtr = NULL;
    mtipSurePtr = NULL;
    mLinkSwitchTipTextViewPtr = NULL;
    mTextView7Ptr = NULL;
    mLinkTipWindowPtr = NULL;
    mfmTipsTextViewPtr = NULL;
    mAuxBtnPtr = NULL;
    mFMBtnPtr = NULL;
    mlinkvoiceBtnPtr = NULL;
    mhornBtnPtr = NULL;
    mTextView11Ptr = NULL;
    mTextView9Ptr = NULL;
    mAUXWndPtr = NULL;
    mTextView10Ptr = NULL;
    mTextView6Ptr = NULL;
    mhornWndPtr = NULL;
    mTextView12Ptr = NULL;
    mTextView5Ptr = NULL;
    mlinkvoiceWndPtr = NULL;
    mfmSeekBarPtr = NULL;
    maddButtonPtr = NULL;
    mdecButtonPtr = NULL;
    mEditTextFmFreqPtr = NULL;
    mTextView3Ptr = NULL;
    mFmTransmitWndPtr = NULL;
}

const char* AudioOutputActivity::getAppName() const{
	return "AudioOutput.ftu";
}

//TAG:onCreate
void AudioOutputActivity::onCreate() {
	Activity::onCreate();
    mfmSeekTextViewPtr = (ZKTextView*)findControlByID(ID_AUDIOOUTPUT_fmSeekTextView);
    mThemeBackPtr = (ZKTextView*)findControlByID(ID_AUDIOOUTPUT_ThemeBack);
    mWindow1Ptr = (ZKWindow*)findControlByID(ID_AUDIOOUTPUT_Window1);
    mWindow2Ptr = (ZKWindow*)findControlByID(ID_AUDIOOUTPUT_Window2);
    mTextView2Ptr = (ZKTextView*)findControlByID(ID_AUDIOOUTPUT_TextView2);
    mFMTextViewPtr = (ZKTextView*)findControlByID(ID_AUDIOOUTPUT_FMTextView);
    mtipCancelPtr = (ZKButton*)findControlByID(ID_AUDIOOUTPUT_tipCancel);
    mtipSurePtr = (ZKButton*)findControlByID(ID_AUDIOOUTPUT_tipSure);
    mLinkSwitchTipTextViewPtr = (ZKTextView*)findControlByID(ID_AUDIOOUTPUT_LinkSwitchTipTextView);
    mTextView7Ptr = (ZKTextView*)findControlByID(ID_AUDIOOUTPUT_TextView7);
    mLinkTipWindowPtr = (ZKWindow*)findControlByID(ID_AUDIOOUTPUT_LinkTipWindow);
    mfmTipsTextViewPtr = (ZKTextView*)findControlByID(ID_AUDIOOUTPUT_fmTipsTextView);
    mAuxBtnPtr = (ZKButton*)findControlByID(ID_AUDIOOUTPUT_AuxBtn);
    mFMBtnPtr = (ZKButton*)findControlByID(ID_AUDIOOUTPUT_FMBtn);
    mlinkvoiceBtnPtr = (ZKButton*)findControlByID(ID_AUDIOOUTPUT_linkvoiceBtn);
    mhornBtnPtr = (ZKButton*)findControlByID(ID_AUDIOOUTPUT_hornBtn);
    mTextView11Ptr = (ZKTextView*)findControlByID(ID_AUDIOOUTPUT_TextView11);
    mTextView9Ptr = (ZKTextView*)findControlByID(ID_AUDIOOUTPUT_TextView9);
    mAUXWndPtr = (ZKWindow*)findControlByID(ID_AUDIOOUTPUT_AUXWnd);
    mTextView10Ptr = (ZKTextView*)findControlByID(ID_AUDIOOUTPUT_TextView10);
    mTextView6Ptr = (ZKTextView*)findControlByID(ID_AUDIOOUTPUT_TextView6);
    mhornWndPtr = (ZKWindow*)findControlByID(ID_AUDIOOUTPUT_hornWnd);
    mTextView12Ptr = (ZKTextView*)findControlByID(ID_AUDIOOUTPUT_TextView12);
    mTextView5Ptr = (ZKTextView*)findControlByID(ID_AUDIOOUTPUT_TextView5);
    mlinkvoiceWndPtr = (ZKWindow*)findControlByID(ID_AUDIOOUTPUT_linkvoiceWnd);
    mfmSeekBarPtr = (ZKSeekBar*)findControlByID(ID_AUDIOOUTPUT_fmSeekBar);if(mfmSeekBarPtr!= NULL){mfmSeekBarPtr->setSeekBarChangeListener(this);}
    maddButtonPtr = (ZKButton*)findControlByID(ID_AUDIOOUTPUT_addButton);
    mdecButtonPtr = (ZKButton*)findControlByID(ID_AUDIOOUTPUT_decButton);
    mEditTextFmFreqPtr = (ZKEditText*)findControlByID(ID_AUDIOOUTPUT_EditTextFmFreq);if(mEditTextFmFreqPtr!= NULL){mEditTextFmFreqPtr->setTextChangeListener(this);}
    mTextView3Ptr = (ZKTextView*)findControlByID(ID_AUDIOOUTPUT_TextView3);
    mFmTransmitWndPtr = (ZKWindow*)findControlByID(ID_AUDIOOUTPUT_FmTransmitWnd);
	mActivityPtr = this;
	onUI_init();
  registerProtocolDataUpdateListener(onProtocolDataUpdate);
  rigesterActivityTimer();
}

void AudioOutputActivity::onClick(ZKBase *pBase) {
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

void AudioOutputActivity::onResume() {
	Activity::onResume();
	EASYUICONTEXT->registerGlobalTouchListener(this);
	startVideoLoopPlayback();
	onUI_show();
}

void AudioOutputActivity::onPause() {
	Activity::onPause();
	EASYUICONTEXT->unregisterGlobalTouchListener(this);
	stopVideoLoopPlayback();
	onUI_hide();
}

void AudioOutputActivity::onIntent(const Intent *intentPtr) {
	Activity::onIntent(intentPtr);
	onUI_intent(intentPtr);
}

bool AudioOutputActivity::onTimer(int id) {
	return onUI_Timer(id);
}

void AudioOutputActivity::onProgressChanged(ZKSeekBar *pSeekBar, int progress){

    int seekBarTablen = sizeof(SZKSeekBarCallbackTab) / sizeof(S_ZKSeekBarCallback);
    for (int i = 0; i < seekBarTablen; ++i) {
        if (SZKSeekBarCallbackTab[i].id == pSeekBar->getID()) {
            SZKSeekBarCallbackTab[i].callback(pSeekBar, progress);
            break;
        }
    }
}

int AudioOutputActivity::getListItemCount(const ZKListView *pListView) const{
    int tablen = sizeof(SListViewFunctionsCallbackTab) / sizeof(S_ListViewFunctionsCallback);
    for (int i = 0; i < tablen; ++i) {
        if (SListViewFunctionsCallbackTab[i].id == pListView->getID()) {
            return SListViewFunctionsCallbackTab[i].getListItemCountCallback(pListView);
            break;
        }
    }
    return 0;
}

void AudioOutputActivity::obtainListItemData(ZKListView *pListView,ZKListView::ZKListItem *pListItem, int index){
    int tablen = sizeof(SListViewFunctionsCallbackTab) / sizeof(S_ListViewFunctionsCallback);
    for (int i = 0; i < tablen; ++i) {
        if (SListViewFunctionsCallbackTab[i].id == pListView->getID()) {
            SListViewFunctionsCallbackTab[i].obtainListItemDataCallback(pListView, pListItem, index);
            break;
        }
    }
}

void AudioOutputActivity::onItemClick(ZKListView *pListView, int index, int id){
    int tablen = sizeof(SListViewFunctionsCallbackTab) / sizeof(S_ListViewFunctionsCallback);
    for (int i = 0; i < tablen; ++i) {
        if (SListViewFunctionsCallbackTab[i].id == pListView->getID()) {
            SListViewFunctionsCallbackTab[i].onItemClickCallback(pListView, index, id);
            break;
        }
    }
}

void AudioOutputActivity::onSlideItemClick(ZKSlideWindow *pSlideWindow, int index) {
    int tablen = sizeof(SSlideWindowItemClickCallbackTab) / sizeof(S_SlideWindowItemClickCallback);
    for (int i = 0; i < tablen; ++i) {
        if (SSlideWindowItemClickCallbackTab[i].id == pSlideWindow->getID()) {
            SSlideWindowItemClickCallbackTab[i].onSlideItemClickCallback(pSlideWindow, index);
            break;
        }
    }
}

bool AudioOutputActivity::onTouchEvent(const MotionEvent &ev) {
    return onAudioOutputActivityTouchEvent(ev);
}

void AudioOutputActivity::onTextChanged(ZKTextView *pTextView, const std::string &text) {
    int tablen = sizeof(SEditTextInputCallbackTab) / sizeof(S_EditTextInputCallback);
    for (int i = 0; i < tablen; ++i) {
        if (SEditTextInputCallbackTab[i].id == pTextView->getID()) {
            SEditTextInputCallbackTab[i].onEditTextChangedCallback(text);
            break;
        }
    }
}

void AudioOutputActivity::rigesterActivityTimer() {
    int tablen = sizeof(REGISTER_ACTIVITY_TIMER_TAB) / sizeof(S_ACTIVITY_TIMEER);
    for (int i = 0; i < tablen; ++i) {
        S_ACTIVITY_TIMEER temp = REGISTER_ACTIVITY_TIMER_TAB[i]; // @suppress("无效参数")
        registerTimer(temp.id, temp.time);
    }
}


void AudioOutputActivity::onVideoPlayerMessage(ZKVideoView *pVideoView, int msg) {
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

void AudioOutputActivity::videoLoopPlayback(ZKVideoView *pVideoView, int msg, size_t callbackTabIndex) {

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

void AudioOutputActivity::startVideoLoopPlayback() {
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

void AudioOutputActivity::stopVideoLoopPlayback() {
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

bool AudioOutputActivity::parseVideoFileList(const char *pFileListPath, std::vector<string>& mediaFileList) {
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

int AudioOutputActivity::removeCharFromString(string& nString, char c) {
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

void AudioOutputActivity::registerUserTimer(int id, int time) {
	registerTimer(id, time);
}

void AudioOutputActivity::unregisterUserTimer(int id) {
	unregisterTimer(id);
}

void AudioOutputActivity::resetUserTimer(int id, int time) {
	resetTimer(id, time);
}