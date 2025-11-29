#include "spinner.h"

// Значения констант
const int Spinner::SPINNER_UPDATE = 70;
const int Spinner::SPINNER_FADE_TIME = 1300; // 2 секунды
const char Spinner::_chars[5] = {'|','/','-', 7, '*'}; // добавлен '*'

// Пиксельный шаблон обратного слеша (код 7)
byte Spinner::_backslashChar[8] = {
  B00000,
  B10000,
  B01000,
  B00100,
  B00010,
  B00001,
  B00000,
  B00000
};

Spinner::Spinner(LiquidCrystal_I2C &lcd, uint8_t col, uint8_t row)
  : _lcd(&lcd), _col(col), _row(row), _active(false), _lastUpdate(0), _state(0), _fadeStart(0) {}

void Spinner::init() {
  // createChar требует массив byte (не const)
  _lcd->createChar(7, _backslashChar);
}

void Spinner::start() {
  _active = true;
  _state = 0;
  _lastUpdate = millis();
  _fadeStart = 0;
}

void Spinner::stop() {
  _active = false;
  _fadeStart = millis();
}

void Spinner::update() {
  if (_active) {
    if (millis() - _lastUpdate >= SPINNER_UPDATE) {
      _lastUpdate = millis();
      _lcd->setCursor(_col, _row);
      printChar(_chars[_state]);
      _state = (_state + 1) % 5;
    }
    return;
  }

  if (_fadeStart != 0) {
    unsigned long elapsed = millis() - _fadeStart;
    if (elapsed >= SPINNER_FADE_TIME) {
      _lcd->setCursor(_col, _row);
      _lcd->print(" ");
      _fadeStart = 0;
    } else {
      unsigned long interval = SPINNER_UPDATE + (SPINNER_FADE_TIME - elapsed) / 4;
      if (millis() - _lastUpdate >= interval) {
        _lastUpdate = millis();
        _lcd->setCursor(_col, _row);
        printChar(_chars[_state]);
        _state = (_state + 1) % 5;
      }
    }
  }
}

void Spinner::printChar(char c) {
  if (c == 7) {
    _lcd->write((uint8_t)7);
  } else {
    _lcd->print(c);
  }
}
