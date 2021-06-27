#include <BaseAuthWrapperImpl.h>
#include <commproto/endpointdevice/BaseEndpointAuth.h>
#include <SmartPot.h>

#include <DHT.h>
//#include <FastLED.h>

#define SOIL_PIN 32
#define LIGHT_SENS_PIN 34
#define PUMP_PIN 15
#define UV_PIN 33
#define NUM_LEDS 8
#define DHT_PIN 18
#define DHTTYPE DHT11 
#define BTN_PIN 27

DHT dht(DHT_PIN, DHTTYPE);

void setupFunc()
{
  Serial.begin(115200);
  pinMode(SOIL_PIN,INPUT); 
  pinMode(UV_PIN,OUTPUT);
  pinMode(PUMP_PIN,OUTPUT);
  pinMode(BTN_PIN,INPUT_PULLUP);
  pinMode(LIGHT_SENS_PIN,INPUT);
  digitalWrite(PUMP_PIN,HIGH);
  dht.begin();
}

class RealPot : public PotWrapper
{
  public:
  RealPot()  : pumpOn(false) , lastPump(0), actualPumpOn(false) , lastPumpOn(0){}
  float getTemperature() 
  {
    return temp;
  }
  float getHumidity()
  {
    return hum;
  }
  uint32_t getSoilHumidity()
  {
    return soil;
  }
  uint32_t getLightExposure()
  {
    return light;
  }
  void togglePump(bool state)
  {
    pumpOn = state;
  }
  void toggleLamp(bool state)
  {
    digitalWrite(UV_PIN,state?HIGH:LOW); 
  }
  void setupBoard()
  {
  }
  uint32_t getMs()
  {
    return millis();
  }
  void getReadings()
  {
    temp = dht.readTemperature();
    hum = dht.readHumidity();
    light = analogRead(LIGHT_SENS_PIN);
    soil = 4096 - analogRead(SOIL_PIN);
    LOG_INFO("H:%.2f T:%.2f S:%d U:%d",hum,temp,soil,light);
  }
  void loop()
  {
    now = millis();
    if(pumpOn)
    {
      pumpFunction();
    } 
    else
    {
      if(actualPumpOn)
      {
        digitalWrite(PUMP_PIN,LOW);
        actualPumpOn = false;
      }
    }
    
  }

  void pumpFunction()
  {
    if(actualPumpOn)
    {
      if(now - lastPumpOn > pumpOnTime)
      {
        toggleRealPump(false);  
      }
    }
    else
    {
      if(now - lastPump > pumpDelay)
      {
        toggleRealPump(true);
        lastPumpOn = now;
        lastPump = now;
      }
    }
  }

  void toggleRealPump(bool state)
  {
    digitalWrite(PUMP_PIN,state?LOW:HIGH);
    actualPumpOn = state;
  }
  
  private:
  float temp;
  float hum;
  int soil;
  int light;
  int lastPump;
  bool pumpOn;
  bool actualPumpOn;
  int lastPumpOn;
  int now;
  static const int pumpDelay = 5000;
  static const int pumpOnTime = 200;
};


class HelperImpl : public commproto::endpointdevice::Helper
{
    public:
    HelperImpl() : on(false){}
    bool readButton() override
    {
      int state = digitalRead(BTN_PIN);
      return state != HIGH;
    }
    void blankLEDs() override{
      digitalWrite(UV_PIN,LOW);
    }
    void advanceLED(const bool apMode) override
    {
      on = !on;
      digitalWrite(UV_PIN,on?HIGH:LOW);
    }
    private:
      bool on; 
};


RealPot pot;
HelperImpl helperimp;

commproto::endpointdevice::BaseAuthWrapperImpl realDevice(helperimp);
commproto::endpointdevice::DeviceDetails details = { "Smart Pot", "Commproto", "Provides readings for soil humidity, UV light exposure as well as ambiental temperature and humidity. Incorporates an irrigation system and  UV lamp for solutioning poor conditions." };
commproto::smartpot::SmartPot device(realDevice, details, pot);


void setup() {
  setupFunc();
  device.setup();
}

void loop() {
  device.loop();
}
