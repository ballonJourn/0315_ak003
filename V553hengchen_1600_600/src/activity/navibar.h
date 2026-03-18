#ifndef _SYSAPP_NAVIBAR_H_
#define _SYSAPP_NAVIBAR_H_

#include "entry/EasyUIContext.h"
#include "app/SysAppFactory.h"

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
#define ID_NAVIBAR_TextView3    50015
#define ID_NAVIBAR_TextView4    50017
#define ID_NAVIBAR_TextView1    50007
#define ID_NAVIBAR_briTextView    50003
#define ID_NAVIBAR_sysTextView    50002
#define ID_NAVIBAR_TempTextView    50001
#define ID_NAVIBAR_DebugWnd    110001
#define ID_NAVIBAR_briButton    20013
#define ID_NAVIBAR_blueBtn    20012
#define ID_NAVIBAR_ButtonDialogButton2    20006
#define ID_NAVIBAR_ButtonDialogButton1    20001
#define ID_NAVIBAR_TopSdBtn    20011
#define ID_NAVIBAR_TopUsbBtn    20010
#define ID_NAVIBAR_TopFmBtn    20008
#define ID_NAVIBAR_TopWifiBtn    20007
#define ID_NAVIBAR_TopTimeAMText    50022
#define ID_NAVIBAR_volumeText    50021
#define ID_NAVIBAR_volumeBtn    20009
#define ID_NAVIBAR_Window1    110009
#define ID_NAVIBAR_sys_back   100
#define ID_NAVIBAR_TopTimeTextView    50020
#define ID_NAVIBAR_TopWindow    110008
#define ID_NAVIBAR_WindowAuth    110004
#define ID_NAVIBAR_ButtonActivationYes    20005
#define ID_NAVIBAR_TextViewTips    50010
#define ID_NAVIBAR_TextViewCountdown    50011
#define ID_NAVIBAR_TextViewCarplay    50016
#define ID_NAVIBAR_TextView6    50014
#define ID_NAVIBAR_TextView5    50013
#define ID_NAVIBAR_TextView2    50009
#define ID_NAVIBAR_Window3    110007
#define ID_NAVIBAR_TextViewMask    50005
#define ID_NAVIBAR_WindowDialog1    110006
#define ID_NAVIBAR_WindowDialogButtons    110005
#define ID_NAVIBAR_TextViewDialogTitle    50008
#define ID_NAVIBAR_TextViewDialogMessage    50006
#define ID_NAVIBAR_TextViewProgressMessage    50012
#define ID_NAVIBAR_TextViewProgressState    50004
#define ID_NAVIBAR_WindowProgress    110002
#define ID_NAVIBAR_WindowPopup    110003
#define ID_NAVIBAR_sys_home   101
/*TAG:Macro宏ID END*/
class navibar : public BaseApp,
                  public ZKSeekBar::ISeekBarChangeListener, 
                  public ZKListView::IItemClickListener,
                  public ZKListView::AbsListAdapter,
                  public ZKSlideWindow::ISlideItemClickListener,
                  public EasyUIContext::ITouchListener,
                  public ZKEditText::ITextChangeListener,
                  public ZKVideoView::IVideoPlayerMessageListener
{
public:
	navibar();
	virtual ~navibar();
	virtual void onCreate();
	virtual void onClick(ZKBase *pBase);
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

	ZKWindow* mainWindow() {
	  return mMainWndPtr;
	}

protected:
    /*TAG:PROTECTED_FUNCTION*/
	virtual const char* getAppName() const;


private:
    /*TAG:PRIVATE_VARIABLE*/
    int mVideoLoopIndex;
    int mVideoLoopErrorCount;
};

#endif