#pragma once

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

class Countdown {
public:
  Countdown(LiquidCrystal_I2C &lcd, uint8_t col_end = 19, uint8_t row = 0);

  // Установить интервал (в миллисекундах)
  void setInterval(unsigned long interval_ms);

  // Установить время последнего обновления (синхронизация с main.cpp)
  void setLastUpdateTime(unsigned long last_update);

  // Обновить и вывести оставшееся время (вызывать часто)
  void update();

private:
  LiquidCrystal_I2C* _lcd;
  uint8_t _colEnd, _row;
  unsigned long _intervalMs;
  unsigned long _lastUpdateTime;
  int _prevFieldLen;

  // Вспомогательные методы
  unsigned long getTimeUntilNext() const;
  void printRightAligned(const String &text);
};
