#include <Arduino.h>

#define AOUT_PIN A12 // Arduino pin that connects to AOUT pin of moisture sensor

const int OpenAirReading = 530; // calibration data 1
const int WaterReading = 270;   // calibration data 2
int MoistureLevel = 0;
int SoilMoisturePercentage = 0;

int high = 0;
int low = 9999;

int done = 0;

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  MoistureLevel = analogRead(AOUT_PIN);

  SoilMoisturePercentage = map(MoistureLevel, OpenAirReading, WaterReading, 0, 100);

  SoilMoisturePercentage = constrain(SoilMoisturePercentage, 0, 100);

  if (MoistureLevel > high)
  {
    high = MoistureLevel;
  };

  if (MoistureLevel < low && MoistureLevel != 0)
  {
    low = MoistureLevel;
  };

  Serial.print("Moisture: ");
  Serial.print(SoilMoisturePercentage);
  Serial.print("% level: ");
  Serial.print(MoistureLevel);
  Serial.print(" low: ");
  Serial.print(low);
  Serial.print(" high: ");
  Serial.print(high);
  Serial.println("");

  delay(1000);
}
