#include <SocketImpl.h>

#include <commproto/authdevice/AuthDeviceWrapper.h>
#include <commproto/endpointdevice/BaseEndpointWrapper.h>
#include <commproto/authdevice/AuthDevice.h>
#include <commproto/endpointdevice/BaseEndpointAuth.h>
#include <commproto/thermo/Thermostat.h>
#include <thread>


class WindowsAuthDeviceWrapper : public commproto::authdevice::AuthDeviceWrapper
{
public:
	std::vector<std::string> listNetworks() override { return std::vector<std::string>(); }
	commproto::sockets::SocketHandle connectTo(const commproto::authdevice::ConnectionData& data) override { return nullptr; }
	commproto::serial::SerialHandle getSerial(const int speed) override {return nullptr; }
	void setLED(const bool on) override{}
	void delayT(const uint32_t delay) override{}
	void reboot() override {}
	uint32_t getMs() override { return 0; }
};


class WindowsEpWrapper : public commproto::endpointdevice::BaseEndpointWrapper
{
public:
	commproto::stream::StreamHandle getStream(const int speed) override { return nullptr; }
	bool hasAuth() override {
		return true;
	}

	commproto::authdevice::ConnectionData getAuthData() override 
	{
		commproto::authdevice::ConnectionData data;
		data.ssid = "eh";
		data.password = "meh";
		data.addr = "localhost";
		data.port = 25565;
		return data;
	}
	commproto::sockets::SocketHandle startAsAP(const commproto::authdevice::ConnectionData& data) override { return nullptr; }
	void saveAPData(const commproto::authdevice::ConnectionData& data) override {}
	void delayT(uint32_t msec) override { std::this_thread::sleep_for(std::chrono::milliseconds(msec)); }
	void reboot() override {}
	bool readAPData() override { 
		return true; 
	}
	void initFs() override{}
	void resetAPData() override{}
	commproto::sockets::SocketHandle connect(const commproto::authdevice::ConnectionData& data, const uint32_t attempts) override
	{
		commproto::sockets::SocketHandle client = std::make_shared<commproto::sockets::SocketImpl>();
		client->initClient(data.addr, data.port);
		return client;
	}

	uint32_t getFreeMem() override { return 0; }
	void tickStatusLED() override{}
	uint32_t getResetBtnCount() override { return 0; }
	void readResetButton() override {};
};

class WindowsThermo : public commproto::thermo::ThermostateWrapper
{
public:
	void setup() override{}
	void loop() override{}
	float getTemp() override { return 13.f; }
	float getHumidity() override { return 13.f; }
	void toggleTempAdjust(float intensity) override {}
	void toggleAutoTempAdjust(const bool on) override{}
	void setDesiredTemp(const float temp) override{}
	uint32_t getMs() override { return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count(); }
};

int main(int argc, const char * argv[])
{
	WindowsAuthDeviceWrapper wrapper;
	commproto::authdevice::AuthDevice dev(wrapper);

	WindowsEpWrapper epauth;
	commproto::endpointdevice::BaseEndpointAuth stat(epauth,{ "Thermostat","Commproto","A simple device that provides data about temperature, humidity and the possibility to start heating." });

	WindowsThermo thermostat;
	commproto::thermo::Thermostat thermoDevice(epauth, { "Thermostat","Commproto","A simple device that provides data about temperature, humidity and the possibility to start heating." },thermostat);
	thermoDevice.setup();
	while(true)
	{
		thermoDevice.loop();
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}