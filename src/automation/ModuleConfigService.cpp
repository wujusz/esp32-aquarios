#include "ModuleConfigService.h"
#include <ArduinoJson.h>
#include <set>

ModuleConfigService::ModuleConfigService(AsyncWebServer* server, FS* fs, SecurityManager* securityManager)
    : _fs(fs),
      _updateHandler(MODULE_UPDATE_SERVICE_PATH,
                     securityManager->wrapCallback(std::bind(&ModuleConfigService::handleUpdate,
                                                           this, std::placeholders::_1, std::placeholders::_2),
                                            AuthenticationPredicates::IS_ADMIN)) {
  _updateHandler.setMethod(HTTP_POST);
  _updateHandler.setMaxContentLength(DEFAULT_JSON_DOCUMENT_SIZE);
  server->addHandler(&_updateHandler);

  server->on(MODULE_READ_SERVICE_PATH,
             HTTP_GET,
             securityManager->wrapRequest(std::bind(&ModuleConfigService::handleRead,
                                                   this, std::placeholders::_1),
                                        AuthenticationPredicates::IS_AUTHENTICATED));
}

void ModuleConfigService::handleRead(AsyncWebServerRequest* request) {
  if (!request->hasParam("id")) {
    request->send(400);
    return;
  }
  String moduleId = request->getParam("id")->value();
  String path = String(FS_CONFIG_DIRECTORY) + "/module" + moduleId + ".json";
  File file = _fs->open(path.c_str(), "r");
  if (!file) {
    request->send(404);
    return;
  }
  String json = file.readString();
  file.close();
  request->send(200, "application/json", json);
}

