/***********************************************
/gen auto by zuitools
***********************************************/
#ifndef __SETTINGSACTIVITY_H__
#define __SETTINGSACTIVITY_H__


#include "app/Activity.h"
#include "entry/EasyUIContext.h"

#include "uart/ProtocolData.h"
#include "uart/ProtocolParser.h"

#include "utils/Log.h"
#include "control/ZKDigitalClock.h"
#include "control/ZKPainter.h"
#include "control/ZKButton.h"
#include "control/ZKCameraView.h"
#include "control/ZKCircleBar.h"
#include "control/ZKDiagram.h"
#include "control/ZKListView.h"
#include "control/ZKPointer.h"
#include "control/ZKQRCode.h"
#include "control/ZKTextView.h"
#include "control/ZKSeekBar.h"
#include "control/ZKEditText.h"
#include "control/ZKVideoView.h"
#include "window/ZKSlideWindow.h"

/*TAG:Macro宏ID*/
#define ID_SETTINGS_TextView14    50027
#define ID_SETTINGS_debugButton    20038
#define ID_SETTINGS_mcuUpdSeekBar    91002
#define ID_SETTINGS_progressWnd    110019
#define ID_SETTINGS_mcuCancel    20037
#define ID_SETTINGS_mcuSure    20036
#define ID_SETTINGS_TextView13    50024
#define ID_SETTINGS_TextView8    50015
#define ID_SETTINGS_UpgradeWindow    110018
#define ID_SETTINGS_textSubItem    24006
#define ID_SETTINGS_HornButton    20033
#define ID_SETTINGS_ThemeBack    50023
#define ID_SETTINGS_FormatSureBtn    20007
#define ID_SETTINGS_FormatCancelBtn    20006
#define ID_SETTINGS_resetSure    20003
#define ID_SETTINGS_resetCancel    20002
#define ID_SETTINGS_clearSure    20018
#define ID_SETTINGS_clearCancel    20017
#define ID_SETTINGS_tipSure    20008
#define ID_SETTINGS_tipCancel    20001
#define ID_SETTINGS_PowerButton    20035
#define ID_SETTINGS_SettingButton    20034
#define ID_SETTINGS_LinkButton    20032
#define ID_SETTINGS_LightButton    20031
#define ID_SETTINGS_bottomWindow    110017
#define ID_SETTINGS_TextView11    50046
#define ID_SETTINGS_OtpWindow    110016
#define ID_SETTINGS_OtpButton    20029
#define ID_SETTINGS_endSureButton    20028
#define ID_SETTINGS_endCancelButton    20027
#define ID_SETTINGS_endTimeSplitText    50045
#define ID_SETTINGS_endMinListView    80008
#define ID_SETTINGS_endHourListView    80007
#define ID_SETTINGS_endMinTextView    50044
#define ID_SETTINGS_endHourTextView    50043
#define ID_SETTINGS_endTimeTitleText    50042
#define ID_SETTINGS_endTimeWindow    110015
#define ID_SETTINGS_startSureButton    20026
#define ID_SETTINGS_startCancelButton    20025
#define ID_SETTINGS_startTimeSplitText    50041
#define ID_SETTINGS_startMinListView    80004
#define ID_SETTINGS_startHourListView    80003
#define ID_SETTINGS_startMinTextView    50040
#define ID_SETTINGS_startHourTextView    50039
#define ID_SETTINGS_startTimeTitleText    50038
#define ID_SETTINGS_startTimeWindow    110014
#define ID_SETTINGS_brigSplitText    50037
#define ID_SETTINGS_brigSureButton    20024
#define ID_SETTINGS_brigCancelButton    20023
#define ID_SETTINGS_SeekBar1    91001
#define ID_SETTINGS_brigpercentTextView    50036
#define ID_SETTINGS_brigTitleTextView    50035
#define ID_SETTINGS_brightnessWindow    110013
#define ID_SETTINGS_Button1    20022
#define ID_SETTINGS_endTimeButton    20021
#define ID_SETTINGS_startTimeButton    20020
#define ID_SETTINGS_brightnessButton    20019
#define ID_SETTINGS_eyeTextView4    50034
#define ID_SETTINGS_eyeTextView3    50033
#define ID_SETTINGS_eyeTextView2    50032
#define ID_SETTINGS_eyeTextView1    50031
#define ID_SETTINGS_eyeTitleTextView    50030
#define ID_SETTINGS_eyeProtectionWindow    110012
#define ID_SETTINGS_selectedSubItem    24004
#define ID_SETTINGS_splitSubItem    24003
#define ID_SETTINGS_TextSubItem    24002
#define ID_SETTINGS_TextView12    50026
#define ID_SETTINGS_WaitMomentWindow    110010
#define ID_SETTINGS_UnderMaskTextView    50025
#define ID_SETTINGS_MinTextView    50022
#define ID_SETTINGS_HourTextView    50021
#define ID_SETTINGS_DayTextView    50020
#define ID_SETTINGS_MonthTextView    50019
#define ID_SETTINGS_YearTextView    50018
#define ID_SETTINGS_SubItem3    24005
#define ID_SETTINGS_formatListView    80006
#define ID_SETTINGS_TextView10    50017
#define ID_SETTINGS_TextView9    50016
#define ID_SETTINGS_ClearTipWindow    110009
#define ID_SETTINGS_ButtonNet    20016
#define ID_SETTINGS_LeftArrowButton    20012
#define ID_SETTINGS_RightArrowButton    20013
#define ID_SETTINGS_BottomLeftArrowButton    20014
#define ID_SETTINGS_BottomRightArrowButton    20015
#define ID_SETTINGS_LinePainter    52001
#define ID_SETTINGS_calib_tips3TextView    50014
#define ID_SETTINGS_calib_tips2TextView    50013
#define ID_SETTINGS_calib_tips1TextView    50012
#define ID_SETTINGS_tipsTextView    50011
#define ID_SETTINGS_Window1    110008
#define ID_SETTINGS_saveCalButton    20011
#define ID_SETTINGS_unCalButton    20030
#define ID_SETTINGS_AdbButton    20010
#define ID_SETTINGS_MoreWindow    110007
#define ID_SETTINGS_MoreButton    20009
#define ID_SETTINGS_LinkSwitchTipTextView    50010
#define ID_SETTINGS_TextView7    50009
#define ID_SETTINGS_LinkTipWindow    110006
#define ID_SETTINGS_TextView6    50008
#define ID_SETTINGS_TextView5    50007
#define ID_SETTINGS_FormatWnd    110005
#define ID_SETTINGS_settimecancel    20005
#define ID_SETTINGS_settimesure    20004
#define ID_SETTINGS_Suby    24015
#define ID_SETTINGS_yearListView    80005
#define ID_SETTINGS_SubItem4    24013
#define ID_SETTINGS_Subh    24012
#define ID_SETTINGS_hourListView    80015
#define ID_SETTINGS_Subd    24017
#define ID_SETTINGS_dayListView    80019
#define ID_SETTINGS_Subm    24016
#define ID_SETTINGS_monthListView    80021
#define ID_SETTINGS_Subs    24014
#define ID_SETTINGS_minListView    80016
#define ID_SETTINGS_TextView4    50006
#define ID_SETTINGS_settimeWindow    110004
#define ID_SETTINGS_CameraViewReverse    97001
#define ID_SETTINGS_revLineWindow    110011
#define ID_SETTINGS_versionWnd    110003
#define ID_SETTINGS_versionText    50005
#define ID_SETTINGS_TextView3    50004
#define ID_SETTINGS_TextView2    50003
#define ID_SETTINGS_TextView1    50002
#define ID_SETTINGS_ResetWindow    110002
#define ID_SETTINGS_modeTextView    50001
#define ID_SETTINGS_optionListView    80002
#define ID_SETTINGS_modeWindow    110001
#define ID_SETTINGS_PicSubItem    24001
#define ID_SETTINGS_modeListView    80001
/*TAG:Macro宏ID END*/

