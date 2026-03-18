/*
 * keytone.h
 *
 *  Created on: 2024年4月2日
 *      Author: pengzc
 */

#ifndef MISC_KEYTONE_H_
#define MISC_KEYTONE_H_

#include <base/handler.hpp>
#include <mpi/common.h>
#include <mutex>

class Keytone {
public:
  virtual ~Keytone();

  DISALLOW_COPY_AND_ASSIGN_METHOD(Keytone);
  DECLARE_SINGLETON_PATTERN(Keytone);

public:
  void click();
  void play(const std::string& wav);

private:
  std::mutex mutex_;
  int count_ = 0;
  base::handler<std::string> handler_;
};

#endif /* MISC_KEYTONE_H_ */
