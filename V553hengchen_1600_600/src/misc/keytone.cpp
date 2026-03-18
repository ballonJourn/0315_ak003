/*
 * keytone.cpp
 *
 *  Created on: 2024年4月2日
 *      Author: pengzc
 */

#include <misc/keytone.h>
#include <manager/ConfigManager.h>
#include "media/wav_utils.h"

Keytone::~Keytone() {
}

void Keytone::play(const std::string& wav) {
  handler_.post([wav](const void* user_data){
    zk_wav_play(wav.c_str());
  }, NULL);
}

void Keytone::click() {
  std::lock_guard<std::mutex> lock(mutex_);
  if (count_ >= 2) {
    return;
  }
  ++count_;
  handler_.post([](const void* user_data) {
    zk_wav_play(CONFIGMANAGER->getResFilePath("key.wav").c_str());
    std::lock_guard<std::mutex> lock(instance().mutex_);
    instance().count_ -= 1;
  }, NULL);
}


