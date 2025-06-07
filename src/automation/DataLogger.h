#ifndef DataLogger_h
#define DataLogger_h

#include <FS.h>
#include <ArduinoJson.h>

class DataLogger {
 public:
  DataLogger(FS* fs, const char* dir = "/logs", size_t limit = 1000);
  void logValue(const String& id, float value);
  bool readLog(const String& id, String& outJson);
 private:
  FS* _fs;
  String _dir;
  size_t _limit;
};

#endif // DataLogger_h