class settingsActivity : public Activity, 
                     public ZKSeekBar::ISeekBarChangeListener, 
                     public ZKListView::IItemClickListener,
                     public ZKListView::AbsListAdapter,
                     public ZKSlideWindow::ISlideItemClickListener,
                     public EasyUIContext::ITouchListener,
                     public ZKEditText::ITextChangeListener,
                     public ZKVideoView::IVideoPlayerMessageListener
{
public:
    settingsActivity();
    virtual ~settingsActivity();

    /**
     * 注册定时器
     */
	void registerUserTimer(int id, int time);
	/**
	 * 取消定时器
	 */
	void unregisterUserTimer(int id);
	/**
	 * 重置定时器
	 */
	void resetUserTimer(int id, int time);

protected:
    /*TAG:PROTECTED_FUNCTION*/
    virtual const char* getAppName() const;
    virtual void onCreate();
    virtual void onClick(ZKBase *pBase);
    virtual void onResume();
    virtual void onPause();
    virtual void onIntent(const Intent *intentPtr);
    virtual bool onTimer(int id);

    virtual void onProgressChanged(ZKSeekBar *pSeekBar, int progress);

    virtual int getListItemCount(const ZKListView *pListView) const;
    virtual void obtainListItemData(ZKListView *pListView, ZKListView::ZKListItem *pListItem, int index);
    virtual void onItemClick(ZKListView *pListView, int index, int subItemIndex);

    virtual void onSlideItemClick(ZKSlideWindow *pSlideWindow, int index);

    virtual bool onTouchEvent(const MotionEvent &ev);

    virtual void onTextChanged(ZKTextView *pTextView, const string &text);

    void rigesterActivityTimer();

    virtual void onVideoPlayerMessage(ZKVideoView *pVideoView, int msg);
    void videoLoopPlayback(ZKVideoView *pVideoView, int msg, size_t callbackTabIndex);
    void startVideoLoopPlayback();
    void stopVideoLoopPlayback();
    bool parseVideoFileList(const char *pFileListPath, std::vector<string>& mediaFileList);
    int removeCharFromString(string& nString, char c);


private:
    /*TAG:PRIVATE_VARIABLE*/
    int mVideoLoopIndex;
    int mVideoLoopErrorCount;

};

#endif