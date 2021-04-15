#ifndef LOGGER_H
#define LOGGER_H
#include <string.h>
#include <stdarg.h>
#include <string>
#include <memory>

namespace commproto
{
	namespace logger
	{
		class Loggable
		{
		public:
			virtual ~Loggable() = default;
			virtual void addLog(const char * line, const uint32_t size) = 0;
			virtual void close() = 0;
			virtual void open() = 0;
		};

		void setLoggable(Loggable* loggable);
		void closeLoggable();


		void log(const char * format, ...);
	}


}
#define FORMAT "\t[%s() in %s(line %d)]\n"


#if defined(ESP8266) || defined(ESP32)
#define ARD_FORMAT "\t[%s() line %d]"
#define LOG_SOURCE(format,...) commproto::logger::log(format, ##__VA_ARGS__)
#define LOG_MACRO(format,...)  commproto::logger::log(format ARD_FORMAT,##__VA_ARGS__, __func__, __LINE__)
#define LOG_ERROR(format,...) LOG_MACRO("[ERR] " format,##__VA_ARGS__)
#define LOG_INFO(format,...) LOG_MACRO("[INF] " format, ##__VA_ARGS__)
#define LOG_WARNING(format,...) LOG_MACRO("[WRN] " format,##__VA_ARGS__)
#define LOG_DEBUG(format,...) LOG_MACRO("[DBG] " format,##__VA_ARGS__)
#elif defined(_MSC_VER)
#define LOG_SOURCE(format,...) commproto::logger::log(format, __VA_ARGS__)
#define LOG_MACRO(format,...) commproto::logger::log(format FORMAT,__VA_ARGS__,__FUNCTION__,strrchr(__FILE__,'\\')+1,__LINE__)
#define LOG_ERROR(format,...) LOG_MACRO("[ERR] " format,__VA_ARGS__)
#define LOG_INFO(format,...) LOG_MACRO("[INF] " format,__VA_ARGS__)
#define LOG_WARNING(format,...) LOG_MACRO("[WRN] " format,__VA_ARGS__)
#define LOG_DEBUG(format,...) LOG_MACRO("[DBG] " format,__VA_ARGS__)
#elif defined(__GNUC__)
#define LOG_SOURCE(format,...) commproto::logger::log(format, ##__VA_ARGS__)
#define LOG_MACRO(format,...) commproto::logger::log(format FORMAT,##__VA_ARGS__,__PRETTY_FUNCTION__,strrchr(__FILE__,'/')+1,__LINE__)
#define LOG_ERROR(format,...) LOG_MACRO("[ERR] " format,##__VA_ARGS__)
#define LOG_INFO(format,...) LOG_MACRO("[INF] " format,##__VA_ARGS__)
#define LOG_WARNING(format,...) LOG_MACRO("[WRN] " format,##__VA_ARGS__)
#define LOG_DEBUG(format,...) LOG_MACRO("[DBG] " format,##__VA_ARGS__)   
#endif




#endif
