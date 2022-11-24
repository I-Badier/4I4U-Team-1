#include <Arduino.h>
#include <ChainableLED.h>

#define NUM_LED 3
#define LIGHT_SENSOR_PIN 0
#define LEFT_BLINKER_BUTTON 5
#define LEFT_BLINKER_LED 4

#define LEFT_BLINKER 0
#define RIGHT_BLINKER 1

ChainableLED leds = ChainableLED(7, 8, NUM_LED);

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("-- SETUP --");
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LIGHT_SENSOR_PIN, INPUT);
  pinMode(LEFT_BLINKER_BUTTON, INPUT);
  pinMode(LEFT_BLINKER_LED, OUTPUT);
  Serial.println("- END OF SETUP --");
}

void loop()
{
  int light_value = analogRead(LIGHT_SENSOR_PIN);
  Serial.print("Luminosity : ");
  Serial.println(light_value);

  int button_left_value = digitalRead(LEFT_BLINKER_BUTTON);
  Serial.print("Left button : ");
  Serial.println(button_left_value);

  long int now = millis();
  leds.setColorRGB((byte)RIGHT_BLINKER, 0, 0, 0);
  while (millis() - now < 2000)
  {
    leds.setColorRGB((byte)LEFT_BLINKER, 255, 100, 0);
    digitalWrite(LEFT_BLINKER_LED, HIGH);
    delay(600);
    leds.setColorRGB((byte)LEFT_BLINKER, 0, 0, 0);
    digitalWrite(LEFT_BLINKER_LED, LOW);
    delay(600);
  }

  now = millis();
  leds.setColorRGB((byte)LEFT_BLINKER, 0, 0, 0);
  while (millis() - now < 2000)
  {
    leds.setColorRGB((byte)RIGHT_BLINKER, 255, 100, 0);
    delay(600);
    leds.setColorRGB((byte)RIGHT_BLINKER, 0, 0, 0);
    delay(600);
  }
}