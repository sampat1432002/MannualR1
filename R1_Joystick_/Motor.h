/* Motor library, for controlling motor with motor driver
  Copyright (c) 2022, GTU Robotics Club

  Version 1.0 - initial release

  Micro-controller support : espressif32-dev,  arduino boards
  Motor-driver support     : cytron-mdds30a,   pin-based motor-drivers
*/

#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>

#if defined(ARDUINO_ARCH_ESP32)     // checks if compiling for espressif32-dev board 
#include <analogWrite.h>            // include analogWrite library for espressif32-dev
#endif

#define ML 0                         // left channel for cytron 0b0000'0'000
#define MR 1                         // right channel for cytron 0b0000'1'000
#define dummy_byte 128              // dummy byte for cytron

class Motor {
  private:
    void send_packet();                     // send data to cytron-mdds30a
  protected:
    byte inA, inB, pwm;                     // pins for pin-based motor-driver
    byte cytron_packet[4] = {85, 0, 0, 0};  // packet for cytron-mdds30a
    bool cytron = false;                    // select the cytron-mdds30a mode
    HardwareSerial *mdds_serial = NULL;             // cytron-mdds30a serial pointer
  public:
    Motor(byte inA, byte inB, byte pwm);            // pin-based motor driver selection
    Motor(HardwareSerial* mdds_serial, byte channel, byte address);   // cytron-mdds30A motor driver selection 
    void config(long baudrate);                 // configuring cytron-mdds30a baudrate
    void config(long baudrate, byte rx_pin, byte tx_pin);     // configuring cytron-mdds30a baudrate with extension of esp32 gpio matrix
    void anti_clockwise(byte speed);
    void clockwise(byte speed);
    void brake();
};

#endif
