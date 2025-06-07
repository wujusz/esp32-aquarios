#ifndef SensorLogService_h
#define SensorLogService_h

#include <HttpEndpoint.h>
#include "DataLogger.h"

#define SENSOR_LOG_SERVICE_PATH "/rest/sensorLog"

class SensorLogService {
 public:
  SensorLogService(AsyncWebServer* server, FS* fs, SecurityManager* securityManager);
 private:
  void handleLog(AsyncWebServerRequest* request);
  DataLogger _logger;
  HttpEndpoint<void> _handler;
};

#endif // SensorLogService_h
