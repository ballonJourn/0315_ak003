/*
 * aux_test.cpp
 *
 *  Created on: 2024年4月13日
 *      Author: pengzc
 */

#include <test/aux_test.h>
#include "loop_player.h"
#include "logic/mode_observer.h"
#include <manager/ConfigManager.h>

namespace test {

AuxTest::AuxTest() : TestItem("Aux", NULL, false) {
}

bool AuxTest::test(Testing* t) {
  t->setMessage("已开启AUX\n"
      "检查声音是否正常");
  mode::set_sound_mode(E_SOUND_MODE_AUX);

  LoopPlayer player(ZKMediaPlayer::E_MEDIA_TYPE_AUDIO);
  player.play(CONFIGMANAGER->getResFilePath("1kHz.wav").c_str());
  return t->await();
}

} /* namespace test */
