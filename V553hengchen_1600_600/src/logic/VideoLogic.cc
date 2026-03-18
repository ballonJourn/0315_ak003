#pragma once
#include "uart/ProtocolSender.h"
/*
*此文件由GUI工具生成
*文件功能：用于处理用户的逻辑相应代码
*功能说明：
*========================onButtonClick_XXXX
当页面中的按键按下后系统会调用对应的函数，XXX代表GUI工具里面的[ID值]名称，
如Button1,当返回值为false的时候系统将不再处理这个按键，返回true的时候系统将会继续处理此按键。比如SYS_BACK.
*========================onSlideWindowItemClick_XXXX(int index) 
当页面中存在滑动窗口并且用户点击了滑动窗口的图标后系统会调用此函数,XXX代表GUI工具里面的[ID值]名称，
如slideWindow1;index 代表按下图标的偏移值
*========================onSeekBarChange_XXXX(int progress) 
当页面中存在滑动条并且用户改变了进度后系统会调用此函数,XXX代表GUI工具里面的[ID值]名称，
如SeekBar1;progress 代表当前的进度值
*========================ogetListItemCount_XXXX() 
当页面中存在滑动列表的时候，更新的时候系统会调用此接口获取列表的总数目,XXX代表GUI工具里面的[ID值]名称，
如List1;返回值为当前列表的总条数
*========================oobtainListItemData_XXXX(ZKListView::ZKListItem *pListItem, int index)
 当页面中存在滑动列表的时候，更新的时候系统会调用此接口获取列表当前条目下的内容信息,XXX代表GUI工具里面的[ID值]名称，
如List1;pListItem 是贴图中的单条目对象，index是列表总目的偏移量。具体见函数说明
*========================常用接口===============
*LOGD(...)  打印调试信息的接口
*mTextXXXPtr->setText("****") 在控件TextXXX上显示文字****
*mButton1Ptr->setSelected(true); 将控件mButton1设置为选中模式，图片会切换成选中图片，按钮文字会切换为选中后的颜色
*mSeekBarPtr->setProgress(12) 在控件mSeekBar上将进度调整到12
*mListView1Ptr->refreshListView() 让mListView1 重新刷新，当列表数据变化后调用
*mDashbroadView1Ptr->setTargetAngle(120) 在控件mDashbroadView1上指针显示角度调整到120度
*
* 在Eclipse编辑器中  使用 “alt + /”  快捷键可以打开智能提示
*/
#include <vector>
#include <base/strings.hpp>
#include "mode_observer.h"
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string>
#include <vector>
#include "media/audio_context.h"
#include "media/media_context.h"
#include "media/music_player.h"
#include <base/handler.hpp>
#include <base/timer.hpp>

static bool is_video_activity_show = false;		//视频界面是否显示
static bool is_pause = false;

static std::vector<string> media_file_list;			// TF视频存放
static int play_index_;
#define DELAY_CHECK_TIMER     20

static bool is_music_tracking = false;
static int music_track_progress = -1;
static LayoutPosition small_video_position = LayoutPosition(0, 0, 640, 460);


enum {
	VideoListMode,
	MusicListMode,
};

enum {
	TimerVideoPlaying,
	TimerMusicPlaying,
};

static int flag = MusicListMode;		//判断进入视频还是音频

void showTop();
void hideTop();
base::timer menu_timer;

static bool is_tracking_ = false;
static int track_progress_ = -1;

const char *video_suffix_tab[] = {
		".mp4", ".mkv", ".avi", ".mov", ".flv", ".ts", ".3gp", ".vob", ".mpg", ".mpeg", ".wmv", NULL
};
static int format = sizeof(video_suffix_tab) / sizeof(video_suffix_tab[0]);

static const char *modetext[] = {
		"列表循环",
		"单个循环",
		"随机播放",
};
static int pic = 0;
const char *pictext[] = {"0", "1", "2"};

