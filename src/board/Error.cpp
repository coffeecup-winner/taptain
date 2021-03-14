#include "Error.h"

void Error(const char* str) {
    Serial.print("ERROR: ");
    Serial.println(str);
    while (1);
}

void assert(const bool condition) {
    if (!condition) {
        Error("Runtime assert failed");
    }
}
