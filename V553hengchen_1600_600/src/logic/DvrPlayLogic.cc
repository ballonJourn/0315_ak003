#pragma once
#include "uart/ProtocolSender.h"
#include <base/base.hpp>
#include <vector>
#include <mpi/case/recorder.h>
#include <mpi/module/vo.h>
#include <base/strings.hpp>
#include <base/files.hpp>
#include <base/time.hpp>
#include <base/handler.hpp>
#include <base/timer.hpp>
#include <mpi/case/jpeg_viewer.h>
#include <base/mount_notification.h>
#include "edge/popup_service.h"
#include "link/context.h"
#include <mpi/case/camera.h>
#include "misc/utility.h"
#include "common.h"
#include "misc/app_server_impl.h"
#include "media/audio_context.h"
#include "mode_observer.h"

#define RECORDER mpi::Recorder::instance()

namespace {

enum {
  TIMER_INIT_PLAYER = 10,
  TIMER_PLAY_PROGRESS,
  TIMER_STOP_RECORDER,
  TIMER_PLAY_NEXT,
  TIMER_PLAY_MENU,
};

struct Entry {
  std::string file;
  bool select;
};

enum Messages {
  kNext = 1,
  kPrev,
  kSeek,
};

const char CHAR_EMPTY = '0';
const char CHAR_UNSELECTED = '1';
const char CHAR_SELECTED = '2';

static bool is_tracking_ = false;
static int track_progress_ = -1;

base::timer menu_timer;
std::vector<Entry> file_entries;
int selected_file_index;
std::shared_ptr<mpi::VO> vo_dev;
int selecting = 0;
std::shared_ptr<base::handler<int>> handler;
std::shared_ptr<mpi::JpegViewer> jpeg_viewer;
std::shared_ptr<base::MountNotification> extsd_mount;

void loadFiles(mpi::FileType type, bool lock) {
  auto view =
      mButtonViewPtr->isSelected() ?
          mpi::VIEW_REAR : mpi::VIEW_FRONT;
  auto files = mpi::Recorder::instance().getFiles((mpi::FileFormat)type, view, lock, true, true);
  file_entries.clear();
  for (auto f : files) {
    file_entries.push_back({f, false});
  }
  if (selected_file_index >= (int)file_entries.size()) {
    selected_file_index = file_entries.size() > 0 ? 0 : -1;
  }
  mListViewFilePtr->refreshListView();
  //mVideoView1Ptr->setVisible(!file_entries.empty());
  //mWindowPlayPtr->setVisible(!file_entries.empty());
  if (file_entries.empty()) {
    mTextViewPlayFilePtr->setText("");
  }
}

std::shared_ptr<mpi::VO> initPlayerEnv() {
  std::shared_ptr<mpi::VO> vo;
  try {
    mpi::initializeSystem();
    vo = std::make_shared<mpi::VO>(0);
    vo->enable();
    VO_PUB_ATTR_S voAttr = vo->getAttr();
    voAttr.enIntfType = VO_INTF_LCD;
    voAttr.enIntfSync = VO_OUTPUT_NTSC;
    vo->setAttr(voAttr);
    return vo;
  } catch (mpi::Exception& e) {
    LOGE_TRACE("%s", e.what());
  }
  return vo;
}

std::string timeString(int seconds) {
  return base::format("%02d:%02d:%02d",
      seconds / 60 / 60, seconds / 60 % 60, seconds % 60);
}

void handleMessage(const base::message<int>* msg, const void* user_data) {
  switch (msg->what) {
    case kNext:
      break;
    case kPrev:
      break;
    case kSeek: {
      int progress = msg->obj;
      mVideoView1Ptr->seekTo(progress);
    }
    break;
    default:
      break;
  }
}

void setProgress(int total, int elapse) {
  int w = mWindowProgressPtr->getPosition().mWidth * (elapse / 1.0 / total);
  auto pos = mTextViewProgressPtr->getPosition();
  pos.mWidth = w;
  mTextViewProgressPtr->setPosition(pos);
  if (!is_tracking_) {
	  mSeekBarProgressPtr->setProgress(elapse);
  }
  LOGD("setProgress total = %d, elapse = %d", total, elapse);
//  mTextViewElapsePtr->setText(timeString(elapse / 1000));
  mTextViewDurationPtr->setText(
      timeString(elapse / 1000) + " / " + timeString(total / 1000));
  if (total == elapse) {
	  mSeekBarProgressPtr->setProgress(mSeekBarProgressPtr->getMax());
	  mActivityPtr->registerUserTimer(TIMER_PLAY_NEXT, 500);
  }
}

bool isLock() {
  return mRadioGroupTypePtr->getCheckedID() == ID_DVRPLAY_RadioButtonLocked;
}

mpi::FileType fileType() {
  using Type = mpi::FileType;
  auto checkedID = mRadioGroupTypePtr->getCheckedID();
  if (checkedID == ID_DVRPLAY_RadioButtonNormal) {
    return Type::FILE_TYPE_VIDEO;
  }
  if (checkedID == ID_DVRPLAY_RadioButtonLocked) {
    return Type::FILE_TYPE_VIDEO;
  }
  if (checkedID == ID_DVRPLAY_RadioButtonPhoto) {
    return Type::FILE_TYPE_PHOTO;
  }
  return Type::FILE_TYPE_VIDEO;
}

class SeekbarChangeListener : public ZKSeekBar::ISeekBarChangeListener {
public:
	virtual void onProgressChanged(ZKSeekBar *pSeekBar, int progress) {
		track_progress_ = progress;
	}
	virtual void onStartTrackingTouch(ZKSeekBar *pSeekBar) {
		is_tracking_ = true;
	}
	virtual void onStopTrackingTouch(ZKSeekBar *pSeekBar) {
		is_tracking_ = false;
		if (track_progress_ >= 0) {
			setProgress(pSeekBar->getMax(), track_progress_);
		    base::message<int> msg(kSeek, track_progress_);
		    handler->send_unique_message_delayed(msg, 150);
		}
		track_progress_ = -1;
	}
};

static SeekbarChangeListener _s_seekbar_listener;

bool selectCount() {
  int counter = 0;
  for (auto it = file_entries.begin(); it != file_entries.end(); ++it) {
    if (it->select) {
      counter ++;
    }
  }
  return counter;
}

void play(const std::string& file, mpi::FileType type, bool lock) {
  if (!vo_dev) {
    return;
  }
  if (!jpeg_viewer) {
    return;
  }
  //mVideoView1Ptr->setVisible(false);
  // 修改：VideoView1和TextViewtop同时显示
  mVideoView1Ptr->setVisible(true);
  mTextViewtopPtr->setVisible(true);
  
  mWindowPlayPtr->showWnd();
  mVideoView1Ptr->stop();
  mTextViewPlayFilePtr->setVisible(true);
  jpeg_viewer->stop();

  mTextViewPlayFilePtr->setText(base::Path(file).stem());
  if (type == mpi::FILE_TYPE_PHOTO) {
    mWindowPlayPtr->setVisible(false);
    auto pos = mVideoView1Ptr->getPosition();
    // 修改：照片模式下VideoView1和TextViewtop同时显示
    mVideoView1Ptr->setVisible(true);
    mTextViewtopPtr->setVisible(true);
    jpeg_viewer->start(file, {pos.mLeft, pos.mTop, pos.mWidth, pos.mHeight});
//    mTextViewBlackPtr->setVisible(true);
    return;
  }
  mActivityPtr->registerUserTimer(TIMER_PLAY_MENU, 500);
  menu_timer.reset();
//  mTextViewBlackPtr->setVisible(false);
  mVideoView1Ptr->play(file.c_str());
}

void stop() {
  if (!vo_dev) {
    return;
  }
  if (!jpeg_viewer) {
    return;
  }
  if (lk::get_lylink_type() == LINK_TYPE_NULL) {
	  audio::change_audio_type(E_AUDIO_TYPE_BT_MUSIC);
  }
  mVideoView1Ptr->stop();
  mTextViewPlayFilePtr->setText("");
  jpeg_viewer->stop();
}

void replay() {
  if (selected_file_index < (int) file_entries.size()
      && selected_file_index >= 0) {
    //切换后，自动播放相同序号的文件
    play(file_entries[selected_file_index].file, fileType(), isLock());
  }
}

} /* anonymous namespace */

