/*
 * app_server_impl.cpp
 *
 *  Created on: 2024年4月29日
 *      Author: pengzc
 */

#include <misc/app_server_impl.h>
#include <base/files.hpp>
#include <manager/ConfigManager.h>
#include <base/strings.hpp>
#include <mpi/case/app_server.h>
#include <mpi/case/real_time_streaming_protocol_server.h>
#include <Json/value.h>
#include <net/NetUtils.h>
#include "config.h"
#include "version.h"
#include "system/setting.h"
#include "system/reverse.h"
#include "misc/utility.h"
#include <app/Activity.h>
#include "edge/fat32/filesystem.h"
#include <base/ui_handler.h>
#include <base/time.hpp>
#include <mpi/case/snapshot.h>

#define RTSP_CH0    "rtsp://" NET_WV_STATIC_IP ":8554/ch0"
#define RTSP_CH1    "rtsp://" NET_WV_STATIC_IP ":8554/ch1"
#define TCP_PORT    8555    // tcp socket server监听端口，主动推送消息，比如插拔卡、后路插拔、录像变化等等事件

static void reply(mpi::HTTPContext& ctx, const Json::Value& info) {
  ctx.response().setContentType("application/json");
  Json::Value json;
  json["result"] = 0;
  json["info"] = info;
  ctx.response().send(json.toStyledString());
}

inline Json::Value paramItem(const std::string& name,
    const std::vector<std::string>& labels) {
  Json::Value obj;
  obj["name"] = name;
  {
    Json::Value items;
    for (auto label : labels) {
      items.append(label);
    }
    obj["items"] = items;
  }
  {
    Json::Value indexes;
    for (int i = 0; i < (int)labels.size(); ++i) {
      indexes.append(i);
    }
    obj["index"] = indexes;
  }
  return obj;
}

inline Json::Value paramValue(const std::string& name, int value) {
  Json::Value obj;
  obj["name"] = name;
  obj["value"] = value;
  return obj;
}

