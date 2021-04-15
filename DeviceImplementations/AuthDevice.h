#ifndef AUTH_DEVICE_H
#define AUTH_DEVICE_H
#include <AuthDeviceWrapper.h>
#include <commproto/logger/Logging.h>
#include <commproto/messages/TypeMapper.h>
#include <commproto/device/AuthChains.h>
#include <map>
#include <commproto/parser/MessageBuilder.h>

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

class LogToSerial : public commproto::logger::Loggable
{
public:
	LogToSerial(const commproto::serial::SerialHandle & serial, const uint32_t logMsgId);
	void addLog(const char* line, const uint32_t size) override;
	void close() override {}
	void open() override {}
private:
	commproto::serial::SerialHandle serial;
	const uint32_t logMsgId;
};
using LogHandle = std::shared_ptr<LogToSerial>;

struct IdProvider
{
	IdProvider(const commproto::messages::TypeMapperHandle &mapper)
		: deviceAuthId{ mapper->registerType<commproto::device::DeviceAuthRequestMessage>() }
		, finishScanId{ mapper->registerType<commproto::device::ScanFinished>() }
	{
	}
	const uint32_t deviceAuthId;
	const uint32_t finishScanId;
};

using ProviderHandle = std::shared_ptr<IdProvider>;

class AuthDevice
{
public:
	AuthDevice(AuthDeviceWrapper& device);
	void setup();
	void loop();
	void finishReading(const EndpointData& data, const std::string & name);
	void scan();
	void responseAccept(const std::string & name, const std::vector<std::string> & reply, const uint32_t port);
	void responseDeny(const std::string & name);
private:
	void scanNetworks();
	bool alreadyScanned(const std::string & name);
	AuthDeviceWrapper& device;
	bool finishedReading;
	EndpointData targetDevice;
	bool serviceConnected;
	commproto::serial::SerialHandle serial;
	commproto::parser::MessageBuilderHandle builder;
	commproto::messages::TypeMapperHandle mapper;
	ProviderHandle provider;
	LogHandle log;
	bool shouldScan;
	std::vector<std::string> previouslyScanned;
};


#endif // AUTH_DEVICE_H