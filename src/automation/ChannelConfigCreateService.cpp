#include "ChannelConfigCreateService.h"
#include "Homeassistant.h"
#include "Utilities.h"

ChannelConfigCreateService::ChannelConfigCreateService(AsyncWebServer* server,
                                                       FS* fs,
                                                       SecurityManager* securityManager)
    : _fs(fs),
      _handler(CHANNEL_CREATE_SERVICE_PATH,
               securityManager->wrapCallback(std::bind(&ChannelConfigCreateService::handleCreate,
                                                     this, std::placeholders::_1, std::placeholders::_2),
                                            AuthenticationPredicates::IS_ADMIN)) {
  _handler.setMethod(HTTP_POST);
  _handler.setMaxContentLength(DEFAULT_JSON_DOCUMENT_SIZE);
  server->addHandler(&_handler);
}

void ChannelConfigCreateService::handleCreate(AsyncWebServerRequest* request, JsonVariant& json) {
  if (!json.is<JsonObject>()) {
    request->send(400);
    return;
  }
  JsonObject obj = json.as<JsonObject>();
  String id = obj["id"] | "";
  int pin = obj["pin"] | -1;
  if (id == "" || pin < 0) {
    request->send(400);
    return;
  }

  File root = _fs->open(FS_CONFIG_DIRECTORY);
  int index = 0;
  if (root) {
    while (true) {
      File f = root.openNextFile();
      if (!f) break;
      if (!f.isDirectory()) {
        String name = f.name();
        if (name.endsWith("State.json")) {
          index++;
        }
      }
    }
    root.close();
  }
  int analogChannel = index;

  String path = String(FS_CONFIG_DIRECTORY) + "/channel" + id + "State.json";
  File file = _fs->open(path.c_str(), "w");
  if (!file) {
    request->send(500);
    return;
  }

  DynamicJsonDocument doc(DEFAULT_JSON_DOCUMENT_SIZE);
  JsonObject rootObj = doc.to<JsonObject>();

  rootObj["controlPin"] = pin;
  rootObj["homeAssistantTopicType"] = HOMEASSISTANT_TOPIC_TYPE_SWITCH;
  rootObj["homeAssistantIcon"] = MDI_POWER;
  rootObj["controlOn"] = DEFAULT_CONTROL_STATE;
  rootObj["name"] = String("Channel ") + id;
  rootObj["enabled"] = true;
  rootObj["brightness"] = 0;
  rootObj["enableTimeSpan"] = false;
  rootObj["lastStartedChangeTime"] = "";
  rootObj["nextRunTime"] = "";
  rootObj["randomize"] = false;
  rootObj["IPAddress"] = "";
  rootObj["uniqueId"] = "";
  rootObj["enableMinimumRunTime"] = false;
  rootObj["enableRemoteConfiguration"] = false;
  rootObj["masterIPAddress"] = "";
  rootObj["restChannelEndPoint"] = String("/rest/channel") + id + "State";
  rootObj["restChannelRestartEndPoint"] = String("/rest/channel") + id + "ScheduleRestart";
  rootObj["enableDateRange"] = false;
  rootObj["activeOutsideDateRange"] = false;
  rootObj["buildVersion"] = BUILD_VERSION;
  rootObj["analogChannel"] = analogChannel;

  JsonArray activeDateRange = rootObj.createNestedArray("activeDateRange");
  activeDateRange.add(DEFAULT_START_DATE_RANGE);
  activeDateRange.add(DEFAULT_END_DATE_RANGE);

  JsonObject schedule = rootObj.createNestedObject("schedule");
  schedule["runEvery"] = 15.0f;
  schedule["offAfter"] = 3.0f;
  schedule["startTimeHour"] = 8;
  schedule["startTimeMinute"] = 0;
  schedule["hotTimeHour"] = 0.0f;
  schedule["overrideTime"] = 15.0f;
  schedule["endTimeHour"] = 16;
  schedule["endTimeMinute"] = 0;
  schedule["isOverride"] = false;
  JsonArray weekDays = schedule.createNestedArray("weekDays");
  for (int i = 0; i < 7; i++) {
    weekDays.add(i);
  }

  serializeJson(doc, file);
  file.close();
  request->send(200);
}
