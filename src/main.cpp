#include <Arduino.h>

#include <SPI.h>
#include <SdFat.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

SdFat sd;
const uint8_t CS_PIN = 10;
unsigned long lineNumber = 0;

// LCD 4x20 на I2C адресе 0x27
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Буфер для прокрутки текста
String lcdBuffer[4] = {"", "", "", ""};
unsigned long lastUpdate = 0;
// Единый интервал для обновления экрана и проверки данных
const unsigned long UPDATE_INTERVAL_MS = 16000; // мс

// Spinner и Countdown вынесены в отдельные файлы
#include "spinner.h"
#include "countdown.h"
#include "logger.h"

// Создаём экземпляры спиннера, обратного отсчёта времени и логера
Spinner spinner(lcd, 19, 3);
Countdown countdown(lcd, 19, 0);
Logger logger(sd);

void setup() {
  Serial.begin(9600);
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);         // сначала отпускаем шину

  // Инициализация LCD
  lcd.init();
  lcd.backlight();
  lcd.print("Init...");

  // Инициализируем spinner (создаёт пользовательский символ)
  spinner.init();
  // Устанавливаем интервал отсчёта времени и синхронизируем с текущим временем
  countdown.setInterval(UPDATE_INTERVAL_MS);
  countdown.setLastUpdateTime(millis());

  if (!sd.begin(CS_PIN, SD_SCK_MHZ(25))) {
    Serial.println("SD ошибка!");
    lcd.clear();
    lcd.print("SD error!");
    while(1);
  }
  Serial.println("Pisec готов");
  lcd.clear();
  lcd.print("Ready!");
  
  // Читаем последний номер лога из имён файлов на SD
  Serial.println("Scanning for log files...");
  spinner.start();
  
  unsigned long lastLogNumber = logger.findLastLogNumber();
  lineNumber = lastLogNumber + 1;
  
  Serial.print("Last log number: ");
  Serial.println(lastLogNumber);
  Serial.print("Starting with counter: ");
  Serial.println(lineNumber);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Counter set");
  lcd.setCursor(0, 1);
  lcd.print(lineNumber);
  delay(1500);
  spinner.stop();
  Serial.println("Готов к записи");
}

// spinner logic moved into Spinner class

void loop() {
  // Проверка интервала для записи данных и обновления экрана
  if (millis() - lastUpdate >= UPDATE_INTERVAL_MS) {
    lastUpdate = millis();
    // Синхронизируем countdown с текущим временем обновления
    countdown.setLastUpdateTime(lastUpdate);

    spinner.start();
    File f = logger.openCurrentLog(lineNumber);
    if (f) {
      lineNumber++;
      f.print(lineNumber);
      f.print(",");
      f.print(millis()/1000);
      f.print(",");
      f.println(analogRead(A0));   // example: humidity
      f.close();
      Serial.print("Line ");
      Serial.print(lineNumber);
      Serial.println(" written");
      
      // LCD buffer update (scroll up)
      lcdBuffer[3] = lcdBuffer[2];
      lcdBuffer[2] = lcdBuffer[1];
      lcdBuffer[1] = lcdBuffer[0];
      
      String newLine = "Line ";
      newLine += lineNumber;
      newLine += ": ";
      newLine += analogRead(A0);
      lcdBuffer[0] = newLine;
    }
    spinner.stop();
    // Шину НЕ освобождаем — мы главные по записи
    
    // Обновление LCD дисплея
    lcd.clear();
    for (int i = 0; i < 4; i++) {
      lcd.setCursor(0, i);
      lcd.print(lcdBuffer[i]);
    }
  }
  
  spinner.update();
  countdown.update();  // Вызываем постоянно, чтобы отсчёт обновлялся
}