class SeekbarChangeListener: public ZKSeekBar::ISeekBarChangeListener {
public:
	virtual void onProgressChanged(ZKSeekBar *pSeekBar, int progress) {
		music_track_progress = progress;
		if (media::music_get_play_index() != -1) {
	        char timeStr[10];
//	        int pos = pSeekBar->getProgress();
	        snprintf(timeStr, sizeof(timeStr), "%02d:%02d", progress / 60, progress % 60);
	        mStartTextViewPtr->setText(timeStr);
		}
	}
	virtual void onStartTrackingTouch(ZKSeekBar *pSeekBar) {
		is_music_tracking = true;
	}
	virtual void onStopTrackingTouch(ZKSeekBar *pSeekBar) {
		is_music_tracking = false;
		if (music_track_progress >= 0) {
			media::music_seek(music_track_progress * 1000);
			music_track_progress = -1;
		}
	}
};
static SeekbarChangeListener progressbar;

std::vector<std::string> read_dir(std::string path, std::string prefix, std::vector<std::string>& content, bool recursion) {
	DIR *dir;
	struct dirent *entry;
	struct stat statbuf;
	if ((dir = opendir(path.c_str())) == NULL) {
		//can't open dir
		return content;
	}
	while ((entry = readdir(dir)) != NULL) {
		stat(entry->d_name, &statbuf);
		if ((strcmp(".", entry->d_name) == 0) || (strcmp("..", entry->d_name) == 0)) {
			continue;
		}
		content.push_back(prefix + "/" + entry->d_name);
		if ((S_IFDIR & statbuf.st_mode) && recursion) {
			read_dir(path + "/" + entry->d_name,
					prefix.empty() ? (entry->d_name):(prefix + "/" + entry->d_name),
							content,
							true);
		}
	}
	closedir(dir);
	return content;
}

std::vector<std::string> read_dir(std::string path, bool recursion) {
	std::vector<std::string> content;
	return read_dir(path, path, content, recursion);
}


static bool endswith(const std::string& str, const std::string& end) {
	int srclen = str.size();
	int endlen = end.size();
	if(srclen >= endlen) {
		std::string temp = str.substr(srclen - endlen, endlen);
		if(temp == end)
			return true;
	}
	return false;
}

// 查找视频文件
std::vector<std::string> find_supported_video_files(std::vector<std::string> &v,const char** file, int index) {
	std::vector<std::string> result;
	for (size_t i = 0; i < v.size(); ++i) {
		for (int j = 0; j < index - 1; j++) {
			if (endswith(v.at(i), file[j])) {
				result.push_back(v.at(i));
			}
		}
	}
	return result;
}

class VideoSeekbarChangeListener: public ZKSeekBar::ISeekBarChangeListener {
public:
	virtual void onProgressChanged(ZKSeekBar *pSeekBar, int progress) {
		track_progress_ = progress;
		char timeStr[10];
		snprintf(timeStr, sizeof(timeStr), "%02d:%02d", progress / 60, progress % 60);
		mStartTextViewPtr->setText(timeStr);
	}
	virtual void onStartTrackingTouch(ZKSeekBar *pSeekBar) {
		is_tracking_ = true;
	}
	virtual void onStopTrackingTouch(ZKSeekBar *pSeekBar) {
		is_tracking_ = false;
		if (track_progress_ >= 0) {
			mVideoView2Ptr->seekTo(track_progress_ * 1000);
			track_progress_ = -1;
		}
	}
};

static VideoSeekbarChangeListener video_progressbar;

static void not_supported_video(const std::string& str) {
	LOGD("str = %s", str.c_str());

	if (!is_video_activity_show) return ;
	std::string filename = str.substr(str.find_last_of("/")+1);
	menu_timer.reset();
	if (endswith(str, ".avi") || endswith(str, ".wmv")) {	// 不支持的视频文件
		mVideoView2Ptr->pause();
		mVideoTipWindowPtr->showWnd();
		mVideoPlayButtonPtr->setSelected(false);
		mPlayBigButtonPtr->setSelected(false);
		mStopButtonPtr->setSelected(false);
		mPlayWindowPtr->setVisible(false);
	} else {
		//mWindow1Ptr->hideWnd();
		//mVideoView2Ptr->setVisible(true);
		//mPlayWindowPtr->setVisible(false);
		//mDisplayButton1Ptr->setVisible(true);
		mTextView12Ptr->setText(filename.c_str());
		mStopButtonPtr->setSelected(true);
		mVideoView2Ptr->play(str.c_str());
	}
}
// 判断循环，单个，随机
static void nextloop(std::vector<string> &List) {
	switch (pic) {
	case 0:
	case 1:
		if (!List.empty()) {
			play_index_ = (play_index_ - 1 + List.size()) % List.size();
			not_supported_video(List[play_index_]);
		}
		break;
	case 2:
		if (!List.empty()) {
			play_index_ = rand() % List.size();
			LOGD("play_index_ == %d\n", play_index_);
			not_supported_video(List[play_index_]);
		}
		break;
	}
}

