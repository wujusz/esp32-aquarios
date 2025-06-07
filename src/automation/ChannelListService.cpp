#include "ChannelListService.h"
#include <ArduinoJson.h>

ChannelListService::ChannelListService(AsyncWebServer* server, FS* fs, SecurityManager* securityManager)
    : _fs(fs) {
  server->on(CHANNEL_LIST_SERVICE_PATH,
             HTTP_GET,
             securityManager->wrapRequest(std::bind(&ChannelListService::handleList,
                                                   this, std::placeholders::_1),
                                        AuthenticationPredicates::IS_AUTHENTICATED));
}

void ChannelListService::handleList(AsyncWebServerRequest* request) {
  DynamicJsonDocument doc(DEFAULT_JSON_DOCUMENT_SIZE);
  JsonArray array = doc.to<JsonArray>();

  File root = _fs->open(FS_CONFIG_DIRECTORY);
  if (root) {
    while (true) {
      File f = root.openNextFile();
      if (!f) break;
      if (!f.isDirectory()) {
        String name = f.name();
        if (name.endsWith("State.json")) {
          String base = name.substring(String(FS_CONFIG_DIRECTORY).length());
          if (base.startsWith("/")) base = base.substring(1);
          if (base.startsWith("channel")) {
            base.remove(base.length() - String("State.json").length());
            base.replace("channel", "");
            array.add(base);
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
