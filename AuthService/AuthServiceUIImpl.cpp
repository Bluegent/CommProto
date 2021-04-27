#include "AuthServiceUIImpl.h"
#include <commproto/control/endpoint/UIFactory.h>



AuthServiceUIImpl::AuthServiceUIImpl(const commproto::sockets::SocketHandle& socket_, const commproto::messages::TypeMapperHandle& mapper_, AuthService& service_)
	: socket{ socket_ }
	, mapper{ mapper_ }
	, service( service_ )
{
}

void AuthServiceUIImpl::scan()
{
	if(service.isScanning())
	{
		if(!scanInProgress)
		{
			return;
		}
		controller->displayNotification(scanInProgress->getId(), "A scan is already in progress.",[](const std::string& option) {});
	}
	service.scan();
}

void AuthServiceUIImpl::updateProgress(const uint32_t current)
{
	if (!bar)
	{
		return;
	}
	bar->setProgress(current);
}

void AuthServiceUIImpl::startScan(const uint32_t total)
{
	if(!bar)
	{
		return;
	}
	if(total == 0 )
	{
		if (!scanInProgress)
		{
			return;
		}
		controller->displayNotification(scanInProgress->getId(), "No devices were detected.", [](const std::string& option) {});
		return;
	}
	controller->setControlShownState(bar->getId(), true);
	bar->setProgress(0);
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

	scanInProgress = factory.makeNotification("Scanning...");
	scanInProgress->addOption("Ok");
	controller->addNotification(scanInProgress);
	controller->setControlShownState(bar->getId(), false);
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
