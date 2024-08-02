#include <Arduino.h>
#include <DFRobot_I2C_Multiplexer.h>
#include <DFRobot_AHT20.h>
#include <DFRobot_RGBLCD1602.h>
#include <GravityRtc.h>
#include <string.h>
#include <math.h>

#define AHT20_COUNT 5

// Constants
#define TIME_OF_POWER_STABILIZATION 400
#define TIME_OF_SPLASH_SCREEN 2000
#define TIME_BETWEEN_ERROR_RETRY 1200
#define TIME_BETWEEN_LED_BLINK 1000
#define TIME_BETWEEN_MEASUREMENTS 1200000
#define TIME_BETWEEN_LCD_CYCLES 3000

#define MUX_LCD 6
#define MUX_AHT20_FIRST 0
#define MUX_RTC 5

#define LCD_ROWS 2

///////////////
#define DEBUG 0
///////////////

#if DEBUG
#define DEBUG_PRINT(...) logger.print(__VA_ARGS__); logger.flush()
#define DEBUG_PRINTLN(...) logger.println(__VA_ARGS__); logger.flush()
#else
#define DEBUG_PRINT(...)
#define DEBUG_PRINTLN(...)
#endif

// Own code
#include "src/Logger.h"
#include "src/Scheduler.h"

DFRobot_I2C_Multiplexer I2CMulti(&Wire, 0x70);
DFRobot_AHT20 aht20[AHT20_COUNT];
DFRobot_RGBLCD1602 lcd(0x2D, 16, 2);
GravityRtc rtc;

#if DEBUG
Logger logger(57600); // 256
#else
Logger logger(57600);
#endif

bool builtInLedState = false;

unsigned long lastLedBlinkTime = 0;
unsigned long lastMeasurementTime = TIME_BETWEEN_MEASUREMENTS;
unsigned long lastLcdCycleTime = 0;

const uint8_t lcdCycles = ceil((float)AHT20_COUNT / LCD_ROWS);
uint8_t lcdCycleCurrent = 0;

// Functions
void blinkLed();
void measure();
void display();
void updateCycle();

void setup();
void loop();

void setup() {
  // Wait for power to stabilize
  delay(TIME_OF_POWER_STABILIZATION / 2);

  // Builtin LED related
  pinMode(LED_BUILTIN, OUTPUT);

  // Logger related
  logger.begin();

  DEBUG_PRINTLN("Starting...");

  // I2C Multiplexer related
  DEBUG_PRINT("I2C Multiplexer initialization... ");
  I2CMulti.begin();

  DEBUG_PRINTLN("done.");

  // LCD related
  DEBUG_PRINT("LCD initialization... ");

  I2CMulti.selectPort(MUX_LCD);
  lcd.init();
  delay(TIME_OF_POWER_STABILIZATION / 2);
  lcd.setRGB(20, 20, 20);

  lcd.setCursor(0, 0);
  lcd.print("Starting...");
  lcd.setCursor(0, 1);
  lcd.print("AHT20 sensors: ");
  lcd.print(AHT20_COUNT);

  DEBUG_PRINTLN("done.");

  // AHT20 sensor related
  for (int i = 0; i < AHT20_COUNT; i++) {
    uint8_t status;

    DEBUG_PRINT("AHT20 ");
    DEBUG_PRINT(i);
    DEBUG_PRINT(" sensor initialization... ");

    I2CMulti.selectPort(MUX_AHT20_FIRST + i);
    while ((status = aht20[i].begin()) != 0) {
      DEBUG_PRINT("failed. error status: ");
      DEBUG_PRINTLN(status);

      delay(TIME_BETWEEN_ERROR_RETRY);
    }

    DEBUG_PRINTLN("done.");
  }

  // RTC related
  DEBUG_PRINT("RTC initialization... ");

  I2CMulti.selectPort(MUX_RTC);
  rtc.setup();
  // rtc.adjustRtc(F(__DATE__), F(__TIME__)); // Run once to set the RTC time

  DEBUG_PRINTLN("done.");

  // Print header
  logger.print("Czas(s)");
  for (int i = 0; i < AHT20_COUNT; i++) {
    logger.print(String(", Temp(C)_" + String(i + 1) + ", Hum(%RH)_" + String(i + 1)).c_str());
  }
  logger.println("");
  logger.flush();

  delay(TIME_OF_SPLASH_SCREEN);
  I2CMulti.selectPort(MUX_LCD);
  lcd.clear();

  I2CMulti.selectPort(8); // Disable all channels
}

void loop() {
  // Measurement related
  scheduleOperation(lastMeasurementTime, TIME_BETWEEN_MEASUREMENTS, measure);

  // LCD related
  scheduleOperation(lastLcdCycleTime, TIME_BETWEEN_LCD_CYCLES, updateCycle);

  // Builtin LED related
  scheduleOperation(lastLedBlinkTime, TIME_BETWEEN_LED_BLINK, blinkLed);
}

// Procedures
void measure() {
  // RTC related
  I2CMulti.selectPort(MUX_RTC);
  char dateTimeString[20];
  rtc.read();
  sprintf(dateTimeString, "%04d-%02d-%02d %02d:%02d:%02d",
          rtc.year, rtc.month, rtc.day, rtc.hour, rtc.minute, rtc.second);
  logger.print(dateTimeString);

  // AHT20 sensor related
  char tempStr[8], humStr[8], buffer[20];
  for (int i = 0; i < AHT20_COUNT; i++) {
    I2CMulti.selectPort(MUX_AHT20_FIRST + i);

    if (aht20[i].startMeasurementReady(true)) {
      // Convert float to string with 2 decimal places
      dtostrf(aht20[i].getTemperature_C(), 6, 2, tempStr);
      dtostrf(aht20[i].getHumidity_RH(), 6, 2, humStr);

      // Format the entire message into a buffer
      sprintf(buffer, ", %s, %s", tempStr, humStr);
      
      logger.print(buffer);
    }
    else {
      logger.print(", ERROR, ERROR");
    }
  }
  logger.print("\n");
  logger.flush();

  display();
  I2CMulti.selectPort(8); // Disable all channels
}

void display() {
  I2CMulti.selectPort(MUX_LCD);
  lcd.clear();

  for (int i = 0; i < LCD_ROWS; i++) {
    uint8_t sensorIndex = lcdCycleCurrent * LCD_ROWS + i;
    if (sensorIndex >= AHT20_COUNT) break;

    I2CMulti.selectPort(MUX_AHT20_FIRST + sensorIndex);
    char tempStr[6], humStr[6], buffer[16];
    dtostrf(aht20[sensorIndex].getTemperature_C(), 3, 1, tempStr);
    dtostrf(aht20[sensorIndex].getHumidity_RH(), 3, 1, humStr);
    sprintf(buffer, "%d: %sC %s%%", sensorIndex, tempStr, humStr);

    I2CMulti.selectPort(MUX_LCD);
    lcd.setCursor(0, i);
    lcd.print(buffer);
  }
  I2CMulti.selectPort(8); // Disable all channels
}

void updateCycle() {
  if (lcdCycles <= 1) return;

  lcdCycleCurrent = (lcdCycleCurrent + 1) % lcdCycles;
}

void blinkLed() {
  // DEBUG_PRINTLN("Blinking LED");
  builtInLedState = !builtInLedState;
  digitalWrite(LED_BUILTIN, builtInLedState);
}