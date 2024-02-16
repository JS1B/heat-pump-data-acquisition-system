#pragma once
#include <Arduino.h>

class Logger {
public:
    Logger(long baudRate = 9600);
    void begin();
    
    template <typename T>
    void print(const T& message);

    template <typename T>
    void println(const T& message);

private:
    long baudRate;
};

#include "Logger.tpp"
