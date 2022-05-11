#include "config.h"

void onConnect() {
#ifdef DEBUG
  Serial << "Connected." << endl;
#endif
  psMillis = millis();
}

void notify() {
  //Ball Loading
  if ( Ps3.event.button_down.square && initiate) {
    digitalWrite(DIR, 1);
    analogWrite(PWM, 255);
#ifdef DEBUG
    Serial << "Loading..." << endl;
#endif
    delay(300);
    analogWrite(PWM, 0);
#ifdef DEBUG
    Serial << "Brake..." << endl;
#endif
  }

  if ( Ps3.event.button_down.circle && initiate) {
    digitalWrite(DIR, 0);
    analogWrite(PWM, 150);
#ifdef DEBUG
    Serial << "Un-Loading..." << endl;
#endif
    delay(300);
    analogWrite(PWM, 0);

#ifdef DEBUG
    Serial << "Brake..." << endl;
#endif
  }
  
  if ( Ps3.event.button_down.triangle && initiate) {
    increaseCIM();
  }

  if ( Ps3.event.button_down.cross && initiate) {
    decreaseCIM();
  }

  // For Using PS
  if ( Ps3.event.button_down.start && !initiate) {
    initiate = true;
#ifdef DEBUG
    Serial << "All Initiate..." << endl;
#endif
  } else if (Ps3.event.button_down.start && initiate) {
    initiate = false;
    onPSDisconnect();
#ifdef DEBUG
    Serial << "All Stops..." << endl;
#endif
  }
}

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, 26, 32);
  UART.setSerialPort(&Serial1);
  Serial2.begin(115200, SERIAL_8N1, 14, 27);
  pinMode(PWM,OUTPUT);
  pinMode(DIR,OUTPUT);
  

#ifdef DEBUG
  Serial << "Ready." << endl;
#endif
  Ps3.attach(notify);
  Ps3.attachOnConnect(onConnect);
  Ps3.begin("11:11:11:11:11:11");
}

void loop() {
  if (bldcStatus == true)
    UART.setRPM(speed);

  if (Ps3.data.analog.stick.ly == -128)
    incrAngle();
  else if (Ps3.data.analog.stick.ly == 127)
    decrAngle();
  else
    brakeM2();

  if (Ps3.data.analog.stick.lx == -128)
    turretClock();
  else if (Ps3.data.analog.stick.lx == 127)
    turretAntiClock();
  else
    brakeM1();



  //---------------------- Battery events ---------------------
  if ( battery != Ps3.data.status.battery ) {
    battery = Ps3.data.status.battery;
#ifdef DEBUG
    Serial.print("The controller battery is ");
#endif
    if ( battery == ps3_status_battery_charging )
    {
      player += 1;
      if (player > 8) player = 0;
#ifdef DEBUG
      Serial << "Charging" << endl;
#endif
    }
    else if ( battery == ps3_status_battery_full )
    {
      Ps3.setPlayer(10);
#ifdef DEBUG
      Serial << "FULL" << endl;
#endif
    }
    else if ( battery == ps3_status_battery_high )
    {
      Ps3.setPlayer(9);
#ifdef DEBUG
      Serial << "HIGH" << endl;
#endif
    }
    else if ( battery == ps3_status_battery_low)
    {
      Ps3.setPlayer(7);
#ifdef DEBUG
      Serial << "LOW" << endl;
#endif
    }
    else if ( battery == ps3_status_battery_dying )
    {
      Ps3.setPlayer(4);
#ifdef DEBUG
      Serial << "DYING" << endl;
#endif
    }
    else if ( battery == ps3_status_battery_shutdown ) {
#ifdef DEBUG
      Serial << "SHUTDOWN" << endl;
#endif
    } else {
#ifdef DEBUG
      Serial << "UNDEFINED" << endl;
#endif
    }
  }
}
