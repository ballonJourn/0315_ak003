/*
 * audio_recorder.cpp
 *
 *  Created on: 2023年2月6日
 *      Author: ZKSWE Develop Team
 */

#include "audio_context.h"
#include "audio_recorder.h"
#include "wav_utils.h"
#include "system/Thread.h"
#include <mpi/case/audio_source.h>
#include <mpi/case/config.h>
#include "media/webrtc_echo.h"


#define DEBUG_SAVE_WAV  0		// 生成调试音频
#define DEBUG_SAVE  1           //  改为1开启调试录音 

namespace audio {

class RecThread : public Thread {
public:
	RecThread() : mRate(16000), mCB(NULL) {

	}

	bool start(uint32_t rate, audio_record_cb cb) {
		mRate = rate;
		mCB = cb;
		return run("rec");
	}

	void stop() {
#if 1
		while(isRunning()){
			requestExit();
			zk_audio_record_wake();
			WAIT(!isRunning(),100,10);
		}
#else
		requestExitAndWait();
#endif
	}

protected:
	static void splitSample(const int16_t *src_data, uint32_t size, int16_t *near_data, int16_t *far_data) {
		uint32_t sample_count = size / 2;
		for (uint32_t i = 0; i < sample_count; ++i) {
			near_data[i] = *src_data++;
			far_data[i] = *src_data++;
		}
	}

  void record() {
    mpi::AudioSource as(AUDIO_IN_CHANNEL_TELEPHONE);
    as.channel().enable();
    as.device().setVolume(90);

#if DEBUG_SAVE
    LOGD("[AEC_DEBUG] 开始录音调试，文件将保存到 /mnt/extsd/");
	void *near_wav = zk_wav_init("/mnt/extsd/near.wav", 1, mRate);   // near (mic) - 麦克风原始采集
	void *far_wav = zk_wav_init("/mnt/extsd/far.wav", 1, mRate);     // far (loopback) - 回采参考信号
	void *echo_wav = zk_wav_init("/mnt/extsd/echo.wav", 1, mRate);   // AEC处理后的信号

	if (!near_wav || !far_wav || !echo_wav) {
		LOGE("[AEC_DEBUG] WAV文件创建失败！请检查SD卡是否挂载");
	} else {
		LOGD("[AEC_DEBUG] WAV文件创建成功");
	}

	const uint32_t kSampleSize = 960 * 2;
	uint8_t *recv_data, *near_data, *far_data, *echo_data;

#endif
    void *echo_handle = webrtc_echo_create(mRate);
    if (!echo_handle) {
        LOGE("webrtc echo create fail!\n");
    }
    const int FRAME_SAMPLE_NUMBER = 960;
    short out_frame[FRAME_SAMPLE_NUMBER];

    while (!exitPending()) {
      try {
        auto frame = as.getFrame(1000);
        const AUDIO_FRAME_S& nearend_frame = frame;
        const AEC_FRAME_S& farend_frame = frame;

        if (!mCB) {
          continue;
        }

        if (farend_frame.stRefFrame.mLen != FRAME_SAMPLE_NUMBER * 2) {
          LOGE("invalid frame length: length = %d",
              farend_frame.stRefFrame.mLen);
          continue;
        }
        webrtc_echo_process(echo_handle, (const int16_t *)nearend_frame.mpAddr, (const int16_t *)farend_frame.stRefFrame.mpAddr,
                FRAME_SAMPLE_NUMBER, (int16_t *) out_frame);
        uint8_t *data = (uint8_t *)out_frame;
        // mpi::AudioSource::multiply(out_frame, sizeof(out_frame)/sizeof(short), 16);
        // LOGD("[record] recording  !!!");
#if DEBUG_SAVE
        near_data = (uint8_t *)nearend_frame.mpAddr;
        far_data = (uint8_t *)farend_frame.stRefFrame.mpAddr;
        echo_data = (uint8_t *)out_frame;
        zk_wav_add_data(near_wav, near_data, kSampleSize);
        zk_wav_add_data(far_wav, far_data, kSampleSize);
        zk_wav_add_data(echo_wav, echo_data, kSampleSize);
#endif
        mCB(out_frame, sizeof(out_frame));
      } catch (mpi::Exception& e) {
        LOGE_TRACE("%s", e.what());
      }
    }
    webrtc_echo_destroy(echo_handle);
#if DEBUG_SAVE
    LOGD("[AEC_DEBUG] 正在保存WAV文件...");
    zk_wav_deinit(near_wav);
	zk_wav_deinit(far_wav);
	zk_wav_deinit(echo_wav);
	LOGD("[AEC_DEBUG] WAV文件保存完成！");
	LOGD("[AEC_DEBUG] near.wav  - 麦克风原始采集（包含回声）");
	LOGD("[AEC_DEBUG] far.wav   - 回采参考信号（扬声器播放的内容）");
	LOGD("[AEC_DEBUG] echo.wav  - AEC处理后的信号（应该没有回声）");
#endif
    //as.device().setVolume(100);
  }

