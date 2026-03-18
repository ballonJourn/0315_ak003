/*
 * ap.cpp
 *
 *  Created on: 2023年3月8日
 *      Author: ZKSWE Develop Team
 */

#include "fy/files.hpp"
#include "system/setting.h"
#include "private.h"
#include "ap.h"

namespace net {
namespace ap {

static void _make_hostapd_script() {
	remove(HOSTAPD_BIN_SH);

//	if (sys::setting::get_link_mode() == E_LINK_MODE_AIRPLAY) {
//		LOGD("[net] ap make %s\n%s\n", HOSTAPD_BIN_SH, HOSTAPD_AP_SCRIPT);
//		fy::files::create_script(HOSTAPD_BIN_SH, HOSTAPD_AP_SCRIPT);
//	} else if (sys::setting::get_link_mode() == E_LINK_MODE_WIFIVIDEO) {
//		LOGD("[net] ap make %s\n%s\n", HOSTAPD_BIN_SH, HOSTAPD_WV_SCRIPT);
//		fy::files::create_script(HOSTAPD_BIN_SH, HOSTAPD_WV_SCRIPT);
//	} else {
//		LOGD("[net] ap make %s\n%s\n", HOSTAPD_BIN_SH, HOSTAPD_SCRIPT);
//		fy::files::create_script(HOSTAPD_BIN_SH, HOSTAPD_SCRIPT);
//	}

	if (sys::setting::get_link_mode() == E_LINK_MODE_WIFIVIDEO) {
		LOGD("[net] ap make %s\n%s\n", HOSTAPD_BIN_SH, HOSTAPD_WV_SCRIPT);
		fy::files::create_script(HOSTAPD_BIN_SH, HOSTAPD_WV_SCRIPT);
	} else if (sys::setting::get_link_mode() == E_LINK_MODE_AIRPLAY) {
		LOGD("[net] ap make %s\n%s\n", HOSTAPD_BIN_SH, HOSTAPD_AP_SCRIPT);
		fy::files::create_script(HOSTAPD_BIN_SH, HOSTAPD_AP_SCRIPT);
	} else {
		LOGD("[net] ap make %s\n%s\n", HOSTAPD_BIN_SH, HOSTAPD_SCRIPT);
		fy::files::create_script(HOSTAPD_BIN_SH, HOSTAPD_SCRIPT);
	}
}

#if 1
static void _make_udhcpd_script() {
	remove(UDHCPD_BIN_SH);
	if (sys::setting::get_link_mode() == E_LINK_MODE_WIFIVIDEO) {
		LOGD("[net] ap make %s\n%s\n", UDHCPD_BIN_SH, UDHCPD_WV_SCRIPT);
		fy::files::create_script(UDHCPD_BIN_SH, UDHCPD_WV_SCRIPT);
	} else {
		LOGD("[net] ap make %s\n%s\n", UDHCPD_BIN_SH, UDHCPD_SCRIPT);
		fy::files::create_script(UDHCPD_BIN_SH, UDHCPD_SCRIPT);
	}
}
#endif

static bool _start_hostapd() {
	fy::net::iface ifc(NET_AP_IFACE);
//	RETURN_VALUE_IF_FAIL(ifc.up(), false);
//	RETURN_VALUE_IF_FAIL(ifc.set_addr(NET_STATIC_IP), false);
//	RETURN_VALUE_IF_FAIL(ifc.set_mask("255.255.255.0"), false);
	RETURN_VALUE_IF_FAIL(ifc.up(), false);
	if (sys::setting::get_link_mode() == E_LINK_MODE_WIFIVIDEO) {
		RETURN_VALUE_IF_FAIL(ifc.set_addr(NET_WV_STATIC_IP), false);
	} else {
		RETURN_VALUE_IF_FAIL(ifc.set_addr(NET_STATIC_IP), false);
	}
	RETURN_VALUE_IF_FAIL(ifc.set_mask("255.255.255.0"), false);

	_make_hostapd_script();
	_make_udhcpd_script();
	return _start_service(NET_AP_SUPP);
}

static void _stop_hostapd() {
#if 0
	if (sys::setting::get_link_mode() == E_LINK_MODE_AIRPLAY) {
		fy::kill_process(NET_AP_SUPP);
		DELAY(2000);
		return;
	}
#endif
	fy::net::iface::down(NET_AP_IFACE);
	_stop_service(NET_AP_SUPP);
	remove(NET_AP_CTRL_PATH);
}

bool enable() {
	LOGD("[net] ap %s\n", __FUNCTION__);
	RETURN_VALUE_IF_FAIL(wifi_load_driver() == 0, false);

	RETURN_VALUE_IF_FAIL(_start_hostapd(), false);

	_update_udhcpd_conf(NET_AP_IFACE);
	RETURN_VALUE_IF_FAIL(_start_service("udhcpd"), false);

	return true;
}

bool disable() {
	LOGD("[net] ap %s\n", __FUNCTION__);
	_stop_hostapd();
	_stop_service("udhcpd");
	wifi_unload_driver();
	return true;
}

bool restart_hostapd() {
	LOGD("[net] ap %s\n", __FUNCTION__);
	_stop_hostapd();
	return _start_hostapd();
}

}
}
