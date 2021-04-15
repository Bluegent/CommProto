#include <AuthDevice.h>
#include <SocketImpl.h>
#include <ESP8266WiFi.h>


class EspAuthDevice : public AuthDeviceWrapper
{
  public:
  void setup()
  {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
  }
  std::vector<std::string> listNetworks() override 
  {
    int numberOfNetworks = WiFi.scanNetworks();
    std::vector<std::string> networks;
    for(int i =0; i<numberOfNetworks; ++i)
    {
      String net = WiFi.SSID(i);
      std::string network = std::string(net.c_str());
      networks.push_back(std::string(network));
    }
    return networks;
  }
  commproto::sockets::SocketHandle connectTo(const std::string& ssid, const std::string& pwd, const std::string& addr, const uint32_t port) override 
  { 
    LOG_INFO("Attempting to connect to wifi network %s",ssid.c_str());
    int status = WL_IDLE_STATUS;
    int attempt = 0;
    int maxAttempts = 5;
    WiFi.begin(ssid.c_str(), pwd.c_str());
    do{
      LOG_INFO("Attempt #%d",attempt+1);    
      status = WiFi.waitForConnectResult();
      delay(500);
      ++attempt;
    } while( status != WL_CONNECTED && attempt != maxAttempts);
    
    if(status!=WL_CONNECTED)
    {
      LOG_INFO("Connection attempt unsuccesful");
      return nullptr;
    }
    LOG_INFO("Connection successful wifi network %s",ssid.c_str());
    commproto::sockets::SocketHandle client = std::make_shared<commproto::sockets::SocketImpl>();
    bool connected = client->initClient(addr,port);
    if(!connected){
      LOG_WARNING("Could not connect to %s:%d",addr.c_str(),port);
      return nullptr; 
    }
    return client; 
  }

  commproto::serial::SerialHandle getSerial(const int speed) override
  {
    commproto::serial::SerialHandle serial =  std::make_shared<commproto::serial::SerialInterface>();
    serial->start(speed);
    return serial;
  }

    void setLED(const bool on) override
    {
      if(on){
         digitalWrite(LED_BUILTIN, LOW);
      }
      else
      {
         digitalWrite(LED_BUILTIN, HIGH);
      }
    }
    void delayT(const uint32_t msec) override
    {
      delay(msec);
    }
};

EspAuthDevice realDevice;
AuthDevice device(realDevice);

void setup() {
  realDevice.setup();
  device.setup();
}

void loop() {
  device.loop();
}
