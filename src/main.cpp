#include <Arduino.h>

#define LED_BUILTIN 2
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
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
}

// the loop function runs over and over again forever
void loop()
{
  digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
  delay(1000);
  // wait for a second
  digitalWrite(LED_BUILTIN, LOW); // turn the LED off by making the voltage LOW
  delay(1000);
  // wait for a second
  MoistureLevel = analogRead(AOUT_PIN); // read the analog value from sensor

  SoilMoisturePercentage = map(MoistureLevel, OpenAirReading, WaterReading, 0, 100);

  SoilMoisturePercentage = constrain(SoilMoisturePercentage, 0, 100);

  Serial.print(SoilMoisturePercentage);
  Serial.println("%");

  if (MoistureLevel > high)
  {
    high = MoistureLevel;
  };

  if (MoistureLevel < low && MoistureLevel != 0)
  {
    low = MoistureLevel;
  };

  Serial.print("Moisture: ");
  Serial.print(MoistureLevel);

  Serial.print(" low: ");
  Serial.print(low);

  Serial.print(" high: ");
  Serial.print(high);

  Serial.println("");
}
