#include "DeviceWrapper.h"

#include <commproto/logger/Logging.h>
#include <commproto/variable/ContextImpl.h>
#include <commproto/parser/ParserDelegatorFactory.h>
#include <TypeMapperImpl.h>
#include <commproto/variable/VariableMappingMessage.h>

DeviceWrapper::DeviceWrapper(SDKWrapper* device_, const char* host_, uint32_t port_)
	: client{ std::make_shared<commproto::sockets::SocketImpl>() }
	, host{ host_ }
	, port{ port_ }
	, device{ device_ }
	, state{ DeviceState::Searching }
{
}


class A
{
	
};

class B: public A
{
	
};
void DeviceWrapper::setTemp(float temp_)
{
    if(temp){
       *temp = temp_;
    }
}

void DeviceWrapper::setup()
{
}
DeviceState DeviceWrapper::loop()
{
	switch (state)
	{
	case DeviceState::Searching:
	{
		LOG_INFO("Attempting to connect to %s:%d", host.c_str(), port);
		bool connected = client->initClient(host, port);
		if (!connected)
		{
			device->delayMs(5000);
		}
		LOG_INFO("Connection established to %s:%d", host.c_str(), port);
		state = DeviceState::Initializing;
	}
	break;
	case DeviceState::Initializing:
	{
        
		messages::TypeMapperObserverHandle observer = std::make_shared<messages::TypeMapperObserver>(client);
		messages::TypeMapperHandle mapper = std::make_shared<messages::TypeMapperImpl>(observer);
        
		ctx = std::make_shared<variable::ContextImpl>(client,mapper->registerType<variable::VariableMessage>(), mapper->registerType<variable::VariableMappingMessage>());
        
		parser::ParserDelegatorHandle delegator = parser::ParserDelegatorFactory::build(ctx);
		builder = std::make_shared<parser::MessageBuilder>(client, delegator);
		variable::VariableCallback cb = std::bind(&SDKWrapper::setLED,device,std::placeholders::_1);
		ctx->subscribe("LED", cb);
        temp = std::make_shared<variable::RealVariable>(ctx,0.0f);
        ctx->registerOutVariable(temp,"TempC");
		state = DeviceState::Connected;
		LOG_INFO("Initialized all necessary thingies, beginning communication");
	}
	break;

	case DeviceState::Connected:
	{
		builder->pollAndRead();
	}
	break;

	default:;
	}
	return state;
}