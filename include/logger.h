#pragma once

#include <Arduino.h>
#include <SdFat.h>

class Logger {
public:
  Logger(SdFat &sd);

  // Найти последний файл лога и получить максимальный номер логера из имён файлов
  unsigned long findLastLogNumber();

  // Получить имя файла логера на основе номера (например, log_001.csv)
  String getLogFilename(unsigned long logNumber);

  // Открыть текущий файл логера для записи
  File openCurrentLog(unsigned long logNumber);

private:
  SdFat* _sd;
  
  // Вспомогательный метод для извлечения номера из имени файла (log_NNN.csv)
  unsigned long extractLogNumber(const char* filename);
};
