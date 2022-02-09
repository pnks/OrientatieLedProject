///////////////////////////////////////////////////////////////////////
///This script is written by Michiel Dirks for MBO Utrecht          ///
///Extended by P.M. Kuipers                                                                 ///
///This script falls under the MIT licens                           ///
///////////////////////////////////////////////////////////////////////

// Temperature from 20 deg center, 3 degs up/down per led (boils down to 2 - 38 degrees
const float temperatureMin = 20-(3*6);
const float temperatureMax = 20+(3*6);
const float temperatureCompensation = -4; // subtract some degrees to compensate for inaccuracy

// Humidity goes the full scale
const float humidityMin = 0;
const float humidityMax = 100;

float Temperature;
float Humidity;

#include <NeoPixelBus.h>
#include <DHT.h>

#define DHTPIN 3
#define NeoPixelPin 2
#define PixelCount 12
#define DHTTYPE DHT11
#define ButtonHumidity 5
#define ButtonTemperature 4
#define colorSaturation 128
byte showState = 1;
byte prevState = 0;

DHT dht(DHTPIN, DHTTYPE);
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount, NeoPixelPin);

RgbColor red(colorSaturation, 0, 0);
RgbColor green(0, colorSaturation, 0);
RgbColor blue(0, 0, colorSaturation);
RgbColor white(colorSaturation);
RgbColor black(0);

void setup() {
  Serial.begin(9600);
  while (!Serial);

  PCICR |= B00000100;
  PCMSK2 |= B00110000;

  dht.begin();

  strip.Begin();
  strip.Show();

  pinMode(ButtonHumidity, INPUT_PULLUP);
  pinMode(ButtonTemperature, INPUT_PULLUP);
}

void loop() {
  static float prevHumidity = -1;
  static float prevTemperature = -273;
  float Humidity = dht.readHumidity();
  float Temperature = dht.readTemperature() + temperatureCompensation;
  if (isnan(Humidity) || isnan(Temperature)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  else {
    Serial.print(Temperature);
    Serial.print(",");
    Serial.println(Humidity);
  }

  if (showState == 1) {
    if(prevState != 1 || fabs(Temperature - prevTemperature) > 1){
      prevState = 1;
      prevTemperature = Temperature;
      showTemp(Temperature);
    }
  }
  else if (showState == 2) {
    if(prevState != 2 || fabs(Humidity - prevHumidity) > 1){
      prevState = 2;
      prevHumidity = Humidity;
      showHumidity(Humidity);
    }
  }
  else if (showState == 3) {
    prevState = 3;
    rotate();
  }

  delay(500);
}

void showTemp(float Temperature) {
  for (int x = 0; x <= 11; x++) {
    strip.SetPixelColor(x, black);
  }
  strip.Show();
  byte showLeds = mapf(Temperature, temperatureMin, temperatureMax, 0, 11);
  for (int x = 0; x <= showLeds; x++) {
    if (showLeds > 12) {
      showLeds = 12;
    }
    int r = map(x, 0, 11, 20, 255);
    int g = r * -1;
    strip.SetPixelColor(x, RgbColor(r, g, 0));
    strip.Show();
  }
}

void showHumidity(float Humidity) {
  for (int x = 0; x <= 11; x++) {
    strip.SetPixelColor(x, black);
  }
  strip.Show();
  byte showLeds = mapf(Humidity, humidityMin, humidityMax, 0, 11);
  for (int x = 0; x <= showLeds; x++) {
    if (showLeds > 12) {
      showLeds = 12;
    }
    int g = map(x, 0, 11, 20, 255);
    int b = g * -1;
    strip.SetPixelColor(x, RgbColor(0, g, b));
    strip.Show();
  }
}

void rotate() {
  for (int x = 0; x <= 11; x++) {
    strip.SetPixelColor(x, black);
  }
  strip.Show();
  int color = 0;
  for (int x = 0; x <= 11; x++) {
    color++;
    if (color == 4) {
      color = 1;
    }
    if (color == 1) {
      strip.SetPixelColor(x, red);
    }
    if (color == 2) {
      strip.SetPixelColor(x, green);
    }
    if (color == 3) {
      strip.SetPixelColor(x, blue);
    }
    strip.Show();

    delay(200);
  }
}

float mapf(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

ISR(PCINT2_vect) {
  if (digitalRead(ButtonHumidity) == LOW) {
    if (digitalRead(ButtonTemperature) == LOW) {
      showState = 3;
    }
    else {
      showState = 2;
    }
  }
  else if (digitalRead(ButtonTemperature) == LOW) {
    showState = 1;
  }
}