/**
 * 注册定时器
 * 填充数组用于注册定时器
 * 注意：id不能重复
 */
static S_ACTIVITY_TIMEER REGISTER_ACTIVITY_TIMER_TAB[] = {
	//{0,  6000}, //定时器id=0, 时间间隔6秒
	{TIMER_STOP_RECORDER,  500}, //定时器id=0, 时间间隔6秒
};

/**
 * 当界面构造时触发
 */
static void onUI_init() {
//  async([]() {
//    NO_EXCEPTION(mpi::Recorder::instance().stop());
//  });
  mSeekBarProgressPtr->setTouchable(false);
  selecting = 0;
  mButtonLockPtr->setVisible(false);
  mWindowPlayPtr->setVisible(false);
  mButtonPausePtr->setSelected(true);
  mSeekBarProgressPtr->setSeekBarChangeListener(&_s_seekbar_listener);
  mTextViewPlayFilePtr->setText("");
  handler = std::make_shared<base::handler<int>>(handleMessage, (void*) NULL);
  mRadioGroupTypePtr->setCheckedID(ID_DVRPLAY_RadioButtonNormal);

  extsd_mount = std::make_shared<base::MountNotification>(TFCARD_MOUNT_POINT,
      [](bool mount_state, const std::string& mount_point) {
    stop();
  });
  menu_timer.reset();
}

