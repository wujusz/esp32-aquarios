#include "ChannelConfigCreateService.h"
#include "Homeassistant.h"
#include "Utilities.h"
#include "ChannelFileUtils.h"

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

  int index = getNextAnalogIndex(_fs);

  DynamicJsonDocument doc(DEFAULT_JSON_DOCUMENT_SIZE);
  JsonObject schedule = doc.to<JsonObject>();
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
  for (int i = 0; i < 7; i++) weekDays.add(i);

  writeChannelJson(_fs,
                  id,
                  pin,
                  0,
                  "switch",
                  "",
                  "",
                  schedule,
                  0,
                  index);

  request->send(200);
}
