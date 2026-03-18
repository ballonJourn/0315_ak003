/*
 * audio_test.cpp
 *
 *  Created on: 2024年4月12日
 *      Author: pengzc
 */

#include <test/audio_test.h>
#include <mpi/case/audio_source.h>
#include <mpi/case/config.h>
#include <fstream>
#include <base/files.hpp>
#include <thread>
#include "media/wav_utils.h"
#include "manager/ConfigManager.h"
#include "media/audio_context.h"
#include "logic/mode_observer.h"

namespace test {

static void pcmDataAddWaveHeader(const std::string& file, int channel,
    int sample_rate, int sample_bit = 16) {
  struct WaveHeader {
    int riff_id;
    int riff_sz;
    int riff_fmt;
    int fmt_id;
    int fmt_sz;
    short audio_fmt;
    short num_chn;
    int sample_rate;
    int byte_rate;
    short block_align;
    short bits_per_sample;
    int data_id;
    int data_sz;
  } header;
  int nDstChnCnt = channel;
  memcpy(&header.riff_id, "RIFF", 4);

  int size = base::file_size(file);

  header.riff_sz = size + sizeof(struct WaveHeader) - 8;
  memcpy(&header.riff_fmt, "WAVE", 4);
  memcpy(&header.fmt_id, "fmt ", 4);
  header.fmt_sz = 16;
  header.audio_fmt = 1;       // s16le
  header.num_chn = nDstChnCnt;
  header.sample_rate = sample_rate;
  header.byte_rate = sample_rate * nDstChnCnt * sample_bit / 8;
  header.block_align = nDstChnCnt * sample_bit / 8;
  header.bits_per_sample = sample_bit;
  memcpy(&header.data_id, "data", 4);
  header.data_sz = size;

  FILE* fp = fopen(file.c_str(), "rb+");
  if (fp == NULL) {
    throw std::invalid_argument("打开文件失败");
  }
  fseek(fp, 0, SEEK_SET);
  fwrite(&header, 1, sizeof(struct WaveHeader), fp);
  fclose(fp);
}

bool AudioTest::test(Testing* t) {
  const std::string ai_file = "/tmp/test_ai.wav";
  bool recording = true;
  mode::set_sound_mode(E_SOUND_MODE_SPK);
  audio::set_system_vol(0.8, true, false);
  t->setMessage("您将听到两遍音频"
      "\n第一遍是原始音频"
      "\n第二遍是录音音频"
      "\n检查声音是否正常");
  std::thread record_thread([ai_file, &recording]() {
    try {
      //录音
      std::ofstream ofs(ai_file, std::ios::out | std::ios::binary);
      mpi::AudioSource as(AUDIO_IN_CHANNEL_TELEPHONE);
      as.channel().enable();
      while (recording) {
        auto frame = as.getFrame(200);
        const AUDIO_FRAME_S& nearend_frame = frame;
        ofs.write((char*) nearend_frame.mpAddr, nearend_frame.mLen);
      }
      ofs.flush();
      ofs.close();
      pcmDataAddWaveHeader(ai_file, 1, 16000, 16);
    } catch (mpi::Exception& e) {
      LOGE_TRACE("%s", e.what());
    }
  });
  //播放
  zk_wav_play(CONFIGMANAGER->getResFilePath("goodmorning.wav").c_str());
  recording = false;
  record_thread.join();
  zk_wav_play(ai_file.c_str());
  //再播放录音
  return t->await();
}

} /* namespace test */
