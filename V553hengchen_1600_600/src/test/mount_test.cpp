/*
 * mount_test_item.cpp
 *
 *  Created on: 2024年4月11日
 *      Author: pengzc
 */

#include <os/MountMonitor.h>
#include <test/mount_test.h>
#include <base/files.hpp>
#include "config.h"

namespace test {

MountTest::MountTest() : TestItem("TF卡", NULL, false) {

}

bool MountTest::test(Testing* t) {
  std::string mount_point = TFCARD_MOUNT_POINT;
  if (!MOUNTMONITOR->isMounted(mount_point.c_str())) {
    throw std::logic_error("未挂载TF卡");
  }
  const std::string path = mount_point + "/write_test.txt";
  const std::string content = "123456789";
  if (!base::write_all(path, content)) {
    throw std::logic_error("写文件失败");
  }
  if (base::read_all(path) != content) {
    throw std::logic_error("文件不一致");
  }
  base::remove(path.c_str());
  return true;
}

} /* namespace test */