static void setDuration() {
    int max = media::music_get_duration() / 1000;
    char timeStr[12] = {0};
    snprintf(timeStr, sizeof(timeStr), "%02d:%02d", max / 60, max % 60);
    mEndTextViewPtr->setText(timeStr);
    mPlaySeekBarPtr->setMax(max);
}

// 音频回调
static void _music_play_status_cb(music_play_status_e status) {
	switch (status) {
	case E_MUSIC_PLAY_STATUS_STARTED:    // 播放开始
		setDuration();
		mPlaySeekBarPtr->setProgress(0);
//		refreshMusicInfo();
		mStopButtonPtr->setSelected(true);
		if(mAVFileListViewPtr->isVisible())  {
			mAVFileListViewPtr->refreshListView();
		}
//		mtitleTextViewPtr->setTextColor(0xFF00FF00);
		break;

	case E_MUSIC_PLAY_STATUS_RESUME:     // 恢复播放
		mStopButtonPtr->setSelected(true);
		if(mAVFileListViewPtr->isVisible())  {
			mAVFileListViewPtr->refreshListView();
		}
		setDuration();
//		mtitleTextViewPtr->setTextColor(0xFF00FF00);
		break;

	case E_MUSIC_PLAY_STATUS_PAUSE:      // 暂停播放
		mStopButtonPtr->setSelected(false);
//		mtitleTextViewPtr->setTextColor(0xFFFFFFFF);
		break;

	case E_MUSIC_PLAY_STATUS_STOP:       // 停止播放
		setDuration();
		if(mAVFileListViewPtr->isVisible())  {
			mAVFileListViewPtr->refreshListView();
		}
//		refreshMusicInfo();
		mStopButtonPtr->setSelected(false);
//		mtitleTextViewPtr->setTextColor(0xFFFFFFFF);
//		mmusicWindowPtr->hideWnd();
		break;

	case E_MUSIC_PLAY_STATUS_ERROR:{// 播放错误
//		merrorWindowPtr->showWnd();
	}
		break;

	case E_MUSIC_PLAY_STATUS_COMPLETED:  // 播放结束
		break;
	}
}


// 下一个视频
static void next() {
	nextloop(media_file_list);
}

// 上一个视频
static void proc() {
	nextloop(media_file_list);
}

std::string timeString(int seconds) {
	return base::format("%02d:%02d:%02d",
			seconds / 60 / 60, seconds / 60, seconds % 60);
}

void setProgress(int total, int elapse) {
	mTextViewTimePtr->setText(timeString(elapse) + " / " + timeString(total));
}
/**
 * 注册定时器
 * 填充数组用于注册定时器
 * 注意：id不能重复
 */
static S_ACTIVITY_TIMEER REGISTER_ACTIVITY_TIMER_TAB[] = {
	//{0,  6000}, //定时器id=0, 时间间隔6秒
	//{TimerVideoPlaying,  1000},

};

/**
 * 当界面构造时触发
 */
static void onUI_init(){
	//hideTop();
    //Tips :添加 UI初始化的显示代码到这里,如:mText1Ptr->setText("123");
//	mVideoSeekBarPtr->setSeekBarChangeListener(&progressbar);
	mVideoView2Ptr->setVolume(0.5);
	play_index_ = -1;
	media::music_add_play_status_cb(_music_play_status_cb);
	mPlaySeekBarPtr->setSeekBarChangeListener(&progressbar);

	mButton6Ptr->setText(pictext[pic]);
	mTextView17Ptr->setText(modetext[pic]);
	mActivityPtr->registerUserTimer(TimerMusicPlaying, 1000);
	menu_timer.reset();
	int curPos = 0;
	int totalPos = 0;
	if (media::music_is_playing()) {
		curPos = media::music_get_current_position() / 1000;
		totalPos = media::music_get_duration() / 1000;
		mStopButtonPtr->setSelected(true);
		mPlaySeekBarPtr->setMax(totalPos);
	} else if (media::music_get_play_index() == -1) {
		curPos = 0;
		mStopButtonPtr->setSelected(false);
	} else {
		curPos = media::music_get_current_position() / 1000;
		totalPos = media::music_get_duration() / 1000;
	}
	if (curPos >= 0) {
		char timeStr[10];
		sprintf(timeStr, "%02d:%02d", curPos / 60, curPos % 60);
		mStartTextViewPtr->setText(timeStr);
		sprintf(timeStr, "%02d:%02d", totalPos / 60, totalPos % 60);
		mEndTextViewPtr->setText(timeStr);
		if (!is_music_tracking) {
			mPlaySeekBarPtr->setProgress(curPos);
		}
	}
	is_pause = false;

}

