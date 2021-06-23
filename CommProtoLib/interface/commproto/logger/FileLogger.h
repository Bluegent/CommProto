#ifndef FILE_LOGGER_H
#define FILE_LOGGER_H
#include <commproto/logger/Logging.h>
#include <fstream>

namespace commproto
{
    namespace logger
    {
		class FileLogger : public Loggable
		{
		public:
			FileLogger(const std::string & fileName, const bool alwaysFlush = true);
			bool isOpen() const;
			void addLog(const char* line, const uint32_t size) override;
			void close() override;
			~FileLogger() override;

			static std::string getTimestamp();

			void open() override;
		private:
			std::string fileName;
			std::ofstream file;
			bool alwaysFlush;
		};

		using FileLoggerHandle = std::shared_ptr<FileLogger>;
    }
}

#endif // FILE_LOGGER_H