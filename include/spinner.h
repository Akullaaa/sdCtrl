#pragma once

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

class Spinner {
public:
  Spinner(LiquidCrystal_I2C &lcd, uint8_t col = 19, uint8_t row = 3);

  // Инициализация (создание пользовательского символа)
  void init();

  // Запуск/остановка спиннера
  void start();
  void stop();

  // Обновление состояния (должно вызываться часто)
  void update();

private:
  LiquidCrystal_I2C* _lcd;
  uint8_t _col, _row;
  bool _active;
  unsigned long _lastUpdate;
  int _state;
  unsigned long _fadeStart;

  static const int SPINNER_UPDATE;    // ms
  static const int SPINNER_FADE_TIME; // ms
  static const char _chars[5];        // символы спиннера (добавлен '*')
  static byte _backslashChar[8];      // шаблон пользовательского символа

  void printChar(char c);
};
