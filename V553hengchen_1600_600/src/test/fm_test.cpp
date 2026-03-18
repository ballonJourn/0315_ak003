/*
 * fm_test.cpp
 *
 *  Created on: 2024年4月13日
 *      Author: pengzc
 */

#include <test/fm_test.h>
#include "logic/mode_observer.h"
#include <media/ZKMediaPlayer.h>
#include <manager/ConfigManager.h>
#include "loop_player.h"
#include <base/strings.hpp>
#include "system/fm_emit.h"

namespace test {

FmTest::FmTest() : TestItem("FM发射", NULL, false) {
}

bool FmTest::test(Testing* t) {
  const int freqMhz = 92;
  t->setMessage(base::format("已开启FM发射\n"
      "频率 %d MHz\n"
      "收听声音是否正常", freqMhz));
  mode::set_sound_mode(E_SOUND_MODE_FM);
  sys::fm_set_mute(false);
  sys::fm_set_freq(freqMhz * 100);

  LoopPlayer player(ZKMediaPlayer::E_MEDIA_TYPE_AUDIO);
  player.play(CONFIGMANAGER->getResFilePath("1kHz.wav").c_str());
  return t->await();
}

} /* namespace test */
