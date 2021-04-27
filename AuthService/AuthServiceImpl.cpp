#include "AuthServiceImpl.h"
#include <commproto/device/AuthChains.h>
#include <commproto/logger/Logging.h>
#include <commproto/parser/ParserDelegatorUtils.h>
#include <commproto/logger/LoggingMessage.h>
#include <thread>
#include <commproto/control/endpoint/ParserDelegatorUtils.h>
#include <commproto/endpoint/DelegatorProvider.h>
#include <commproto/service/ServiceChains.h>

#include "AuthServiceHandlers.h"
#include "AuthServiceUIImpl.h"
#include "commproto/endpoint/ParserDelegatorFactory.h"

//for device
commproto::parser::ParserDelegatorHandle build(const AuthServiceHandle & service)
{
	commproto::parser::ParserDelegatorHandle delegator = std::make_shared<commproto::parser::ParserDelegator>();
	commproto::parser::DelegatorUtils::buildBase(delegator);
	commproto::parser::DelegatorUtils::addParserHandlerPair<commproto::logger::LogParser, commproto::logger::LogMessage>(delegator, std::make_shared<commproto::logger::LogHandler>());
	commproto::parser::DelegatorUtils::addParserHandlerPair<commproto::device::DeviceAuthRequestParser, commproto::device::DeviceAuthRequestMessage>(delegator, std::make_shared<DeviceReqHandler>(service));
	commproto::parser::DelegatorUtils::addParserHandlerPair<commproto::device::ScanFinishedParser, commproto::device::ScanFinished>(delegator, std::make_shared<ScanFinishedHandler>(service));
	commproto::parser::DelegatorUtils::addParserHandlerPair<commproto::device::KeepAliveParser, commproto::device::KeepAlive>(delegator, std::make_shared<KeepAliveHandler>(service));
	commproto::parser::DelegatorUtils::addParserHandlerPair<commproto::device::ScanProgressParser, commproto::device::ScanProgress>(delegator, std::make_shared<ScanProgressHandler>(service));
	commproto::parser::DelegatorUtils::addParserHandlerPair<commproto::device::ScanStartedParser, commproto::device::ScanStarted>(delegator, std::make_shared<ScanStartHandler>(service));

	return delegator;
}

//for dispatch
commproto::parser::ParserDelegatorHandle buildSelfDelegator()
{
	std::shared_ptr<commproto::parser::ParserDelegator> delegator = std::make_shared<commproto::parser::ParserDelegator>();
	commproto::parser::DelegatorUtils::buildBase(delegator);
	return delegator;
}


class EndpointProvider : public commproto::endpoint::DelegatorProvider {
public:
	EndpointProvider(const commproto::messages::TypeMapperHandle & mapper_, const commproto::control::endpoint::UIControllerHandle & controller_)
		: mapper{ mapper_ }
		, controller{ controller_ }
	{

	}

	commproto::parser::ParserDelegatorHandle provide(const std::string& name, const uint32_t id) override
	{
		commproto::parser::ParserDelegatorHandle delegator = buildSelfDelegator();
		commproto::control::endpoint::DelegatorUtils::addParsers(delegator, controller);

		return delegator;
	}
private:
	commproto::messages::TypeMapperHandle mapper;
	commproto::control::endpoint::UIControllerHandle controller;
};


IdProvider::IdProvider(const commproto::messages::TypeMapperHandle& mapper)
	: scanId{ mapper->registerType<commproto::device::ScanForNetworksMessage>() }
	, authorizeId{ mapper->registerType<commproto::device::DeviceAuthAccept>() }
	, rejectId{ mapper->registerType<commproto::device::DeviceAuthReject>() }
	, keepAliveId{ mapper->registerType<commproto::device::KeepAlive>() }
{
}


AuthServiceImpl::AuthServiceImpl(const commproto::stream::StreamHandle& stream_, const commproto::sockets::SocketHandle& socket_)
	: scanning(false)
	, stream{ stream_ }
	, socket{ socket_ }
{
}

void AuthServiceImpl::scan()
{
	if (!provider)
	{
		return;
	}
	if (scanning)
	{
		return;
	}
	scanning = true;
	commproto::Message scan = commproto::device::ScanForNetworksSerializer::serialize(std::move(commproto::device::ScanForNetworksMessage(provider->scanId)));
	stream->sendBytes(scan);
}

void AuthServiceImpl::setScanFinished()
{
	scanning = false;
}

