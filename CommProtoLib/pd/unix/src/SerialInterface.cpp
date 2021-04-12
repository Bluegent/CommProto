#include "../interface/SerialInterface.h"
#include <commproto/logger/Logging.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

namespace commproto
{
namespace serial
{
    void SerialInterface::setTimeout(const uint32_t msec)
    {

    }

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

    int32_t SerialInterface::pollSocket()
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

    bool SerialInterface::initClient(const std::string &addr, const uint32_t speed)
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
        return true;

    }

    bool SerialInterface::initServer(const std::string &addr, const uint32_t port)
    {
        return false;
    }

    sockets::SocketHandle SerialInterface::acceptNext()
    {
        return nullptr;
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
