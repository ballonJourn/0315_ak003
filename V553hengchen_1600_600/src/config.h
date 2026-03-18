/*
 * config.h
 *
 *  Created on: 2023年4月9日
 *      Author: ZKSWE Develop Team
 */

#ifndef _CONFIG_H_
#define _CONFIG_H_

#define SCREEN_WIDTH         1600
#define SCREEN_HEIGHT        600

// 授权数据在otp中的偏移位置及长度
#define LINK_LIC_OFFSET      0
#define LINK_LIC_SIZE        32
#define BT_LIC_OFFSET        (LINK_LIC_OFFSET + LINK_LIC_SIZE)
#define BT_LIC_SIZE          220

// 手机互联相关配置
#define LINK_VIEW_WIDTH      SCREEN_WIDTH
#define LINK_VIEW_HEIGHT     SCREEN_HEIGHT
#define LINK_USB_ENABLE      0    // 1: 打开有线功能, 0: 关闭有线功能    备注: 打开有线功能,有线adb不能使用,只能用网络adb调试

#define LINK_MFI_SET_ENABLE  1    // 1: 指定mfi i2c配置, 0: 自动检测
#define LINK_MFI_I2C_N       3    // 指定mfi i2c号
#define LINK_MFI_I2C_ADDR    0    // 指定mfi i2c地址, 0: 0x10, 1: 0x11

#define LINK_SERVICE         "link"
#define LINK_BIN_SH          "/tmp/link.sh"

#define LINK_SCRIPT       	 "#!/bin/sh\n" \
							  "export LOCAL_IP_ADDRESS=" NET_STATIC_IP "\n" \
							 "export PRODUCT_NAME=\"%s\"\n" \
                             "/res/bin/lylinkapp -v=a\n"  // "/res/bin/lylinkapp -v=ic\n" 跑授权芯片   国外版本（android auto）

// 蓝牙相关配置
#define BT_MODULE_LYGOC      1    // 辽原
#define BT_MODULE_BLINK      2    // 诚谦
#define BT_MODULE_GOCSDK     3    // 顾凯		// 没有电话本，通话记录功能
#define BT_MODULE            BT_MODULE_LYGOC    // 选择使用哪一模块
#define BT_SERVICE           "bt"
#define BT_BIN_SH            "/tmp/bt.sh"

#if BT_MODULE == BT_MODULE_LYGOC
#define BT_UART_DEV          "/dev/ttyS3"       // 物理串口
#define BT_SERIAL            "/dev/goc_serial"
#define BT_BAUDRATE          B9600
#define BT_SCRIPT            "#!/bin/sh\n" \
                             "/res/bin/gocsdk -T" BT_UART_DEV "\n"
#elif BT_MODULE == BT_MODULE_BLINK
#define BT_UART_DEV          "/dev/BT_serial"
#define BT_BAUDRATE          B500000

#define BT_AUTH_CODE         "XXXXXXXXXXX"  	 // 公司授权码  (已改为出厂uuid授权,无需联网授权功能)ZCLNHSEWB7VGI6GE
#define BT_PHY_SERIAL        "/dev/ttyS3"        	 // 物理串口
#define BT_SCRIPT            "#!/bin/sh\n" \
                             "/res/bin/blink -c" BT_AUTH_CODE " -T" BT_PHY_SERIAL "\n"
#elif BT_MODULE == BT_MODULE_GOCSDK
#define BT_UART_DEV          "/dev/goc_serial"
#define BT_BAUDRATE          B500000
#define BT_SCRIPT            "#!/bin/sh\n" \
                             "/res/bin/gocsdk -T/dev/ttyS3 -B1500000\n"
#endif

#define MEDIA_ECHO_NS_ENABLE 1  // 是否开启回声降噪, mic录到的声音底噪声比较大情况可开启改善效果
#define MEDIA_ECHO_FAR_DELAY 0  // 回采和录音数据相位有较大偏移，通过这个进行调整，单位ms
#define MEDIA_SOUND_USE_MCU  1  // 声音通过mcu输出
#define MEDIA_SOUND_MAX_VOL  120  // 媒体限制最大音量
#define PHONE_SOUND_MAX_VOL  160  // 通话限制最大音量

// 网络相关配置
#define NET_STA_IFACE		 "wlan0"
#define NET_AP_IFACE		 "wlan0"
#define NET_P2P_IFACE		 "p2p0"

#define NET_STATIC_IP        "192.168.43.1"
#define NET_WV_STATIC_IP     "192.168.169.1"
#define NET_AP_SUPP			 "hostapd"
#define NET_AP_CTRL_DIR		 "/dev/socket/ap/"
#define NET_AP_CTRL_PATH	 NET_AP_CTRL_DIR NET_AP_IFACE

