#include "filesystem.h"
#include <errno.h>
#include <unistd.h>
#include <linux/fs.h>
#include <sys/mount.h>
#include "utils/Log.h"
#include "newfs_msdos.h"
#include "base/strings.hpp"
#include <base/files.hpp>

#include <base/os.hpp>
#include <os/MountMonitor.h>
#include <mpi/common.h>
#include "edge/popup_service.h"
#include "mpi/case/recorder.h"
#include <system/setting.h>
#include "config.h"
#include "misc/utility.h"

namespace base {

namespace fat32 {

/**
 *  如果pDev不存在，将阻塞
 */
bool format_fat32fs(const char *block) {
    LOGD("format %s to fat32 fs", block);

    const char *args[13] = {0};

    args[0] = "zkrecovery";
    args[1] = "-F";
    args[2] = "32";
    args[3] = "-O";
    args[4] = "zkswe";
    args[5] = "-c";
    args[6] = "128";
    args[7] = "-b";
	args[8] = "65536";
	args[9] = block;

    newfs_msdos_main(10, args);
    return true;
}

std::string mount_vfat(const char* dev, const char* mount_point) {
  unsigned long flags = MS_NODEV | MS_NOSUID | MS_DIRSYNC;
  int ret = ::mount(dev, mount_point, "vfat", flags, NULL);
  if (ret) {
    return base::format("mount failed , error = %s", strerror(errno));
  }
  return "";
}

bool umount(const char *mount_point) {
    int ret = ::umount(mount_point);
    if (ret) {
        LOGE("umount errno : %s\n", strerror(errno));
    }
    return ret == 0;
}

bool testReadAndWrite(const std::string& dir) {
  auto file = base::Path::get(dir, "test_rw_file");
  const std::string content = "1234";
  base::write_all(file, content);
  bool ret = base::read_all(file) == content;
  base::remove(file.c_str());
  return ret;
}

void checkAndMount(const std::string& block, const std::string& mount_point) {
  if (MOUNTMONITOR->isMounted(mount_point.c_str())
      && testReadAndWrite(mount_point)) {
    return;
  }
  LOGE_TRACE("");
  if (!base::exists(block.c_str())) {
    throw mpi::Exception(-1, "tfcard_not_found");
  }
  LOGE_TRACE("");
  base::fat32::umount(mount_point.c_str());
  std::string err;
  for (int i = 0; i < 5; ++i) {
    LOGE_TRACE("");
    err = base::fat32::mount_vfat(block.c_str(),
        mount_point.c_str());
    if (err.empty() && testReadAndWrite(mount_point)) {
      //mount success
      return;
    }
    usleep(1000 * 1000);
  }
  LOGE_TRACE("");
  if (!base::fat32::format_fat32fs(block.c_str())) {
    throw mpi::Exception(-1, "tfcard_format_error");
  }
  LOGE_TRACE("");
  err = base::fat32::mount_vfat(block.c_str(),
      mount_point.c_str());
  if (err.empty() && testReadAndWrite(mount_point)) {
    //mount success
    return;
  }
  throw mpi::Exception(-1, "tfcard_mount_error");
}

int getBlockSize(const std::string& mount_point) {
	  base::StatFs s(mount_point.c_str());
	  return static_cast<int>(s.blockSize());
}

} /* namespace fat32 */

} /* namespace base */

void formatTfcardProcess(bool record) {
	PopupService::show([record](PopupService* srv){
    try {
      srv->setMessage("@formating_memory_card");
      if (!base::exists(TFCARD_BLOCK)) {
        throw mpi::Exception(-1, "tfcard_not_found");
      }
      mpi::Recorder::instance().stop();
      base::fat32::umount(TFCARD_MOUNT_POINT);
      if (!base::fat32::format_fat32fs(TFCARD_BLOCK)) {
        throw mpi::Exception(-1, "tfcard_format_error");
      }
      LOGE_TRACE("format start");
      base::fat32::checkAndMount(TFCARD_BLOCK, TFCARD_MOUNT_POINT);
      LOGE_TRACE("format end");
      srv->success("@tfcard_format_success", 1000);
      if (record) {
		  NO_EXCEPTION(restartRecorder(true));
	  }
      return 0;
    } catch (mpi::Exception& e) {
      LOGE_TRACE("%s", e.what());
      srv->setMessage(e.localizedMessage("error"));
      return -1;
    }
  });
}


