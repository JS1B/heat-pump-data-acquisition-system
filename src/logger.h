#pragma once
#include <Arduino.h>

#ifndef DEBUG
#define DEBUG 0
#endif // DEBUG

class Logger
{
public:
  Logger(int buffer_size);
  ~Logger();
  void begin(int baudrate = 57600);
  void addMsg(const char *message);
  void log();

private:
  char *buffer;
  int buffer_size;
};
