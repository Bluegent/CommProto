#include "AuthDevice.h"


class WindowsAuthDeviceWrapper : public AuthDeviceWrapper
{
public:
	std::vector<std::string> listNetworks() override { return std::vector<std::string>(); }
	void setBaudRate(const uint32_t bytes) override {};
	commproto::sockets::SocketHandle connectTo(const std::string& ssid, const std::string& pwd, const std::string& addr, const uint32_t port) override { return nullptr; };
};

int main(int argc, const char * argv[])
{
	WindowsAuthDeviceWrapper wrapper;
	AuthDevice dev(&wrapper);
}