#ifndef ChannelConfigCreateService_h
#define ChannelConfigCreateService_h

#include <ESPAsyncWebServer.h>
#include <SecurityManager.h>
#include <FS.h>
#include <AsyncJson.h>

#define CHANNEL_CREATE_SERVICE_PATH "/rest/createChannel"

class ChannelConfigCreateService {
 public:
  ChannelConfigCreateService(AsyncWebServer* server, FS* fs, SecurityManager* securityManager);

 private:
  FS* _fs;
  AsyncCallbackJsonWebHandler _handler;

  void handleCreate(AsyncWebServerRequest* request, JsonVariant& json);
};

#endif // ChannelConfigCreateService_h
