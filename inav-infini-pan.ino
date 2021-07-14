#include <Servo.h>
#include <ESP32TimerInterrupt.h>
#include <ESP32_ISR_Timer-Impl.h>
#include <ESP32_ISR_Timer.h>
#include <SBUS.h>

#define PIN_1 12
#define PIN_2 32
#define PIN_3 25
#define PIN_4 27
#define PIN_TLT 4
#define PAN_CH 6
#define TLT_CH 4
#define HOM_CH 5

#define SBUS_MIN 192
#define SBUS_MAX 1792
#define SBUS_MID ((SBUS_MAX-SBUS_MIN)/2 + SBUS_MIN)
#define SBUS_MIDRANGE ((SBUS_MAX-SBUS_MIN)/2)
#define STEPTIME_MIN 900
#define STEPTIME_MAX 5000

SBUS sbus(Serial2);
ESP32Timer myTimer(1);
Servo tiltServo;

volatile uint8_t stepNo = 0;
volatile uint16_t pos = 0;
bool stepDir = false;

uint16_t channels[16];
bool failSafe;
bool lostFrame;
bool timerRunning = false;
bool homing;

void IRAM_ATTR Tick()
{
  Step();
}

void setup() {
  pinMode(PIN_1, OUTPUT);
  pinMode(PIN_2, OUTPUT);
  pinMode(PIN_3, OUTPUT);
  pinMode(PIN_4, OUTPUT);
  sbus.begin();
  tiltServo.attach(PIN_TLT);
}

void loop()
{
  if (homing && pos == 0)
  {
    myTimer.detachInterrupt();
    timerRunning = false;
  }
  if (sbus.read(&channels[0], &failSafe, &lostFrame))
  {
    noInterrupts();
    uint16_t p = channels[PAN_CH];
    uint16_t t = channels[TLT_CH];
    uint16_t h = channels[HOM_CH];
    interrupts();

    if (h > SBUS_MID + (SBUS_MID/2))
    {
      if (!homing)
      {
        homing = true;
        myTimer.attachInterruptInterval(STEPTIME_MIN, Tick);
        timerRunning = true;
        stepDir = (pos >= 2048);
      }
    }
    else
    {
      homing = false;
      uint16_t panCh;
      tiltServo.write(map(t, SBUS_MIN, SBUS_MAX, 0, 180));
      if (p > SBUS_MID)
      {
        panCh = map(p - SBUS_MID, 0, SBUS_MIDRANGE, 0, 1000);
        stepDir = false;
      }
      else
      {
        panCh = map(SBUS_MID - p, 0, SBUS_MIDRANGE, 0, 1000);
        stepDir = true;
      }
      if (panCh > 50 && !timerRunning)
      {
        myTimer.attachInterruptInterval(map(panCh, 0, 1000, STEPTIME_MAX, STEPTIME_MIN), Tick);
        timerRunning = true;
      }
      else if (panCh > 50)
        myTimer.setInterval(map(panCh, 0, 1000, STEPTIME_MAX, STEPTIME_MIN), Tick);
      else
      {
        myTimer.detachInterrupt();
        timerRunning = false;
      }
    }
  }
}

void Step()
{
  if (stepDir)
  {
    stepNo = (stepNo + 1) % 8;
    pos = (pos + 1) % 4096;
  }
  else
  {
    if (stepNo == 0)
      stepNo = 7;
    else
      stepNo--;

    if (pos == 0)
      pos = 4095;
    else
      pos--;
  }
  digitalWrite(PIN_1, stepNo == 7 || stepNo == 0 || stepNo == 1);
  digitalWrite(PIN_2, stepNo == 1 || stepNo == 2 || stepNo == 3);
  digitalWrite(PIN_3, stepNo == 3 || stepNo == 4 || stepNo == 5);
  digitalWrite(PIN_4, stepNo == 5 || stepNo == 6 || stepNo == 7);
}