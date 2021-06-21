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


float humidity = 0.f;
//utility functions
float sensorGetHumidity()
{
  float hum =  dht.readHumidity();
  if (!isnan(hum))
  {
    humidity = hum;
  }
  Serial.print("Humidity:");
  Serial.print(humidity);
  Serial.println(" %");
  return humidity;
}

float temperature = 0.f;
float sensorGetTemperature()
{
  float temp =  dht.readTemperature();
  if (!isnan(temp))
  {
    temperature = temp;
  }
  Serial.print("Temperature:");
  Serial.print(temperature);
  Serial.println(" C");
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
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(10);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RESET_BTN_PIN,INPUT_PULLUP);
  blankLEDS();
  dht.begin();
}

namespace commproto
{
namespace thermo
{
class ThermostateWrapper
{
  public:
    virtual ~ThermostateWrapper() = default;
    virtual void setup() = 0;
    virtual void loop() = 0;
    virtual int32_t getTemp() = 0;
    virtual int32_t getHumidity() = 0;
    virtual void toggleTempAdjust(const float intensity) = 0;
    virtual void toggleAutoTempAdjust(const bool on) = 0;
    virtual void setDesiredTemp(const float temp) = 0;
};
}
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

      Serial.println("Auto temperature adjustment is on");
      Serial.print("Desired temp: ");
      Serial.print(desiredTemp);
      Serial.print(" C Actual temp: ");
      Serial.print(temp);
      Serial.println(" C");
      float distance = fabs(desiredTemp - temp);
      if ( distance < tolerance)
      {
        return;
      }
      int intensity = 0;
      if (distance > 20.f)
      {
        intensity = HEAT_LED;
      }
      else
      {
        intensity = (int)(ceil(distance / 2.5f));
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

    int32_t getTemp() override
    {
      return temp;
    }

    int32_t getHumidity() override
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

ThermostatDevice thermo;

void setup()
{
  Serial.begin(115200);
  setupBoardStuff();
}

void loop()
{
  thermo.loop();
  delay(1000);
}
