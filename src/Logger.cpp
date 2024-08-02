#include "Logger.h"

Logger::Logger(long baudRate, size_t bufferSize)
    : baudRate(baudRate), bufferSize(bufferSize), bufferIndex(0)
{

    this->buffer = new char[bufferSize];
    if (this->buffer == nullptr)
    {
        Serial.println("Failed to allocate memory for the logger buffer");
        while (true)
        {
            delay(5000);
        }
    }

    this->buffer[0] = '\0';
}

Logger::~Logger()
{
    delete[] buffer;
}

void Logger::begin()
{
    Serial.begin(this->baudRate);
#ifdef USBCON
    // Only wait for Serial connection if the board has USB communication
    while (!Serial)
    {
        delay(10); // Wait for Serial port to connect.
    }
#endif
}

void Logger::print(const char *message) {
    size_t messageLength = strlen(message);
    if (this->bufferIndex + messageLength >= this->bufferSize) {
        this->flush();
    }

    if (messageLength < this->bufferSize) {
        strcat(this->buffer + this->bufferIndex, message);
        this->bufferIndex += messageLength;
    }
}

void Logger::println(const char *message)
{
    this->print(message);
    if (this->bufferIndex + 2 >= this->bufferSize) { 
        // For newline and null terminator
        this->flush();
    }
   
    strcat(this->buffer + this->bufferIndex, "\n");
    this->bufferIndex += 1;
}

void Logger::flush(void)
{
    if (this->bufferIndex <= 0)
        return;

    Serial.print(this->buffer);
    memset(this->buffer, 0, this->bufferSize); // Clear the entire buffer
    this->bufferIndex = 0;
}
