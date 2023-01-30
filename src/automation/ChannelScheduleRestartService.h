#ifndef ChannelScheduleRestartService_h
#define ChannelScheduleRestartService_h

#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif

#include <ESPAsyncWebServer.h>
#include <SecurityManager.h>
#include "TaskScheduler.h"
#include "channels.h"

class ChannelScheduleRestartService {
 public:
  ChannelScheduleRestartService(
    AsyncWebServer* server,
    SecurityManager* securityManager,
    TaskScheduler* channel,
    char* restChannelRestartEndPoint
    );

 private:
  TaskScheduler* _channel;
  void scheduleRestart(AsyncWebServerRequest* request);
};

#endif  // end ChannelScheduleRestartService_h
