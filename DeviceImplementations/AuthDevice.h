#ifndef AUTH_DEVICE_H
#define AUTH_DEVICE_H
#include <AuthDeviceWrapper.h>
#include <commproto/parser/Handler.h>

namespace commproto {namespace messages {
	class MessageBase;
}
}

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


class AuthDevice
{
public:
	AuthDevice(AuthDeviceWrapper* device);
	void setup();
	void loop();
	void finishReading(const EndpointData& data);
private:
	AuthDeviceWrapper* device;
	bool finishedReading;
	EndpointData targetDevice;
};

class DeviceDataHandler : public commproto::parser::Handler
{
public:
	DeviceDataHandler(AuthDevice* device_);
	void handle(commproto::messages::MessageBase&& data) override;
public:
	AuthDevice* device;
};

#endif // AUTH_DEVICE_H