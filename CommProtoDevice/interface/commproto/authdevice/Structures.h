#ifndef DEVICE_STRUCTURES_H
#define DEVICE_STRUCTURES_H
#include <string>


namespace commproto
{
	namespace authdevice
	{
		struct EndpointData
		{
			void reset()
			{
				name = std::string{};
				manufacturer = std::string{};
				description = std::string{};
			}
			std::string name;
			std::string manufacturer;
			std::string description;
		};

		struct ConnectionData
		{
			std::string ssid;
			std::string password;
			std::string addr;
			uint32_t port;
			static const ConnectionData defaultData;
		};


	}
}


#endif //DEVICE_STRUCTURES_H