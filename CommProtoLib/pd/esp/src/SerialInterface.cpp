#include <SerialInterface.h>
#include "Arduino.h"

namespace commproto
{
namespace serial
{
    int32_t SerialInterface::sendBytes(const Message &message) 
    {
        return Serial.write(message.data(),message.size());
    }
    
    int32_t SerialInterface::receive(Message &message, const uint32_t size) 
    {
        message.resize(size);
        message.reserve(size);
        return Serial.readBytes(message.data(),message.size());
    }
    
    int32_t SerialInterface::available()  
    {
        return Serial.available();
    }
    
    int SerialInterface::readByte() 
    {
        char byte = Serial.read();
        return byte;
    }
    
    int SerialInterface::sendByte(const char byte) 
    {
        return Serial.write(byte);
    }
    
    bool SerialInterface::start(const int speed)
    {
        Serial.begin(speed);
    }
    
    bool SerialInterface::connected() 
    {
        return Serial;
    }
    void SerialInterface::shutdown() 
    {
        Serial.end();
    };
    SerialInterface::~SerialInterface()
    {
        shutdown();
    }
}
}
