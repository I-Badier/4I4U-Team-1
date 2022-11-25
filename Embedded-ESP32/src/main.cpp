#include <Arduino.h>
#include <ChainableLED.h>

#define NUM_LED 3
#define LIGHT_SENSOR_PIN 0

#define LEFT_BLINKER_BUTTON 5
#define LEFT_BLINKER_LED 4
#define RIGHT_BLINKER_BUTTON 18
#define RIGHT_BLINKER_LED 19

#define LEFT_BLINKER 0
#define RIGHT_BLINKER 1

ChainableLED leds = ChainableLED(7, 8, NUM_LED);

volatile bool start_left_blinkers = false;
volatile bool start_right_blinkers = false;
volatile long int last_push_left = 0;
volatile long int last_push_right = 0;

void IRAM_ATTR ISR_left_blinkers_button()
{
  if (!start_right_blinkers)
  {
    if ((millis() - last_push_left) > 100)
    {
      ets_printf("Left pressed\n");
      last_push_left = millis();
      if (start_left_blinkers)
      {
        start_left_blinkers = false;
      }
      else
      {
        start_left_blinkers = true;
      }
    }
  }
}

void IRAM_ATTR ISR_right_blinkers_button()
{
  if (!start_left_blinkers)
  {
    if ((millis() - last_push_right) > 100)
    {
      ets_printf("Right pressed\n");
      last_push_right = millis();
      if (start_right_blinkers)
      {
        start_right_blinkers = false;
      }
      else
      {
        start_right_blinkers = true;
      }
    }
  }
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("-- SETUP --");
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LIGHT_SENSOR_PIN, INPUT);
  pinMode(LEFT_BLINKER_BUTTON, INPUT);
  pinMode(LEFT_BLINKER_LED, OUTPUT);
  pinMode(RIGHT_BLINKER_BUTTON, INPUT);
  pinMode(RIGHT_BLINKER_LED, OUTPUT);

  attachInterrupt(LEFT_BLINKER_BUTTON, ISR_left_blinkers_button, FALLING);
  attachInterrupt(RIGHT_BLINKER_BUTTON, ISR_right_blinkers_button, FALLING);

  leds.setColorRGB((byte)RIGHT_BLINKER, 0, 0, 0);
  leds.setColorRGB((byte)LEFT_BLINKER, 0, 0, 0);

  Serial.println("- END OF SETUP --");
}

void loop()
{
  if (start_left_blinkers)
  {
    while (start_left_blinkers)
    {
      leds.setColorRGB((byte)LEFT_BLINKER, 255, 100, 0);
      digitalWrite(LEFT_BLINKER_LED, HIGH);
      delay(600);
      leds.setColorRGB((byte)LEFT_BLINKER, 0, 0, 0);
      digitalWrite(LEFT_BLINKER_LED, LOW);
      delay(600);
    }
  }
  else
  {
    leds.setColorRGB((byte)LEFT_BLINKER, 0, 0, 0);
    digitalWrite(LEFT_BLINKER_LED, LOW);
  }

  if (start_right_blinkers)
  {
    while (start_right_blinkers)
    {
      leds.setColorRGB((byte)RIGHT_BLINKER, 255, 100, 0);
      digitalWrite(RIGHT_BLINKER_LED, HIGH);
      delay(600);
      leds.setColorRGB((byte)RIGHT_BLINKER, 0, 0, 0);
      digitalWrite(RIGHT_BLINKER_LED, LOW);
      delay(600);
    }
  }
  else
  {
    leds.setColorRGB((byte)RIGHT_BLINKER, 0, 0, 0);
    digitalWrite(RIGHT_BLINKER_LED, LOW);
  }
}