#include "DataLogger.h"
#include <time.h>

DataLogger::DataLogger(FS* fs, const char* dir, size_t limit)
    : _fs(fs), _dir(dir), _limit(limit) {
  if(!_fs->exists(_dir.c_str())) {
    _fs->mkdir(_dir.c_str());
  }
}

void DataLogger::logValue(const String& id, float value) {
  String path = _dir + "/" + id + ".csv";
  File f = _fs->open(path.c_str(), "a+");
  if(!f) return;
  String line = String(time(nullptr)) + "," + String(value, 2) + "\n";
  f.print(line);
  size_t lines = 0;
  f.seek(0, SeekSet);
  while(f.available()) if(f.read() == '\n') lines++;
  if(lines > _limit) {
    // trim oldest lines
    f.seek(0, SeekSet);
    String all = f.readString();
    int pos = -1;
    lines = 0;
    for(size_t i = all.length()-1; i >=0 && lines < _limit; --i){
      if(all[i]=='\n') lines++;
      pos = i;
      if(i==0) break;
    }
    String trimmed = all.substring(pos);
    f.close();
    f = _fs->open(path.c_str(), "w");
    if(f) { f.print(trimmed); f.close(); }
    return;
  }
  f.close();
}

bool DataLogger::readLog(const String& id, String& outJson) {
  String path = _dir + "/" + id + ".csv";
  File f = _fs->open(path.c_str(), "r");
  if(!f) return false;
  DynamicJsonDocument doc(DEFAULT_JSON_DOCUMENT_SIZE);
  JsonArray arr = doc.to<JsonArray>();
  while(f.available()) {
    String line = f.readStringUntil('\n');
    int idx = line.indexOf(',');
    if(idx > 0) {
      JsonObject obj = arr.createNestedObject();
      obj["t"] = line.substring(0, idx).toInt();
      obj["v"] = line.substring(idx+1).toFloat();
    }
  }
  f.close();
  serializeJson(arr, outJson);
  return true;
}
