#include "Error.h"

void Error(const char* str) {
    Serial.print("ERROR: ");
    Serial.println(str);
    Serial.flush();
    while (1) {
        delay(10);
    }
}

void assert(const bool condition) {
    if (!condition) {
        Error("Runtime assert failed");
    }
}
