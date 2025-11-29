#include "logger.h"

Logger::Logger(SdFat &sd) : _sd(&sd) {}

unsigned long Logger::extractLogNumber(const char* filename) {
  // Ожидаем формат: log_NNN.csv
  // Извлекаем число из позиции 4-6
  if (strlen(filename) >= 11 && strncmp(filename, "log_", 4) == 0) {
    String numStr = String(filename).substring(4, 7);
    return numStr.toInt();
  }
  return 0;
}

unsigned long Logger::findLastLogNumber() {
  unsigned long maxNumber = 0;
  File root = _sd->open("/");
  
  if (!root) {
    return 0;
  }

  File file = root.openNextFile();
  while (file) {
    if (!file.isDir()) {
      char filename[256];
      file.getName(filename, sizeof(filename));
      Serial.print("Found file: ");
      Serial.println(filename);
      unsigned long num = extractLogNumber(filename);
      if (num > maxNumber) {
        maxNumber = num;
      }
    }
    file.close();
    file = root.openNextFile();
  }
  
  root.close();
  return maxNumber;
}

String Logger::getLogFilename(unsigned long logNumber) {
  String filename = "log_";
  if (logNumber < 10) {
    filename += "00";
  } else if (logNumber < 100) {
    filename += "0";
  }
  filename += logNumber;
  filename += ".csv";
  return filename;
}

File Logger::openCurrentLog(unsigned long logNumber) {
  String filename = getLogFilename(logNumber);
  return _sd->open(filename.c_str(), FILE_WRITE);
}
