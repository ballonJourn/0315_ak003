/***********************************************
/gen auto by zuitools
***********************************************/
#ifndef __MAINACTIVITY_H__
#define __MAINACTIVITY_H__


#include "app/Activity.h"
#include "entry/EasyUIContext.h"

#include "uart/ProtocolData.h"
#include "uart/ProtocolParser.h"

#include "utils/Log.h"
#include "control/ZKDigitalClock.h"
#include "control/ZKButton.h"
#include "control/ZKCircleBar.h"
#include "control/ZKDiagram.h"
#include "control/ZKListView.h"
#include "control/ZKPointer.h"
#include "control/ZKQRCode.h"
#include "control/ZKTextView.h"
#include "control/ZKSeekBar.h"
#include "control/ZKEditText.h"
#include "control/ZKVideoView.h"
#include "control/ZKRadioGroup.h"
#include "window/ZKSlideWindow.h"

/*TAG:Macro宏ID*/
#define ID_MAIN_BluetoothappButton    20019
#define ID_MAIN_dockVideoButton    20017
#define ID_MAIN_appsButton    20016
#define ID_MAIN_dockBtButton    20015
#define ID_MAIN_dockPowerButton    20014
#define ID_MAIN_dockSettingButton    20013
#define ID_MAIN_dockVolumeoutputButton    20012
#define ID_MAIN_dockLinkButton    20003
#define ID_MAIN_TextView4    50001
#define ID_MAIN_mainandroidautoButton    20009
#define ID_MAIN_maincarplayButton    20008
#define ID_MAIN_timeTextView    50011
#define ID_MAIN_weekTextView    50010
#define ID_MAIN_dateTextView    50009
#define ID_MAIN_RadioButton1    22002
#define ID_MAIN_RadioButton0    22001
#define ID_MAIN_StatusRadioGroup    94001
#define ID_MAIN_MCButton    20021
#define ID_MAIN_Window2    110008
#define ID_MAIN_wifiVideoButton    20011
#define ID_MAIN_AAButton    20005
#define ID_MAIN_AudioOutputButton    20004
#define ID_MAIN_PlaybackButton    20002
#define ID_MAIN_SETButton    20010
#define ID_MAIN_PowerButton    20027
#define ID_MAIN_SettingButton    20026
#define ID_MAIN_HornButton    20025
#define ID_MAIN_LinkButton    20024
#define ID_MAIN_LightButton    20023
#define ID_MAIN_bottomWindow    110009
#define ID_MAIN_Window1    110007
#define ID_MAIN_videoCheckButton    20020
#define ID_MAIN_DvrTimeWindow    110006
#define ID_MAIN_Button1    20018
#define ID_MAIN_dvrTimeTextView    50005
#define ID_MAIN_dvrPlayingTextView    50002
#define ID_MAIN_VideoView1    95001
#define ID_MAIN_DvrWindow    110005
#define ID_MAIN_linkTipsTextView    50003
#define ID_MAIN_linkTipsWindow    110002
#define ID_MAIN_ACButton    20007
#define ID_MAIN_APButton    20006
#define ID_MAIN_AppWindow1    110003
#define ID_MAIN_CPButton    20001
/*TAG:Macro宏ID END*/

class mainActivity : public Activity, 
                     public ZKSeekBar::ISeekBarChangeListener, 
                     public ZKListView::IItemClickListener,
                     public ZKListView::AbsListAdapter,
                     public ZKSlideWindow::ISlideItemClickListener,
                     public EasyUIContext::ITouchListener,
                     public ZKRadioGroup::ICheckedChangeListener,
                     public ZKEditText::ITextChangeListener,
                     public ZKVideoView::IVideoPlayerMessageListener
{
public:
    mainActivity();
    virtual ~mainActivity();

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
    virtual bool onBack();
    virtual void onIntent(const Intent *intentPtr);
    virtual bool onTimer(int id);

    virtual void onProgressChanged(ZKSeekBar *pSeekBar, int progress);

    virtual int getListItemCount(const ZKListView *pListView) const;
    virtual void obtainListItemData(ZKListView *pListView, ZKListView::ZKListItem *pListItem, int index);
    virtual void onItemClick(ZKListView *pListView, int index, int subItemIndex);

    virtual void onSlideItemClick(ZKSlideWindow *pSlideWindow, int index);

    virtual bool onTouchEvent(const MotionEvent &ev);
    virtual void onCheckedChanged(ZKRadioGroup* pRadioGroup, int checkedID);

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