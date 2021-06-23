#include <BaseAuthWrapperImpl.h>
#include <commproto/endpointdevice/BaseEndpointAuth.h>
#include <commproto/thermo/Thermostat.h>

#include <DHT.h>;
#include <FastLED.h>

//pins
#define LED_PIN 16
#define NUM_LEDS 8
#define HEAT_LED 4

#define DHT_PIN 18
#define DHT_TYPE DHT22   // DHT 22  (AM2302)
#define RESET_BTN_PIN 32

// sensors/actuators
DHT dht(DHT_PIN, DHT_TYPE); //// Initialize DHT sensor for normal 16mhz Arduino
CRGB leds[NUM_LEDS];



float humidity = 13.f;
//utility functions
float sensorGetHumidity()
{
  float hum =  dht.readHumidity();
  if (!isnan(hum))
  {
    humidity = hum;
  }
  return humidity;
}

float temperature = 13.f;
float sensorGetTemperature()
{
  float temp =  dht.readTemperature();
  if (!isnan(temp))
  {
    temperature = temp;
  }
  return temperature;
}

void blankLEDS()
{
  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = CRGB::Black;
  }
  FastLED.show();
}

void setHeat(int intensity)
{
  blankLEDS();
  if (intensity > HEAT_LED)
  {
    intensity = HEAT_LED;
  }
  int top  = HEAT_LED + intensity;

  for (int i = HEAT_LED; i < top; ++i)
  {
    leds[i] = CRGB::Red;
  }
  FastLED.show();
}


void setCool(int intensity)
{
  blankLEDS();
  if (intensity > HEAT_LED)
  {
    intensity = HEAT_LED;
  }
  int top = HEAT_LED - intensity;
  for (int i = HEAT_LED - 1; i >= top; --i)
  {
    leds[i] = CRGB::Blue;
  }
  FastLED.show();
}

void setBuiltinState(bool on)
{
  if (on)
  {
    digitalWrite(LED_BUILTIN, LOW);
  }
  else
  {
    digitalWrite(LED_BUILTIN, HIGH);
  }
}

void setupBoardStuff()
{
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);  // GRB ordering is assumed
  FastLED.setBrightness(10);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RESET_BTN_PIN,INPUT_PULLUP);
  blankLEDS();
  dht.begin();
}

class ThermostatDevice : public commproto::thermo::ThermostateWrapper
{
  public:
    ThermostatDevice()
      : hum(0.f)
      , temp(0.f)
      , desiredTemp(0.f)
      , autoTemp(false)
      , tolerance(1.f)
    {

    }

    uint32_t getMs() override
    {
      return millis();
    }
    void setup() override
    {
      setupBoardStuff();
    }
    void loop() override
    {
      hum = sensorGetHumidity();
      temp = sensorGetTemperature();
      if (!autoTemp)
      {
        return;
      }
      
      float distance = fabs(desiredTemp - temp);
      if ( distance < tolerance)
      {
        setHeat(0);
        return;
      }
      int intensity = 0;
      if (distance > 20.f)
      {
        intensity = HEAT_LED;
      }
      else
      {
        intensity = (int)(ceil(distance / 5.f));
      }
      bool heat = desiredTemp > temp;
      if (heat)
      {
        setHeat(intensity);
      }
      else
      {
        setCool(intensity);
      }
    }

    float getTemp() override
    {
      return temp;
    }

    float getHumidity() override
    {
      return hum;
    }

    void toggleTempAdjust(const float intensity) override
    {
      autoTemp = false;
      float internalIntensity = intensity;
      if (internalIntensity > HEAT_LED)
      {
        internalIntensity = HEAT_LED;
      }
      if (internalIntensity < -1 * HEAT_LED)
      {
        internalIntensity = -1 * HEAT_LED;
      }
      int int_intensity = (int)fabs(internalIntensity);
      if (internalIntensity > 0)
      {
        setHeat(int_intensity);
      }
      else
      {
        setCool(int_intensity);
      }
    }
    void toggleAutoTempAdjust(const bool on) override
    {
      autoTemp = on;
      setHeat(0);
    }
    void setDesiredTemp(const float temp) override
    {
      desiredTemp = temp;
    }
  private:
    float hum;
    float temp;
    float desiredTemp;
    bool autoTemp;
    const float tolerance;
};

class HelperImpl : public commproto::endpointdevice::Helper
{
    public:
    HelperImpl() : ledCount(0){}
    bool readButton() override
    {
      int state = digitalRead(RESET_BTN_PIN);
      return state != HIGH;
    }
    void blankLEDs() override{
      blankLEDS();
    }
    void advanceLED(const bool apMode) override
    {
      blankLEDS();
      if(apMode)
        leds[ledCount] = CRGB::Green;
      else
        leds[ledCount] = CRGB::Blue;  
      FastLED.show();
      ++ledCount;
      if(ledCount >= NUM_LEDS)
      {
        ledCount = 0;
      }
    }
    private:
      int ledCount; 
};


ThermostatDevice thermo;
HelperImpl helperimp;

commproto::endpointdevice::BaseAuthWrapperImpl realDevice(helperimp);
commproto::endpointdevice::DeviceDetails details = { "Thermostat", "Commproto", "A simple device that provides data about temperature, humidity and the possibility to start heating." };
commproto::thermo::Thermostat device(realDevice, details, thermo);

void setup()
{
  setupBoardStuff();
  device.setup();
}

void loop()
{
  device.loop();
}
