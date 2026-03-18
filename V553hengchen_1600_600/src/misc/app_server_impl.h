/*
 * app_server_impl.h
 *
 *  Created on: 2024年4月29日
 *      Author: pengzc
 */

#ifndef MISC_APP_SERVER_IMPL_H_
#define MISC_APP_SERVER_IMPL_H_

#include <mpi/case/real_time_streaming_protocol_server.h>
#include <mpi/case/app_server.h>

void startAppServer();
void stopAppServer();
bool isAppServerRunning();

#define INTENT_ENTER_WIFI_VIDEO "enter_wifi_video"

#endif /* MISC_APP_SERVER_IMPL_H_ */
