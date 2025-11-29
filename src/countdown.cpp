#include "countdown.h"

Countdown::Countdown(LiquidCrystal_I2C &lcd, uint8_t col_end, uint8_t row)
  : _lcd(&lcd), _colEnd(col_end), _row(row), _intervalMs(5000), _lastUpdateTime(millis()), _prevFieldLen(0) {}

void Countdown::setInterval(unsigned long interval_ms) {
  _intervalMs = interval_ms;
}

void Countdown::setLastUpdateTime(unsigned long last_update) {
  _lastUpdateTime = last_update;
}

unsigned long Countdown::getTimeUntilNext() const {
  if (_lastUpdateTime == 0) {
    return _intervalMs / 1000UL;
  }
  unsigned long now = millis();
  unsigned long elapsed = now - _lastUpdateTime;
  
  // Защита от переполнения и выхода за границы
  if (elapsed > _intervalMs) {
    return 0;
  }
  
  unsigned long remaining = _intervalMs - elapsed;
  return remaining / 1000UL;
}

void Countdown::printRightAligned(const String &text) {
  String tstr = text;
  size_t maxWidth = (size_t)(_colEnd + 1); // максимально доступная ширина от 0 до colEnd

  // Обрежем строку, если она больше доступной ширины (покажем правую часть)
  if (tstr.length() > maxWidth) {
    tstr = tstr.substring(tstr.length() - maxWidth);
  }

  // Вычислим желаемую ширину поля: минимум 2, не меньше предыдущей длины
  size_t desiredWidth = 2;
  if ((size_t)_prevFieldLen > desiredWidth) desiredWidth = (size_t)_prevFieldLen;
  if (tstr.length() > desiredWidth) desiredWidth = tstr.length();
  if (desiredWidth > maxWidth) desiredWidth = maxWidth;

  // Создаём строку с ведущими нулями
  String padded = tstr;
  while (padded.length() < desiredWidth) {
    padded = String('0') + padded;
  }

  int newLen = padded.length();
  int newStart = _colEnd - newLen + 1;
  if (newStart < 0) newStart = 0;

  int prevLen = _prevFieldLen;
  int prevStart = (prevLen > 0) ? (_colEnd - prevLen + 1) : newStart;

  int clearStart = min(prevStart, newStart);
  if (clearStart < 0) clearStart = 0;
  int clearLen = max(prevLen, newLen);
  if (clearStart + clearLen - 1 > _colEnd) clearLen = _colEnd - clearStart + 1;

  // Очистим область, где могли остаться символы
  if (clearLen > 0) {
    _lcd->setCursor(clearStart, _row);
    for (int i = 0; i < clearLen; i++) _lcd->print(' ');
  }

  // Печать новой строки, выровненной по правому краю с ведущими нулями
  _lcd->setCursor(newStart, _row);
  _lcd->print(padded);
  _prevFieldLen = newLen;
}

void Countdown::update() {
  unsigned long timeUntilNext = getTimeUntilNext();
  String tstr = String(timeUntilNext);
  printRightAligned(tstr);
}