void ModuleConfigService::handleUpdate(AsyncWebServerRequest* request, JsonVariant& json) {
  if (!json.is<JsonObject>()) {
    request->send(400);
    return;
  }
  JsonObject obj = json.as<JsonObject>();
  String moduleId = obj["id"] | "";
  String moduleName = obj["name"] | "";
  float targetTemp = obj["targetTemp"] | 0.0f;
  JsonObject scheduleObj = obj["schedule"].as<JsonObject>();
  JsonArray schedulesArr = obj["schedules"].as<JsonArray>();
  JsonArray channels = obj["channels"].as<JsonArray>();
  if (moduleId == "") {
    request->send(400);
    return;
  }

  // Load existing module channels and schedule count
  std::set<String> oldChannels;
  int oldScheduleCount = 0;
  String modulePath = String(FS_CONFIG_DIRECTORY) + "/module" + moduleId + ".json";
  File existing = _fs->open(modulePath.c_str(), "r");
  if (existing) {
    DynamicJsonDocument doc(DEFAULT_JSON_DOCUMENT_SIZE);
    DeserializationError err = deserializeJson(doc, existing);
    if (!err) {
      JsonArray arr = doc["channels"].as<JsonArray>();
      if (arr) {
        for (JsonVariant v : arr) {
          JsonObject c = v.as<JsonObject>();
          String id = c["id"] | "";
          if (id != "") oldChannels.insert(id);
        }
      }
      JsonArray oldSched = doc["schedules"].as<JsonArray>();
      if (!oldSched && doc["schedule"]) {
        oldSched = doc.createNestedArray("schedules");
        oldSched.add(doc["schedule"].as<JsonObject>());
      }
      oldScheduleCount = oldSched ? oldSched.size() : 0;
    }
    existing.close();
  }

  std::set<String> newChannels;
  if (channels) {
    for (JsonVariant v : channels) {
      JsonObject c = v.as<JsonObject>();
      String id = c["id"] | "";
      if (id != "") newChannels.insert(id);
    }
  }

  // remove channel configs not present anymore
  for (const String& id : oldChannels) {
    if (newChannels.find(id) == newChannels.end()) {
      for (int si = 0; si < oldScheduleCount; ++si) {
        String path = String(FS_CONFIG_DIRECTORY) + "/channel" + id + "_" + String(si) + "State.json";
        _fs->remove(path.c_str());
      }
    }
  }

  int newScheduleCount = schedulesArr ? schedulesArr.size() : (scheduleObj.isNull() ? 0 : 1);

  for (const String& id : newChannels) {
    for (int si = newScheduleCount; si < oldScheduleCount; ++si) {
      String path = String(FS_CONFIG_DIRECTORY) + "/channel" + id + "_" + String(si) + "State.json";
      _fs->remove(path.c_str());
    }
  }

  // write module config
  File moduleFile = _fs->open(modulePath.c_str(), "w");
  if (moduleFile) {
    if (!schedulesArr && scheduleObj) {
      schedulesArr = obj.createNestedArray("schedules");
      schedulesArr.add(scheduleObj);
    }
    serializeJson(obj, moduleFile);
    moduleFile.close();
  }

  // Determine next analog channel index
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

  int schedIdx = 0;
  JsonArray scheduleArray = schedulesArr;
  if (!scheduleArray && scheduleObj) {
    scheduleArray = obj.createNestedArray("_tmp_sched");
    scheduleArray.add(scheduleObj);
  }

  for (JsonVariant v : channels) {
    JsonObject ch = v.as<JsonObject>();
    String cid = ch["id"] | "";
    int pin = ch["pin"] | -1;
    int brightness = ch["brightness"] | 0;
    if (cid == "" || pin < 0) continue;

    schedIdx = 0;
    for (JsonVariant sv : scheduleArray) {
      JsonObject schedObj = sv.as<JsonObject>();

      String path = String(FS_CONFIG_DIRECTORY) + "/channel" + cid + "_" + String(schedIdx) + "State.json";
      File file = _fs->open(path.c_str(), "w");
      if (!file) { schedIdx++; continue; }

      DynamicJsonDocument doc(DEFAULT_JSON_DOCUMENT_SIZE);
      JsonObject rootObj = doc.to<JsonObject>();

      String moduleType = obj["type"] | "switch";
      rootObj["controlPin"] = pin;
      if (moduleType == "lighting") {
        rootObj["homeAssistantTopicType"] = HOMEASSISTANT_TOPIC_TYPE_LIGHT;
        rootObj["homeAssistantIcon"] = MDI_LIGHTBULB;
      } else {
        rootObj["homeAssistantTopicType"] = HOMEASSISTANT_TOPIC_TYPE_SWITCH;
        rootObj["homeAssistantIcon"] = MDI_POWER;
      }
      rootObj["controlOn"] = DEFAULT_CONTROL_STATE;
      rootObj["name"] = moduleName == "" ? String("Module ") + moduleId + " " + cid : moduleName;
      rootObj["enabled"] = true;
      rootObj["brightness"] = brightness;
      rootObj["enableTimeSpan"] = false;
      rootObj["lastStartedChangeTime"] = "";
      rootObj["nextRunTime"] = "";
      rootObj["randomize"] = false;
      rootObj["IPAddress"] = "";
      rootObj["uniqueId"] = "";
      rootObj["enableMinimumRunTime"] = false;
      rootObj["enableRemoteConfiguration"] = false;
      rootObj["masterIPAddress"] = "";
      rootObj["restChannelEndPoint"] = String("/rest/channel") + cid + "_" + String(schedIdx) + "State";
      rootObj["restChannelRestartEndPoint"] = String("/rest/channel") + cid + "_" + String(schedIdx) + "ScheduleRestart";
      rootObj["enableDateRange"] = false;
      rootObj["activeOutsideDateRange"] = false;
      rootObj["buildVersion"] = BUILD_VERSION;
      rootObj["analogChannel"] = index++;
      rootObj["moduleId"] = moduleId;
      rootObj["moduleType"] = obj["type"] | "switch";

      JsonArray activeDateRange = rootObj.createNestedArray("activeDateRange");
      activeDateRange.add(DEFAULT_START_DATE_RANGE);
      activeDateRange.add(DEFAULT_END_DATE_RANGE);

      JsonObject schedule = rootObj.createNestedObject("schedule");
      schedule["runEvery"] = schedObj["runEvery"] | 15.0f;
      schedule["offAfter"] = schedObj["offAfter"] | 3.0f;
      schedule["startTimeHour"] = schedObj["startTimeHour"] | 8;
      schedule["startTimeMinute"] = schedObj["startTimeMinute"] | 0;
      schedule["hotTimeHour"] = schedObj["hotTimeHour"] | 0.0f;
      schedule["overrideTime"] = schedObj["overrideTime"] | 15.0f;
      schedule["endTimeHour"] = schedObj["endTimeHour"] | 16;
      schedule["endTimeMinute"] = schedObj["endTimeMinute"] | 0;
      schedule["isOverride"] = schedObj["isOverride"] | false;
      JsonArray weekDays = schedule.createNestedArray("weekDays");
      JsonArray srcWd = schedObj["weekDays"].as<JsonArray>();
      if (srcWd) {
        for (JsonVariant vv : srcWd) weekDays.add(vv.as<int>());
      } else {
        for (int i = 0; i < 7; i++) weekDays.add(i);
      }

      serializeJson(doc, file);
      file.close();
      schedIdx++;
    }
  }

  if (!schedulesArr && scheduleObj) {
    obj.remove("_tmp_sched");
  }

  request->send(200);
}
