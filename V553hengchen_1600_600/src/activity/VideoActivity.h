/***********************************************
/gen auto by zuitools
***********************************************/
#ifndef __VIDEOACTIVITY_H__
#define __VIDEOACTIVITY_H__


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
#define ID_VIDEO_SplitText    50001
#define ID_VIDEO_fullButton    20001
#define ID_VIDEO_AudioButton    20014
#define ID_VIDEO_VideoButton    20013
#define ID_VIDEO_VideoBackButton    20033
#define ID_VIDEO_DisButton1    20032
#define ID_VIDEO_DisButton    20031
#define ID_VIDEO_VideoNextButton    20025
#define ID_VIDEO_VideoPlayButton    20024
#define ID_VIDEO_VideoProcButton    20023
#define ID_VIDEO_VideoSeekBar    91003
#define ID_VIDEO_TextView15    50027
#define ID_VIDEO_FullButton    20022
#define ID_VIDEO_TextView12    50020
#define ID_VIDEO_PlayBigButton    20021
#define ID_VIDEO_TextView11    50019
#define ID_VIDEO_TextViewTime    50021
#define ID_VIDEO_PlayWindow    110007
#define ID_VIDEO_Button5    20018
#define ID_VIDEO_Button4    20017
#define ID_VIDEO_TextView4    50010
#define ID_VIDEO_TextView3    50009
#define ID_VIDEO_VideoTipWindow    110006
#define ID_VIDEO_DisplayButton1    20030
#define ID_VIDEO_TextView17    50029
#define ID_VIDEO_Button6    20020
#define ID_VIDEO_TextView8    50016
#define ID_VIDEO_TextView7    50015
#define ID_VIDEO_TextView6    50012
#define ID_VIDEO_TextView5    50011
#define ID_VIDEO_PlaySeekBar    91002
#define ID_VIDEO_TextView14    50026
#define ID_VIDEO_NextButton    20012
#define ID_VIDEO_StopButton    20011
#define ID_VIDEO_ProcButton    20010
#define ID_VIDEO_StartTextView    50014
#define ID_VIDEO_EndTextView    50013
#define ID_VIDEO_TextView9    50017
#define ID_VIDEO_TextView2    50008
#define ID_VIDEO_Button3    20009
#define ID_VIDEO_TextView10    50018
#define ID_VIDEO_SubItem1    24003
#define ID_VIDEO_AVFileListView    80004
#define ID_VIDEO_Window1    110005
#define ID_VIDEO_VideoView2    95002
/*TAG:Macro宏ID END*/

class VideoActivity : public Activity, 
                     public ZKSeekBar::ISeekBarChangeListener, 
                     public ZKListView::IItemClickListener,
                     public ZKListView::AbsListAdapter,
                     public ZKSlideWindow::ISlideItemClickListener,
                     public EasyUIContext::ITouchListener,
                     public ZKEditText::ITextChangeListener,
                     public ZKVideoView::IVideoPlayerMessageListener
{
public:
    VideoActivity();
    virtual ~VideoActivity();

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