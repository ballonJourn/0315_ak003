/*
 * loop_player.cpp
 *
 *  Created on: 2024年4月13日
 *      Author: pengzc
 */

#include "loop_player.h"
#include <utils/Log.h>

namespace test {

LoopPlayer::LoopPlayer(ZKMediaPlayer::EMediaType type): player_(type) {
  player_.setPlayerMessageListener(this);
}

LoopPlayer::~LoopPlayer() {
  player_.setPlayerMessageListener(NULL);
}

void LoopPlayer::play(const std::string& file) {
  file_ = file;
  player_.play(file.c_str());
}

void LoopPlayer::onPlayerMessage(ZKMediaPlayer *pMediaPlayer, int msg,
    void *pMsgData) {
  switch (msg) {
    case ZKMediaPlayer::E_MSGTYPE_PLAY_STARTED:
      break;
    case ZKMediaPlayer::E_MSGTYPE_PLAY_COMPLETED:
      player_.play(file_.c_str());
      break;
    case ZKMediaPlayer::E_MSGTYPE_ERROR_MEDIA_ERROR:
      LOGE("onPlayerMessage E_MSGTYPE_ERROR_MEDIA_ERROR");
      break;
    default:
      break;
  }
}

} /* namespace test */
