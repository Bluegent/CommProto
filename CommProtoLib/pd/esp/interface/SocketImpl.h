#ifndef COMMPROTO_ESP_SOCKETIMPL_H
#define COMMPROTO_ESP_SOCKETIMPL_H

#ifdef ESP32
    #include <WiFi.h>
#elif defined(ESP8266)
    #include <ESP8266WiFi.h>
#endif
#include <commproto/sockets/Socket.h>
#include <commproto/common/Common.h>
#include <commproto/logger/Logging.h>

namespace commproto
{
    namespace sockets
    {

    enum class Mode : uint8_t {
        Unassigned,
        Server,
        Client
    };

class SocketImpl : public Socket {
private:
    WiFiClient client;
    WiFiServer server;
    Mode socketMode;
    bool isInitialized;
public:
    SocketImpl() 
      : server(0)
      , socketMode(Mode::Unassigned)
      , isInitialized(false)
    {
    }

    void setTimeout(const uint32_t msec) override
    {
    }

    bool connected() override{
        return client.connected();
    }
       
    void shutdown() override{
        isInitialized = false;
        socketMode = Mode::Unassigned;
        client.stop();
    }
    
    int32_t sendBytes(const Message& message) override
    {
        if (!isInitialized || socketMode == Mode::Unassigned)
        {
          return 0;
        }
        int ret = client.write(reinterpret_cast<const uint8_t*>(message.data()),message.size());
        return ret;
    }
    
    int32_t sendByte(const char byte) override
    {
        if (!isInitialized || socketMode == Mode::Unassigned)
        {
          return 0;
        }
        int ret = client.write(reinterpret_cast<const uint8_t*>(&byte),1);
        return ret;
    }
        

    //block the thread until the socket receives [size] bytes
    int32_t receive(Message& message, const uint32_t size) override{
        if (!isInitialized || socketMode != Mode::Client)
        {
          return 0;
        }

        message.reserve(size);
        return client.read(reinterpret_cast<uint8_t*>(message.data()),size);
    }

    //poll the socket to see how many bytes are buffered
    int32_t available() override{
        if (!isInitialized || socketMode != Mode::Client)
        {
          return 0;
        }
        return client.available();
    }

    //block the thread until single byte of data can be read from the socket
    int32_t readByte() override
    {
       if (!isInitialized || socketMode != Mode::Client)
        {
          return -1;
        }
        
        return client.read();
    }

    //initialize the socket as a client and connect it to [addr]:[port]
    bool initClient(const std::string& addr, const uint32_t port) override
    {
        if (isInitialized || socketMode != Mode::Unassigned)
        {
          return false;
        }

        bool result = client.connect(addr.c_str(),static_cast<uint16_t>(port));
        if(result){
            socketMode = Mode::Client;
            isInitialized = true;
        }
        return result;
    }

    //initialize the socket as a server and bind it to [port]
    bool initServer(const std::string& addr, const uint32_t port) override
    {
        if (isInitialized || socketMode != Mode::Unassigned)
        {
          return false;
        }

        socketMode = Mode::Server;
#ifdef ESP8266
        server.begin(static_cast<uint16_t>(port),1);
#else
        server.begin(static_cast<uint16_t>(port));
#endif
        
        return isInitialized;
    }

    //block the thread until a connection is available, then accept it
    SocketHandle acceptNext() override{
       if (isInitialized || socketMode != Mode::Server)
        {
          return SocketHandle();
        }

      WiFiClient possibleClient = server.available();
      if(!possibleClient){
        return SocketHandle();
      }
      return SocketHandle(new SocketImpl(possibleClient));
    }
    private:

    SocketImpl(WiFiClient client_)
      : server(0)
      , client(client_)
      , socketMode(Mode::Client)
      , isInitialized(true)
    {
    }
};


    }
}
#endif
