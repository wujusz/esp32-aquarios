#include <ESP8266React.h>
#include "./automation/TaskSchedulerHelper.h"

#define SERIAL_BAUD_RATE 115200

AsyncWebServer server(80);
ESP8266React esp8266React(&server);

Automation automation = Automation();
Schedules schedules(&automation);

void setup() {
  // start serial and filesystem
  Serial.begin(SERIAL_BAUD_RATE);

  // start the framework and demo project
  esp8266React.begin();

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
