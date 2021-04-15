#include <commproto/logger/Logging.h>
#if defined(ESP32) || defined(ESP8266)
#include "Arduino.h"
#endif
#include <stdio.h>


namespace commproto
{
	namespace logger
	{
		Loggable* logDest;

		void setLoggable(Loggable* loggable)
		{
			logDest = loggable;
		}

		void closeLoggable()
		{
			if(logDest)
			{
				logDest->close();
				logDest = nullptr;
			}
		}







		void log(const char * format, ...)
		{


			if (logDest)
			{
				static char buf[1024] = { 0 };
				int printed = 0;
				va_list args;
				va_start(args, format);
				printed = vsprintf(buf, format, args);
				va_end(args);
				logDest->addLog(buf, printed);
			}
			else 
			{
#if defined(ESP32) || defined(ESP8266)

				static char buf[1024] = { 0 };
				int printed = 0;
				va_list args;
				va_start(args, format);
				printed = vsprintf(buf, format, args);
				va_end(args);
#ifdef ESP32
				Serial.write(reinterpret_cast<uint8_t*>(buf), printed);
#else 
				Serial.write(buf, printed);
#endif
				Serial.println();
#endif
#if WIN32
				va_list args;
				va_start(args, format);
				vprintf(format, args);
				va_end(args);
#endif
			}
		}

	}
}