///////////////////////////////////////////////////////////////////////
///dit script code is geschreven door Michiel Dirks voor MBO Utrecht///
///                                                                 ///
///Dit script valt onder de MIT licentie                            ///
///////////////////////////////////////////////////////////////////////

const float temperatureMin = 15;
const float temperatureMax = 30;
const float humidityMin = 20;
const float humidityMax = 70;

float Temperature;
float Humidity;

#include <NeoPixelBus.h>
#include <DHT.h>

#define DHTPIN 3
#define NeoPixelPin 2
#define PixelCount 12
#define DHTTYPE DHT11
#define ButtonHumidity 4
#define ButtonTemperature 5
#define colorSaturation 128
byte showState = 0;

DHT dht(DHTPIN, DHTTYPE);
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount, NeoPixelPin);

RgbColor red(colorSaturation, 0, 0);
RgbColor green(0, colorSaturation, 0);
RgbColor blue(0, 0, colorSaturation);
RgbColor white(colorSaturation);
RgbColor black(0);

HslColor hslRed(red);
HslColor hslGreen(green);
HslColor hslBlue(blue);
HslColor hslWhite(white);
HslColor hslBlack(black);

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
  delay(200);
  Humidity = dht.readHumidity();
  Temperature = dht.readTemperature();

  if (isnan(Humidity) || isnan(Temperature)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  if (showState == 1) {
    showTemp();
  }
  else if (showState == 2) {
  showHumidity();
  }
  else if (showState == 3) {
    rotate();
  }
}

void showTemp() {
  for (int x = 0; x <= 11; x++) {
    strip.SetPixelColor(x, black);
  }
  strip.Show();
  byte showLeds = mapf(Temperature, temperatureMin, temperatureMax, 0, 11);
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

void showHumidity(){
  for (int x = 0; x <= 11; x++) {
    strip.SetPixelColor(x, black);
  }
  strip.Show();
  byte showLeds = mapf(Humidity, humidityMin, humidityMax, 0, 11);
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

ISR(PCINT2_vect){
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
