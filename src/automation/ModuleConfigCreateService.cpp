#include "ModuleConfigCreateService.h"
#include "Homeassistant.h"
#include "Utilities.h"
#include "ChannelFileUtils.h"

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
  int index = getNextAnalogIndex(_fs);

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
      writeChannelJson(_fs,
                      chanId,
                      pin,
                      brightness,
                      moduleType,
                      moduleName,
                      moduleId,
                      schedObj,
                      schedIdx,
                      index);
    }
  }

  if (!schedulesArr && scheduleObj) {
    obj.remove("_tmp_sched");
  }

  request->send(200);
}
