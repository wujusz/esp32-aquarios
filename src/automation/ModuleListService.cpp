#include "ModuleListService.h"
#include <ArduinoJson.h>

ModuleListService::ModuleListService(AsyncWebServer* server, FS* fs, SecurityManager* securityManager)
    : _fs(fs) {
  server->on(MODULE_LIST_SERVICE_PATH,
             HTTP_GET,
             securityManager->wrapRequest(std::bind(&ModuleListService::handleList,
                                                   this, std::placeholders::_1),
                                        AuthenticationPredicates::IS_AUTHENTICATED));
}

void ModuleListService::handleList(AsyncWebServerRequest* request) {
  DynamicJsonDocument doc(DEFAULT_JSON_DOCUMENT_SIZE);
  JsonArray array = doc.to<JsonArray>();

  File root = _fs->open(FS_CONFIG_DIRECTORY);
  if (root) {
    while (true) {
      File f = root.openNextFile();
      if (!f) break;
      if (!f.isDirectory()) {
        String name = f.name();
        if (name.endsWith(".json") && name.indexOf("module") >= 0) {
          String base = name.substring(String(FS_CONFIG_DIRECTORY).length());
          if (base.startsWith("/")) base = base.substring(1);
          if (base.startsWith("module")) {
            String id = base;
            id.remove(id.length() - String(".json").length());
            id.replace("module", "");

            DynamicJsonDocument modDoc(DEFAULT_JSON_DOCUMENT_SIZE);
            File modF = _fs->open(name.c_str(), "r");
            if (modF) {
              DeserializationError err = deserializeJson(modDoc, modF);
              if (!err) {
                JsonObject obj = modDoc.as<JsonObject>();
                JsonObject item = array.createNestedObject();
                item["id"] = id;
                item["name"] = obj["name"] | String("Module ") + id;
                item["type"] = obj["type"] | "switch";
                JsonArray chArr = obj["channels"].as<JsonArray>();
                int count = chArr ? chArr.size() : 0;
                item["channels"] = count;
              }
              modF.close();
            }
          }
        }
      }
    }
    root.close();
  }

  String out;
  serializeJson(array, out);
  request->send(200, "application/json", out);
}
