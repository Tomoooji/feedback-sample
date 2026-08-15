#pragma once
#include <Arduino.h>

constexpr int SONIC_SPEED = 340;

class HC_SR04{
private:
  volatile float _distance;
  uint8_t _trig;
  uint8_t _echo;
  const int _send_time_long;//[microsec]
  const int _wait_time_long;//[millisec]
  const unsigned long _timeout;//[microsec]
  unsigned long _last_send_time = -1;
  
public:
  HC_SR04(const unsigned long timeout = 100000/*microsec*/, const int SendTimeLong = 10/*microsec*/, const int WaitTimeLong = 200/*millisec*/):
    _send_time_long(SendTimeLong), _wait_time_long(WaitTimeLong), _timeout(timeout){}

  void attach(uint8_t pinTrig, uint8_t pinEcho){
    this->_trig = pinTrig;
    this->_echo = pinEcho;
    pinMode(this->_trig, OUTPUT);
    pinMode(this->_echo, INPUT);
  }

  bool readDist(){
    if(this->_last_send_time==-1 || millis()-this->_last_send_time > this->_wait_time_long){
      //digitalWrite(this->_trig, LOW);
      //delayMicroseconds(2);
      digitalWrite(this->_trig, HIGH);
      delayMicroseconds(_send_time_long);
      digitalWrite(this->_trig, LOW);

      return this->_distance = this->_calcDist(pulseIn(this->_echo, HIGH, this->_timeout));
    }
    return -1;
  }

  int _calcDist(unsigned long pulseWidth){
    return SONIC_SPEED/2*pulseWidth*0.0001;
  }

  const int distance(){
    return this->_distance;
  }

};