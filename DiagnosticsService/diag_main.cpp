
#include <SocketImpl.h>
#include <commproto/logger/Logging.h>	
#include <commproto/service/Connection.h>
#include <commproto/config/ConfigParser.h>
#include <commproto/logger/FileLogger.h>
#include <commproto/device/AuthChains.h>
#include <chrono>
#include <commproto/service/ServiceChains.h>
#include <commproto/control/endpoint/UIController.h>
#include <commproto/control/endpoint/UIFactory.h>
#include <commproto/parser/ParserDelegatorUtils.h>
#include <commproto/control/endpoint/ParserDelegatorUtils.h>
#include <commproto/endpoint/DelegatorProvider.h>
#include <commproto/endpoint/ChannelParserDelegator.h>
#include <commproto/endpoint/ParserDelegatorFactory.h>
#include <commproto/service/DiagnosticChains.h>

using namespace commproto;

namespace ConfigValues
{
	static constexpr const char * const logToConsole = "logToConsole";
	static constexpr const bool logToConsoleDefault = true;

	static constexpr const char * const serverPort = "serverPort";
	static constexpr const int32_t defaultServerPort = 25565;

	static constexpr const char * const serverAddress = "serverAddress";
	static constexpr const char * const serverAddressDefault = "127.0.0.1";

	static constexpr const char * const channelNames = "channelNames";
};

using Labels = std::map<std::string, control::endpoint::LabelHandle>;

class ResponseHandler : public parser::Handler
{
public:
	ResponseHandler(const Labels & labels_)
		:labels{ labels_ }
	{

	}

	void handle(messages::MessageBase&& data) override
	{
		diagnostics::AllChannelsResponse & msg = static_cast<diagnostics::AllChannelsResponse&>(data);
		for (auto & label : labels)
		{
			auto it = std::find(msg.prop.begin(), msg.prop.end(), label.first);

			if (it == msg.prop.end())
			{
				label.second->setText("Down");
			}
			else
			{
				label.second->setText("Running");
			}
		}
	}
private:
	Labels labels;
};


parser::ParserDelegatorHandle buildSelfDelegator(const Labels & labels)
{
	std::shared_ptr<parser::ParserDelegator> delegator = std::make_shared<parser::ParserDelegator>();
	parser::DelegatorUtils::buildBase(delegator);
	return delegator;
}


class DiagnosticsProvider : public endpoint::DelegatorProvider {
public:
	DiagnosticsProvider(const messages::TypeMapperHandle & mapper_, const control::endpoint::UIControllerHandle & controller_, const Labels & labels_)
		: mapper{ mapper_ }
		, controller{ controller_ }
		, labels{ labels_ }
	{

	}
	parser::ParserDelegatorHandle provide(const std::string& name, const uint32_t id) override
	{
		parser::ParserDelegatorHandle delegator = buildSelfDelegator(labels);
		control::endpoint::DelegatorUtils::addParsers(delegator, controller);

		return delegator;
	}
private:
	messages::TypeMapperHandle mapper;
	control::endpoint::UIControllerHandle controller;
	Labels labels;
};



int main(int argc, const char * argv[]) {

	const char * configFile;
	if (argc <= 1)
	{
		configFile = "diag.cfg";
	}
	else
	{
		configFile = argv[1];
	}

	rapidjson::Document doc = config::ConfigParser(configFile).get();

	bool logToConsole = config::getValueOrDefault(doc, ConfigValues::logToConsole, ConfigValues::logToConsoleDefault);


	logger::FileLogger logger("diag_log_" + logger::FileLogger::getTimestamp() + ".txt");
	if (!logToConsole)
	{
		logger.open();
		logger::setLoggable(&logger);
	}

	const int32_t port = config::getValueOrDefault(doc, ConfigValues::serverPort, ConfigValues::defaultServerPort);
	const char * const address = config::getValueOrDefault(doc, ConfigValues::serverAddress, ConfigValues::serverAddressDefault);

	LOG_INFO("Diagnostics service attempting to connect to %s:%d", address, port);


	SenderMapping::InitializeName("Service::Diagnostics");
	sockets::SocketHandle socket = std::make_shared<sockets::SocketImpl>();
	if (!socket->initClient(address, port))
	{
		LOG_ERROR("A problem occurred while starting service, shutting down...");
		return 1;
	}

	LOG_INFO("Diagnostics started...");

	//send ptr size
	socket->sendByte(sizeof(void*));

	//core dependencies
	messages::TypeMapperHandle mapper = messages::TypeMapperFactory::build(socket);

	//registering our channel name
	uint32_t registerId = mapper->registerType<service::RegisterChannelMessage>();
	service::RegisterChannelMessage nameMsg(registerId, SenderMapping::getName());

	Message nameSerialized = service::RegisterChannelSerializer::serialize(std::move(nameMsg));
	socket->sendBytes(nameSerialized);

	//requesting connected channels
	uint32_t reqChannelID = mapper->registerType<diagnostics::RequestAllConnections>();


	// ui definition
	auto uiFactory = std::make_shared<control::endpoint::UIFactory>("myUI", mapper, socket);
	control::endpoint::UIControllerHandle controller = uiFactory->makeController();


	std::map<std::string, std::string> channels = commproto::config::getValueOrDefault(doc, ConfigValues::channelNames, std::map<std::string, std::string>{});;
	Labels labels;
	for (const auto & channel : channels)
	{
		auto label = uiFactory->makeLabel(channel.first, "Down");
		controller->addControl(label);
		labels.emplace(channel.second, label);
	}

	auto memUsageLabel = uiFactory->makeLabel("Memory Usage", "0/0 KB");
	auto memUsage = uiFactory->makeProgresBar("");

	controller->addControl(memUsageLabel);
	controller->addControl(memUsage);

	auto procUsageLabel = uiFactory->makeLabel("CPU Usage", "0 %");
	auto procUsage = uiFactory->makeProgresBar("");

	controller->addControl(procUsageLabel);
	controller->addControl(procUsage);

	//delegator to parse incoming messages
	std::shared_ptr<DiagnosticsProvider> provider = std::make_shared<DiagnosticsProvider>(mapper, controller,labels);
	endpoint::ChannelParserDelegatorHandle channelDelegator = std::make_shared<endpoint::ChannelParserDelegator>(provider);
	parser::ParserDelegatorHandle delegator = endpoint::ParserDelegatorFactory::build(channelDelegator);
	channelDelegator->addDelegator(0, delegator);
	parser::DelegatorUtils::addParserHandlerPair<diagnostics::AllChannelsResponseParser, diagnostics::AllChannelsResponse>(delegator, std::make_shared<ResponseHandler>(labels));
	parser::MessageBuilderHandle builder = std::make_shared<parser::MessageBuilder>(socket, channelDelegator);

	LOG_INFO("Waiting to acquire ID");
	//wait until we are sure we have an ID
	do
	{
		builder->pollAndRead();
	} while (SenderMapping::getId() == 0);
	LOG_INFO("Acquired ID: %d", SenderMapping::getId());


	Message reqSerialized = diagnostics::RequestAllConnectionsSerializer::serialize(diagnostics::RequestAllConnections(reqChannelID));

	auto now = std::chrono::system_clock::now().time_since_epoch();
	auto then = now;
	const uint32_t update = 500;

	while (true)
	{
		builder->pollAndReadTimes(100);
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		now = std::chrono::system_clock::now().time_since_epoch();
		if (std::chrono::duration_cast<std::chrono::milliseconds>(now - then).count() >= update)
		{
			then = now;
			//update all labels
			socket->sendBytes(reqSerialized);
		}
	}
	socket->shutdown();
	return 0;
}
