#ifndef TemperatureMonitorService_h
#define TemperatureMonitorService_h

#include <vector>
#include <Ticker.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DataLogger.h"

class TemperatureMonitorService {
 public:
  TemperatureMonitorService(FS* fs);
  void begin();
 private:
  struct TempSensor {
    String id;
    int pin;
    float target;
    int heaterPin;
    OneWire* wire;
    DallasTemperature* sensor;
  };
  std::vector<TempSensor> _sensors;
  FS* _fs;
  DataLogger _logger;
  Ticker _ticker;

  void loadModules();
  void poll();
};

#endif // TemperatureMonitorService_h
