#include <ESP8266React.h>
#include "./automation/TaskSchedulerHelper.h"
#include "./automation/ChannelConfigCreateService.h"
#include "./automation/ModuleConfigCreateService.h"
#include "./automation/ChannelListService.h"
#include "./automation/ModuleListService.h"
#include "./automation/ModuleConfigService.h"
#include "./automation/SensorLogService.h"
#include "./automation/TemperatureMonitorService.h"

#define SERIAL_BAUD_RATE 115200

AsyncWebServer server(80);
ESP8266React esp8266React(&server);

Automation automation = Automation();
Schedules schedules(&automation);
ChannelConfigCreateService* channelCreateService;
ModuleConfigCreateService* moduleCreateService;
ChannelListService* channelListService;
ModuleListService* moduleListService;
ModuleConfigService* moduleConfigService;
SensorLogService* sensorLogService;
TemperatureMonitorService* temperatureMonitorService;

void setup() {
  // start serial and filesystem
  Serial.begin(SERIAL_BAUD_RATE);

  // start the framework and demo project
  esp8266React.begin();

  channelCreateService = new ChannelConfigCreateService(&server, esp8266React.getFS(), esp8266React.getSecurityManager());
  moduleCreateService = new ModuleConfigCreateService(&server, esp8266React.getFS(), esp8266React.getSecurityManager());
  channelListService = new ChannelListService(&server, esp8266React.getFS(), esp8266React.getSecurityManager());
  moduleListService = new ModuleListService(&server, esp8266React.getFS(), esp8266React.getSecurityManager());
  moduleConfigService = new ModuleConfigService(&server, esp8266React.getFS(), esp8266React.getSecurityManager());
  sensorLogService = new SensorLogService(&server, esp8266React.getFS(), esp8266React.getSecurityManager());
  temperatureMonitorService = new TemperatureMonitorService(esp8266React.getFS());
  temperatureMonitorService->begin();

  // Tworzenie dynamiczne obiektów TaskScheduler na podstawie plików konfiguracyjnych
  createChannelSchedules("/config", schedules);

  schedules.beginSchedules();
  schedules.setScheduleTimes();

  // start the server
  server.begin();
}

void loop() {
  // run the framework's loop function
  esp8266React.loop();

  // run the automation schedules
  schedules.runSchedules();
}
