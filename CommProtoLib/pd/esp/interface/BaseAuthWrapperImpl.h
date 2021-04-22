#ifndef BASE_AUTH_WRAPPER_IMPL_H
#define BASE_AUTH_WRAPPER_IMPL_H

#ifdef ESP32
    #include <WiFi.h>
    #include <SPIFFS.h>
#elif defined(ESP8266)
    #include <ESP8266WiFi.h>
    #include <LittleFS.h>
#endif
#include <commproto/endpointdevice/BaseEndpointWrapper.h>
#include <SocketImpl.h>
#include <SerialInterface.h>
#include <Fs.h>

#include <string>

IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

namespace commproto
{
	namespace endpointdevice
	{

		class BaseAuthWrapperImpl : public BaseEndpointWrapper
		{
		public:
             
            uint32_t getFreeMem()
            {
                return ESP.getFreeHeap();
            }
            void initFs() override
            {
                
#ifdef ESP32                
                if(!SPIFFS.begin(true))        
#elif defined(ESP8266)
                if(!LittleFS.begin()) 
#endif
                {
                    Serial.println("Could not mount filesystem");
                }
            }
            
			BaseAuthWrapperImpl()
				: readAuth(false)
			{

			}
			commproto::stream::StreamHandle getStream(const int speed) override {
				commproto::serial::SerialHandle serial = std::make_shared<commproto::serial::SerialInterface>();
				serial->start(speed);
				return serial;
			}
			bool hasAuth() override {
				return readAuth;
			}
			authdevice::ConnectionData getAuthData() override {
				return apData;
			}

			commproto::sockets::SocketHandle startAsAP(const authdevice::ConnectionData& data) override
			{  
                             
				Serial.println();
				Serial.println("Starting as wifi access point");
				WiFi.mode(WIFI_AP);
				IPAddress local;

				local.fromString(data.addr.c_str());
				WiFi.softAP(data.ssid.c_str(), data.password.c_str());
				WiFi.softAPConfig(local, gateway, subnet);

				IPAddress IP = WiFi.softAPIP();
				Serial.print("AP IP address: ");
				Serial.println(IP);

				commproto::sockets::SocketHandle socket = std::make_shared<commproto::sockets::SocketImpl>();
				socket->initServer("", data.port);
				return socket;
			}

			commproto::sockets::SocketHandle connect(const authdevice::ConnectionData& data, const uint32_t attempts) override
			{
                WiFi.mode(WIFI_STA);
                
				LOG_INFO("Attempting to connect to wifi network \"%s\"", data.ssid.c_str());
				int status = WL_IDLE_STATUS;
				int attempt = 0;
				WiFi.begin(data.ssid.c_str(), data.password.c_str());
				do {
					LOG_INFO("Attempt #%d connecting to wifi", attempt + 1);
					status = WiFi.waitForConnectResult();
                    if(status!=WL_CONNECTED)
                    {
                        delay(6000);
                    }
					++attempt;
				} while (status != WL_CONNECTED && attempt != attempts);

				if (status != WL_CONNECTED)
				{
					LOG_INFO("Connection attempt unsuccesful with code %d", status);
					return nullptr;
				}
                attempt = 0;
				LOG_INFO("Connection successful wifi network %s", data.ssid.c_str());
				commproto::sockets::SocketHandle client;
                LOG_INFO("Attempting to connect to dispatch at %s:%d", data.addr.c_str(),data.port);
                
                bool connected = false;
                do{
                    LOG_INFO("Attempt #%d connecting to dispatch", attempt + 1);
                    client = std::make_shared<commproto::sockets::SocketImpl>();
                    client->initClient(data.addr, data.port);
                    connected = client->connected();
                    if(!connected)
                    {
                        delay(6000);
                    }
                    ++attempt;
                    if(attempt == 10)
                    {
                        break;
                    }
				}while(!connected); 
                
                if (!client->connected()) {
					LOG_WARNING("Could not connect to %s:%d", data.addr.c_str(), data.port);
					return nullptr;
				}
				return client;
			}

			void delayT(uint32_t msec) override
			{
				delay(msec);
			}
			void reboot() override {
				Serial.println("Rebooting...");
				ESP.restart();
			}

			void saveAPData(const authdevice::ConnectionData& data) override
			{
				LOG_INFO("Saving hub AP data"); 
                File f;
#ifdef ESP32           
                f = SPIFFS.open("/config.txt", "w");
#elif defined(ESP8266)
                f = LittleFS.open("/config.txt", "w");
#endif
				if (!f)
				{
                    LOG_INFO("File did not open.");
					return;
				}
                LOG_INFO("Server SSID:\"%s\" PASS:\"%s\" ADDR:%s:%d", data.ssid.c_str(), data.password.c_str(), data.addr.c_str(), data.port);
				f.println(data.ssid.c_str());
				f.println(data.password.c_str());
				f.println(data.addr.c_str());
				f.println(data.port);
				f.close();
				LOG_INFO("Complete");
			}

			bool readAPData() override
			{
				LOG_INFO("Attempting to read auth config");      
                File f;                
#ifdef ESP32           
                f = SPIFFS.open("/config.txt", "r");
#elif defined(ESP8266)
                f = LittleFS.open("/config.txt", "r");
#endif
				
				if (!f)
				{
                    LOG_INFO("File did not open.");
					return false;
				}
				std::vector<String> lines;
				while (f.available())
				{
					lines.push_back(f.readStringUntil('\n'));
				}
				f.close();
				if (lines.size() < 4)
				{
                    LOG_ERROR("Insufficient data in file (lines:%d)",lines.size());
                    LOG_INFO("File contents:");
                    for(String str : lines)
                    {
                        LOG_INFO("%s",str.c_str());
                    }
					return false;
				}
				apData.ssid = lines[0].c_str();
                apData.ssid = apData.ssid.substr(0,apData.ssid.size()-1);
                
				apData.password = lines[1].c_str();
                apData.password = apData.password.substr(0,apData.password.size()-1);
                
				apData.addr = lines[2].c_str();
                apData.addr = apData.addr.substr(0,apData.addr.size()-1);
                
				apData.port = lines[3].toInt();
				if (apData.port == 0)
				{
                    LOG_ERROR("A problem occurred while trying to parse port %s",lines[3].c_str());
                    LOG_INFO("File contents:");
                    for(String str : lines)
                    {
                        LOG_INFO("%s",str.c_str());
                    }
					return false;
				}
				LOG_INFO("Successfully read config");
				LOG_INFO("WiFi: %s:%s, Dispatch %s:%d", apData.ssid.c_str(), apData.password.c_str(), apData.addr.c_str(), apData.port);
				readAuth = true;
				return true;

			}
            
            void resetAPData() override
            {
#ifdef ESP32                       
                bool success = SPIFFS.remove("/config.txt");
#elif defined(ESP8266)
                bool success = LittleFS.remove("/config.txt");
#endif
                 
                 if(!success)
                 {
                     LOG_ERROR("Could not delete config file");
                 }
                 
            }
		private:
			authdevice::ConnectionData apData;
			bool readAuth;

		};
	}
}


#endif //BASE_AUTH_WRAPPER_IMPL_H