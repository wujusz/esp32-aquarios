#ifndef ChannelListService_h
#define ChannelListService_h

#include <ESPAsyncWebServer.h>
#include <SecurityManager.h>
#include <FS.h>
#include "FactoryResetService.h"

#define CHANNEL_LIST_SERVICE_PATH "/rest/channelList"

class ChannelListService {
 public:
  ChannelListService(AsyncWebServer* server, FS* fs, SecurityManager* securityManager);

 private:
  FS* _fs;
  void handleList(AsyncWebServerRequest* request);
};

#endif // ChannelListService_h
