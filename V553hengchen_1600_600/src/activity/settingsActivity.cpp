/***********************************************
/gen auto by zuitools
***********************************************/
#include "settingsActivity.h"

/*TAG:GlobalVariable全局变量*/
static ZKTextView* mTextView14Ptr;
static ZKButton* mdebugButtonPtr;
static ZKSeekBar* mmcuUpdSeekBarPtr;
static ZKWindow* mprogressWndPtr;
static ZKButton* mmcuCancelPtr;
static ZKButton* mmcuSurePtr;
static ZKTextView* mTextView13Ptr;
static ZKTextView* mTextView8Ptr;
static ZKWindow* mUpgradeWindowPtr;
static ZKButton* mHornButtonPtr;
static ZKTextView* mThemeBackPtr;
static ZKButton* mPowerButtonPtr;
static ZKButton* mSettingButtonPtr;
static ZKButton* mLinkButtonPtr;
static ZKButton* mLightButtonPtr;
static ZKWindow* mbottomWindowPtr;
static ZKTextView* mTextView11Ptr;
static ZKWindow* mOtpWindowPtr;
static ZKButton* mOtpButtonPtr;
static ZKButton* mendSureButtonPtr;
static ZKButton* mendCancelButtonPtr;
static ZKTextView* mendTimeSplitTextPtr;
static ZKListView* mendMinListViewPtr;
static ZKListView* mendHourListViewPtr;
static ZKTextView* mendMinTextViewPtr;
static ZKTextView* mendHourTextViewPtr;
static ZKTextView* mendTimeTitleTextPtr;
static ZKWindow* mendTimeWindowPtr;
static ZKButton* mstartSureButtonPtr;
static ZKButton* mstartCancelButtonPtr;
static ZKTextView* mstartTimeSplitTextPtr;
static ZKListView* mstartMinListViewPtr;
static ZKListView* mstartHourListViewPtr;
static ZKTextView* mstartMinTextViewPtr;
static ZKTextView* mstartHourTextViewPtr;
static ZKTextView* mstartTimeTitleTextPtr;
static ZKWindow* mstartTimeWindowPtr;
static ZKTextView* mbrigSplitTextPtr;
static ZKButton* mbrigSureButtonPtr;
static ZKButton* mbrigCancelButtonPtr;
static ZKSeekBar* mSeekBar1Ptr;
static ZKTextView* mbrigpercentTextViewPtr;
static ZKTextView* mbrigTitleTextViewPtr;
static ZKWindow* mbrightnessWindowPtr;
static ZKButton* mButton1Ptr;
static ZKButton* mendTimeButtonPtr;
static ZKButton* mstartTimeButtonPtr;
static ZKButton* mbrightnessButtonPtr;
static ZKTextView* meyeTextView4Ptr;
static ZKTextView* meyeTextView3Ptr;
static ZKTextView* meyeTextView2Ptr;
static ZKTextView* meyeTextView1Ptr;
static ZKTextView* meyeTitleTextViewPtr;
static ZKWindow* meyeProtectionWindowPtr;
static ZKTextView* mTextView12Ptr;
static ZKWindow* mWaitMomentWindowPtr;
static ZKTextView* mUnderMaskTextViewPtr;
static ZKTextView* mMinTextViewPtr;
static ZKTextView* mHourTextViewPtr;
static ZKTextView* mDayTextViewPtr;
static ZKTextView* mMonthTextViewPtr;
static ZKTextView* mYearTextViewPtr;
static ZKListView* mformatListViewPtr;
static ZKButton* mclearCancelPtr;
static ZKButton* mclearSurePtr;
static ZKTextView* mTextView10Ptr;
static ZKTextView* mTextView9Ptr;
static ZKWindow* mClearTipWindowPtr;
static ZKButton* mButtonNetPtr;
static ZKButton* mLeftArrowButtonPtr;
static ZKButton* mRightArrowButtonPtr;
static ZKButton* mBottomLeftArrowButtonPtr;
static ZKButton* mBottomRightArrowButtonPtr;
static ZKPainter* mLinePainterPtr;
static ZKTextView* mcalib_tips3TextViewPtr;
static ZKTextView* mcalib_tips2TextViewPtr;
static ZKTextView* mcalib_tips1TextViewPtr;
static ZKTextView* mtipsTextViewPtr;
static ZKWindow* mWindow1Ptr;
static ZKButton* msaveCalButtonPtr;
static ZKButton* munCalButtonPtr;
static ZKButton* mAdbButtonPtr;
static ZKWindow* mMoreWindowPtr;
static ZKButton* mMoreButtonPtr;
static ZKButton* mtipCancelPtr;
static ZKButton* mtipSurePtr;
static ZKTextView* mLinkSwitchTipTextViewPtr;
static ZKTextView* mTextView7Ptr;
static ZKWindow* mLinkTipWindowPtr;
static ZKButton* mFormatCancelBtnPtr;
static ZKButton* mFormatSureBtnPtr;
static ZKTextView* mTextView6Ptr;
static ZKTextView* mTextView5Ptr;
static ZKWindow* mFormatWndPtr;
static ZKButton* msettimecancelPtr;
static ZKButton* msettimesurePtr;
static ZKListView* myearListViewPtr;
static ZKListView* mhourListViewPtr;
static ZKListView* mdayListViewPtr;
static ZKListView* mmonthListViewPtr;
static ZKListView* mminListViewPtr;
static ZKTextView* mTextView4Ptr;
static ZKWindow* msettimeWindowPtr;
static ZKCameraView* mCameraViewReversePtr;
static ZKWindow* mrevLineWindowPtr;
static ZKWindow* mversionWndPtr;
static ZKTextView* mversionTextPtr;
static ZKTextView* mTextView3Ptr;
static ZKButton* mresetCancelPtr;
static ZKButton* mresetSurePtr;
static ZKTextView* mTextView2Ptr;
static ZKTextView* mTextView1Ptr;
static ZKWindow* mResetWindowPtr;
static ZKTextView* mmodeTextViewPtr;
static ZKListView* moptionListViewPtr;
static ZKWindow* mmodeWindowPtr;
static ZKListView* mmodeListViewPtr;
static settingsActivity* mActivityPtr;

