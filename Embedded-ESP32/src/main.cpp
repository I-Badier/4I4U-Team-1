#include <Arduino.h>
#include <ChainableLED.h>
#include <Adafruit_NeoPixel.h>

// Pin used fot the light sensor
#define LIGHT_SENSOR_PIN 0

// Pins used for headlights and rear lights
#define LED_STICK_PIN_FRONT 6
#define LED_STICK_PIN_BACK 10

// Number of LEDs on a LED stick
#define NUM_PIXEL_STICK 10

// Pins used for Grove LED button : activate or deactivate the blinkers
#define LEFT_BLINKER_BUTTON 5
#define LEFT_BLINKER_LED 4
#define RIGHT_BLINKER_BUTTON 18
#define RIGHT_BLINKER_LED 19

// Number of LEDs and position in the Chainable LED queue
#define NUM_LED 4
#define LEFT_BLINKER_1 0
#define LEFT_BLINKER_2 1
#define RIGHT_BLINKER_1 2
#define RIGHT_BLINKER_2 3

ChainableLED leds = ChainableLED(7, 8, NUM_LED);
Adafruit_NeoPixel led_stick_front = Adafruit_NeoPixel(NUM_PIXEL_STICK, LED_STICK_PIN_FRONT, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel led_stick_back = Adafruit_NeoPixel(NUM_PIXEL_STICK, LED_STICK_PIN_BACK, NEO_GRB + NEO_KHZ800);

hw_timer_t *timer_left_blinkers = NULL;
hw_timer_t *timer_right_blinkers = NULL;

volatile bool left_blinkers_on = false;
volatile bool right_blinkers_on = false;
volatile bool start_left_blinkers = false;
volatile bool start_right_blinkers = false;

bool night_detected = false;
bool last_state_night_detected = false;

int threshold_luminosity = 1000;

// Left button pressed 
void IRAM_ATTR ISR_left_blinkers_button()
{
  if (!start_left_blinkers)
  {
    // Activate left blinkers : turn on the light, restart timer and enable interruptions
    start_left_blinkers = true;
    left_blinkers_on = true;
    digitalWrite(LEFT_BLINKER_LED, HIGH);
    leds.setColorRGB((byte)LEFT_BLINKER_1, 255, 100, 0);
    leds.setColorRGB((byte)LEFT_BLINKER_2, 255, 100, 0);
    timerAlarmEnable(timer_left_blinkers);
    timerRestart(timer_left_blinkers);
  }
  else
  {
    // Deactivate left blinkers : turn off the light and disable interruptions
    start_left_blinkers = false;
    left_blinkers_on = false;
    digitalWrite(LEFT_BLINKER_LED, LOW);
    leds.setColorRGB((byte)LEFT_BLINKER_1, 0, 0, 0);
    leds.setColorRGB((byte)LEFT_BLINKER_2, 0, 0, 0);
    timerAlarmDisable(timer_left_blinkers);
  }
}


// Right button pressed 
void IRAM_ATTR ISR_right_blinkers_button()
{
  if (!start_right_blinkers)
  {
    // Activate right blinkers : turn on the light, restart timer and enable interruptions
    start_right_blinkers = true;
    right_blinkers_on = true;
    digitalWrite(RIGHT_BLINKER_LED, HIGH);
    leds.setColorRGB((byte)RIGHT_BLINKER_1, 255, 100, 0);
    leds.setColorRGB((byte)RIGHT_BLINKER_2, 255, 100, 0);
    timerAlarmEnable(timer_right_blinkers);
    timerRestart(timer_right_blinkers);
  }
  else
  {
    // Deactivate right blinkers : turn off the light and disable interruptions
    start_right_blinkers = false;
    right_blinkers_on = false;
    digitalWrite(RIGHT_BLINKER_LED, LOW);
    leds.setColorRGB((byte)RIGHT_BLINKER_1, 0, 0, 0);
    leds.setColorRGB((byte)RIGHT_BLINKER_2, 0, 0, 0);
    timerAlarmDisable(timer_right_blinkers);
  }
}

// Left timer interruption : toggle the state of the left blinkers' light 
void IRAM_ATTR ISR_left_timer()
{
  if (left_blinkers_on)
  {
    left_blinkers_on = false;
    digitalWrite(LEFT_BLINKER_LED, HIGH);
    leds.setColorRGB((byte)LEFT_BLINKER_1, 255, 100, 0);
    leds.setColorRGB((byte)LEFT_BLINKER_2, 255, 100, 0);
  }
  else
  {
    left_blinkers_on = true;
    digitalWrite(LEFT_BLINKER_LED, LOW);
    leds.setColorRGB((byte)LEFT_BLINKER_1, 0, 0, 0);
    leds.setColorRGB((byte)LEFT_BLINKER_2, 0, 0, 0);
  }
}

// Right timer interruption : toggle the state of the right blinkers' light 
void IRAM_ATTR ISR_right_timer()
{
  if (right_blinkers_on)
  {
    right_blinkers_on = false;
    digitalWrite(RIGHT_BLINKER_LED, HIGH);
    leds.setColorRGB((byte)RIGHT_BLINKER_1, 255, 100, 0);
    leds.setColorRGB((byte)RIGHT_BLINKER_2, 255, 100, 0);
  }
  else
  {
    right_blinkers_on = true;
    digitalWrite(RIGHT_BLINKER_LED, LOW);
    leds.setColorRGB((byte)RIGHT_BLINKER_1, 0, 0, 0);
    leds.setColorRGB((byte)RIGHT_BLINKER_2, 0, 0, 0);
  }
}


void setup()
{
  Serial.begin(9600);
  Serial.println("-- SETUP --");
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LIGHT_SENSOR_PIN, INPUT);
  pinMode(LEFT_BLINKER_BUTTON, INPUT);
  pinMode(LEFT_BLINKER_LED, OUTPUT);
  pinMode(RIGHT_BLINKER_BUTTON, INPUT);
  pinMode(RIGHT_BLINKER_LED, OUTPUT);

  // External interruptions for buttons
  attachInterrupt(LEFT_BLINKER_BUTTON, ISR_left_blinkers_button, FALLING);
  attachInterrupt(RIGHT_BLINKER_BUTTON, ISR_right_blinkers_button, FALLING);

  // Configuration of the timers
  timer_left_blinkers = timerBegin(0, 80, true);
  timerAttachInterrupt(timer_left_blinkers, &ISR_left_timer, true);
  timerAlarmWrite(timer_left_blinkers, 600000, true);

  timer_right_blinkers = timerBegin(1, 80, true);
  timerAttachInterrupt(timer_right_blinkers, &ISR_right_timer, true);
  timerAlarmWrite(timer_right_blinkers, 600000, true);

  // Turn off the light
  leds.setColorRGB((byte)RIGHT_BLINKER_1, 0, 0, 0);
  leds.setColorRGB((byte)LEFT_BLINKER_1, 0, 0, 0);
  leds.setColorRGB((byte)RIGHT_BLINKER_2, 0, 0, 0);
  leds.setColorRGB((byte)LEFT_BLINKER_2, 0, 0, 0);

  led_stick_front.setBrightness(255);
  led_stick_front.begin();

  led_stick_back.setBrightness(255);
  led_stick_back.begin();

  Serial.println("- END OF SETUP --");
}

void loop()
{
  int light_value = analogRead(LIGHT_SENSOR_PIN);

  if (light_value < threshold_luminosity)
  {
    night_detected = true;
  }
  else
  {
    night_detected = false;
  }

  if (night_detected != last_state_night_detected)
  {
    if (night_detected)
    {
      // Turn on the light because night was detected
      for (int i = 0; i < NUM_PIXEL_STICK; i++)
      {
        led_stick_front.setPixelColor(i, led_stick_front.Color(255, 255, 255));
        led_stick_back.setPixelColor(i, led_stick_back.Color(255, 0, 0));
      }
      led_stick_front.show();
      led_stick_back.show();
    }
    else
    {
      // Turn off the light because day was detected
      for (int i = 0; i < NUM_PIXEL_STICK; i++)
      {
        led_stick_front.setPixelColor(i, led_stick_front.Color(0, 0, 0));
        led_stick_back.setPixelColor(i, led_stick_back.Color(0, 0, 0));
      }
      led_stick_front.show();
      led_stick_back.show();
    }
  }

  last_state_night_detected = night_detected;

  delay(100);
}