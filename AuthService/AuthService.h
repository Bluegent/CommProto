#ifndef AUTH_SERVICE_H
#define AUTH_SERVICE_H
#include <memory>
#include <string>

struct APData
{
	std::string ssid;
	std::string name;
	std::string manufacturer;
	std::string description;
};

class AuthService
{
public:
	virtual ~AuthService() = default;
	virtual void scan() = 0;
	virtual void setScanFinished() = 0;
	virtual void handleRequest(const APData& data) = 0;
	virtual void accept(const std::string& name) = 0;
	virtual void reject(const std::string& name) = 0;
	virtual void sendPong() = 0;
	virtual void scanStarted(const uint32_t scanAmount) = 0;
	virtual void scanProgress(const uint32_t complete) = 0;
};

using AuthServiceHandle = std::shared_ptr<AuthService>;

#endif //AUTH_SERVICE_H