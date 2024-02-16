#include <Arduino.h>
#include <DFRobot_I2C_Multiplexer.h>
#include <DFRobot_AHT20.h>
#include <DFRobot_RGBLCD1602.h>
#include <GravityRtc.h>
#include <string.h>

#define AHT20_COUNT 2

#define TIME_BETWEEN_MEASUREMENTS 2000
#define TIME_BETWEEN_ERROR_RETRY 3000

#define MUX_LCD 6
#define MUX_AHT20_FIRST 0
#define MUX_RTC 5

///////////////
#define DEBUG 1
///////////////

#if DEBUG
#define DEBUG_PRINT(...) Serial.print(__VA_ARGS__)
#define DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#define DEBUG_PRINTLN(...)
#endif

// Own code
#include "src/sensor.h"
#include "src/logger.h"

DFRobot_I2C_Multiplexer I2CMulti(&Wire, 0x70);
DFRobot_AHT20 aht20[AHT20_COUNT];
DFRobot_RGBLCD1602 lcd(0x2D, 16, 2);
GravityRtc rtc;
Logger logger(512);

bool builtInLedState = false;
char buffer[16];

void setup()
{
  Serial.begin(57600);
  // logger.begin(57600);

  pinMode(LED_BUILTIN, OUTPUT);

  // LCD related
  DEBUG_PRINT("LCD initialization... ");

  I2CMulti.selectPort(MUX_LCD);
  lcd.init();
  lcd.setRGB(20, 20, 20);
  lcd.setCursor(0, 0);
  lcd.print("Starting...");
  lcd.setCursor(0, 1);
  lcd.print("AHT20 sensors: ");
  lcd.print(AHT20_COUNT);

  DEBUG_PRINTLN("done.");

  // AHT20 sensor related
  for (int i = 0; i < AHT20_COUNT; i++)
  {
    uint8_t status;

    DEBUG_PRINT("AHT20 ");
    DEBUG_PRINT(i);
    DEBUG_PRINT(" sensor initialization... ");

    I2CMulti.selectPort(MUX_AHT20_FIRST + i);
    while ((status = aht20[i].begin()) != 0)
    {
      // logger.addMsg("AHT20 sensor initialization failed. error status : ");
      // logger.addMsg(status);
      // logger.log();

      DEBUG_PRINT("failed. error status: ");
      DEBUG_PRINTLN(status);

      delay(TIME_BETWEEN_ERROR_RETRY);
    }

    DEBUG_PRINTLN("done.");

    // logger.addMsg("AHT20 ");
    // logger.addMsg(i);
    // logger.addMsg(" sensor initialization success");
    // logger.log();
  }

  // RTC related
  DEBUG_PRINT("RTC initialization... ");

  I2CMulti.selectPort(MUX_RTC);
  rtc.setup();
  // rtc.adjustRtc(F(__DATE__), F(__TIME__)); // Run once to set the RTC time

  DEBUG_PRINTLN("done.");

  // logger.log();

  // Print header
  Serial.println("Czas(s), Przeplyw(L/min), Temp(C)_1, Hum(%RH)_1, Temp(C)_2, Hum(%RH)_2");
  // logger.addMsg("Czas, Przeplyw(L/min), ");
  // logger.addMsg("Temperature(C)_1, Humidity(%RH)_1, ");
  // logger.addMsg("Temperature(C)_2, Humidity(%RH)_2");
  // logger.log();
  delay(TIME_BETWEEN_MEASUREMENTS);
}

void loop()
{
  // logger.addMsg(String(millis()).c_str());
  I2CMulti.selectPort(MUX_RTC);
  char dateTimeString[20];
  rtc.read();
  sprintf(dateTimeString, "%04d-%02d-%02d %02d:%02d:%02d", 
            rtc.year, rtc.month, rtc.day, rtc.hour, rtc.minute, rtc.second);
  Serial.print(dateTimeString);

  Serial.print(", TODO");

  // AHT20 sensor related
  for (int i = 0; i < AHT20_COUNT; i++)
  {
    I2CMulti.selectPort(MUX_AHT20_FIRST + i);
    if (aht20[i].startMeasurementReady(true))
    {
      Serial.print(", ");
      Serial.print(aht20[i].getTemperature_C());
      Serial.print(", ");
      Serial.print(aht20[i].getHumidity_RH());

      // LCD related
      char tempStr[8], humStr[8];
      dtostrf(aht20[i].getTemperature_C(), 4, 2, tempStr);
      dtostrf(aht20[i].getHumidity_RH(), 4, 2, humStr);
      sprintf(buffer, "S%d %sC %s%%", i, tempStr, humStr);

      I2CMulti.selectPort(MUX_LCD);
      lcd.setCursor(0, i);
      lcd.print(buffer);
      // logger.addMsg(", ");
      // logger.addMsg(String(aht20[i].getTemperature_C()).c_str());
      // logger.addMsg(", ");
      // logger.addMsg(String(aht20[i].getHumidity_RH()).c_str());
    }
  }

  // builtin LED related
  builtInLedState = !builtInLedState;
  digitalWrite(LED_BUILTIN, builtInLedState);

  Serial.println();
  // logger.log();
  delay(TIME_BETWEEN_MEASUREMENTS);
}
