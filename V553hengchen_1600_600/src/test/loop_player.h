/*
 * loop_player.h
 *
 *  Created on: 2024年4月13日
 *      Author: pengzc
 */

#ifndef TEST_LOOP_PLAYER_H_
#define TEST_LOOP_PLAYER_H_

#include <string>
#include <media/ZKMediaPlayer.h>

namespace test {

class LoopPlayer : protected ZKMediaPlayer::IPlayerMessageListener {
public:
  LoopPlayer(ZKMediaPlayer::EMediaType type);
  virtual ~LoopPlayer();

  void play(const std::string& file);

protected:
  virtual void onPlayerMessage(ZKMediaPlayer *pMediaPlayer, int msg,
      void *pMsgData);
private:
  ZKMediaPlayer player_;
  std::string file_;
};

} /* namespace test */

#endif /* TEST_LOOP_PLAYER_H_ */
