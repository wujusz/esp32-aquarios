#ifndef ModuleConfigCreateService_h
#define ModuleConfigCreateService_h

#include <ESPAsyncWebServer.h>
#include <SecurityManager.h>
#include <FS.h>
#include <AsyncJson.h>

#define MODULE_CREATE_SERVICE_PATH "/rest/createModule"

class ModuleConfigCreateService {
 public:
  ModuleConfigCreateService(AsyncWebServer* server, FS* fs, SecurityManager* securityManager);

 private:
  FS* _fs;
  AsyncCallbackJsonWebHandler _handler;
  void handleCreate(AsyncWebServerRequest* request, JsonVariant& json);
};

#endif // ModuleConfigCreateService_h