/**
 * 当切换到该界面时触发
 */
static void onUI_intent(const Intent *intentPtr) {
}

/*
 * 当界面显示时触发
 */
static void onUI_show() {
  stopAppServer();
  LOGD_TRACE("");
  NO_EXCEPTION(mpi::FrontCamera::instance().stop());
  NO_EXCEPTION(mpi::RearCamera::instance().stop());
  mActivityPtr->registerUserTimer(TIMER_INIT_PLAYER, 50); // @suppress("无效参数")
}

/*
 * 当界面隐藏时触发
 */
static void onUI_hide() {
  LOGD_TRACE("");
  stop();
  mActivityPtr->unregisterUserTimer(TIMER_INIT_PLAYER); // @suppress("无效参数")
  CLOSE_ACTIVITY_WITH(mActivityPtr);
}

static bool onUI_back() {
	if (PopupService::isBusy()) return false;

	if (mVideoView1Ptr->isVisible()) {
//		  mVideoView1Ptr->stop();
		  stop();
		  // 修改：VideoView1和TextViewtop同时隐藏
		  mVideoView1Ptr->setVisible(false);
		  mTextViewtopPtr->setVisible(false);
		  
		  mWindowRightPtr->showWnd();
		  mListViewFilePtr->setVisible(true);
		  mWindowPlayPtr->hideWnd();
		  mTextViewPlayFilePtr->setVisible(false);
		  mActivityPtr->unregisterUserTimer(TIMER_PLAY_MENU);
		return false;
	}
	stop();
	{
	  //这里提前销毁VO
	  jpeg_viewer.reset();
	  vo_dev.reset();
	}
	if (mode::get_switch_mode()) {
		OPEN_APP("Dvr");
	} else {
		OPEN_APP("main");
	}
	return true;

}

/*
 * 当界面完全退出时触发
 */
static void onUI_quit() {
  LOGD_TRACE("");
  // 退出界面时关闭弹窗
  if (PopupService::isBusy()) {
	  PopupService::closeDialog();
  }
  stop();
  handler.reset();
  jpeg_viewer.reset();
  vo_dev.reset();
  extsd_mount.reset();
  file_entries.clear();
}

/**
 * 串口数据回调接口
 */
static void onProtocolDataUpdate(const SProtocolData &data) {

}

/**
 * 定时器触发函数
 * 不建议在此函数中写耗时操作，否则将影响UI刷新
 * 参数： id
 *         当前所触发定时器的id，与注册时的id相同
 * 返回值: true
 *             继续运行当前定时器
 *         false
 *             停止运行当前定时器
 */
