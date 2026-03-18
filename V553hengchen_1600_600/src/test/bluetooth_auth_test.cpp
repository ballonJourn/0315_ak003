/*
 * bluetooth_auth_test.cpp
 *
 *  Created on: 2024年4月13日
 *      Author: pengzc
 */

#include "bluetooth_auth_test.h"
#include "bt/context.h"
#include <net/NetManager.h>
#include <base/strings.hpp>
#include <utils/Log.h>

namespace test {

BluetoothAuthTest::BluetoothAuthTest(): TestItem("蓝牙授权", NULL, true) {

}

bool BluetoothAuthTest::test(Testing* t) {
  //该设备搭载的蓝牙自身联网授权
  auto conf = t->getConfig();

  auto connected = []() {
//    LOGE("is connected %d", NETMANAGER->getWifiManager()->isConnected());
    return NETMANAGER->getWifiManager()->getConnectionInfo() != NULL;
  };

  if (!awaitAndCountdown(t, base::format("WIFI名称：%s\n"
      "WIFI密码：%s\n"
      "等待WIFI连接", conf.wifi.name.c_str(), conf.wifi.password.c_str()),
      connected, 30 * 1000, 1000)) {
    throw std::logic_error("WIFI未连接");
  }

  auto auth = []() {
    return bt::get_is_authorized();
  };
  if (!awaitAndCountdown(t, "等待蓝牙授权", auth, 30 * 1000)) {
    throw std::logic_error("蓝牙未授权");
  }
  return true;
}

} /* namespace test */
