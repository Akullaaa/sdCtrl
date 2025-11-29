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
const unsigned long UPDATE_INTERVAL_MS = 7000; // мс

// Spinner и Countdown вынесены в отдельные файлы
#include "spinner.h"
#include "countdown.h"

// Создаём экземпляры спиннера и обратного отсчёта времени
Spinner spinner(lcd, 19, 3);
Countdown countdown(lcd, 19, 0);

void setup() {
  Serial.begin(9600);
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);         // сначала отпускаем шину

  // Инициализация LCD
  lcd.init();
  lcd.backlight();
  lcd.print("Pisec init...");

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
  
  // Читаем последнюю строку из файла и продолжаем счётчик
  Serial.println("Читаю счетчик из файла...");
  spinner.start();
  File f = sd.open("datalog.csv", FILE_READ);
  if (f) {
    unsigned long fileSize = f.fileSize();
    Serial.print("Размер файла: ");
    Serial.print(fileSize);
    Serial.println(" байт");
    
    if (fileSize > 0) {
      // Читаем с начала файла
      f.seekSet(0);
      
      String line = "";
      while (f.available()) {
        spinner.update();
        char c = f.read();
        if (c == '\n') {
          // Парсим строку (первое число до запятой)
          int commaPos = line.indexOf(',');
          if (commaPos > 0) {
            String lineStr = line.substring(0, commaPos);
            lineNumber = lineStr.toInt();
          }
          line = "";  // Очищаем для следующей строки
        } else if (c != '\r') {
          line += c;
        }
      }
      
      // Обрабатываем последнюю строку, если нет символа новой строки в конце
      if (line.length() > 0) {
        int commaPos = line.indexOf(',');
        if (commaPos > 0) {
          String lineStr = line.substring(0, commaPos);
          lineNumber = lineStr.toInt();
        }
      }
      
      Serial.print("Счетчик установлен на: ");
      Serial.println(lineNumber);
    } else {
      Serial.println("Файл пуст, начинаю счет с 0");
    }
    f.close();
  } else {
    Serial.println("Файл не найден, начинаю счет с 0");
  }
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
    File f = sd.open("datalog.csv", FILE_WRITE);
    if (f) {
      lineNumber++;
      f.print(lineNumber);
      f.print(",");
      f.print(millis()/1000);
      f.print(",");
      f.println(analogRead(A0));   // пример: влажность
      f.close();
      Serial.print("Строка ");
      Serial.print(lineNumber);
      Serial.println(" записана");
      
      // Обновление LCD буфера (прокрутка вверх)
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