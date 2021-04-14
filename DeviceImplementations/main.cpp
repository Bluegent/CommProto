#include "AuthDevice.h"
#include "ThermostatDevice.h"

class WindowsAuthDeviceWrapper : public AuthDeviceWrapper
{
public:
	std::vector<std::string> listNetworks() override { return std::vector<std::string>(); }
	void setBaudRate(const uint32_t bytes) override {};
	commproto::sockets::SocketHandle connectTo(const std::string& ssid, const std::string& pwd, const std::string& addr, const uint32_t port) override { return nullptr; };
};


class WindowsThermo : public ThermostatWrapper
{
public:
	commproto::stream::StreamHandle getStream(uint32_t speed) override { return nullptr; }
	bool hasAuth() override {
		return false;
	}
	APData getAuthData() override { return APData(); }
	commproto::sockets::SocketHandle startAsAP(const APData& data) override { return nullptr; }
	commproto::sockets::SocketHandle connect(const APData& data) override { return nullptr; }
	void saveAPData(const APData& data) override {}
	void delay(uint32_t msec) override { }
	void reboot() override {}
	int32_t getTemp() override { return  0; }
};

int main(int argc, const char * argv[])
{
	WindowsAuthDeviceWrapper wrapper;
	AuthDevice dev(wrapper);

	WindowsThermo thermo;
	Thermostat stat(thermo);
}