/*register activity*/
REGISTER_ACTIVITY(settingsActivity);

typedef struct {
	int id; // 定时器ID ， 不能重复
	int time; // 定时器  时间间隔  单位 毫秒
}S_ACTIVITY_TIMEER;

#include "logic/settingsLogic.cc"

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
    ID_SETTINGS_debugButton, onButtonClick_debugButton,
    ID_SETTINGS_mcuCancel, onButtonClick_mcuCancel,
    ID_SETTINGS_mcuSure, onButtonClick_mcuSure,
    ID_SETTINGS_HornButton, onButtonClick_HornButton,
    ID_SETTINGS_PowerButton, onButtonClick_PowerButton,
    ID_SETTINGS_SettingButton, onButtonClick_SettingButton,
    ID_SETTINGS_LinkButton, onButtonClick_LinkButton,
    ID_SETTINGS_LightButton, onButtonClick_LightButton,
    ID_SETTINGS_OtpButton, onButtonClick_OtpButton,
    ID_SETTINGS_endSureButton, onButtonClick_endSureButton,
    ID_SETTINGS_endCancelButton, onButtonClick_endCancelButton,
    ID_SETTINGS_startSureButton, onButtonClick_startSureButton,
    ID_SETTINGS_startCancelButton, onButtonClick_startCancelButton,
    ID_SETTINGS_brigSureButton, onButtonClick_brigSureButton,
    ID_SETTINGS_brigCancelButton, onButtonClick_brigCancelButton,
    ID_SETTINGS_Button1, onButtonClick_Button1,
    ID_SETTINGS_endTimeButton, onButtonClick_endTimeButton,
    ID_SETTINGS_startTimeButton, onButtonClick_startTimeButton,
    ID_SETTINGS_brightnessButton, onButtonClick_brightnessButton,
    ID_SETTINGS_clearCancel, onButtonClick_clearCancel,
    ID_SETTINGS_clearSure, onButtonClick_clearSure,
    ID_SETTINGS_ButtonNet, onButtonClick_ButtonNet,
    ID_SETTINGS_LeftArrowButton, onButtonClick_LeftArrowButton,
    ID_SETTINGS_RightArrowButton, onButtonClick_RightArrowButton,
    ID_SETTINGS_BottomLeftArrowButton, onButtonClick_BottomLeftArrowButton,
    ID_SETTINGS_BottomRightArrowButton, onButtonClick_BottomRightArrowButton,
    ID_SETTINGS_saveCalButton, onButtonClick_saveCalButton,
    ID_SETTINGS_unCalButton, onButtonClick_unCalButton,
    ID_SETTINGS_AdbButton, onButtonClick_AdbButton,
    ID_SETTINGS_MoreButton, onButtonClick_MoreButton,
    ID_SETTINGS_tipCancel, onButtonClick_tipCancel,
    ID_SETTINGS_tipSure, onButtonClick_tipSure,
    ID_SETTINGS_FormatCancelBtn, onButtonClick_FormatCancelBtn,
    ID_SETTINGS_FormatSureBtn, onButtonClick_FormatSureBtn,
    ID_SETTINGS_settimecancel, onButtonClick_settimecancel,
    ID_SETTINGS_settimesure, onButtonClick_settimesure,
    ID_SETTINGS_resetCancel, onButtonClick_resetCancel,
    ID_SETTINGS_resetSure, onButtonClick_resetSure,
};
/***************/


