#include <Arduino.h>
#include "secrets.h"

// wifi
#include <WiFiClient.h>
#include <WiFi.h>
const char *ssid = SECRET_SSID;
const char *password = SECRET_PASS;
WiFiClient client;

// thingspeak
#include "ThingSpeak.h"
unsigned long myChannelNumber = SECRET_CH_ID;
const char *myWriteAPIKey = SECRET_WRITE_APIKEY;

// sensors

// soil
#define AOUT_PIN A6 // Arduino pin that connects to AOUT pin of moisture sensor (D34 maps to ADC6)
// this ADC pin must not be on ADC2 circuit.  Try a pin above for example D33+

const int OpenAirReading = 2700; // calibration data 1
const int WaterReading = 800;    // calibration data 2
int MoistureLevel = 0;
float SoilMoisturePercentage = 0;

int high = 0;
int low = 9999;

// air
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 23 // Digital pin connected to the DHT sensor

#define DHTTYPE DHT11 // DHT 11

DHT_Unified dht(DHTPIN, DHTTYPE);

float temp;
float humidity;

// loop timings
unsigned long lastTime = 0;
unsigned long timerDelay = 60000;
unsigned long count = 0;
unsigned long totalMoisture = 0;

void setup()
{
  Serial.begin(9600);

  dht.begin();

  WiFi.begin(ssid, password);

  ThingSpeak.begin(client);
}

void loop()
{
  MoistureLevel = analogRead(AOUT_PIN);

  if (MoistureLevel == 0)
    return;

  count++;
  totalMoisture += MoistureLevel;

  high = max(MoistureLevel, high);
  low = min(MoistureLevel, low);

  // air
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  float loggedTemp = event.temperature;
  temp += loggedTemp;
  dht.humidity().getEvent(&event);
  float loggedHumidity = event.relative_humidity;
  humidity += loggedHumidity;

  // debug region start
  SoilMoisturePercentage = map(MoistureLevel, OpenAirReading, WaterReading, 0, 100);
  SoilMoisturePercentage = constrain(SoilMoisturePercentage, 0, 100);

  Serial.print("Debug -> Moisture: ");
  Serial.print(SoilMoisturePercentage);
  Serial.print("% level: ");
  Serial.print(MoistureLevel);
  Serial.print(" low: ");
  Serial.print(low);
  Serial.print(" high: ");
  Serial.print(high);
  Serial.print(" Temp: ");
  Serial.print(loggedTemp);
  Serial.print("°C Humidity: ");
  Serial.print(loggedHumidity);
  Serial.println("%");

  // debug region end

  if ((millis() - lastTime) > timerDelay)
  {
    lastTime = millis();

    float averageMoistureLevel = totalMoisture / count;

    SoilMoisturePercentage = map(averageMoistureLevel, OpenAirReading, WaterReading, 0, 100);

    SoilMoisturePercentage = constrain(SoilMoisturePercentage, 0, 100);

    temp = temp / (float)count;
    humidity = humidity / (float)count;

    ThingSpeak.setField(1, SoilMoisturePercentage);
    ThingSpeak.setField(2, low);
    ThingSpeak.setField(3, high);
    ThingSpeak.setField(4, temp);
    ThingSpeak.setField(5, humidity);
    int result = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

    Serial.print("Thingspeak: ");
    Serial.print(result);
    Serial.print(" Moisture: ");
    Serial.print(SoilMoisturePercentage);
    Serial.print("% low: ");
    Serial.print(low);
    Serial.print(" high: ");
    Serial.print(high);
    Serial.print(" temp: ");
    Serial.print(temp);
    Serial.print("°C humidity: ");
    Serial.print(humidity);
    Serial.println("%");

    count = 0;
    totalMoisture = 0;
    high = 0;
    low = 9999;
    temp = 0;
    humidity = 0;
  }
  else
  {
    delay(1000);
  }
}
