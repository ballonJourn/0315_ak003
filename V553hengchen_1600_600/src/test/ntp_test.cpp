/*
 * ntp_test.cpp
 *
 *  Created on: 2024年4月13日
 *      Author: pengzc
 */

#include <test/ntp_test.h>
#include <ntp/ntp.h>
#include <net/NetManager.h>

namespace test {

NtpTest::NtpTest() : TestItem("网络时间", NULL, true) {

}

bool NtpTest::test(Testing* t) {
  t->setMessage("等待同步时间");

  auto connected = []() {
//    LOGE("is connected %d", NETMANAGER->getWifiManager()->isConnected());
    return NETMANAGER->getWifiManager()->getConnectionInfo() != NULL;
  };

  if (!awaitAndCountdown(t, "等待网络连接",
      connected, 30 * 1000, 1000)) {
    throw std::logic_error("WIFI未连接");
  }

  if (!ntp::syncTime(ntp::defaultServerList(), 3000)) {
    throw std::logic_error("同步时间失败");
    return false;
  }
  return true;
}

} /* namespace test */