void startAppServer() {
  auto& srv = mpi::AppServer::instance();
  if (srv.isStarted()) {
    return;
  }
  srv.clear();
  srv.GET("/app/getdeviceattr", [](mpi::HTTPContext& ctx) {
    Json::Value info;
    auto mac = base::replace(NetUtils::getMacAddr("wlan0"), ":","");

    info["uuid"] = base::replace(NetUtils::getMacAddr("wlan0"), ":","");  // wifi mac   去掉':'
    auto dt = base::buildDateTime();
    info["softver"] = sys::setting::get_app_softver();
    info["hwver"] = "v1.0";
    info["ssid"] = sys::setting::get_wifivideo_ap_name();
    info["bssid"] = mac;
    info["camnum"] = sys::has_reverse_camera() ? 2 : 1;
    info["curcamid"] = 0;
    reply(ctx, info);
  });
  srv.GET("/app/getmediainfo", [](mpi::HTTPContext& ctx) {
    Json::Value info;
    info["rtsp"] = RTSP_CH0;
    info["transport"] = "tcp";
    info["port"] = TCP_PORT;

    if (sys::has_reverse_camera()) {
      Json::Value rtsps;
      rtsps.append(RTSP_CH0);
      rtsps.append(RTSP_CH1);
      info["rtsps"] = rtsps;
    }
    reply(ctx, info);
  });
  srv.GET("/app/getproductinfo", [](mpi::HTTPContext& ctx) {
    Json::Value info;
    info["model"] = "";
    info["company"] = "HC";
    info["soc"] = "allwinner";
    info["sp"] = "zkswe";
    reply(ctx, info);
  });
  srv.GET("/app/playback", [](mpi::HTTPContext& ctx) {
    auto& srv = mpi::RealTimeStreamingProtocolServer::instance();
    auto param = ctx.params().get("param", "");
    if (param == "enter") {
      srv.stop();
      reply(ctx, "success");
    }
    if (param == "exit") {
      srv.stop();
      srv.start({srv.view(), false});
      reply(ctx, "success");
    }
  });
  srv.onAppGetParams([](mpi::HTTPContext& ctx) {
    Json::Value info;
    info.append(paramItem("mic", {"off", "on"}));
    if (sys::setting::is_resolution_show_4K()) {
    	// info.append(paramItem("rec_resolution", {"1080P", "2K", "4K"}));
        info.append(paramItem("rec_resolution", {"1080P", "2.5K", "4K"}));
    } else {
    	info.append(paramItem("rec_resolution", {"1080P", "2.5K", "4K"}));
    }
    info.append(paramItem("rec_split_duration", {"1 min", "3 min", "5 min"}));
    info.append(paramItem("rear_mirror", {"off", "on"}));
    info.append(paramItem("key_tone", {"off", "on"}));
    reply(ctx, info);
  });
  srv.onAppSetParamValue([](mpi::HTTPContext& ctx) {
    auto param = ctx.params().get("param", "");
    if (param == "switchcam") {
      mpi::View view = (mpi::View)ctx.params().getInt("value", 0);
      auto& srv = mpi::RealTimeStreamingProtocolServer::instance();
      if (srv.isStarted() && srv.view() == view) {
        return;
      }
      srv.stop();
      bool rear_mirror = sys::setting::is_rear_mirror();
      mpi::RealTimeStreamingProtocolServerParams params;
      params.video_device = view == mpi::VIEW_FRONT ?
              mpi::VIDEO_DEVICE_FRONT : mpi::VIDEO_DEVICE_REAR;
      params.mirror = view == mpi::VIEW_FRONT ? false : rear_mirror;
      srv.start(params);
      if (strcmp("DvrActivity", EASYUICONTEXT->currentAppName()) == 0) {
    	  base::runInUiThread("setup camera", [view]() {
    		  ViewType view_type = (view == mpi::VIEW_FRONT) ? VIEW_TYPE_FRONT : VIEW_TYPE_REAR;
    		  NO_EXCEPTION(setupCamera(view_type, true));
    	  });
      }
      reply(ctx, "set success");
      return;
    }
    if (param == "rec") {
      int start = ctx.params().getInt("value", 0);
      if (start) {
        NO_EXCEPTION(restartRecorder(false));
      } else {
        mpi::Recorder::instance().stop();
      }
      reply(ctx, "set success");
      return;
    }
    if (param == "mic") {
       auto en = ctx.params().getInt("value", 0);
      sys::setting::set_dvr_record_sound_enable(en);
      NO_EXCEPTION(restartRecorder(true));
      reply(ctx, "set success");
      return;
    }
    if (param == "rec_resolution") {
      auto index = ctx.params().getInt("value", 0);
      sys::setting::set_resolution_mode((resolution_mode_e)index);
      NO_EXCEPTION(restartRecorder(true));
      reply(ctx, "set success");
      return;
    }
    if (param == "rec_split_duration") {
      auto index = ctx.params().getInt("value", 0);
      sys::setting::set_dvr_record_time(index + 1);
      NO_EXCEPTION(restartRecorder(true));
      reply(ctx, "set success");
      return;
    }
    if (param == "key_tone") {
      auto en = ctx.params().getInt("value", 0);
      sys::setting::set_touch_sound_enable(en);
      reply(ctx, "set success");
      return;
    }
    if (param == "rear_mirror") {
      auto en = ctx.params().getInt("value", 0);
//      mpi::View view = (mpi::View)ctx.params().getInt("value", 0);
      sys::setting::set_rear_mirror(en);
      auto& srv = mpi::RealTimeStreamingProtocolServer::instance();
      auto view = srv.view();
      //判断当前是前摄还是后摄，后摄的情况进行设置
      if(view == mpi::VIEW_REAR){
          srv.setMirror(en ? true : false);
      }
      if (strcmp("DvrActivity", EASYUICONTEXT->currentAppName()) == 0) {
    	  base::runInUiThread("setup camera", [view]() {
          // ViewType view_type = VIEW_TYPE_REAR;
    		  ViewType view_type = (view == mpi::VIEW_FRONT) ? VIEW_TYPE_FRONT : VIEW_TYPE_REAR;
    		  NO_EXCEPTION(setupCamera(view_type, true));
    	  });
      }
      reply(ctx, "set success");
      return;
    }
  });

  srv.onAppGetParamValue([](mpi::HTTPContext& ctx){
    auto param = ctx.params().get("param", "");
    if (param == "all") {
      Json::Value info;
      info.append(paramValue(
          "mic", sys::setting::is_dvr_record_sound_enabled()));
      info.append(paramValue(
          "rec_resolution",(int)sys::setting::get_resolution_mode()));
      info.append(paramValue(
          "rec_split_duration",(int)sys::setting::get_dvr_record_time() - 1));
      info.append(paramValue(
          "rear_mirror", sys::setting::is_rear_mirror()));
      info.append(paramValue(
          "key_tone", sys::setting::is_touch_sound_enabled()));
      reply(ctx, info);
      return;
    }
    if (param == "rec") {
      Json::Value info;
      info["value"] = mpi::Recorder::instance().isStarted() ? 1 : 0;
      reply(ctx, info);
      return;
    }
  });

  srv.onAppSnapshot([]() {
	std::vector<mpi::VideoDeviceName> names;
	names.push_back(mpi::VIDEO_DEVICE_FRONT);
	if(sys::has_reverse_camera()) {
		names.push_back(mpi::VIDEO_DEVICE_REAR);
	}
	NO_EXCEPTION(mpi::Snapshot::instance().takePicture(names, {}));
//    NO_EXCEPTION(mpi::Recorder::instance().takePicture(sys::has_reverse_camera()));
  });
  srv.onAppStorageFormat([](){
    formatTfcardProcess(true);
  });
  srv.onAppReset([](){
    resetAllProcess();
  });

  auto& rtsp = mpi::RealTimeStreamingProtocolServer::instance();
  rtsp.setNewClientListener([]() {
    if (!mpi::AppServer::instance().isStarted()) {
      return;
    }
    Intent* intent = new Intent();
    intent->putExtra(INTENT_ENTER_WIFI_VIDEO, "true");
    EASYUICONTEXT->openActivity("DvrActivity", intent);
  });

  srv.start(80);
  mpi::RealTimeStreamingProtocolServerParams params;
  params.video_device = mpi::VIDEO_DEVICE_FRONT;
  rtsp.start(params);
}

void stopAppServer() {
  mpi::AppServer::instance().stop();
  mpi::RealTimeStreamingProtocolServer::instance().stop();
}

bool isAppServerRunning() {
  return mpi::AppServer::instance().isStarted();
}