#define NET_P2P_SUPP		 "p2p_supplicant"
#define NET_P2P_CTRL_PATH	 "/dev/socket/" NET_P2P_IFACE

#define NET_COUNTRY_CODE     "CN"  // wifi驱动安装指定国家码      https://www.cnblogs.com/rougungun/p/14338586.html

#define NET_HOSTAPD_DIR          "/tmp/hostapd/"
#define NET_UDHCPD_DIR			 "/tmp/udhcpd/"
#define NET_HOSTAPD_CONF_FILE    NET_HOSTAPD_DIR "hostapd.conf"
#define NET_HOSTAPD_AP_CONF_FILE NET_HOSTAPD_DIR "hostapd_ap.conf"
#define NET_HOSTAPD_WV_CONF_FILE NET_HOSTAPD_DIR "hostapd_wv.conf"
#define NET_P2P_SUP_CONF_FILE    "/data/misc/wifi/p2p_supplicant.conf"
#define NET_UDHCPD_CONF_FILE     NET_UDHCPD_DIR "udhcpd.conf"			// "/data/udhcpd.conf"
#define NET_UDHCPD_WV_CONF_FILE  NET_UDHCPD_DIR "udhcpd_wv.conf"
#define NET_UDHCPD_LEASES_PATH   NET_UDHCPD_DIR "udhcpd.leases"

// miracast 分辨率配置文件
#define MC_RESOLUTIONS_CONF "/tmp/wfd.cfg"

//	"1,2";	1024*768
//	"2,8";	960*540
//	"0,5";	1280*720
#define MC_RESOLUTIONS_CONF_DATA "0,5"

// *.conf文件容易变成windows格式，行的结束符为\r\n，导致解析异常，改用代码动态去生成
// hostapd.conf
#define NET_HOSTAPD_CONF_DATA    "interface=wlan0\n"                  \
                                 "ctrl_interface=/dev/socket/ap\n"    \
                                 "ssid=%s\n"                          \
                                 "wpa_passphrase=12345678\n"          \
                                 "channel=36\n"                       \
                                 "auth_algs=3\n"                      \
                                 "dtim_period=1\n"                    \
                                 "wpa=2\n"                            \
                                 "driver=nl80211\n"                   \
                                 "ieee80211n=1\n"                     \
                                 "hw_mode=a\n"                        \
                                 "ignore_broadcast_ssid=0\n"          \
                                 "wpa_key_mgmt=WPA-PSK\n"             \
                                 "wpa_pairwise=CCMP\n"                \
                                 "rsn_pairwise=CCMP\n"                \
                                 "max_num_sta=8\n"                    \
                                 "wpa_group_rekey=86400\n"            \
                                 "ht_capab=[SHORT-GI-20][SHORT-GI-40][HT40+]\n"

// airplay需要加上vendor_elements=dd0411223301
#define NET_HOSTAPD_AP_CONF_DATA    NET_HOSTAPD_CONF_DATA "vendor_elements=dd0411223301\n"

// p2p_supplicant.conf
#define NET_P2P_SUP_CONF_DATA    "ctrl_interface=/dev/socket\n"       \
                                 "update_config=1\n"                  \
                                 "device_name=%s\n"                   \
                                 "device_type=1-0050F204-1\n"         \
                                 "p2p_no_group_iface=1\n"             \
                                 "p2p_no_go_freq=2412-2484\n"         \
                                 "p2p_go_max_inactivity=72000\n"      \
                                 "wowlan_triggers=any\n"              \
                                 "config_methods=display push_button keypad virtual_push_button physical_display\n"

// udhcpd.conf
#define NET_UDHCPD_CONF_DATA     "interface wlan0\n"                  \
                                 "start   192.168.43.50\n"            \
                                 "end     192.168.43.200\n"           \
                                 "\n"                                 \
                                 "lease_file /tmp/udhcpd.leases\n"  \
                                 "pidfile	/tmp/udhcpd.pid\n"   \
                                 "\n"                                 \
                                 "option  subnet  255.255.255.0\n"    \
                                 "option  domain  local\n"            \
                                 "option  lease   864000\n"

#define NET_UDHCPD_WV_CONF_DATA  "interface wlan0\n"                  \
                                 "start   192.168.169.50\n"            \
                                 "end     192.168.169.200\n"           \
                                 "\n"                                 \
                                 "lease_file /tmp/udhcpd.leases\n"  \
                                 "pidfile	/tmp/udhcpd.pid\n"   \
                                 "\n"                                 \
                                 "option  subnet  255.255.255.0\n"    \
                                 "option  domain  local\n"            \
                                 "option  lease   864000\n"


