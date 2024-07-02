#pragma once
#include <Arduino.h>

class Logger {
public:
    Logger(long baudRate = 9600, size_t bufferSize = 128);
    ~Logger();
    void begin();
    
    void print(const char* message);
    void println(const char* message);

    void flush(void);

private:
    long baudRate;
    char *buffer;
    size_t bufferSize;
    size_t bufferIndex;
};
