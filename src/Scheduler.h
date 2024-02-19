#pragma once
#include <Arduino.h>

typedef void (*OperationFunction)();

void scheduleOperation(unsigned long &lastExecutionTime, unsigned long interval, OperationFunction operation);