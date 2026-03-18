/***********************************************
/gen auto by zuitools
***********************************************/
#ifndef __LINKACTIVITY_H__
#define __LINKACTIVITY_H__


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
#include "window/ZKSlideWindow.h"

/*TAG:Macro宏ID*/
#define ID_LINK_Window3    110015
#define ID_LINK_TextView17    50051
#define ID_LINK_TextView16    50050
#define ID_LINK_cpbtTextview    50049
#define ID_LINK_bluemanageTextView    50048
#define ID_LINK_TextView15    50047
#define ID_LINK_TextView14    50046
#define ID_LINK_TextView13    50045
#define ID_LINK_accodeTextView    50042
#define ID_LINK_aicastButton    20017
#define ID_LINK_acTipsTextView    50044
#define ID_LINK_acTipsTitleText    50043
#define ID_LINK_aicastWindow    110014
#define ID_LINK_TextView11    50041
#define ID_LINK_TextView10    50040
#define ID_LINK_TextView2    50028
#define ID_LINK_TextView8    50024
#define ID_LINK_wifiSetupTitleView    50039
#define ID_LINK_mcTipsTitleView    50038
#define ID_LINK_apTipsTitleView    50037
#define ID_LINK_aaTipsTitleView    50036
#define ID_LINK_cpTipsTitleView    50035
#define ID_LINK_musicZhen    20016
#define ID_LINK_albumIcon    50032
#define ID_LINK_artistIcon    50031
#define ID_LINK_changesureBtn    20008
#define ID_LINK_changecancelBtn    20007
#define ID_LINK_line4    50034
#define ID_LINK_line3    50033
#define ID_LINK_line2    50020
#define ID_LINK_line1    50002
#define ID_LINK_Window2    110012
#define ID_LINK_TextView12    50030
#define ID_LINK_Window1    110013
#define ID_LINK_musicPng    50029
#define ID_LINK_WinMusicPng    110011
#define ID_LINK_wifiButton    20015
#define ID_LINK_wifiVideoTextView    50027
#define ID_LINK_wifiSetupTextView    50023
#define ID_LINK_wifiWindow    110003
#define ID_LINK_HornButton    20014
#define ID_LINK_btButton    20005
#define ID_LINK_mcTipsTextView    50001
#define ID_LINK_mcStepTextView    50003
#define ID_LINK_apTipsTextView    50019
#define ID_LINK_apStepTextView    50011
#define ID_LINK_aaTipsTextView    50015
#define ID_LINK_aaStepTextView    50013
#define ID_LINK_cpTipsTextView    50008
#define ID_LINK_cpStepTextView    50012
#define ID_LINK_connectedNameTextView    50026
#define ID_LINK_disConnectButton    20033
#define ID_LINK_TextView9    50025
#define ID_LINK_disconCancelButton    20034
#define ID_LINK_disConnectWindow    110010
#define ID_LINK_nameLongTipsWindow    110009
#define ID_LINK_autoConnButton    20012
#define ID_LINK_disconnectBtn    20010
#define ID_LINK_connDevText    20009
#define ID_LINK_TextView7    50022
#define ID_LINK_enableButton    20011
#define ID_LINK_btNameEditText    51001
#define ID_LINK_TextView6    50021
#define ID_LINK_TextView5    50018
#define ID_LINK_TextView4    50017
#define ID_LINK_TextView3    50016
#define ID_LINK_TextView1    50014
#define ID_LINK_BtWnd    110008
#define ID_LINK_TipinfoText    50007
#define ID_LINK_TipWnd    110001
#define ID_LINK_airplayBtn    20003
#define ID_LINK_miracastBtn    20004
#define ID_LINK_androidautoBtn    20002
#define ID_LINK_bluemusicBtn    20006
#define ID_LINK_CarplayBtn    20001
#define ID_LINK_miracastWindow    110007
#define ID_LINK_apWindow    110005
#define ID_LINK_cpbtBtn    20013
#define ID_LINK_cpWindow    110004
#define ID_LINK_aaWindow    110006
#define ID_LINK_tittleTextView    50004
#define ID_LINK_artistTextView    50005
#define ID_LINK_albumTextView    50006
#define ID_LINK_curposTextView    50009
#define ID_LINK_durationTextView    50010
#define ID_LINK_nextButton    20030
#define ID_LINK_prevButton    20031
#define ID_LINK_playButton    20032
#define ID_LINK_musicSeekBar    91002
#define ID_LINK_bluemusicWnd    110002
/*TAG:Macro宏ID END*/

class LinkActivity : public Activity, 
                     public ZKSeekBar::ISeekBarChangeListener, 
                     public ZKListView::IItemClickListener,
                     public ZKListView::AbsListAdapter,
                     public ZKSlideWindow::ISlideItemClickListener,
                     public EasyUIContext::ITouchListener,
                     public ZKEditText::ITextChangeListener,
                     public ZKVideoView::IVideoPlayerMessageListener
{
public:
    LinkActivity();
    virtual ~LinkActivity();

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