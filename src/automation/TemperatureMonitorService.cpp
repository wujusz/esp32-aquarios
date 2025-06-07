#include "TemperatureMonitorService.h"
#include <ArduinoJson.h>
#include "ModuleConfigService.h"

TemperatureMonitorService::TemperatureMonitorService(FS* fs)
    : _fs(fs), _logger(fs) {}

void TemperatureMonitorService::begin() {
  loadModules();
  _ticker.attach(60, [this]() { this->poll(); });
}

void TemperatureMonitorService::loadModules() {
  _sensors.clear();
  File root = _fs->open(FS_CONFIG_DIRECTORY);
  if(!root) return;
  while(true) {
    File f = root.openNextFile();
    if(!f) break;
    if(f.isDirectory()) continue;
    String name = f.name();
    if(name.endsWith(".json") && name.indexOf("module") >= 0) {
      DynamicJsonDocument doc(DEFAULT_JSON_DOCUMENT_SIZE);
      DeserializationError err = deserializeJson(doc, f);
      if(!err) {
        JsonObject obj = doc.as<JsonObject>();
        String type = obj["type"] | "";
        if(type == "thermometer") {
          TempSensor ts{};
          ts.id = obj["id"].as<String>();
          ts.pin = -1;
          ts.target = obj["targetTemp"].as<float>();
          ts.heaterPin = -1;
          JsonArray ch = obj["channels"].as<JsonArray>();
          if(ch && ch.size()>0) {
            JsonObject c = ch[0];
            ts.pin = c["pin"].as<int>();
          }
          // find heater module
          File root2 = _fs->open(FS_CONFIG_DIRECTORY);
          if(root2) {
            while(true) {
              File fh = root2.openNextFile();
              if(!fh) break;
              if(fh.isDirectory()) continue;
              String n2 = fh.name();
              if(n2.endsWith(".json") && n2.indexOf("module") >= 0) {
                DynamicJsonDocument dh(DEFAULT_JSON_DOCUMENT_SIZE);
                if(!deserializeJson(dh, fh)) {
                  JsonObject oh = dh.as<JsonObject>();
                  if((String)(oh["type"]|"") == "heater") {
                    JsonArray chh = oh["channels"].as<JsonArray>();
                    if(chh && chh.size()>0) {
                      JsonObject chObj = chh[0];
                      ts.heaterPin = chObj["pin"].as<int>();
                      break;
                    }
                  }
                }
              }
            }
            root2.close();
          }
          if(ts.pin >= 0) {
            ts.wire = new OneWire(ts.pin);
            ts.sensor = new DallasTemperature(ts.wire);
            ts.sensor->begin();
            _sensors.push_back(ts);
          }
        }
      }
      f.close();
    }
  }
  root.close();
}

void TemperatureMonitorService::poll() {
  for(auto& s : _sensors) {
    s.sensor->requestTemperatures();
    float temp = s.sensor->getTempCByIndex(0);
    _logger.logValue("temp_" + s.id, temp);
    if(s.heaterPin >= 0 && s.target != 0) {
      pinMode(s.heaterPin, OUTPUT);
      float hyst = 0.5;
      if(temp < s.target - hyst) {
        digitalWrite(s.heaterPin, HIGH);
      } else if(temp > s.target + hyst) {
        digitalWrite(s.heaterPin, LOW);
      }
    }
  }
}
