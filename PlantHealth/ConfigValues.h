#ifndef CONFIG_VALUES_H
#define CONFIG_VALUES_H

namespace ConfigValues
{
	static constexpr const char * const logToConsole = "logToConsole";
	static constexpr const bool logToConsoleDefault = true;

	static constexpr const char * const serverPort = "serverPort";
	static constexpr const int32_t defaultServerPort = 25565;

	static constexpr const char * const serverAddress = "serverAddress";
	static constexpr const char * const serverAddressDefault = "127.0.0.1";

	static constexpr const char * const targetEpName = "targetEpName";
	static constexpr const char * const targetEpNameDefault = "Endpoint::SmartPot";

};


#endif