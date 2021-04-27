#ifndef AUTH_SERVICE_UI_H
#define AUTH_SERVICE_UI_H
#include <stdint.h>
#include <memory>
#include <commproto/control/endpoint/UIController.h>


class AuthServiceUI
{
public:
	virtual ~AuthServiceUI() = default;
	virtual commproto::control::endpoint::UIControllerHandle build() = 0;
	virtual void scan() = 0;
	virtual void updateProgress(const uint32_t current) = 0;
	virtual void startScan(const uint32_t total) = 0;
	virtual void scanFinished() = 0;
	virtual void notifyAuthRequest(const std::string & text, const std::string & name) = 0;
	virtual void response(const std::string & name, const bool accept) = 0;
};

using UIHandle = std::shared_ptr<AuthServiceUI>;
#endif //AUTH_SERVICE_UI_H