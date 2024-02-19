#include "Scheduler.h"

void scheduleOperation(unsigned long &lastExecutionTime, unsigned long interval, OperationFunction operation) {
    unsigned long currentMillis = millis();
    if (currentMillis - lastExecutionTime >= interval) {
        lastExecutionTime = currentMillis;
        operation(); // Execute the operation
    }
}