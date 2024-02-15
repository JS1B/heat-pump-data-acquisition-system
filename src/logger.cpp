#include "logger.h"

Logger::Logger(int buffer_size) {
  this->buffer = new char[buffer_size];
  this->buffer[0] = '\0';
  this->buffer_size = buffer_size;
}

Logger::~Logger() {
  delete[] this->buffer;
}

void Logger::begin(int baudrate) {
  Serial.begin(baudrate);
  while(!Serial){
    delay(100);
  }
  delay(1000); // wait for console and data logger
  Serial.println("Serial communication initialized");
}

void Logger::addMsg(const char *message) {
  int currentLength = strlen(this->buffer);
  int messageLength = strlen(message);
  if (currentLength + messageLength < this->buffer_size) {
    strcat(this->buffer, message);
  }
}

void Logger::log() {
  Serial.println(this->buffer);
  this->buffer[0] = '\0';
}