#ifndef BASE_AUTH_WRAPPER_IMPL_H
#define BASE_AUTH_WRAPPER_IMPL_H

#include <ESP8266WiFi.h>
#include <BaseEndpointWrapper.h>
#include <SocketImpl.h>
#include <SerialInterface.h>
#include <FS.h>
#include <string>

void(* resetFunc) (void) = 0;

IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);


class BaseAuthWrapperImpl : public BaseEndpointWrapper
{
public:
    
    BaseAuthWrapperImpl()
        : readAuth(false)
    {
        
    }
    commproto::stream::StreamHandle getStream(const int speed) override { 
        commproto::serial::SerialHandle serial =  std::make_shared<commproto::serial::SerialInterface>();
        serial->start(speed);
        return serial; 
    }
    bool hasAuth() override {
        return readAuth;
    }
    APData getAuthData() override { 
        return apData;
    }
    
    commproto::sockets::SocketHandle startAsAP(const APData& data) override 
    {
        Serial.println();
        Serial.println("Starting as wifi access point");
        WiFi.mode(WIFI_AP);
        IPAddress local;
        
        local.fromString(data.addr.c_str());
        WiFi.softAP(data.ssid.c_str(),data.password.c_str());
        WiFi.softAPConfig(local, gateway, subnet);
        
        IPAddress IP = WiFi.softAPIP();
        Serial.print("AP IP address: ");
        Serial.println(IP);
        
        commproto::sockets::SocketHandle socket = std::make_shared<commproto::sockets::SocketImpl>();
        socket->initServer("",data.port);
        return socket;
    }
    
    commproto::sockets::SocketHandle connect(const APData& data) override { return nullptr; }
    
    void delayT(uint32_t msec) override 
    {
        delay(msec);
    }
    void reboot() override {
        Serial.println("Rebooting...");
        resetFunc();
    }
    int32_t getTemp() override { return  0; }
    
    void saveAPData(const APData& data) override 
    {
        LOG_INFO("Saving hub AP data");
        File f = SPIFFS.open("/config", "w");
        if(!f)
        {
            return;
        }
        f.println(apData.ssid.c_str());
        f.println(apData.password.c_str());
        f.println(apData.addr.c_str());
        f.println(apData.port);
        f.close();
        LOG_INFO("Complete");
    }
    
    bool readAPData()
    {
        LOG_INFO("Attempting to read auth config");
        File f = SPIFFS.open("/config", "r");
        if(!f)
        {
            return false;
        }
        std::vector<String> lines;
        while(f.available()) 
        {
             lines.push_back(f.readStringUntil('\n'));
        }
        f.close();
        if(lines.size() != 4)
        {
            return false;
        }
        apData.ssid = lines[0].c_str();
        apData.password = lines[1].c_str();
        apData.addr = lines[2].c_str();
        apData.port =  lines[3].toInt();
        if(apData.port == 0)
        {
            return false;
        }
        LOG_INFO("Successfully read config");
        LOG_INFO("WiFi: %s:%s, Dispatch %s:%d",apData.ssid.c_str(),apData.password.c_str(),apData.addr.c_str(),apData.port);
        readAuth = true;
        return true;
        
    }
private:
    APData apData;
    bool readAuth;
    
};

    
#endif //BASE_AUTH_WRAPPER_IMPL_H