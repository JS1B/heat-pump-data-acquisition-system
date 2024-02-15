#include <Arduino.h>
#include <DFRobot_I2C_Multiplexer.h>
#include <DFRobot_AHT20.h>
#include <DFRobot_RGBLCD1602.h>
#include <string.h>

#define AHT20_COUNT 2
#define TIME_BETWEEN_MEASUREMENTS 2000

#define DEBUG 1

// Own code
#include "src/sensor.h"
#include "src/logger.h"

DFRobot_I2C_Multiplexer I2CMulti(&Wire, 0x70);
DFRobot_AHT20 aht20[AHT20_COUNT];
DFRobot_RGBLCD1602 lcd(0x2D, 16, 2);
Logger logger(512);

bool builtInLedState = false;

void setup()
{
  Serial.begin(57600);
  // logger.begin(57600);

  pinMode(LED_BUILTIN, OUTPUT);

  // AHT20 sensor related
  for (int i = 0; i < AHT20_COUNT; i++)
  {
    I2CMulti.selectPort(i);
    uint8_t status;
    while ((status = aht20[i].begin()) != 0)
    {
      // logger.addMsg("AHT20 sensor initialization failed. error status : ");
      // logger.addMsg(status);
      // logger.log();
      if (DEBUG)
      {
        Serial.print("AHT20 ");
        Serial.print(i);
        Serial.print(" sensor initialization failed. error status: ");
        Serial.println(status);
      }
      delay(3000);
    }

    if (DEBUG)
    {
      Serial.print("AHT20 ");
      Serial.print(i);
      Serial.println(" sensor initialization success");
    }

    // logger.addMsg("AHT20 ");
    // logger.addMsg(i);
    // logger.addMsg(" sensor initialization success");
    // logger.log();
  }

  // LCD related
  I2CMulti.selectPort(6);
  lcd.init();

  if (DEBUG)
  {
    Serial.print("LCD initialization... ");

    Serial.println("success");
  }
  // logger.log();

  // Print the header
  Serial.println("Czas(s), Przeplyw(L/min), Temp(C)_1, Hum(%RH)_1, Temp(C)_2, Hum(%RH)_2");
  // logger.addMsg("Czas, Przeplyw(L/min), ");
  // logger.addMsg("Temperature(C)_1, Humidity(%RH)_1, ");
  // logger.addMsg("Temperature(C)_2, Humidity(%RH)_2");
  // logger.log();
}

void loop()
{
  // logger.addMsg(String(millis()).c_str());
  Serial.print(float(millis() / 1000));
  Serial.print(", TODO");

  // AHT20 sensor related
  for (int i = 0; i < AHT20_COUNT; i++)
  {
    I2CMulti.selectPort(i);
    if (aht20[i].startMeasurementReady(true))
    {
      Serial.print(", ");
      Serial.print(aht20[i].getTemperature_C());
      Serial.print(", ");
      Serial.print(aht20[i].getHumidity_RH());

      // LCD related
      I2CMulti.selectPort(6);
      lcd.setCursor(0, i);
      lcd.print("S");
      lcd.print(i);
      lcd.print(" ");
      lcd.print(aht20[i].getTemperature_C());
      lcd.print("C ");
      lcd.print(aht20[i].getHumidity_RH());
      lcd.print("%");
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