void AuthServiceImpl::handleRequest(const APData& data)
{
	LOG_INFO("Name: \"%s\" (ssid:%s)", data.name.c_str(), data.ssid.c_str());
	LOG_INFO("Manufacturer: \"%s\"", data.manufacturer.c_str());
	LOG_INFO("Description: \"%s\"", data.description.c_str());
	accept(data.ssid);
}

void AuthServiceImpl::accept(const std::string& name)
{
	if (!provider)
	{
		return;
	}
	std::vector<std::string> props;
	props.push_back("EstiNebun"); //ssid of hub
	props.push_back("01LMS222"); //password for hub
	props.push_back("192.168.1.2"); //dispatch address
	uint32_t port = 25565; //dispatch port
	commproto::Message accept = commproto::device::DeviceAuthAcceptSerializer::serialize(std::move(commproto::device::DeviceAuthAccept(provider->authorizeId, name, props, port)));
	stream->sendBytes(accept);
}

void AuthServiceImpl::reject(const std::string& name)
{
	if (!provider)
	{
		return;
	}
	commproto::Message reject = commproto::device::DeviceAuthRejectSerializer::serialize(std::move(commproto::device::DeviceAuthReject(provider->rejectId, name)));
	stream->sendBytes(reject);
}

void AuthServiceImpl::sendPong()
{
	if (!provider)
	{
		return;
	}
	commproto::Message reject = commproto::device::KeepAliveSerializer::serialize(std::move(commproto::device::KeepAlive(provider->keepAliveId)));
	stream->sendBytes(reject);
}

void AuthServiceImpl::scanStarted(const uint32_t scanAmount)
{
	if(!ui)
	{
		return;
	}
	ui->startScan(scanAmount);
}

void AuthServiceImpl::scanProgress(const uint32_t complete)
{
	if(!ui)
	{
		return;
	}
	ui->updateProgress(complete);
}

void AuthServiceImpl::initializeDevice()
{
	LOG_INFO("Initializing connection with authentification device");
	if(!stream)
	{
		return;
	}

	int res = stream->sendByte(sizeof(void*));
	if (res != 1)
	{
		LOG_ERROR("An issue occurred when sending sizeof pointer on host system");
	}


	provider = std::make_shared<IdProvider>(commproto::messages::TypeMapperFactory::build(stream));
	deviceDelegator = build(shared_from_this());
	deviceBuilder = std::make_shared<commproto::parser::MessageBuilder>(stream, deviceDelegator);

	do
	{
		deviceBuilder->pollAndReadTimes(100);
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	} while (!deviceDelegator->hasMapping<commproto::logger::LogMessage>());
	LOG_INFO("Connection with authentification device established");
}



void AuthServiceImpl::initializeDispatch()
{
	LOG_INFO("Initializing connection with dispatch service");
	if(!socket)
	{
		return;
	}




	commproto::SenderMapping::InitializeName("Service::Authentification");
	socket->sendByte(sizeof(void*));



	dispatchMapper = commproto::messages::TypeMapperFactory::build(socket);

	ui = std::make_shared<AuthServiceUIImpl>(socket, dispatchMapper, *this);
	auto controller = ui->build();

	dispatchDelegator = std::make_shared<commproto::endpoint::ChannelParserDelegator>(std::make_shared<EndpointProvider>(dispatchMapper, controller));
	dispatchBuilder = std::make_shared<commproto::parser::MessageBuilder>(socket, dispatchDelegator);

	commproto::parser::ParserDelegatorHandle initialDelegator = commproto::endpoint::ParserDelegatorFactory::build(dispatchDelegator);
	dispatchDelegator->addDelegator(0, initialDelegator);

	//registering channel name
	uint32_t registerId = dispatchMapper->registerType<commproto::service::RegisterChannelMessage>();
	commproto::service::RegisterChannelMessage nameMsg(registerId, commproto::SenderMapping::getName());
	commproto::Message nameSerialized = commproto::service::RegisterChannelSerializer::serialize(std::move(nameMsg));
	socket->sendBytes(nameSerialized);

	do
	{
		dispatchBuilder->pollAndReadTimes(100);
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	} while (commproto::SenderMapping::getId() == 0);
}

void AuthServiceImpl::loopBlocking()
{
	while(true)
	{

		if(deviceBuilder)
		{
			deviceBuilder->pollAndReadTimes(100);
		}

		if(dispatchBuilder)
		{
			dispatchBuilder->pollAndReadTimes(100);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}
