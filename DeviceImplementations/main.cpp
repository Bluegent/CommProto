#include "AuthDevice.h"
#include "ThermostatDevice.h"

class WindowsAuthDeviceWrapper : public AuthDeviceWrapper
{
public:
	std::vector<std::string> listNetworks() override { return std::vector<std::string>(); }
	commproto::sockets::SocketHandle connectTo(const std::string& ssid, const std::string& pwd, const std::string& addr, const uint32_t port) override { return nullptr; }
	commproto::serial::SerialHandle getSerial(const int speed) override {return nullptr; }
	void setLED(const bool on) override{}
	void delayT(const uint32_t delay) override{}
};


class WindowsThermo : public ThermostatWrapper
{
public:
	commproto::stream::StreamHandle getStream(const int speed) override { return nullptr; }
	bool hasAuth() override {
		return false;
	}
	APData getAuthData() override { return APData(); }
	commproto::sockets::SocketHandle startAsAP(const APData& data) override { return nullptr; }
	commproto::sockets::SocketHandle connect(const APData& data) override { return nullptr; }
	void saveAPData(const APData& data) override {}
	void delayT(uint32_t msec) override { }
	void reboot() override {}
	int32_t getTemp() override { return  0; }
};

int main(int argc, const char * argv[])
{
	WindowsAuthDeviceWrapper wrapper;
	AuthDevice dev(wrapper);

	WindowsThermo thermo;
	BaseEndpointAuth stat(thermo);
}