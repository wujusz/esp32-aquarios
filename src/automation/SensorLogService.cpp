#include "SensorLogService.h"

SensorLogService::SensorLogService(AsyncWebServer* server, FS* fs, SecurityManager* securityManager)
    : _logger(fs),
      _handler(SENSOR_LOG_SERVICE_PATH,
               securityManager->wrapCallback(std::bind(&SensorLogService::handleLog,
                                                     this, std::placeholders::_1),
                                            AuthenticationPredicates::IS_AUTHENTICATED)) {
  _handler.setMethod(HTTP_GET);
  server->addHandler(&_handler);
}

void SensorLogService::handleLog(AsyncWebServerRequest* request) {
  if(!request->hasParam("id")) { request->send(400); return; }
  String id = request->getParam("id")->value();
  String json;
  if(_logger.readLog(id, json)) {
    request->send(200, "application/json", json);
  } else {
    request->send(404);
  }
}