static bool onUI_Timer(int id) {
  switch (id) {
    case TIMER_INIT_PLAYER: {
      /** 延迟初始化，避免MPP冲突 */
      vo_dev.reset();
      vo_dev = initPlayerEnv();
      jpeg_viewer = std::make_shared<mpi::JpegViewer>(*vo_dev);
      /** 自动播放第一个视频 */
//      if (!file_entries.empty()) {
//        auto first = file_entries.front().file;
//        play(first, fileType(), isLock());
//      }
      return false;
    }
        break;
    case TIMER_PLAY_PROGRESS:
        setProgress(mSeekBarProgressPtr->getMax(),
        		mVideoView1Ptr->getCurrentPosition());
//      mSeekBarProgressPtr->setProgress(mVideoView1Ptr->getCurrentPosition());
//      LOGD("seek currentPosition = %d", mSeekBarProgressPtr->getProgress());
//      LOGD("mVideoView1Ptr currentPosition = %d", mVideoView1Ptr->getCurrentPosition());
        break;
    case TIMER_STOP_RECORDER:
        NO_EXCEPTION(mpi::Recorder::instance().stop());
        mSeekBarProgressPtr->setTouchable(true);
        return false;
        break;
    case TIMER_PLAY_NEXT: {
    	if (file_entries.empty()) {
			return false;
		}
		if (selected_file_index >= (int)file_entries.size()) {
			return false;
		}
		selected_file_index = (selected_file_index + 1) % file_entries.size();
		auto &file = file_entries[selected_file_index].file;
		LOGD("click %s", file.c_str());
		play(file, fileType(), isLock());
		return false;
    }
    case TIMER_PLAY_MENU:
    	if (menu_timer.elapsed() > 10000) {
    		mWindowPlayPtr->hideWnd();
    	}
    	break;
    default:
      break;
  }
  return true;
}

/**
 * 有新的触摸事件时触发
 * 参数：ev
 *         新的触摸事件
 * 返回值：true
 *            表示该触摸事件在此被拦截，系统不再将此触摸事件传递到控件上
 *         false
 *            触摸事件将继续传递到控件上
 */
static bool onDvrPlayActivityTouchEvent(const MotionEvent &ev) {
    switch (ev.mActionStatus) {
		case MotionEvent::E_ACTION_DOWN://触摸按下
			if (PopupService::isBusy()) break;
			menu_timer.reset();
			if (mVideoView1Ptr->isVisible() && (fileType() != mpi::FileType::FILE_TYPE_PHOTO)) {
				if (!mWindowPlayPtr->isWndShow()) {
					mWindowPlayPtr->showWnd();
					return true;
				} else if (!mWindowPlayPtr->getPosition().isHit(ev.mX, ev.mY)){
					mWindowPlayPtr->hideWnd();
				}
			}
			//LOGD("时刻 = %ld 坐标  x = %d, y = %d", ev.mEventTime, ev.mX, ev.mY);
			break;
		case MotionEvent::E_ACTION_MOVE://触摸滑动
			break;
		case MotionEvent::E_ACTION_UP:  //触摸抬起
			break;
		default:
			break;
	}
	return false;
}

static void onVideoViewPlayerMessageListener_VideoView1(ZKVideoView *pVideoView, int msg) {
  switch (msg) {
    case ZKVideoView::E_MSGTYPE_VIDEO_PLAY_STARTED: {
      audio::change_audio_type(E_AUDIO_TYPE_VIDEO);
      // 修改：VideoView1和TextViewtop同时显示
      mVideoView1Ptr->setVisible(true);
      mTextViewtopPtr->setVisible(true);
      
      mWindowPlayPtr->setVisible(true);
      mSeekBarProgressPtr->setMax(mVideoView1Ptr->getDuration());
      LOGD("mVideoView1Ptr getDuration = %d", mVideoView1Ptr->getDuration());
      mButtonPausePtr->setSelected(true);
      setProgress(pVideoView->getDuration(), 0);
      mActivityPtr->registerUserTimer(TIMER_PLAY_PROGRESS, 1000); // @suppress("无效参数")

      if (lk::music_is_playing()) {
        lk::music_pause();
      }
    }
      break;
    case ZKVideoView::E_MSGTYPE_VIDEO_PLAY_COMPLETED: {
//      setProgress(mSeekBarProgressPtr->getMax(), 0);
      mActivityPtr->unregisterUserTimer(TIMER_PLAY_PROGRESS); // @suppress("无效参数")
      mButtonPausePtr->setSelected(false);
      mSeekBarProgressPtr->setProgress(mSeekBarProgressPtr->getMax());
      mActivityPtr->registerUserTimer(TIMER_PLAY_NEXT, 500);
    }
      break;
    case ZKVideoView::E_MSGTYPE_VIDEO_PLAY_ERROR:
      LOGE("play error");
      mActivityPtr->unregisterUserTimer(TIMER_PLAY_PROGRESS); // @suppress("无效参数")
      break;
  }
}

static int getListItemCount_ListViewFile(const ZKListView *pListView) {
  return file_entries.size();
}

