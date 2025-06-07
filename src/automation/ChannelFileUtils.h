#ifndef ChannelFileUtils_h
#define ChannelFileUtils_h

#include <FS.h>
#include <ArduinoJson.h>
#include "Homeassistant.h"
#include "ChannelState.h" // for DEFAULT_JSON_DOCUMENT_SIZE and DEFAULT_CONTROL_STATE
#include "FactoryResetService.h" // for FS_CONFIG_DIRECTORY

inline int getNextAnalogIndex(FS* fs) {
  File root = fs->open(FS_CONFIG_DIRECTORY);
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
  return index;
}

inline void writeChannelJson(FS* fs,
                             const String& chanId,
                             int pin,
                             int brightness,
                             const String& moduleType,
                             const String& moduleName,
                             const String& moduleId,
                             JsonObject schedObj,
                             int schedIdx,
                             int& analogIndex) {
  String path = String(FS_CONFIG_DIRECTORY) + "/channel" + chanId + "_" + String(schedIdx) + "State.json";
  File file = fs->open(path.c_str(), "w");
  if (!file) return;

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
  rootObj["analogChannel"] = analogIndex++;
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
}

#endif // ChannelFileUtils_h