#define HOSTAPD_BIN_SH           "/tmp/hostapd.sh"
#define UDHCPD_BIN_SH            "/tmp/udhcpd.sh"
#define HOSTAPD_SCRIPT           "#!/bin/sh\n" \
                                 "/res/bin/hostapd " NET_HOSTAPD_CONF_FILE "\n"
#define HOSTAPD_AP_SCRIPT        "#!/bin/sh\n" \
                                 "/res/bin/hostapd " NET_HOSTAPD_AP_CONF_FILE "\n"
#define HOSTAPD_WV_SCRIPT        "#!/bin/sh\n" \
                                 "/res/bin/hostapd " NET_HOSTAPD_WV_CONF_FILE "\n"
#define UDHCPD_SCRIPT            "#!/bin/sh\n" \
								 "/res/bin/udhcpd "  NET_UDHCPD_CONF_FILE "\n"
#define UDHCPD_WV_SCRIPT         "#!/bin/sh\n" \
								 "/res/bin/udhcpd "  NET_UDHCPD_WV_CONF_FILE "\n"

// 倒车轨迹线
#define REVERSE_LINE_WIDTH       4    // 倒车线宽
#define REVERSE_LINE_CORNER_LEN  80
#define REVERSE_LINE_DEF_LTX     640  // 左上角x坐标
#define REVERSE_LINE_DEF_LTY     200  // 左上角y坐标
#define REVERSE_LINE_DEF_RTX     (SCREEN_WIDTH - REVERSE_LINE_DEF_LTX)  // 右上角x坐标
#define REVERSE_LINE_DEF_RTY     REVERSE_LINE_DEF_LTY  // 右上角y坐标
#define REVERSE_LINE_DEF_LBX     320  // 左下角x坐标
#define REVERSE_LINE_DEF_LBY     SCREEN_HEIGHT  // 左下角y坐标
#define REVERSE_LINE_DEF_RBX     (SCREEN_WIDTH - REVERSE_LINE_DEF_LBX) // 右下角x坐标
#define REVERSE_LINE_DEF_RBY     REVERSE_LINE_DEF_LBY  // 右下角y坐标
#define REVERSE_LINE_G_COLOR     0xFF00FF00  // green
#define REVERSE_LINE_Y_COLOR     0xFFFFFF00  // yellow
#define REVERSE_LINE_R_COLOR     0xFFFF0000  // red
#define REVERSE_LINE_G_RATIO     0.25
#define REVERSE_LINE_Y_RATIO     0.3
#define REVERSE_LINE_R_RATIO     0.45

#define APP_TYPE_SYS_CTRLBAR     100
#define APP_TYPE_SYS_FLOATWND    101

#define SHUTDOWN_PROP_KEY        "sys.shutdown.state"

#define BRIGHTNESS_DAY      	 50  // 白天模式的亮度
#define BRIGHTNESS_MIDDLE      	 30  // 中等亮度
#define BRIGHTNESS_NIGHT      	 10  // 夜晚模式的亮度
#define BRIGHTNESS_MAX_VAL       52  // 限制最大亮度

#define ADC_DAY_MODE_THRESHOLD         2800
#define ADC_NIGHT_MODE_THRESHOLD       2000

#define GET_ISP_COUNT	10			// 检测次数
#define NIGHT_MODE_THRESHOLD  	400
#define DAY_MODE_THRESHOLD  	800

const int ENABLE_AUTO_RECORD = 1;
#define TFCARD_BLOCK "/dev/block/mmcblk0"
#define TFCARD_MOUNT_POINT "/mnt/extsd"
#define PRODUCT_TEST_FILE_NAME "product_test.ini"


//ISP IQ
#define DEV_PATH_GC2093 "/sys/devices/gc2093_mipi"
#define DEV_PATH_GC4663 "/sys/devices/gc4663_mipi"
#define DEV_PATH_GC2083 "/sys/devices/gc2083_mipi"
#define ISP_PARAM_FILE "isp_param_config.bin"
#define ISP_PARAM_PATH "/tmp/isp/"
#define ISP_PARAM_GC2093 "/res/bin/isp_param_config_2093.bin"
#define ISP_PARAM_GC4663 "/res/bin/isp_param_config_4663.bin"
#define ISP_PARAM_GC2083 "/res/bin/isp_param_config_2083.bin"


#endif /* _CONFIG_H_ */
