#ifndef ModuleConfigService_h
#define ModuleConfigService_h

#include <ESPAsyncWebServer.h>
#include <SecurityManager.h>
#include <FS.h>
#include <AsyncJson.h>

#define MODULE_READ_SERVICE_PATH "/rest/module"
#define MODULE_UPDATE_SERVICE_PATH "/rest/updateModule"

class ModuleConfigService {
 public:
  ModuleConfigService(AsyncWebServer* server, FS* fs, SecurityManager* securityManager);

 private:
  FS* _fs;
  AsyncCallbackJsonWebHandler _updateHandler;
  void handleRead(AsyncWebServerRequest* request);
  void handleUpdate(AsyncWebServerRequest* request, JsonVariant& json);
};

#endif // ModuleConfigService_h
