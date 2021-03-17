#include "Error.h"

#include "Message.h"

void Error(const char* str) {
    Message message = {};
    message.type = Message::Type::Error;
    Serial.write((const uint8_t*)&message, sizeof(message));
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
