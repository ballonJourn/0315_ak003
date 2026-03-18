/***********************************************
/gen auto by zuitools
***********************************************/
#ifndef __DVRPLAYACTIVITY_H__
#define __DVRPLAYACTIVITY_H__


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
#define ID_DVRPLAY_TextViewtop    50008
#define ID_DVRPLAY_SubItemID    24003
#define ID_DVRPLAY_TextView2    50011
#define ID_DVRPLAY_TextView1    50009
#define ID_DVRPLAY_WindowLeft    110002
#define ID_DVRPLAY_TextViewbg    50001
#define ID_DVRPLAY_ButtonPlayDelete    20010
#define ID_DVRPLAY_ButtonNext    20007
#define ID_DVRPLAY_ButtonPrev    20006
#define ID_DVRPLAY_ButtonLock    20009
#define ID_DVRPLAY_TextViewPhoto    50007
#define ID_DVRPLAY_TextViewPlayFile    50006
#define ID_DVRPLAY_ButtonPause    20008
#define ID_DVRPLAY_TextViewProgressBg    50005
#define ID_DVRPLAY_TextViewProgress    50004
#define ID_DVRPLAY_WindowProgress    110003
#define ID_DVRPLAY_WindowPlay    110004
#define ID_DVRPLAY_TextViewDuration    50003
#define ID_DVRPLAY_TextViewElapse    50002
#define ID_DVRPLAY_SeekBarProgress    91001
#define ID_DVRPLAY_SubItemCheck    24002
#define ID_DVRPLAY_Button2    20005
#define ID_DVRPLAY_Button1    20004
#define ID_DVRPLAY_SubItemDevide    24001
#define ID_DVRPLAY_RadioGroupType    94001
#define ID_DVRPLAY_RadioButtonPhoto    22003
#define ID_DVRPLAY_RadioButtonLocked    22002
#define ID_DVRPLAY_RadioButtonNormal    22001
#define ID_DVRPLAY_ButtonDelete    20003
#define ID_DVRPLAY_ButtonSelect    20002
#define ID_DVRPLAY_ButtonView    20001
#define ID_DVRPLAY_WindowRight    110001
#define ID_DVRPLAY_ListViewFile    80001
#define ID_DVRPLAY_VideoView1    95001
/*TAG:Macro宏ID END*/

class DvrPlayActivity : public Activity, 
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
    DvrPlayActivity();
    virtual ~DvrPlayActivity();

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