  virtual bool threadLoop() {
    LOGD("rec thread start...\n");
#if DEBUG_SAVE_WAV
		void *recv_wav = zk_wav_init("/mnt/extsd/recv.wav", 2, mRate);
		void *near_wav = zk_wav_init("/mnt/extsd/near.wav", 1, mRate);   // near (mic)
		void *far_wav = zk_wav_init("/mnt/extsd/far.wav", 1, mRate);     // far (loopback)
		void *echo_wav = zk_wav_init("/mnt/extsd/echo.wav", 1, mRate);
#else
#endif

      while (!exitPending()) {
        try {
          record();
        } catch (std::exception& e) {
          LOGE_TRACE("%s", e.what());
          Thread::sleep(3000);
        }
      }
    LOGD("rec thread end...\n");
    return false;
  }

    virtual bool threadLoop2() {
#if 0
        LOGD("rec thread start...\n");

        const uint32_t kSampleSize = 1280;
        const uint32_t kRecvSize = kSampleSize * 2;
        uint8_t *recv_data, *near_data, *far_data, *echo_data;

        recv_data = (uint8_t *) malloc(kRecvSize);
        near_data = (uint8_t *) malloc(kSampleSize);
        far_data = (uint8_t *) malloc(kSampleSize);
        echo_data = (uint8_t *) malloc(kSampleSize);

#if DEBUG_SAVE_WAV
        void *recv_wav = zk_wav_init("/mnt/extsd/recv.wav", 2, mRate);
        void *near_wav = zk_wav_init("/mnt/extsd/near.wav", 1, mRate);   // near (mic)
        void *far_wav = zk_wav_init("/mnt/extsd/far.wav", 1, mRate);     // far (loopback)
        void *echo_wav = zk_wav_init("/mnt/extsd/echo.wav", 1, mRate);
#else
#endif

        void *echo_handle = webrtc_echo_create(mRate);
        if (!echo_handle) {
            LOGE("webrtc echo create fail!\n");
        }

        zk_audio_record_init(E_AUDIO_RECORD_TYPE_MIC, 2, mRate, 1024, 4);

        while (!exitPending()) {
            if (zk_audio_record_get_frame(recv_data, kRecvSize) != 0) {
                LOGE("get frame err !!!\n");
                break;
            }

            // 分离左右声道数据
            splitSample((int16_t *) recv_data, kSampleSize, (int16_t *) near_data, (int16_t *) far_data);

#if DEBUG_SAVE_WAV
            zk_wav_add_data(recv_wav, recv_data, kRecvSize);
            zk_wav_add_data(near_wav, near_data, kSampleSize);
            zk_wav_add_data(far_wav, far_data, kSampleSize);
#endif

            if (!echo_handle) {
                if (mCB) {
                    mCB(near_data, kSampleSize);
                }
                continue;
            }
            webrtc_echo_process(echo_handle, (const int16_t *) near_data, (const int16_t *) far_data, kSampleSize / 2, (int16_t *) echo_data);
            if (mCB) {
                mCB(echo_data, kSampleSize);
            }
#if DEBUG_SAVE_WAV
            zk_wav_add_data(echo_wav, echo_data, kSampleSize);
#endif
        }

        zk_audio_record_deinit();
        webrtc_echo_destroy(echo_handle);

        free(recv_data);
        free(near_data);
        free(far_data);
        free(echo_data);

#if DEBUG_SAVE_WAV
        zk_wav_deinit(recv_wav);
        zk_wav_deinit(near_wav);
        zk_wav_deinit(far_wav);
        zk_wav_deinit(echo_wav);
#endif

        LOGD("rec thread end...\n");
#endif
        return false;
    }

private:
	uint32_t mRate;
	audio_record_cb mCB;
};

static RecThread _s_rec_thread;

bool record_start(uint32_t rate, audio_record_cb cb) {
	return _s_rec_thread.start(rate, cb);
}

void record_stop() {
	_s_rec_thread.stop();
}

}
