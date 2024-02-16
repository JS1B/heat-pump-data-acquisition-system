#include "Logger.h"

Logger::Logger(long baudRate) {
    this->baudRate = baudRate;
}

void Logger::begin() {
    Serial.begin(baudRate);
    #ifdef USBCON
    // Only wait for Serial connection if the board has USB communication
    while (!Serial) {
        ; // Wait for Serial port to connect.
    }
    #endif
}
