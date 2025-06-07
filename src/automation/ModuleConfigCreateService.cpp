#include "ModuleConfigCreateService.h"
#include "Homeassistant.h"
#include "Utilities.h"

ModuleConfigCreateService::ModuleConfigCreateService(AsyncWebServer* server, FS* fs, SecurityManager* securityManager)
    : _fs(fs),
      _handler(MODULE_CREATE_SERVICE_PATH,
               securityManager->wrapCallback(std::bind(&ModuleConfigCreateService::handleCreate,
                                                     this, std::placeholders::_1, std::placeholders::_2),
                                            AuthenticationPredicates::IS_ADMIN)) {
  _handler.setMethod(HTTP_POST);
  _handler.setMaxContentLength(DEFAULT_JSON_DOCUMENT_SIZE);
  server->addHandler(&_handler);
}

void ModuleConfigCreateService::handleCreate(AsyncWebServerRequest* request, JsonVariant& json) {
  if (!json.is<JsonObject>()) {
    request->send(400);
    return;
  }
  JsonObject obj = json.as<JsonObject>();
  String moduleId = obj["id"] | "";
  String moduleType = obj["type"] | "switch";
  String moduleName = obj["name"] | "";
  float targetTemp = obj["targetTemp"] | 0.0f;
  JsonObject scheduleObj = obj["schedule"].as<JsonObject>();
  JsonArray schedulesArr = obj["schedules"].as<JsonArray>();
  JsonArray channels = obj["channels"].as<JsonArray>();
  if (moduleId == "" || !channels) {
    request->send(400);
    return;
  }

  // Save module configuration
  String modulePath = String(FS_CONFIG_DIRECTORY) + "/module" + moduleId + ".json";
  File moduleFile = _fs->open(modulePath.c_str(), "w");
  if (moduleFile) {
    // ensure schedules array is stored even if single schedule object provided
    if (!schedulesArr && scheduleObj) {
      schedulesArr = obj.createNestedArray("schedules");
      JsonObject s = schedulesArr.createNestedObject();
      s["runEvery"] = scheduleObj["runEvery"] | 15.0f;
      s["offAfter"] = scheduleObj["offAfter"] | 3.0f;
      s["startTimeHour"] = scheduleObj["startTimeHour"] | 8;
      s["startTimeMinute"] = scheduleObj["startTimeMinute"] | 0;
      s["hotTimeHour"] = scheduleObj["hotTimeHour"] | 0.0f;
      s["overrideTime"] = scheduleObj["overrideTime"] | 15.0f;
      s["endTimeHour"] = scheduleObj["endTimeHour"] | 16;
      s["endTimeMinute"] = scheduleObj["endTimeMinute"] | 0;
      s["isOverride"] = scheduleObj["isOverride"] | false;
      JsonArray wd = s.createNestedArray("weekDays");
      JsonArray srcWd = scheduleObj["weekDays"].as<JsonArray>();
      if (srcWd) {
        for (JsonVariant vv : srcWd) wd.add(vv.as<int>());
      } else {
        for (int i = 0; i < 7; i++) wd.add(i);
      }
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
    String chanId = ch["id"] | "";
    int pin = ch["pin"] | -1;
    int brightness = ch["brightness"] | 0;
    if (chanId == "" || pin < 0) continue;

    schedIdx = 0;
    for (JsonVariant sv : scheduleArray) {
      JsonObject schedObj = sv.as<JsonObject>();

      String path = String(FS_CONFIG_DIRECTORY) + "/channel" + chanId + "_" + String(schedIdx) + "State.json";
      File file = _fs->open(path.c_str(), "w");
      if (!file) { schedIdx++; continue; }

      DynamicJsonDocument doc(DEFAULT_JSON_DOCUMENT_SIZE);
      JsonObject rootObj = doc.to<JsonObject>();

      rootObj["controlPin"] = pin;
      if (moduleType == "lighting") {
        rootObj["homeAssistantTopicType"] = HOMEASSISTANT_TOPIC_TYPE_LIGHT;
        rootObj["homeAssistantIcon"] = MDI_LIGHTBULB;
      } else {
        rootObj["homeAssistantTopicType"] = HOMEASSISTANT_TOPIC_TYPE_SWITCH;
        rootObj["homeAssistantIcon"] = MDI_POWER;
      }
      rootObj["controlOn"] = DEFAULT_CONTROL_STATE;
      rootObj["name"] = moduleName == "" ? String("Module ") + moduleId + " " + chanId : moduleName;
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
      rootObj["restChannelEndPoint"] = String("/rest/channel") + chanId + "_" + String(schedIdx) + "State";
      rootObj["restChannelRestartEndPoint"] = String("/rest/channel") + chanId + "_" + String(schedIdx) + "ScheduleRestart";
      rootObj["enableDateRange"] = false;
      rootObj["activeOutsideDateRange"] = false;
      rootObj["buildVersion"] = BUILD_VERSION;
      rootObj["analogChannel"] = index++;
      rootObj["moduleId"] = moduleId;
      rootObj["moduleType"] = moduleType;

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
