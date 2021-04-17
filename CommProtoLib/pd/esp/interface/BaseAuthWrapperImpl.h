#ifndef BASE_AUTH_WRAPPER_IMPL_H
#define BASE_AUTH_WRAPPER_IMPL_H

#include <ESP8266WiFi.h>
#include <commproto/endpointdevice/BaseEndpointWrapper.h>
#include <SocketImpl.h>
#include <SerialInterface.h>
#include <FS.h>
#include <LittleFS.h>
#include <string>

void(*resetFunc) (void) = 0;

IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

namespace commproto
{
	namespace endpointdevice
	{

		class BaseAuthWrapperImpl : public BaseEndpointWrapper
		{
		public:
            
            void initFs() override
            {
                if(!LittleFS.begin())
                {
                    Serial.println("Could not moutn filesystem");
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

			commproto::sockets::SocketHandle connect(const authdevice::ConnectionData& data) override
			{
				LOG_INFO("Attempting to connect to wifi network %s", data.ssid.c_str());
				int status = WL_IDLE_STATUS;
				int attempt = 0;
				int maxAttempts = 5;
				WiFi.begin(data.ssid.c_str(), data.password.c_str());
				do {
					LOG_INFO("Attempt #%d", attempt + 1);
					status = WiFi.waitForConnectResult();
					delay(500);
					++attempt;
				} while (status != WL_CONNECTED && attempt != maxAttempts);

				if (status != WL_CONNECTED)
				{
					LOG_INFO("Connection attempt unsuccesful");
					return nullptr;
				}
				LOG_INFO("Connection successful wifi network %s", data.ssid.c_str());
				commproto::sockets::SocketHandle client = std::make_shared<commproto::sockets::SocketImpl>();
				bool connected = client->initClient(data.addr, data.port);
				if (!connected) {
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
				resetFunc();
			}

			void saveAPData(const authdevice::ConnectionData& data) override
			{
				LOG_INFO("Saving hub AP data");
				File f = LittleFS.open("/config.txt", "w");
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
				File f = LittleFS.open("/config.txt", "r");
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
				apData.password = lines[1].c_str();
				apData.addr = lines[2].c_str();
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
		private:
			authdevice::ConnectionData apData;
			bool readAuth;

		};
	}
}


#endif //BASE_AUTH_WRAPPER_IMPL_H