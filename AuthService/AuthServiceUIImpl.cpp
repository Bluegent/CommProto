#include "AuthServiceUIImpl.h"
#include <commproto/control/endpoint/UIFactory.h>

const std::string & AuthServiceUIImpl::noString = "No";
const std::string & AuthServiceUIImpl::yesString = "Yes";

AuthServiceUIImpl::AuthServiceUIImpl(const commproto::sockets::SocketHandle& socket_, const commproto::messages::TypeMapperHandle& mapper_, AuthService& service_)
	: socket{ socket_ }
	, mapper{ mapper_ }
	, service(service_)
	, currentTotal(0)
{
}

void AuthServiceUIImpl::scan()
{
	if (service.isScanning())
	{
		if (!scanInProgress)
		{
			return;
		}
		controller->displayNotification(scanInProgress->getId(), "A scan is already in progress.", [](const std::string& option) {});
	}
	service.scan();
	if (!bar)
	{
		return;
	}
	bar->setProgress(0);
	controller->setControlShownState(bar->getId(), true);
}

void AuthServiceUIImpl::updateProgress(const uint32_t current)
{
	if (!bar)
	{
		return;
	}
	float currentF = current;
	float totalF = currentTotal;
	uint32_t precentage = 10 + static_cast<uint32_t>(currentF / totalF * 90.f);
	bar->setProgress(precentage);
}

void AuthServiceUIImpl::startScan(const uint32_t total)
{
	if (!bar)
	{
		return;
	}
	if (total == 0)
	{
		if (!scanInProgress)
		{
			return;
		}
		controller->displayNotification(scanInProgress->getId(), "No devices were detected.", [](const std::string& option) {});
		return;
	}
	currentTotal = total;
	bar->setProgress(10);
}

commproto::control::endpoint::UIControllerHandle AuthServiceUIImpl::build()
{
	if (!socket || !mapper)
	{
		return controller;
	}
	auto factory = commproto::control::endpoint::UIFactory("UI", mapper, socket);
	controller = factory.makeController();
	button = factory.makeButton("Scan", std::bind(&AuthServiceUIImpl::scan, this));
	controller->addControl(button);

	bar = factory.makeProgresBar("Scan progres");
	controller->addControl(bar);
	controller->setControlShownState(bar->getId(), false);


	scanInProgress = factory.makeNotification("Scanning...");
	scanInProgress->addOption("Ok");
	controller->addNotification(scanInProgress);


	authorizeEndpoint = factory.makeNotification("Allow endpoint");
	authorizeEndpoint->addOption(yesString);
	authorizeEndpoint->addOption(noString);
	controller->addNotification(authorizeEndpoint);

	return controller;

}

void AuthServiceUIImpl::scanFinished()
{
	if (!bar)
	{
		return;
	}
	controller->setControlShownState(bar->getId(), false);
}

void AuthServiceUIImpl::notifyAuthRequest(const std::string& text, const std::string& name)
{
	if (!authorizeEndpoint)
	{
		return;
	}

	controller->displayNotification(authorizeEndpoint->getId(), text, [&name, this](const std::string& option)
	{
		if (option == yesString)
		{
			response(name, true);
		}
		else
		{
			response(name, false);
		}
	});

}

void AuthServiceUIImpl::response(const std::string& name, const bool accept)
{
	if (accept)
	{
		service.accept(name);
	}
	else
	{
		service.reject(name);
	}
}
