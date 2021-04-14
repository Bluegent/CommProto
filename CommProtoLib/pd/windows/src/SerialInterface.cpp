#include "../interface/SerialInterface.h"
#include <commproto/logger/Logging.h>

namespace commproto
{
	namespace serial
	{

		SerialInterface::~SerialInterface()
		{
			SerialInterface::shutdown();
		}

		SerialInterface::SerialInterface()
			: serialPort{ nullptr }
		{

		}

		int32_t SerialInterface::sendBytes(const Message &message)
		{
			if (!connected())
			{
				return -1;
			}
			DWORD bytes = 0;
			bool success = ::WriteFile(serialPort, message.data(), message.size(), &bytes, NULL);
			if (success)
			{
				return bytes;
			}
			return -1;
		}

		int32_t SerialInterface::receive(Message &message, const uint32_t size)
		{
			if (!connected())
			{
				return -1;
			}

			message.reserve(size);
			message.resize(size);
			DWORD bytes = 0;
			bool success = ::ReadFile(serialPort, message.data(), message.size(), &bytes, NULL);
			if (success)
			{
				return bytes;
			}
			return -1;
		}

		int32_t SerialInterface::available()
		{
			if (!connected())
			{
				return -1;
			}
			DWORD errors = 0;
			COMSTAT status;
			bool success = ClearCommError(serialPort, &errors, &status);
			if(!success)
			{
				return -1;
			}
			
			return status.cbInQue;
		}

		int SerialInterface::readByte()
		{
			if (!connected())
			{
				return -1;
			}
			DWORD bytes = 0;
			char output = 0;
			bool success = ::ReadFile(serialPort, &output, 1, &bytes, NULL);
			if (success)
			{
				return output;
			}
			return -1;
		}

		int SerialInterface::sendByte(const char byte)
		{
			if (!connected())
			{
				return -1;
			}
			DWORD bytes = 0;
			bool success = ::WriteFile(serialPort, &byte, 1, &bytes, NULL);
			if (success)
			{
				return bytes;
			}
			return -1;
		}

		bool SerialInterface::start(const std::string &addr, const uint32_t speed)
		{
			if (connected())
			{
				return false;
			}

			const char * const addrStr = addr.c_str();
			
			serialPort = CreateFileA(addrStr,
				GENERIC_READ | GENERIC_WRITE,
				0,      //  must be opened with exclusive-access
				NULL,   //  default security attributes
				OPEN_EXISTING, //  must use OPEN_EXISTING
				0,      //  not overlapped I/O
				NULL); //  hTemplate must be NULL for comm devices

			if (serialPort == INVALID_HANDLE_VALUE)
			{
				LOG_ERROR("Could not open serial port \"%s\"", addrStr);
				serialPort = nullptr;
				return false;
			}
			DCB dcb;
			SecureZeroMemory(&dcb, sizeof(DCB));
			dcb.DCBlength = sizeof(DCB);
			BOOL fSuccess = GetCommState(serialPort, &dcb);

			if (!fSuccess)
			{
				LOG_ERROR("Could not get serial port \"%s\" state", addrStr);
				serialPort = nullptr;
				return false;
			}

			dcb.BaudRate = speed;		//  baud rate
			dcb.ByteSize = 8;           //  data size, xmit and rcv
			dcb.Parity = NOPARITY;      //  parity bit
			dcb.StopBits = ONESTOPBIT;  //  stop bit

			fSuccess = SetCommState(serialPort, &dcb);

			if (!fSuccess)
			{
				LOG_ERROR("Could not set serial port \"%s\" state", addrStr);
				serialPort = nullptr;
				return false;
			}

			return true;

		}

		bool SerialInterface::connected()
		{
			return serialPort != nullptr;
		}

		void SerialInterface::shutdown()
		{
			if (!connected())
			{
				return;
			}
			CloseHandle(serialPort);
			serialPort = nullptr;
		}

	}
}
