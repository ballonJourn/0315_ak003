/***********************************************
/gen auto by zuitools
***********************************************/
#ifndef __AUDIOOUTPUTACTIVITY_H__
#define __AUDIOOUTPUTACTIVITY_H__


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
#define ID_AUDIOOUTPUT_fmSeekTextView    50004
#define ID_AUDIOOUTPUT_ThemeBack    50014
#define ID_AUDIOOUTPUT_Window1    110003
#define ID_AUDIOOUTPUT_Window2    110007
#define ID_AUDIOOUTPUT_TextView2    50011
#define ID_AUDIOOUTPUT_FMTextView    50002
#define ID_AUDIOOUTPUT_tipCancel    20004
#define ID_AUDIOOUTPUT_tipSure    20003
#define ID_AUDIOOUTPUT_LinkSwitchTipTextView    50013
#define ID_AUDIOOUTPUT_TextView7    50012
#define ID_AUDIOOUTPUT_LinkTipWindow    110002
#define ID_AUDIOOUTPUT_fmTipsTextView    50006
#define ID_AUDIOOUTPUT_AuxBtn    20009
#define ID_AUDIOOUTPUT_FMBtn    20008
#define ID_AUDIOOUTPUT_linkvoiceBtn    20006
#define ID_AUDIOOUTPUT_hornBtn    20005
#define ID_AUDIOOUTPUT_TextView11    50009
#define ID_AUDIOOUTPUT_TextView9    50007
#define ID_AUDIOOUTPUT_AUXWnd    110006
#define ID_AUDIOOUTPUT_TextView10    50008
#define ID_AUDIOOUTPUT_TextView6    50005
#define ID_AUDIOOUTPUT_hornWnd    110005
#define ID_AUDIOOUTPUT_TextView12    50010
#define ID_AUDIOOUTPUT_TextView5    50001
#define ID_AUDIOOUTPUT_linkvoiceWnd    110004
#define ID_AUDIOOUTPUT_fmSeekBar    91001
#define ID_AUDIOOUTPUT_addButton    20002
#define ID_AUDIOOUTPUT_decButton    20001
#define ID_AUDIOOUTPUT_EditTextFmFreq    51001
#define ID_AUDIOOUTPUT_TextView3    50003
#define ID_AUDIOOUTPUT_FmTransmitWnd    110001
/*TAG:Macro宏ID END*/

class AudioOutputActivity : public Activity, 
                     public ZKSeekBar::ISeekBarChangeListener, 
                     public ZKListView::IItemClickListener,
                     public ZKListView::AbsListAdapter,
                     public ZKSlideWindow::ISlideItemClickListener,
                     public EasyUIContext::ITouchListener,
                     public ZKEditText::ITextChangeListener,
                     public ZKVideoView::IVideoPlayerMessageListener
{
public:
    AudioOutputActivity();
    virtual ~AudioOutputActivity();

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