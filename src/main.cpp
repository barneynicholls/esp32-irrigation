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
#define AOUT_PIN A6 
// Arduino pin that connects to AOUT pin of moisture sensor 
// this ADC pin must not be on ADC2 circuit.  Try a pin above for example D33+ (D34 maps to ADC6)

const unsigned short OpenAirReading = 2700; // calibration data 1
const unsigned short WaterReading = 800;    // calibration data 2

unsigned long moisture = 0;
unsigned short capacitanceHigh = 0;
unsigned short capacitanceLow = 9999;

// air
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 23 // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11 // DHT 11

DHT_Unified dht(DHTPIN, DHTTYPE);

float temp;
float humidity;

//relay
#define RELAY_PIN 22
bool off = true;

// loop timings
unsigned long lastTime = 0;
unsigned long timerDelay = 60000;
unsigned long count = 0;

void setup()
{
  Serial.begin(9600);

  pinMode(RELAY_PIN, OUTPUT);

  dht.begin();

  WiFi.begin(ssid, password);

  ThingSpeak.begin(client);
}

void loop()
{

  // soil
  u_int16_t moistureReading = analogRead(AOUT_PIN);

  if (moistureReading == 0)
    return;
    
  // relay test
digitalWrite(RELAY_PIN, off ? LOW : HIGH);

off = !off;

  count++;

  moisture += moistureReading;
  capacitanceHigh = max(moistureReading, capacitanceHigh);
  capacitanceLow = min(moistureReading, capacitanceLow);

  // air
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  float tempReading = event.temperature;
  temp += tempReading;
  dht.humidity().getEvent(&event);
  float humidityReading = event.relative_humidity;
  humidity += humidityReading;

  // debug region start
  long soilMoisturePercentage = map(moistureReading, OpenAirReading, WaterReading, 0, 100);
  soilMoisturePercentage = constrain(soilMoisturePercentage, 0, 100);

  Serial.print("Debug -> Moisture: ");
  Serial.print(soilMoisturePercentage);
  Serial.print("% level: ");
  Serial.print(moistureReading);
  Serial.print(" low: ");
  Serial.print(capacitanceLow);
  Serial.print(" high: ");
  Serial.print(capacitanceHigh);
  Serial.print(" Temp: ");
  Serial.print(tempReading);
  Serial.print("°C Humidity: ");
  Serial.print(humidityReading);
  Serial.println("%");

  // debug region end

  if ((millis() - lastTime) > timerDelay)
  {
    lastTime = millis();

    moisture = moisture / count;

    moisture = map(moisture, OpenAirReading, WaterReading, 0, 100);
    moisture = constrain(moisture, 0, 100);

    temp = temp / count;
    humidity = humidity / count;

    ThingSpeak.setField(1, (long) moisture);
    ThingSpeak.setField(2, capacitanceLow);
    ThingSpeak.setField(3, capacitanceHigh);
    ThingSpeak.setField(4, temp);
    ThingSpeak.setField(5, humidity);
    int result = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

    Serial.print("Thingspeak: ");
    Serial.print(result);
    Serial.print(" Moisture: ");
    Serial.print(moisture);
    Serial.print("% low: ");
    Serial.print(capacitanceLow);
    Serial.print(" high: ");
    Serial.print(capacitanceHigh);
    Serial.print(" temp: ");
    Serial.print(temp);
    Serial.print("°C humidity: ");
    Serial.print(humidity);
    Serial.println("%");

    count = 0;
    moisture = 0;
    capacitanceHigh = 0;
    capacitanceLow = 9999;
    temp = 0;
    humidity = 0;
  }
  else
  {
    delay(1000);
  }
}
