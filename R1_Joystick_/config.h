#include "Motor.h"
#include<Ps3Controller.h>
#include "RoboClaw.h"
#include <VescUart.h>

#define address 0x80
#define DEBUG
#define endl "\n"
#define PWM 6
#define DIR 7

template<class T> inline Print& operator<<(Print& serial, T args) {
  serial.print(args);
  return serial;
}

RoboClaw roboclaw(&Serial2, 10000);
VescUart UART;

int battery = 0, player = 0;
int speed = 5000;
unsigned long psMillis;
boolean initiate = true;
boolean bldcStatus = false;

float mapf(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void turretAntiClock() {
  roboclaw.ForwardM1(address, 40);
}

void turretClock() {
  roboclaw.BackwardM1(address, 40);
}

void brakeM1() {
  roboclaw.BackwardM1(address, 0);
}

void decrAngle() {
  roboclaw.ForwardM2(address, 64);
}

void incrAngle() {
  roboclaw.BackwardM2(address, 64);
}

void brakeM2() {
  roboclaw.BackwardM2(address, 0);
}

void decreaseCIM() {
  if (bldcStatus == true) {
    if (speed > 0)
      speed -= 5000;
    if (speed <= 0)
      speed = 0;
  }
#ifdef DEBUG
  Serial << "BLDC Speed Decrease... "  << speed << endl;
#endif
}

void increaseCIM() {
#ifdef DEBUG
  Serial << "BLDC Speed Increase... "  << speed << endl;
#endif
  if (bldcStatus == false) {
    for (int i = 0; i < speed; i += 1000) {
      UART.setRPM(i);
      delay(50);
    }
    bldcStatus = true;
  } else if (bldcStatus == true) {
    if (speed <= 50000)
      speed += 5000;
    if (speed > 50000)
      speed = 50000;
  }
}

void onPSDisconnect() {
  UART.setRPM(0);
  analogWrite(PWM, 0);
}