/**
 * 当切换到该界面时触发
 */
static void onUI_intent(const Intent *intentPtr) {
    if (intentPtr != NULL) {
        //TODO
    }
}

/*
 * 当界面显示时触发
 */
static void onUI_show() {
	//hideTop();
	is_video_activity_show = true;
	// 推迟检测播放，处理倒车界面未完全退出，视频播放异常问题
	mActivityPtr->registerUserTimer(DELAY_CHECK_TIMER, 0);


//	mode::set_switch_mode(E_SWITCH_MODE_LINK);

	media_file_list.clear();
	// 递归扫描TF卡内的文件
	media_file_list = read_dir("/mnt/extsd", true);
	// 根据后缀名查找视频格式文件
	media_file_list = find_supported_video_files(media_file_list, video_suffix_tab, format);
	LOGD("播放列表 size = %d", media_file_list.size());
	for (size_t i = 0; i < media_file_list.size(); ++i) {
		LOGD("%s", media_file_list.at(i).c_str());
	}
}

/*
 * 当界面隐藏时触发
 */
static void onUI_hide() {
	is_video_activity_show = false;

	if (mVideoView2Ptr->isVisible()) {
		is_pause = !mVideoView2Ptr->isPlaying();
		track_progress_ = mVideoView2Ptr->getCurrentPosition();
	} else {
		is_pause = false;
	}
	mVideoView2Ptr->stop();
	mPlayBigButtonPtr->setSelected(false);
	mVideoPlayButtonPtr->setSelected(false);
	mStopButtonPtr->setSelected(false);
}

/*
 * 当界面完全退出时触发
 */
