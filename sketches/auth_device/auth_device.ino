#include <AuthDevice.h>
#include <SocketImpl.h>
#include <ESP8266WiFi.h>

class EspAuthDevice : public AuthDeviceWrapper
{
  public:
  std::vector<std::string> listNetworks() override 
  {
    int numberOfNetworks = WiFi.scanNetworks();
    std::vector<std::string> networks;
    Serial.println("Scanned networks:");
    for(int i =0; i<numberOfNetworks; ++i)
    {
      String net = WiFi.SSID(i);
      std::string network = std::string(net.c_str());
      networks.push_back(std::string(network));
    }
    return networks;
  }
  
  void setBaudRate(const uint32_t bytes) override 
  {
   
    Serial.begin(bytes);
  };
  
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
      delay(2000);
      ++attempt;
    } while( status != WL_CONNECTED && attempt != maxAttempts);
    
    if(status!=WL_CONNECTED)
    {
      LOG_INFO("Connection attempt unsuccesful");
      Serial.println(WiFi.status());
      return nullptr;
    }
    LOG_INFO("Connection successful wifi network %s",ssid.c_str());
    commproto::sockets::SocketHandle client = std::make_shared<commproto::sockets::SocketImpl>();
    WiFiClient cli;
    bool cliConnected = cli.connect(addr.c_str(),static_cast<uint16_t>(port));
    if(cliConnected){
      LOG_INFO("Could connect via standard WiFiClient");
    }
    if(cli.connected()){
      LOG_INFO("Could check via connected method");
    }
    cli.stop();
    delay(1000); 
    bool connected = client->initClient(addr,port);
    if(!connected){
      LOG_WARNING("Could not connect to %s:%d",addr.c_str(),port);
      return nullptr; 
    }
    return client; 
  }
};

EspAuthDevice realDevice;
AuthDevice device(&realDevice);

void setup() {
  device.setup();

}

void loop() {
  device.loop();
  delay(5000);
}
