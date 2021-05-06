#include <commproto/logger/Logging.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "../interface/SerialInterface.h"

namespace commproto
{
namespace serial
{

    SerialInterface::~SerialInterface()
    {
        shutdown();
    }

    SerialInterface::SerialInterface()
        : serialPort{-1}
    {

    }

    int32_t SerialInterface::sendBytes(const Message &message)
    {
        if(!connected())
        {
            return -1;
        }
         return write(serialPort, message.data(), message.size());
    }

    int32_t SerialInterface::receive(Message &message, const uint32_t size)
    {
        if(!connected())
        {
            return -1;
        }

        message.reserve(size);
        return read(serialPort, message.data(), size);
    }

    int32_t SerialInterface::available()
    {
        if(!connected())
        {
            return -1;
        }
        int count;
        ioctl(serialPort, FIONREAD, &count);
        return count;
    }

    int SerialInterface::readByte()
    {
        if(!connected())
        {
            return -1;
        }
        char output;
        int res = read(serialPort, &output, 1);
        if(res!=1){
            return -1;
        }
        return output;
    }

    int SerialInterface::sendByte(const char byte)
    {
        if(!connected())
        {
            return -1;
        }
        return write(serialPort,&byte,1);
    }

    bool SerialInterface::start(const std::string &addr, const uint32_t speed)
    {
        if(connected())
        {
            return false;
        }
        serialPort = open(addr.c_str(), O_RDWR);
        if(serialPort < 0)
        {
            LOG_ERROR("Failed to open serial port on \"%s\"",addr.c_str());
            return false;
        }
        struct termios tty;
        if(tcgetattr(serialPort, &tty) != 0) {
            LOG_ERROR("Failed to open serial port on \"%s\"",addr.c_str());
            return false;
        }

        cfsetspeed(&tty, speed);
        ioctl(serialPort, TCFLSH, 2);
        return true;

    }

    bool SerialInterface::connected()
    {
        return serialPort >= 0;
    }

    void SerialInterface::shutdown()
    {
        if(!connected())
        {
            return;
        }
        close(serialPort);
        serialPort = -1;
    }

}
}