typedef void (*SeekBarCallback)(ZKSeekBar *pSeekBar, int progress);
typedef struct {
    int id;
    SeekBarCallback callback;
}S_ZKSeekBarCallback;
/*TAG:SeekBarCallbackTab*/
static S_ZKSeekBarCallback SZKSeekBarCallbackTab[] = {
    ID_SETTINGS_mcuUpdSeekBar, onProgressChanged_mcuUpdSeekBar,
    ID_SETTINGS_SeekBar1, onProgressChanged_SeekBar1,
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
    ID_SETTINGS_endMinListView, getListItemCount_endMinListView, obtainListItemData_endMinListView, onListItemClick_endMinListView,
    ID_SETTINGS_endHourListView, getListItemCount_endHourListView, obtainListItemData_endHourListView, onListItemClick_endHourListView,
    ID_SETTINGS_startMinListView, getListItemCount_startMinListView, obtainListItemData_startMinListView, onListItemClick_startMinListView,
    ID_SETTINGS_startHourListView, getListItemCount_startHourListView, obtainListItemData_startHourListView, onListItemClick_startHourListView,
    ID_SETTINGS_formatListView, getListItemCount_formatListView, obtainListItemData_formatListView, onListItemClick_formatListView,
    ID_SETTINGS_yearListView, getListItemCount_yearListView, obtainListItemData_yearListView, onListItemClick_yearListView,
    ID_SETTINGS_hourListView, getListItemCount_hourListView, obtainListItemData_hourListView, onListItemClick_hourListView,
    ID_SETTINGS_dayListView, getListItemCount_dayListView, obtainListItemData_dayListView, onListItemClick_dayListView,
    ID_SETTINGS_monthListView, getListItemCount_monthListView, obtainListItemData_monthListView, onListItemClick_monthListView,
    ID_SETTINGS_minListView, getListItemCount_minListView, obtainListItemData_minListView, onListItemClick_minListView,
    ID_SETTINGS_optionListView, getListItemCount_optionListView, obtainListItemData_optionListView, onListItemClick_optionListView,
    ID_SETTINGS_modeListView, getListItemCount_modeListView, obtainListItemData_modeListView, onListItemClick_modeListView,
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


settingsActivity::settingsActivity() {
	//todo add init code here
	mVideoLoopIndex = -1;
	mVideoLoopErrorCount = 0;
}

settingsActivity::~settingsActivity() {
  //todo add init file here
  // 退出应用时需要反注册
    EASYUICONTEXT->unregisterGlobalTouchListener(this);
    unregisterProtocolDataUpdateListener(onProtocolDataUpdate);
    onUI_quit();
    mActivityPtr = NULL;
    mTextView14Ptr = NULL;
    mdebugButtonPtr = NULL;
    mmcuUpdSeekBarPtr = NULL;
    mprogressWndPtr = NULL;
    mmcuCancelPtr = NULL;
    mmcuSurePtr = NULL;
    mTextView13Ptr = NULL;
    mTextView8Ptr = NULL;
    mUpgradeWindowPtr = NULL;
    mHornButtonPtr = NULL;
    mThemeBackPtr = NULL;
    mPowerButtonPtr = NULL;
    mSettingButtonPtr = NULL;
    mLinkButtonPtr = NULL;
    mLightButtonPtr = NULL;
    mbottomWindowPtr = NULL;
    mTextView11Ptr = NULL;
    mOtpWindowPtr = NULL;
    mOtpButtonPtr = NULL;
    mendSureButtonPtr = NULL;
    mendCancelButtonPtr = NULL;
    mendTimeSplitTextPtr = NULL;
    mendMinListViewPtr = NULL;
    mendHourListViewPtr = NULL;
    mendMinTextViewPtr = NULL;
    mendHourTextViewPtr = NULL;
    mendTimeTitleTextPtr = NULL;
    mendTimeWindowPtr = NULL;
    mstartSureButtonPtr = NULL;
    mstartCancelButtonPtr = NULL;
    mstartTimeSplitTextPtr = NULL;
    mstartMinListViewPtr = NULL;
    mstartHourListViewPtr = NULL;
    mstartMinTextViewPtr = NULL;
    mstartHourTextViewPtr = NULL;
    mstartTimeTitleTextPtr = NULL;
    mstartTimeWindowPtr = NULL;
    mbrigSplitTextPtr = NULL;
    mbrigSureButtonPtr = NULL;
    mbrigCancelButtonPtr = NULL;
    mSeekBar1Ptr = NULL;
    mbrigpercentTextViewPtr = NULL;
    mbrigTitleTextViewPtr = NULL;
    mbrightnessWindowPtr = NULL;
    mButton1Ptr = NULL;
    mendTimeButtonPtr = NULL;
    mstartTimeButtonPtr = NULL;
    mbrightnessButtonPtr = NULL;
    meyeTextView4Ptr = NULL;
    meyeTextView3Ptr = NULL;
    meyeTextView2Ptr = NULL;
    meyeTextView1Ptr = NULL;
    meyeTitleTextViewPtr = NULL;
    meyeProtectionWindowPtr = NULL;
    mTextView12Ptr = NULL;
    mWaitMomentWindowPtr = NULL;
    mUnderMaskTextViewPtr = NULL;
    mMinTextViewPtr = NULL;
    mHourTextViewPtr = NULL;
    mDayTextViewPtr = NULL;
    mMonthTextViewPtr = NULL;
    mYearTextViewPtr = NULL;
    mformatListViewPtr = NULL;
    mclearCancelPtr = NULL;
    mclearSurePtr = NULL;
    mTextView10Ptr = NULL;
    mTextView9Ptr = NULL;
    mClearTipWindowPtr = NULL;
    mButtonNetPtr = NULL;
    mBottomLeftArrowButtonPtr = NULL;
    mBottomRightArrowButtonPtr = NULL;
    mLinePainterPtr = NULL;
    mcalib_tips3TextViewPtr = NULL;
    mcalib_tips2TextViewPtr = NULL;
    mcalib_tips1TextViewPtr = NULL;
    mtipsTextViewPtr = NULL;
    mWindow1Ptr = NULL;
    msaveCalButtonPtr = NULL;
    munCalButtonPtr = NULL;
    mAdbButtonPtr = NULL;
    mMoreWindowPtr = NULL;
    mMoreButtonPtr = NULL;
    mtipCancelPtr = NULL;
    mtipSurePtr = NULL;
    mLinkSwitchTipTextViewPtr = NULL;
    mTextView7Ptr = NULL;
    mLinkTipWindowPtr = NULL;
    mFormatCancelBtnPtr = NULL;
    mFormatSureBtnPtr = NULL;
    mTextView6Ptr = NULL;
    mTextView5Ptr = NULL;
    mFormatWndPtr = NULL;
    msettimecancelPtr = NULL;
    msettimesurePtr = NULL;
    myearListViewPtr = NULL;
    mhourListViewPtr = NULL;
    mdayListViewPtr = NULL;
    mmonthListViewPtr = NULL;
    mminListViewPtr = NULL;
    mTextView4Ptr = NULL;
    msettimeWindowPtr = NULL;
    mCameraViewReversePtr = NULL;
    mrevLineWindowPtr = NULL;
    mversionWndPtr = NULL;
    mversionTextPtr = NULL;
    mTextView3Ptr = NULL;
    mresetCancelPtr = NULL;
    mresetSurePtr = NULL;
    mTextView2Ptr = NULL;
    mTextView1Ptr = NULL;
    mResetWindowPtr = NULL;
    mmodeTextViewPtr = NULL;
    moptionListViewPtr = NULL;
    mmodeWindowPtr = NULL;
    mmodeListViewPtr = NULL;
}

const char* settingsActivity::getAppName() const{
	return "settings.ftu";
}

//TAG:onCreate
void settingsActivity::onCreate() {
	Activity::onCreate();
    mTextView14Ptr = (ZKTextView*)findControlByID(ID_SETTINGS_TextView14);
    mdebugButtonPtr = (ZKButton*)findControlByID(ID_SETTINGS_debugButton);
    mmcuUpdSeekBarPtr = (ZKSeekBar*)findControlByID(ID_SETTINGS_mcuUpdSeekBar);if(mmcuUpdSeekBarPtr!= NULL){mmcuUpdSeekBarPtr->setSeekBarChangeListener(this);}
    mprogressWndPtr = (ZKWindow*)findControlByID(ID_SETTINGS_progressWnd);
    mmcuCancelPtr = (ZKButton*)findControlByID(ID_SETTINGS_mcuCancel);
    mmcuSurePtr = (ZKButton*)findControlByID(ID_SETTINGS_mcuSure);
    mTextView13Ptr = (ZKTextView*)findControlByID(ID_SETTINGS_TextView13);
    mTextView8Ptr = (ZKTextView*)findControlByID(ID_SETTINGS_TextView8);
    mUpgradeWindowPtr = (ZKWindow*)findControlByID(ID_SETTINGS_UpgradeWindow);
    mHornButtonPtr = (ZKButton*)findControlByID(ID_SETTINGS_HornButton);
    mThemeBackPtr = (ZKTextView*)findControlByID(ID_SETTINGS_ThemeBack);
    mPowerButtonPtr = (ZKButton*)findControlByID(ID_SETTINGS_PowerButton);
    mSettingButtonPtr = (ZKButton*)findControlByID(ID_SETTINGS_SettingButton);
    mLinkButtonPtr = (ZKButton*)findControlByID(ID_SETTINGS_LinkButton);
    mLightButtonPtr = (ZKButton*)findControlByID(ID_SETTINGS_LightButton);
    mbottomWindowPtr = (ZKWindow*)findControlByID(ID_SETTINGS_bottomWindow);
    mTextView11Ptr = (ZKTextView*)findControlByID(ID_SETTINGS_TextView11);
    mOtpWindowPtr = (ZKWindow*)findControlByID(ID_SETTINGS_OtpWindow);
    mOtpButtonPtr = (ZKButton*)findControlByID(ID_SETTINGS_OtpButton);
    mendSureButtonPtr = (ZKButton*)findControlByID(ID_SETTINGS_endSureButton);
    mendCancelButtonPtr = (ZKButton*)findControlByID(ID_SETTINGS_endCancelButton);
    mendTimeSplitTextPtr = (ZKTextView*)findControlByID(ID_SETTINGS_endTimeSplitText);
    mendMinListViewPtr = (ZKListView*)findControlByID(ID_SETTINGS_endMinListView);if(mendMinListViewPtr!= NULL){mendMinListViewPtr->setListAdapter(this);mendMinListViewPtr->setItemClickListener(this);}
    mendHourListViewPtr = (ZKListView*)findControlByID(ID_SETTINGS_endHourListView);if(mendHourListViewPtr!= NULL){mendHourListViewPtr->setListAdapter(this);mendHourListViewPtr->setItemClickListener(this);}
    mendMinTextViewPtr = (ZKTextView*)findControlByID(ID_SETTINGS_endMinTextView);
    mendHourTextViewPtr = (ZKTextView*)findControlByID(ID_SETTINGS_endHourTextView);
    mendTimeTitleTextPtr = (ZKTextView*)findControlByID(ID_SETTINGS_endTimeTitleText);
    mendTimeWindowPtr = (ZKWindow*)findControlByID(ID_SETTINGS_endTimeWindow);
    mstartSureButtonPtr = (ZKButton*)findControlByID(ID_SETTINGS_startSureButton);
    mstartCancelButtonPtr = (ZKButton*)findControlByID(ID_SETTINGS_startCancelButton);
    mstartTimeSplitTextPtr = (ZKTextView*)findControlByID(ID_SETTINGS_startTimeSplitText);
    mstartMinListViewPtr = (ZKListView*)findControlByID(ID_SETTINGS_startMinListView);if(mstartMinListViewPtr!= NULL){mstartMinListViewPtr->setListAdapter(this);mstartMinListViewPtr->setItemClickListener(this);}
    mstartHourListViewPtr = (ZKListView*)findControlByID(ID_SETTINGS_startHourListView);if(mstartHourListViewPtr!= NULL){mstartHourListViewPtr->setListAdapter(this);mstartHourListViewPtr->setItemClickListener(this);}
    mstartMinTextViewPtr = (ZKTextView*)findControlByID(ID_SETTINGS_startMinTextView);
    mstartHourTextViewPtr = (ZKTextView*)findControlByID(ID_SETTINGS_startHourTextView);
    mstartTimeTitleTextPtr = (ZKTextView*)findControlByID(ID_SETTINGS_startTimeTitleText);
    mstartTimeWindowPtr = (ZKWindow*)findControlByID(ID_SETTINGS_startTimeWindow);
    mbrigSplitTextPtr = (ZKTextView*)findControlByID(ID_SETTINGS_brigSplitText);
    mbrigSureButtonPtr = (ZKButton*)findControlByID(ID_SETTINGS_brigSureButton);
    mbrigCancelButtonPtr = (ZKButton*)findControlByID(ID_SETTINGS_brigCancelButton);
    mSeekBar1Ptr = (ZKSeekBar*)findControlByID(ID_SETTINGS_SeekBar1);if(mSeekBar1Ptr!= NULL){mSeekBar1Ptr->setSeekBarChangeListener(this);}
    mbrigpercentTextViewPtr = (ZKTextView*)findControlByID(ID_SETTINGS_brigpercentTextView);
    mbrigTitleTextViewPtr = (ZKTextView*)findControlByID(ID_SETTINGS_brigTitleTextView);
    mbrightnessWindowPtr = (ZKWindow*)findControlByID(ID_SETTINGS_brightnessWindow);
    mButton1Ptr = (ZKButton*)findControlByID(ID_SETTINGS_Button1);
    mendTimeButtonPtr = (ZKButton*)findControlByID(ID_SETTINGS_endTimeButton);
    mstartTimeButtonPtr = (ZKButton*)findControlByID(ID_SETTINGS_startTimeButton);
    mbrightnessButtonPtr = (ZKButton*)findControlByID(ID_SETTINGS_brightnessButton);
    meyeTextView4Ptr = (ZKTextView*)findControlByID(ID_SETTINGS_eyeTextView4);
    meyeTextView3Ptr = (ZKTextView*)findControlByID(ID_SETTINGS_eyeTextView3);
    meyeTextView2Ptr = (ZKTextView*)findControlByID(ID_SETTINGS_eyeTextView2);
    meyeTextView1Ptr = (ZKTextView*)findControlByID(ID_SETTINGS_eyeTextView1);
    meyeTitleTextViewPtr = (ZKTextView*)findControlByID(ID_SETTINGS_eyeTitleTextView);
    meyeProtectionWindowPtr = (ZKWindow*)findControlByID(ID_SETTINGS_eyeProtectionWindow);
    mTextView12Ptr = (ZKTextView*)findControlByID(ID_SETTINGS_TextView12);
    mWaitMomentWindowPtr = (ZKWindow*)findControlByID(ID_SETTINGS_WaitMomentWindow);
    mUnderMaskTextViewPtr = (ZKTextView*)findControlByID(ID_SETTINGS_UnderMaskTextView);
    mMinTextViewPtr = (ZKTextView*)findControlByID(ID_SETTINGS_MinTextView);
    mHourTextViewPtr = (ZKTextView*)findControlByID(ID_SETTINGS_HourTextView);
    mDayTextViewPtr = (ZKTextView*)findControlByID(ID_SETTINGS_DayTextView);
    mMonthTextViewPtr = (ZKTextView*)findControlByID(ID_SETTINGS_MonthTextView);
    mYearTextViewPtr = (ZKTextView*)findControlByID(ID_SETTINGS_YearTextView);
    mformatListViewPtr = (ZKListView*)findControlByID(ID_SETTINGS_formatListView);if(mformatListViewPtr!= NULL){mformatListViewPtr->setListAdapter(this);mformatListViewPtr->setItemClickListener(this);}
    mclearCancelPtr = (ZKButton*)findControlByID(ID_SETTINGS_clearCancel);
    mclearSurePtr = (ZKButton*)findControlByID(ID_SETTINGS_clearSure);
    mTextView10Ptr = (ZKTextView*)findControlByID(ID_SETTINGS_TextView10);
    mTextView9Ptr = (ZKTextView*)findControlByID(ID_SETTINGS_TextView9);
    mClearTipWindowPtr = (ZKWindow*)findControlByID(ID_SETTINGS_ClearTipWindow);
    mButtonNetPtr = (ZKButton*)findControlByID(ID_SETTINGS_ButtonNet);
    mLeftArrowButtonPtr = (ZKButton*)findControlByID(ID_SETTINGS_LeftArrowButton);
    mRightArrowButtonPtr = (ZKButton*)findControlByID(ID_SETTINGS_RightArrowButton);
    mBottomLeftArrowButtonPtr = (ZKButton*)findControlByID(ID_SETTINGS_BottomLeftArrowButton);
    mBottomRightArrowButtonPtr = (ZKButton*)findControlByID(ID_SETTINGS_BottomRightArrowButton);
    mLinePainterPtr = (ZKPainter*)findControlByID(ID_SETTINGS_LinePainter);
    mcalib_tips3TextViewPtr = (ZKTextView*)findControlByID(ID_SETTINGS_calib_tips3TextView);
    mcalib_tips2TextViewPtr = (ZKTextView*)findControlByID(ID_SETTINGS_calib_tips2TextView);
    mcalib_tips1TextViewPtr = (ZKTextView*)findControlByID(ID_SETTINGS_calib_tips1TextView);
    mtipsTextViewPtr = (ZKTextView*)findControlByID(ID_SETTINGS_tipsTextView);
    mWindow1Ptr = (ZKWindow*)findControlByID(ID_SETTINGS_Window1);
    msaveCalButtonPtr = (ZKButton*)findControlByID(ID_SETTINGS_saveCalButton);
    munCalButtonPtr = (ZKButton*)findControlByID(ID_SETTINGS_unCalButton);
    mAdbButtonPtr = (ZKButton*)findControlByID(ID_SETTINGS_AdbButton);
    mMoreWindowPtr = (ZKWindow*)findControlByID(ID_SETTINGS_MoreWindow);
    mMoreButtonPtr = (ZKButton*)findControlByID(ID_SETTINGS_MoreButton);
    mtipCancelPtr = (ZKButton*)findControlByID(ID_SETTINGS_tipCancel);
    mtipSurePtr = (ZKButton*)findControlByID(ID_SETTINGS_tipSure);
    mLinkSwitchTipTextViewPtr = (ZKTextView*)findControlByID(ID_SETTINGS_LinkSwitchTipTextView);
    mTextView7Ptr = (ZKTextView*)findControlByID(ID_SETTINGS_TextView7);
    mLinkTipWindowPtr = (ZKWindow*)findControlByID(ID_SETTINGS_LinkTipWindow);
    mFormatCancelBtnPtr = (ZKButton*)findControlByID(ID_SETTINGS_FormatCancelBtn);
    mFormatSureBtnPtr = (ZKButton*)findControlByID(ID_SETTINGS_FormatSureBtn);
    mTextView6Ptr = (ZKTextView*)findControlByID(ID_SETTINGS_TextView6);
    mTextView5Ptr = (ZKTextView*)findControlByID(ID_SETTINGS_TextView5);
    mFormatWndPtr = (ZKWindow*)findControlByID(ID_SETTINGS_FormatWnd);
    msettimecancelPtr = (ZKButton*)findControlByID(ID_SETTINGS_settimecancel);
    msettimesurePtr = (ZKButton*)findControlByID(ID_SETTINGS_settimesure);
    myearListViewPtr = (ZKListView*)findControlByID(ID_SETTINGS_yearListView);if(myearListViewPtr!= NULL){myearListViewPtr->setListAdapter(this);myearListViewPtr->setItemClickListener(this);}
    mhourListViewPtr = (ZKListView*)findControlByID(ID_SETTINGS_hourListView);if(mhourListViewPtr!= NULL){mhourListViewPtr->setListAdapter(this);mhourListViewPtr->setItemClickListener(this);}
    mdayListViewPtr = (ZKListView*)findControlByID(ID_SETTINGS_dayListView);if(mdayListViewPtr!= NULL){mdayListViewPtr->setListAdapter(this);mdayListViewPtr->setItemClickListener(this);}
    mmonthListViewPtr = (ZKListView*)findControlByID(ID_SETTINGS_monthListView);if(mmonthListViewPtr!= NULL){mmonthListViewPtr->setListAdapter(this);mmonthListViewPtr->setItemClickListener(this);}
    mminListViewPtr = (ZKListView*)findControlByID(ID_SETTINGS_minListView);if(mminListViewPtr!= NULL){mminListViewPtr->setListAdapter(this);mminListViewPtr->setItemClickListener(this);}
    mTextView4Ptr = (ZKTextView*)findControlByID(ID_SETTINGS_TextView4);
    msettimeWindowPtr = (ZKWindow*)findControlByID(ID_SETTINGS_settimeWindow);
    mCameraViewReversePtr = (ZKCameraView*)findControlByID(ID_SETTINGS_CameraViewReverse);
    mrevLineWindowPtr = (ZKWindow*)findControlByID(ID_SETTINGS_revLineWindow);
    mversionWndPtr = (ZKWindow*)findControlByID(ID_SETTINGS_versionWnd);
    mversionTextPtr = (ZKTextView*)findControlByID(ID_SETTINGS_versionText);
    mTextView3Ptr = (ZKTextView*)findControlByID(ID_SETTINGS_TextView3);
    mresetCancelPtr = (ZKButton*)findControlByID(ID_SETTINGS_resetCancel);
    mresetSurePtr = (ZKButton*)findControlByID(ID_SETTINGS_resetSure);
    mTextView2Ptr = (ZKTextView*)findControlByID(ID_SETTINGS_TextView2);
    mTextView1Ptr = (ZKTextView*)findControlByID(ID_SETTINGS_TextView1);
    mResetWindowPtr = (ZKWindow*)findControlByID(ID_SETTINGS_ResetWindow);
    mmodeTextViewPtr = (ZKTextView*)findControlByID(ID_SETTINGS_modeTextView);
    moptionListViewPtr = (ZKListView*)findControlByID(ID_SETTINGS_optionListView);if(moptionListViewPtr!= NULL){moptionListViewPtr->setListAdapter(this);moptionListViewPtr->setItemClickListener(this);}
    mmodeWindowPtr = (ZKWindow*)findControlByID(ID_SETTINGS_modeWindow);
    mmodeListViewPtr = (ZKListView*)findControlByID(ID_SETTINGS_modeListView);if(mmodeListViewPtr!= NULL){mmodeListViewPtr->setListAdapter(this);mmodeListViewPtr->setItemClickListener(this);}
	mActivityPtr = this;
	onUI_init();
  registerProtocolDataUpdateListener(onProtocolDataUpdate);
  rigesterActivityTimer();
}

void settingsActivity::onClick(ZKBase *pBase) {
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

void settingsActivity::onResume() {
	Activity::onResume();
//    mCameraViewReversePtr->startPreview();
	EASYUICONTEXT->registerGlobalTouchListener(this);
	startVideoLoopPlayback();
	onUI_show();
}

void settingsActivity::onPause() {
	Activity::onPause();
//    mCameraViewReversePtr->stopPreview();
	EASYUICONTEXT->unregisterGlobalTouchListener(this);
	stopVideoLoopPlayback();
	onUI_hide();
}

void settingsActivity::onIntent(const Intent *intentPtr) {
	Activity::onIntent(intentPtr);
	onUI_intent(intentPtr);
}

bool settingsActivity::onTimer(int id) {
	return onUI_Timer(id);
}

void settingsActivity::onProgressChanged(ZKSeekBar *pSeekBar, int progress){

    int seekBarTablen = sizeof(SZKSeekBarCallbackTab) / sizeof(S_ZKSeekBarCallback);
    for (int i = 0; i < seekBarTablen; ++i) {
        if (SZKSeekBarCallbackTab[i].id == pSeekBar->getID()) {
            SZKSeekBarCallbackTab[i].callback(pSeekBar, progress);
            break;
        }
    }
}

int settingsActivity::getListItemCount(const ZKListView *pListView) const{
    int tablen = sizeof(SListViewFunctionsCallbackTab) / sizeof(S_ListViewFunctionsCallback);
    for (int i = 0; i < tablen; ++i) {
        if (SListViewFunctionsCallbackTab[i].id == pListView->getID()) {
            return SListViewFunctionsCallbackTab[i].getListItemCountCallback(pListView);
            break;
        }
    }
    return 0;
}

void settingsActivity::obtainListItemData(ZKListView *pListView,ZKListView::ZKListItem *pListItem, int index){
    int tablen = sizeof(SListViewFunctionsCallbackTab) / sizeof(S_ListViewFunctionsCallback);
    for (int i = 0; i < tablen; ++i) {
        if (SListViewFunctionsCallbackTab[i].id == pListView->getID()) {
            SListViewFunctionsCallbackTab[i].obtainListItemDataCallback(pListView, pListItem, index);
            break;
        }
    }
}

void settingsActivity::onItemClick(ZKListView *pListView, int index, int id){
    int tablen = sizeof(SListViewFunctionsCallbackTab) / sizeof(S_ListViewFunctionsCallback);
    for (int i = 0; i < tablen; ++i) {
        if (SListViewFunctionsCallbackTab[i].id == pListView->getID()) {
            SListViewFunctionsCallbackTab[i].onItemClickCallback(pListView, index, id);
            break;
        }
    }
}

void settingsActivity::onSlideItemClick(ZKSlideWindow *pSlideWindow, int index) {
    int tablen = sizeof(SSlideWindowItemClickCallbackTab) / sizeof(S_SlideWindowItemClickCallback);
    for (int i = 0; i < tablen; ++i) {
        if (SSlideWindowItemClickCallbackTab[i].id == pSlideWindow->getID()) {
            SSlideWindowItemClickCallbackTab[i].onSlideItemClickCallback(pSlideWindow, index);
            break;
        }
    }
}

bool settingsActivity::onTouchEvent(const MotionEvent &ev) {
    return onsettingsActivityTouchEvent(ev);
}

void settingsActivity::onTextChanged(ZKTextView *pTextView, const std::string &text) {
    int tablen = sizeof(SEditTextInputCallbackTab) / sizeof(S_EditTextInputCallback);
    for (int i = 0; i < tablen; ++i) {
        if (SEditTextInputCallbackTab[i].id == pTextView->getID()) {
            SEditTextInputCallbackTab[i].onEditTextChangedCallback(text);
            break;
        }
    }
}

void settingsActivity::rigesterActivityTimer() {
    int tablen = sizeof(REGISTER_ACTIVITY_TIMER_TAB) / sizeof(S_ACTIVITY_TIMEER);
    for (int i = 0; i < tablen; ++i) {
        S_ACTIVITY_TIMEER temp = REGISTER_ACTIVITY_TIMER_TAB[i];
        registerTimer(temp.id, temp.time);
    }
}


void settingsActivity::onVideoPlayerMessage(ZKVideoView *pVideoView, int msg) {
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

void settingsActivity::videoLoopPlayback(ZKVideoView *pVideoView, int msg, size_t callbackTabIndex) {

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
		/* no break */
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

void settingsActivity::startVideoLoopPlayback() {
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

void settingsActivity::stopVideoLoopPlayback() {
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

bool settingsActivity::parseVideoFileList(const char *pFileListPath, std::vector<string>& mediaFileList) {
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

int settingsActivity::removeCharFromString(string& nString, char c) {
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

void settingsActivity::registerUserTimer(int id, int time) {
	registerTimer(id, time);
}

void settingsActivity::unregisterUserTimer(int id) {
	unregisterTimer(id);
}

void settingsActivity::resetUserTimer(int id, int time) {
	resetTimer(id, time);
}