static void onUI_quit() {
	if (mVideoView2Ptr->isPlaying()) {
		mVideoView2Ptr->stop();
		mActivityPtr->unregisterUserTimer(TimerVideoPlaying);
	}
	media::music_remove_play_status_cb(_music_play_status_cb);
	//showTop();
	mActivityPtr->unregisterUserTimer(TimerMusicPlaying);
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
static bool onUI_Timer(int id){
	switch (id) {
	case TimerVideoPlaying: {
		int curPos = -1;
		if (menu_timer.elapsed() > 10000) {
			mPlayWindowPtr->hideWnd();
		}
		if (mVideoView2Ptr->isPlaying()) {
			curPos = mVideoView2Ptr->getCurrentPosition() / 1000;
			// LOGD("curPos = %d", mVideoView2Ptr->getDuration());
		}
		if (curPos >= 0) {
			char timeStr[10];
			sprintf(timeStr, "%02d:%02d", curPos / 60, curPos % 60);
			setProgress(mVideoSeekBarPtr->getMax(), curPos);
			mStartTextViewPtr->setText(timeStr);
			if (!is_tracking_) {
				mVideoSeekBarPtr->setProgress(curPos);
				mPlaySeekBarPtr->setProgress(curPos);
			}
			if (curPos > mVideoSeekBarPtr->getMax()) {
				next();
			}
		}
		break;
	}
	case TimerMusicPlaying: {
		int curPos = 0;
		if (media::music_is_playing()) {
			curPos = media::music_get_current_position() / 1000;
		} else if (media::music_get_play_index() == -1) {
			curPos = 0;
			mStopButtonPtr->setSelected(false);
		}
		if (curPos >= 0) {
			char timeStr[10];
			sprintf(timeStr, "%02d:%02d", curPos / 60, curPos % 60);
			mStartTextViewPtr->setText(timeStr);
			if (!is_music_tracking) {
				mPlaySeekBarPtr->setProgress(curPos);
			}
		}
		break;
	}
	case DELAY_CHECK_TIMER:
		if (is_video_activity_show && mVideoView2Ptr->isVisible()) {
			LOGD("[video] delay check timer\n");
			if (play_index_ != -1) {
				mVideoView2Ptr->play(media_file_list[play_index_].c_str(), track_progress_);
				if (is_pause) {
					usleep(1000*100);
					mVideoView2Ptr->pause();
					mPlayBigButtonPtr->setSelected(false);
					mVideoPlayButtonPtr->setSelected(false);
					mStopButtonPtr->setSelected(false);
				}
			}
			if (mVideoView2Ptr->getPosition() == mWindow1Ptr->getPosition()) {
				mDisplayButton1Ptr->setVisible(true);
			}
		}
		// 只执行一次，停掉该定时器
		return false;
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
static bool onVideoActivityTouchEvent(const MotionEvent &ev) {
    switch (ev.mActionStatus) {
		case MotionEvent::E_ACTION_DOWN://触摸按下
			menu_timer.reset();
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

static void onVideoViewPlayerMessageListener_VideoView2(ZKVideoView *pVideoView, int msg) {
	switch (msg) {
	case ZKVideoView::E_MSGTYPE_VIDEO_PLAY_STARTED:
		{
			audio::change_audio_type(E_AUDIO_TYPE_VIDEO);
			int max = pVideoView->getDuration() / 1000;
			setProgress(max, 0);
			char timeStr[10];
			sprintf(timeStr, "%02d:%02d", max / 60, max % 60);
			mEndTextViewPtr->setText(timeStr);
			mPlaySeekBarPtr->setMax(max);
			mVideoSeekBarPtr->setMax(max);
			mPlayBigButtonPtr->setSelected(true);
			mVideoPlayButtonPtr->setSelected(true);
			mStopButtonPtr->setSelected(true);
			mAVFileListViewPtr->refreshListView();
		}
		break;
	case ZKVideoView::E_MSGTYPE_VIDEO_PLAY_COMPLETED:
		if (pic == 0 || pic == 2) {
			next();
		} else if (pic == 1) {
			not_supported_video(media_file_list[play_index_]);
		}
		break;
	case ZKVideoView::E_MSGTYPE_VIDEO_PLAY_ERROR:
		break;
	}
}

static int getListItemCount_AVFileListView(const ZKListView *pListView) {
    //LOGD("getListItemCount_AVFileListView !\n");
	if (flag == VideoListMode) {
		return media_file_list.size();
	} else {
		return media::get_audio_list_size(E_STORAGE_TYPE_SD);
	}
}

static void obtainListItemData_AVFileListView(ZKListView *pListView,ZKListView::ZKListItem *pListItem, int index) {
    //LOGD(" obtainListItemData_ AVFileListView  !!!\n");
	if (flag == VideoListMode) {
		std::string video_file = media_file_list[index].substr(media_file_list[index].find_last_of("/")+1);
		pListItem->setSelected(index == play_index_);
		pListItem->setText(video_file.c_str());
	} else {
		std::string music_file = media::get_audio_file(E_STORAGE_TYPE_SD, index);
		pListItem->setSelected(index == media::music_get_play_index());
		pListItem->setText(music_file.substr(music_file.rfind("/")+1));
	}
}

static void onListItemClick_AVFileListView(ZKListView *pListView, int index, int id) {
    //LOGD(" onListItemClick_ AVFileListView  !!!\n");
	if (flag == VideoListMode) {
		if (index >= (int)media_file_list.size()) {
			return;
		}
		mPlaySeekBarPtr->setSeekBarChangeListener(&video_progressbar);
		mVideoSeekBarPtr->setSeekBarChangeListener(&video_progressbar);
		mActivityPtr->unregisterUserTimer(TimerMusicPlaying);
		mActivityPtr->registerUserTimer(TimerVideoPlaying, 1000);
		mVideoView2Ptr->setVisible(true);
		mfullButtonPtr->setVisible(true);
		mVideoPlayButtonPtr->setSelected(true);
		mStopButtonPtr->setSelected(true);
		not_supported_video(media_file_list[index]);
	} else {
			mVideoView2Ptr->stop();
			mVideoView2Ptr->setVisible(false);
			mfullButtonPtr->setVisible(false);
			mActivityPtr->unregisterUserTimer(TimerVideoPlaying);
			mActivityPtr->registerUserTimer(TimerMusicPlaying, 1000);
			mPlaySeekBarPtr->setSeekBarChangeListener(&progressbar);
		if (media::music_get_play_index() == index) {
			media::music_resume();
		} else {
			media::music_play(E_STORAGE_TYPE_SD, index);
		}
	}
}

static bool onButtonClick_Button3(ZKButton *pButton) {
    LOGD(" ButtonClick Button3 !!!\n");
    if (mAVFileListViewPtr->isVisible()) {
    	mAVFileListViewPtr->setVisible(false);
    	mTextView5Ptr->setVisible(true);
    	mTextView6Ptr->setVisible(true);
    	mVideoButtonPtr->setVisible(true);
    	mAudioButtonPtr->setVisible(true);
    } else {
    	EASYUICONTEXT->goHome();
    }
    return false;
}

static bool onButtonClick_ProcButton(ZKButton *pButton) {
    LOGD(" ButtonClick ProcButton !!!\n");
    if (mVideoView2Ptr->isVisible()) {
    	proc();
    } else {
    	media::music_prev(true);
    }
    return false;
}

static bool onButtonClick_StopButton(ZKButton *pButton) {
    LOGD(" ButtonClick StopButton !!!\n");
    if (mVideoView2Ptr->isVisible()) {
		if (mVideoView2Ptr->isPlaying()) {
			mVideoView2Ptr->pause();
			pButton->setSelected(false);
			mPlayBigButtonPtr->setSelected(false);
			mVideoPlayButtonPtr->setSelected(false);
			mStopButtonPtr->setSelected(false);
		} else {
			mVideoView2Ptr->resume();
			pButton->setSelected(true);
			mPlayBigButtonPtr->setSelected(true);
			mVideoPlayButtonPtr->setSelected(true);
			mStopButtonPtr->setSelected(true);
		}
    } else {
    	if (media::music_get_play_index() == -1) {
			return false;
		} else if (media::music_is_playing()) {
			media::music_pause();
		} else {
			media::music_resume();
		}
    }
    return false;
}

static bool onButtonClick_NextButton(ZKButton *pButton) {
    LOGD(" ButtonClick NextButton !!!\n");
    if (mVideoView2Ptr->isVisible()) {
    	next();
    } else {
    	media::music_next(true);
    }
    return false;
}

static void onProgressChanged_PlaySeekBar(ZKSeekBar *pSeekBar, int progress) {
    //LOGD(" ProgressChanged PlaySeekBar %d !!!\n", progress);
}

static bool onButtonClick_Button6(ZKButton *pButton) {
    LOGD(" ButtonClick Button6 !!!\n");
	pic = (pic + 1) % 3;
	switch (pic) {
	case 0:
		media::music_set_play_mode(E_MEDIA_PLAY_MODE_CYCLE);
		break;
	case 1:
		media::music_set_play_mode(E_MEDIA_PLAY_MODE_SINGLE);
		break;
	case 2:
		media::music_set_play_mode(E_MEDIA_PLAY_MODE_RANDOM);
		break;
	default:
		break;
	}
	mTextView17Ptr->setText(modetext[pic]);
	pButton->setText(pictext[pic]);
    return false;
}

static bool onButtonClick_DisplayButton1(ZKButton *pButton) {
    LOGD(" ButtonClick DisplayButton1 !!!\n");
    if (mPlayWindowPtr->isVisible()) {
    	mPlayWindowPtr->setVisible(false);
    } else {
    	mPlayWindowPtr->setVisible(true);
    }
    return false;
}

static bool onButtonClick_Button4(ZKButton *pButton) {
    LOGD(" ButtonClick Button4 !!!\n");
    mAVFileListViewPtr->refreshListView();
    mPlayWindowPtr->hideWnd();
    mVideoTipWindowPtr->hideWnd();
    mVideoView2Ptr->stop();
    mActivityPtr->unregisterUserTimer(TimerVideoPlaying);
    mDisplayButton1Ptr->setVisible(false);
    mVideoView2Ptr->setPosition(small_video_position);
    return false;
}

static bool onButtonClick_Button5(ZKButton *pButton) {
    LOGD(" ButtonClick Button5 !!!\n");
    mAVFileListViewPtr->refreshListView();
	mPlayWindowPtr->hideWnd();
	mVideoTipWindowPtr->hideWnd();
	mVideoView2Ptr->stop();
	mActivityPtr->unregisterUserTimer(TimerVideoPlaying);
	mDisplayButton1Ptr->setVisible(false);
	mVideoView2Ptr->setPosition(small_video_position);
    return false;
}

static bool onButtonClick_PlayBigButton(ZKButton *pButton) {
    LOGD(" ButtonClick PlayBigButton !!!\n");
    if (mVideoView2Ptr->isPlaying()) {
    	mVideoView2Ptr->pause();
    	pButton->setSelected(false);
    	mVideoPlayButtonPtr->setSelected(false);
    	mStopButtonPtr->setSelected(false);
    } else {
    	mVideoView2Ptr->resume();
    	pButton->setSelected(true);
    	mVideoPlayButtonPtr->setSelected(true);
    	mStopButtonPtr->setSelected(true);
    }
    return false;
}

static bool onButtonClick_FullButton(ZKButton *pButton) {
    LOGD(" ButtonClick FullButton !!!\n");
    return false;
}

static void onProgressChanged_VideoSeekBar(ZKSeekBar *pSeekBar, int progress) {
    //LOGD(" ProgressChanged VideoSeekBar %d !!!\n", progress);
}

static bool onButtonClick_VideoProcButton(ZKButton *pButton) {
    LOGD(" ButtonClick VideoProcButton !!!\n");
    proc();
    return false;
}

static bool onButtonClick_VideoPlayButton(ZKButton *pButton) {
    LOGD(" ButtonClick VideoPlayButton !!!\n");
    if (mVideoView2Ptr->isPlaying()) {
    	mVideoView2Ptr->pause();
    	pButton->setSelected(false);
    	mPlayBigButtonPtr->setSelected(false);
    	mStopButtonPtr->setSelected(false);
    	mVideoPlayButtonPtr->setSelected(false);
    } else {
    	mVideoView2Ptr->resume();
    	pButton->setSelected(true);
    	mPlayBigButtonPtr->setSelected(true);
    	mVideoPlayButtonPtr->setSelected(true);
    	mStopButtonPtr->setSelected(true);
    }
    return false;
}

static bool onButtonClick_VideoNextButton(ZKButton *pButton) {
    LOGD(" ButtonClick VideoNextButton !!!\n");
    next();
    return false;
}

static bool onButtonClick_DisButton(ZKButton *pButton) {
    LOGD(" ButtonClick DisButton !!!\n");
    mPlayWindowPtr->setVisible(false);
    return false;
}

static bool onButtonClick_DisButton1(ZKButton *pButton) {
    LOGD(" ButtonClick DisButton1 !!!\n");
    mPlayWindowPtr->setVisible(false);
    return false;
}

static bool onButtonClick_VideoBackButton(ZKButton *pButton) {
    LOGD(" ButtonClick VideoBackButton !!!\n");
    mAVFileListViewPtr->refreshListView();
    mPlayWindowPtr->hideWnd();
//    mVideoView2Ptr->stop();
//    mActivityPtr->unregisterUserTimer(TimerVideoPlaying);
	mDisplayButton1Ptr->setVisible(false);
    mVideoView2Ptr->setPosition(small_video_position);
    mfullButtonPtr->setVisible(true);
    return false;
}
static bool onButtonClick_VideoButton(ZKButton *pButton) {
    LOGD(" ButtonClick VideoButton !!!\n");
    mVideoButtonPtr->setVisible(false);
    mAudioButtonPtr->setVisible(false);
    mTextView5Ptr->setVisible(false);
    mTextView6Ptr->setVisible(false);
    mAVFileListViewPtr->setVisible(true);
    mAVFileListViewPtr->refreshListView();
    flag = VideoListMode;
    return false;
}

static bool onButtonClick_AudioButton(ZKButton *pButton) {
    LOGD(" ButtonClick AudioButton !!!\n");
    mVideoButtonPtr->setVisible(false);
    mAudioButtonPtr->setVisible(false);
    mTextView5Ptr->setVisible(false);
    mTextView6Ptr->setVisible(false);
    mAVFileListViewPtr->setVisible(true);
    mAVFileListViewPtr->refreshListView();
    flag = MusicListMode;
    return false;
}
static bool onButtonClick_fullButton(ZKButton *pButton) {
    LOGD(" ButtonClick fullButton !!!\n");
    mVideoView2Ptr->setPosition(mWindow1Ptr->getPosition());
    mfullButtonPtr->setVisible(false);
    //mPlayWindowPtr->showWnd();
    mDisplayButton1Ptr->setVisible(true);
    return false;
}