static void obtainListItemData_ListViewFile(ZKListView *pListView,ZKListView::ZKListItem *pListItem, int index) {
  pListItem->setSelected(selected_file_index == index);
  ZKListView::ZKListSubItem *SubItemID = pListItem->findSubItemByID(ID_DVRPLAY_SubItemID);
  SubItemID->setText(index+1);
  auto check = pListItem->findSubItemByID(ID_DVRPLAY_SubItemCheck);
  if (!check) {
    return;
  }
  if (index < (int)file_entries.size()) {
    auto& entry = file_entries[index];
    auto name = base::Path(entry.file).stem();
//    if (base::file_size(entry.file) <= 0) {
//      name += "(0 byte)";
//    }

    const char char_pic = selecting ?
            (entry.select ? CHAR_SELECTED : CHAR_UNSELECTED) : CHAR_EMPTY;
    check->setText(char_pic);
    pListItem->setText(name);
  }
}

static void onListItemClick_ListViewFile(ZKListView *pListView, int index, int id) {
  if (index >= (int)file_entries.size()) {
    return;
  }
  if (selecting) {
    file_entries[index].select = !file_entries[index].select;
    return;
  }
  selected_file_index = index;
  auto file = file_entries[index].file;
  LOGD("click %s", file.c_str());
  play(file, fileType(), isLock());
}

static bool onButtonClick_ButtonView(ZKButton *pButton) {
  pButton->setSelected(!pButton->isSelected());
  if (vo_dev) {
    mVideoView1Ptr->stop();
  }
  loadFiles(fileType(), isLock());
  if (selected_file_index < (int) file_entries.size()
      && selected_file_index >= 0) {
    //切换后，自动播放相同序号的文件
//    play(file_entries[selected_file_index].file, fileType(), isLock());
  }
  return false;
}

static bool onButtonClick_ButtonSelect(ZKButton *pButton) {
  selecting = (selecting + 1) % 3;
  switch (selecting) {
    case 0:
      for (auto& entry : file_entries) {
        entry.select = false;
      }
      break;
    case 2: //全选
      for (auto& entry : file_entries) {
        entry.select = true;
      }
      break;
    default:
      break;
  }
  mListViewFilePtr->refreshListView();
  return false;
}

static bool onButtonClick_ButtonDelete(ZKButton *pButton) {
  if (selecting <= 0) {
    return false;
  }
  if (selectCount() <= 0) {
    return false;
  }
  std::vector<PopupButton> buttons;
  buttons.push_back( { "@confirm", []() {
    stop();
    for (auto it = file_entries.begin(); it != file_entries.end();) {
      if (it->select) {
        base::remove(it->file.c_str());
        it = file_entries.erase(it);
        continue;
      }
      ++it;
    }
    loadFiles(fileType(), isLock());
  } });
  buttons.push_back( { "@cancel" });
  PopupService::dialog("@Tips", "@ask_delete_file", buttons, 0,
      { 539, 136, 558, 360 });
  return false;
}

static void onCheckedChanged_RadioGroupType(ZKRadioGroup* pRadioGroup, int checkedID) {
  mTextViewPlayFilePtr->setText("");
  if (checkedID == ID_DVRPLAY_RadioButtonNormal) {
    loadFiles(mpi::FILE_TYPE_VIDEO, false);
    mButtonLockPtr->setVisible(true);
    mButtonLockPtr->setSelected(false);
  }
  if (checkedID == ID_DVRPLAY_RadioButtonLocked) {
    loadFiles(mpi::FILE_TYPE_VIDEO, true);
    mButtonLockPtr->setVisible(true);
    mButtonLockPtr->setSelected(true);
  }
  if (checkedID == ID_DVRPLAY_RadioButtonPhoto) {
    loadFiles(mpi::FILE_TYPE_PHOTO, false);
    mButtonLockPtr->setVisible(false);
  }
  if (file_entries.empty()) {
    stop();
  }
//  if (!file_entries.empty()) {
//    //每次切换后，自动播放第一个文件
//    play(file_entries.front().file, fileType(), isLock());
//  }
  mListViewFilePtr->setSelection(0);
}

static bool onButtonClick_Button1(ZKButton *pButton) {
  EASYUICONTEXT->goBack();
  return false;
}

static bool onButtonClick_Button2(ZKButton *pButton) {
    LOGD(" ButtonClick Button2 !!!\n");
    EASYUICONTEXT->goHome();
    return false;
}

