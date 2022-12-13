#include <Arduino.h>
#include <ChainableLED.h>
#include <Adafruit_NeoPixel.h>

#define LIGHT_SENSOR_PIN 0
#define LED_STICK_PIN_FRONT 6
#define LED_STICK_PIN_BACK 10
#define NUM_LED 3
#define NUM_PIXEL_STICK 10

#define LEFT_BLINKER_BUTTON 5
#define LEFT_BLINKER_LED 4
#define RIGHT_BLINKER_BUTTON 18
#define RIGHT_BLINKER_LED 19

#define LEFT_BLINKER 0
#define RIGHT_BLINKER 1

ChainableLED leds = ChainableLED(7, 8, NUM_LED);
Adafruit_NeoPixel led_stick_front = Adafruit_NeoPixel(NUM_PIXEL_STICK, LED_STICK_PIN_FRONT, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel led_stick_back = Adafruit_NeoPixel(NUM_PIXEL_STICK, LED_STICK_PIN_BACK, NEO_GRB + NEO_KHZ800);

volatile long int last_push_left = 0;
volatile long int last_push_right = 0;
hw_timer_t *timer_left_blinkers = NULL;
hw_timer_t *timer_right_blinkers = NULL;

volatile bool left_blinkers_on = false;
volatile bool right_blinkers_on = false;
volatile bool start_left_blinkers = false;
volatile bool start_right_blinkers = false;

bool night_detected = false;
bool last_state_night_detected = false;

int threshold_luminosity = 1000;

void IRAM_ATTR ISR_left_blinkers_button()
{
  ets_printf("Left");
  if (!start_left_blinkers)
  {
    start_left_blinkers = true;
    left_blinkers_on = true;
    digitalWrite(LEFT_BLINKER_LED, HIGH);
    leds.setColorRGB((byte)LEFT_BLINKER, 255, 100, 0);
    timerAlarmEnable(timer_left_blinkers);
    timerRestart(timer_left_blinkers);
  }
  else
  {
    start_left_blinkers = false;
    left_blinkers_on = false;
    digitalWrite(LEFT_BLINKER_LED, LOW);
    leds.setColorRGB((byte)LEFT_BLINKER, 0, 0, 0);
    timerAlarmDisable(timer_left_blinkers);
  }
}

void IRAM_ATTR ISR_right_blinkers_button()
{
  ets_printf("Right");
  if (!start_right_blinkers)
  {
    start_right_blinkers = true;
    right_blinkers_on = true;
    digitalWrite(RIGHT_BLINKER_LED, HIGH);
    leds.setColorRGB((byte)RIGHT_BLINKER, 255, 100, 0);
    timerAlarmEnable(timer_right_blinkers);
    timerRestart(timer_right_blinkers);
  }
  else
  {
    start_right_blinkers = false;
    right_blinkers_on = false;
    digitalWrite(RIGHT_BLINKER_LED, LOW);
    leds.setColorRGB((byte)RIGHT_BLINKER, 0, 0, 0);
    timerAlarmDisable(timer_right_blinkers);
  }
}

void IRAM_ATTR ISR_left_timer()
{
  if (left_blinkers_on)
  {
    left_blinkers_on = false;
    digitalWrite(LEFT_BLINKER_LED, HIGH);
    leds.setColorRGB((byte)LEFT_BLINKER, 255, 100, 0);
  }
  else
  {
    left_blinkers_on = true;
    digitalWrite(LEFT_BLINKER_LED, LOW);
    leds.setColorRGB((byte)LEFT_BLINKER, 0, 0, 0);
  }
}

void IRAM_ATTR ISR_right_timer()
{
  if (right_blinkers_on)
  {
    right_blinkers_on = false;
    digitalWrite(RIGHT_BLINKER_LED, HIGH);
    leds.setColorRGB((byte)RIGHT_BLINKER, 255, 100, 0);
  }
  else
  {
    right_blinkers_on = true;
    digitalWrite(RIGHT_BLINKER_LED, LOW);
    leds.setColorRGB((byte)RIGHT_BLINKER, 0, 0, 0);
  }
}

// void IRAM_ATTR ISR_left_blinkers_button()
// {
//   if (!start_right_blinkers)
//   {
//     if ((millis() - last_push_left) > 100)
//     {
//       ets_printf("Left pressed\n");
//       last_push_left = millis();
//       if (start_left_blinkers)
//       {
//         start_left_blinkers = false;
//       }
//       else
//       {
//         start_left_blinkers = true;
//       }
//     }
//   }
// }

// void IRAM_ATTR ISR_right_blinkers_button()
// {
//   if (!start_left_blinkers)
//   {
//     if ((millis() - last_push_right) > 100)
//     {
//       ets_printf("Right pressed\n");
//       last_push_right = millis();
//       if (start_right_blinkers)
//       {
//         start_right_blinkers = false;
//       }
//       else
//       {
//         start_right_blinkers = true;
//       }
//     }
//   }
// }

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

  timer_left_blinkers = timerBegin(0, 80, true);
  timerAttachInterrupt(timer_left_blinkers, &ISR_left_timer, true);
  timerAlarmWrite(timer_left_blinkers, 600000, true);

  timer_right_blinkers = timerBegin(1, 80, true);
  timerAttachInterrupt(timer_right_blinkers, &ISR_right_timer, true);
  timerAlarmWrite(timer_right_blinkers, 600000, true);

  leds.setColorRGB((byte)RIGHT_BLINKER, 0, 0, 0);
  leds.setColorRGB((byte)LEFT_BLINKER, 0, 0, 0);

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
  Serial.println(night_detected);

  if (night_detected != last_state_night_detected)
  {
    if (night_detected)
    {
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