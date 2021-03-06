#ifndef AUTH_SERVICE_UI_IMPL_H
#define AUTH_SERVICE_UI_IMPL_H
#include "AuthServiceUI.h"
#include <commproto/control/endpoint/UIController.h>
#include <commproto/control/endpoint/ProgressBar.h>
#include <commproto/control/endpoint/Button.h>
#include <commproto/control/endpoint/Notification.h>
#include <commproto/sockets/Socket.h>
#include <commproto/messages/TypeMapper.h>
#include "AuthService.h"
#include "commproto/control/endpoint/Label.h"

class AuthServiceUIImpl : public AuthServiceUI
{
public:
	AuthServiceUIImpl(const commproto::sockets::SocketHandle & socket_, const commproto::messages::TypeMapperHandle & mapper_,  AuthService & service_);
	void scan() override;
	void updateProgress(const uint32_t current) override;
	void startScan(const uint32_t total) override;
	commproto::control::endpoint::UIControllerHandle build() override;
	void scanFinished() override;
	void notifyAuthRequest(const std::string& text, const std::string& name) override;
	void response(const std::string& name, const bool accept) override;
private:
	commproto::sockets::SocketHandle socket;
	commproto::messages::TypeMapperHandle mapper;
	AuthService & service;
	commproto::control::endpoint::UIControllerHandle controller;
	commproto::control::endpoint::ProgressBarHandle bar;
	commproto::control::endpoint::LabelHandle count;
	commproto::control::endpoint::NotificationHandle scanInProgress;
	commproto::control::endpoint::ButtonHandle button;
	commproto::control::endpoint::NotificationHandle authorizeEndpoint;
	uint32_t currentTotal;
	static const std::string & yesString;
	static const std::string & noString;
};

#endif //AUTH_SERVICE_UI_IMPL_H