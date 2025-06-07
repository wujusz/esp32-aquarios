#ifndef ModuleListService_h
#define ModuleListService_h

#include <ESPAsyncWebServer.h>
#include <SecurityManager.h>
#include <FS.h>

#define MODULE_LIST_SERVICE_PATH "/rest/moduleList"

class ModuleListService {
 public:
  ModuleListService(AsyncWebServer* server, FS* fs, SecurityManager* securityManager);

 private:
  FS* _fs;
  void handleList(AsyncWebServerRequest* request);
};

#endif // ModuleListService_h
