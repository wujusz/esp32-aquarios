#ifndef TaskSchedulerHelper_h
#define TaskSchedulerHelper_h

#include <ESP8266React.h>
#include <vector>
#include <memory>
#include "Schedules.h"
#include "TaskScheduler.h"
#include "ChannelMqttSettingsService.h"
#include "ChannelScheduleRestartService.h"
#include "channels.h"

// Extern declarations for objects defined in main.cpp
extern AsyncWebServer server;
extern ESP8266React esp8266React;

inline void createChannelSchedules(const char* configDir, Schedules& schedules) {
  FS* fs = esp8266React.getFS();
  File root = fs->open(configDir);
  if (!root || !root.isDirectory()) {
    Serial.println(F("Config directory missing"));
    return;
  }

  static std::vector<std::unique_ptr<ChannelMqttSettingsService>> mqttServices;
  static std::vector<std::unique_ptr<TaskScheduler>> schedulerObjs;
  static std::vector<std::unique_ptr<ChannelScheduleRestartService>> restartSvcs;

  while (true) {
    File f = root.openNextFile();
    if (!f) break;
    if (f.isDirectory()) continue;

    String fname = f.name();
    if (!fname.endsWith("State.json")) continue;

    String base = fname.substring(String(configDir).length());
    if (base.startsWith("/")) base = base.substring(1);
    if (!base.startsWith("channel")) continue;
    String chanId = base.substring(0, base.length() - String("State.json").length());

    int index = schedulerObjs.size() + 1;

    String restEndpoint = String("/rest/") + chanId + "State";
    String socketPath = String("/ws/") + chanId + "State";
    String restartEndpoint = String("/rest/") + chanId + "ScheduleRestart";
    String brokerFile = String("/config/C") + String(index) + "BrokerSettings.json";
    String brokerRest = String("/rest/Channel") + chanId.substring(7) + "BrokerSettings";

    auto mqtt = std::make_unique<ChannelMqttSettingsService>(
        &server,
        fs,
        esp8266React.getSecurityManager(),
        brokerFile.c_str(),
        brokerRest,
        0,
        chanId,
        chanId.substring(7),
        HOMEASSISTANT_TOPIC_TYPE_SWITCH,
        "mdi:power-switch");

    auto sched = std::make_unique<TaskScheduler>(
        &server,
        esp8266React.getSecurityManager(),
        esp8266React.getMqttClient(),
        fs,
        0,
        fname.c_str(),
        restEndpoint,
        socketPath.c_str(),
        0,
        0,
        0,
        0,
        0,
        0,
        false,
        0,
        chanId,
        false,
        mqtt.get(),
        false,
        0,
        0,
        false,
        HOMEASSISTANT_TOPIC_TYPE_SWITCH,
        "mdi:power-switch",
        REMOTE_CONFIG_ENABLED,
        MASTER_DEVICE,
        restartEndpoint,
        false,
        false,
        "",
        "",
        BUILD_VERSION,
        "0,1,2,3,4,5,6",
        index - 1);

    auto restartSvc = std::make_unique<ChannelScheduleRestartService>(
        &server,
        esp8266React.getSecurityManager(),
        sched.get(),
        restartEndpoint);

    schedules.addSchedule({sched.get(), false, TOGGLE_READ_PIN, LED, LED_ON, 0});

    mqtt->begin();
    sched->begin();

    mqttServices.push_back(std::move(mqtt));
    schedulerObjs.push_back(std::move(sched));
    restartSvcs.push_back(std::move(restartSvc));
  }
}

#endif // TaskSchedulerHelper_h
