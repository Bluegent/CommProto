
#include <commproto/authdevice/AuthDeviceWrapper.h>
#include <commproto/endpointdevice/BaseEndpointWrapper.h>
#include <commproto/authdevice/AuthDevice.h>
#include <commproto/endpointdevice/BaseEndpointAuth.h>

class WindowsAuthDeviceWrapper : public commproto::authdevice::AuthDeviceWrapper
{
public:
	std::vector<std::string> listNetworks() override { return std::vector<std::string>(); }
	commproto::sockets::SocketHandle connectTo(const commproto::authdevice::ConnectionData& data) override { return nullptr; }
	commproto::serial::SerialHandle getSerial(const int speed) override {return nullptr; }
	void setLED(const bool on) override{}
	void delayT(const uint32_t delay) override{}
};


class WindowsThermo : public commproto::endpointdevice::BaseEndpointWrapper
{
public:
	commproto::stream::StreamHandle getStream(const int speed) override { return nullptr; }
	bool hasAuth() override {
		return false;
	}

	commproto::authdevice::ConnectionData getAuthData() override { return commproto::authdevice::ConnectionData(); }
	commproto::sockets::SocketHandle startAsAP(const commproto::authdevice::ConnectionData& data) override { return nullptr; }
	commproto::sockets::SocketHandle connect(const commproto::authdevice::ConnectionData& data) override { return nullptr; }
	void saveAPData(const commproto::authdevice::ConnectionData& data) override {}
	void delayT(uint32_t msec) override { }
	void reboot() override {}
	bool readAPData() override { return false; }
	void initFs() override{};
};

int main(int argc, const char * argv[])
{
	WindowsAuthDeviceWrapper wrapper;
	commproto::authdevice::AuthDevice dev(wrapper);

	WindowsThermo thermo;
	commproto::endpointdevice::BaseEndpointAuth stat(thermo,{ "Thermostat","Commproto","A simple device that provides data about temperature, humidity and the possibility to start heating." });
}