static void onProgressChanged_SeekBarProgress(ZKSeekBar *pSeekBar, int progress) {
//  setProgress(pSeekBar->getMax(), progress);
//  base::message<int> msg(kSeek, progress);
//  handler->send_unique_message_delayed(msg, 150);
}

static bool onButtonClick_ButtonPause(ZKButton *pButton) {
  pButton->setSelected(!pButton->isSelected());

  bool is_play_complete = mVideoView1Ptr->getCurrentPosition()
      == mVideoView1Ptr->getDuration();
  if (is_play_complete) {
    replay();
    return false;
  }

  if (pButton->isSelected()) {
    mVideoView1Ptr->resume();
  } else {
    mVideoView1Ptr->pause();
  }
  setProgress(mSeekBarProgressPtr->getMax(),
      mVideoView1Ptr->getCurrentPosition());
  return false;
}

static bool onButtonClick_ButtonLock(ZKButton *pButton) {
  if (file_entries.empty()) {
    return false;
  }
  if (selected_file_index >= (int)file_entries.size()) {
    return false;
  }
  auto file = file_entries[selected_file_index].file;

  if (mVideoView1Ptr->isPlaying()) {
	  mVideoView1Ptr->pause();
	  mButtonPausePtr->setSelected(false);
  }

  const bool sos = isLock();
  std::vector<PopupButton> buttons;
  buttons.push_back( { "@confirm", [sos, file]() {
    stop();
    if (sos) {
      NO_EXCEPTION(mpi::Recorder::instance().unlockFile(file));
    } else {
      NO_EXCEPTION(mpi::Recorder::instance().lockFile(file));
    }
    loadFiles(fileType(), sos);
    mActivityPtr->registerUserTimer(TIMER_PLAY_NEXT, 500);
  } });
  buttons.push_back( { "@cancel" });
  auto msg = sos ? "@unlock_this_video" : "@lock_this_video";
  PopupService::dialog("@Tips", msg, buttons, 0, { 539, 136, 558, 360 });
  return false;
}
static bool onButtonClick_ButtonPrev(ZKButton *pButton) {
    LOGD(" ButtonClick ButtonPrev !!!\n");
    if (file_entries.empty()) {
    	return false;
    }
    if (selected_file_index >= (int)file_entries.size()) {
    	return false;
    }
    selected_file_index = (selected_file_index == 0) ? (file_entries.size() - 1) : (selected_file_index - 1);
    auto &file = file_entries[selected_file_index].file;
	LOGD("click %s", file.c_str());
	play(file, fileType(), isLock());
    return false;
}
static bool onButtonClick_ButtonNext(ZKButton *pButton) {
    LOGD(" ButtonClick ButtonNext !!!\n");
    if (file_entries.empty()) {
    	return false;
    }
    if (selected_file_index >= (int)file_entries.size()) {
    	return false;
    }
    selected_file_index = (selected_file_index + 1) % file_entries.size();
    auto &file = file_entries[selected_file_index].file;
    LOGD("click %s", file.c_str());
    play(file, fileType(), isLock());
    return false;
}

static bool onButtonClick_ButtonPlayDelete(ZKButton *pButton) {
    LOGD(" ButtonClick ButtonPlayDelete !!!\n");
    if (file_entries.empty()) {
    	return false;
    }
    if (selected_file_index >= (int)file_entries.size()) {
    	return false;
    }
    if (mVideoView1Ptr->isPlaying()) {
   	    mVideoView1Ptr->pause();
   	    mButtonPausePtr->setSelected(false);
    }
    auto cur_file = file_entries[selected_file_index].file;
    std::vector<PopupButton> buttons;
    buttons.push_back({"@confirm", [cur_file]() {
    	stop();
    	for (auto it = file_entries.begin(); it != file_entries.end(); ++ it) {
    		if ((*it).file == cur_file) {
    			base::remove(cur_file.c_str());
    			it = file_entries.erase(it);
    			break;
    		}
    	}
    	loadFiles(fileType(), isLock());
    	mActivityPtr->registerUserTimer(TIMER_PLAY_NEXT, 500);
    }});
    buttons.push_back({ "@cancel" });
    PopupService::dialog("@Tips", "@ask_delete_file", buttons, 0,
    		{ 539, 136, 558, 360 });
    return false;
}
