#include "Motor.h"

// variable for checking if dummy byte is already sent or not
extern bool configure_cytron = true;

// constructor for pin-based motor driver
Motor::Motor(byte inA, byte inB, byte pwm) {
  this->inA = inA;
  this->inB = inB;
  this->pwm = pwm;
  pinMode(this->inA, OUTPUT);
  pinMode(this->inB, OUTPUT);
  pinMode(this->pwm, OUTPUT);
}

/*
   constructor for cytron-mdds30a motor driver
   cytron_packet[4] = { header, channel + address, speed, checksum }
   channel + address byte is obtained by shifting 3 bits of channel and performing or operation on it by address byte
   Example : channel : '1' or '0b00000001', address : '1' or '0b00000001'
   1-> channel << 3           -> 0b00001000
   2-> channel << 3 | address -> 0b00001001
   3-> hence, channel + address byte is obtained i->e-> 0b00001001
*/
Motor::Motor(HardwareSerial* mdds_serial, byte channel, byte address) {
  this->mdds_serial = mdds_serial;
  cytron_packet[1] = channel << 3 | address;
  cytron = true;                              // switching mode to cytron-mdds30a
}

void Motor::config(long baudrate) {
  if (!cytron)
    return;
  if (configure_cytron) {                         // initialises cytron and configures dummy byte if not done already
    this->mdds_serial->begin(baudrate);
    delay(1000);
    this->mdds_serial->write(dummy_byte);
    configure_cytron = false;
  }
}

// gpio matrix accessibility for uart in esp32 
void Motor::config(long baudrate, byte rx_pin, byte tx_pin) {
  if (!cytron)
    return;
#if defined(ARDUINO_ARCH_ESP32)
  if (configure_cytron) {                         // initialises cytron and configures dummy byte if not done already
    this->mdds_serial->begin(baudrate, SERIAL_8N1, rx_pin, tx_pin);
    delay(1000);
    this->mdds_serial->write(dummy_byte);
    configure_cytron = false;
  }
#endif
}

void Motor::anti_clockwise(byte speed) {
  if (!cytron) {                                      // check for pin-based motor-driver mode
    digitalWrite(inA, HIGH);
    digitalWrite(inB, LOW);
    analogWrite(pwm, speed);
  } else {
    cytron_packet[2] = map(speed, 0, 255, 127, 0);  // mapping the command byte for speed as per locked anti-phase
    send_packet();                                  // send packets to cytron-mdds30a
  }
}

void Motor::clockwise(byte speed) {
  if (!cytron) {                                       // check for pin-based motor-driver mode
    digitalWrite(inA, LOW);
    digitalWrite(inB, HIGH);
    analogWrite(pwm, speed);
  } else {
    cytron_packet[2] = map(speed, 0, 255, 127, 255); // mapping the command byte for speed as per locked anti-phase
    send_packet();                                   // send packets to cytron-mdds30a
  }
}

void Motor::brake() {
  if (!cytron) {
    digitalWrite(inA, LOW);
    digitalWrite(inB, LOW);
    analogWrite(pwm, 0);
  } else {
    cytron_packet[2] = 127;                         // brake in locked anti-phase is given by 127
    send_packet();                                  // send packets to cytron-mdds30a
  }
}

void Motor::send_packet() {
  cytron_packet[3] = cytron_packet[0] + cytron_packet[1] + cytron_packet[2];  // check sum calculation for cytron-mdds30a
  for (byte idx = 0; idx < 4; idx++)
    this->mdds_serial->write(cytron_packet[idx]);
}
