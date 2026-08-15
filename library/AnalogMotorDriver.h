#pragma once

inline int sign(auto x){
  return (x>0) - (x<0);
}

class AnalogMotor_Base{
 protected:
  const uint8_t* _pins = nullptr;
  int _speed=0;
 public:
  AnalogMotor_Base(){}
  virtual void attach(const uint8_t pins[])=0;
  virtual void move(int speed) = 0;
  void move(){
    return this->move(this->_speed);
  }
  int set_speed(int speed){
    this->_speed = constrain(speed, -255, 255);
    return this->_speed;
  }
  const int get_speed(){
    return this->_speed;
  }
  const uint8_t get_pin(uint8_t idx){
    return this->_pins[idx];
  }
};

#if defined(ARDUINO_ARCH_AVR)

class AnalogMotor_Arduino: public AnalogMotor_Base{
  public:
  using AnalogMotor_Base::AnalogMotor_Base;
  void attach(const uint8_t pins[])override{
    this->_pins = pins;
    pinMode(this->_pins[0], OUTPUT);
    pinMode(this->_pins[1], OUTPUT);
  }
  void move(int speed)override{
    this->_speed = constrain(abs(speed), 0, 255) * sign(speed);
    analogWrite(this->_pins[0], this->_speed>0?  this->_speed: 0);
    analogWrite(this->_pins[1], this->_speed<0? -this->_speed: 0);
  }
};
using AnalogMotor = AnalogMotor_Arduino;

class AnalogMotor_3pin_Arduino: public AnalogMotor_Base{
  public:
  using AnalogMotor_Base::AnalogMotor_Base;
  void attach(const uint8_t pins[])override{
    this->_pins = pins;
    pinMode(this->_pins[0], OUTPUT);
    pinMode(this->_pins[1], OUTPUT);
    pinMode(this->_pins[2], OUTPUT);
  }
  void move(int speed)override{
    this->_speed = constrain(abs(speed), 0, 255) * sign(speed);
    digitalWrite(this->_pins[0], this->_speed>0);
    digitalWrite(this->_pins[1], this->_speed<0);
    analogWrite(this->_pins[2], abs(this->_speed));
  }
};
using AnalogMotor_3pin = AnalogMotor_3pin_Arduino;

#elif defined(ESP32)

class AnalogMotor_ESP32:public AnalogMotor_Base{
  public:
  using AnalogMotor_Base::AnalogMotor_Base;
  void attach(const uint8_t pins[])override{
    this->_pins = pins;
    ledcAttach(this->_pins[0], 12800, 8);
    ledcAttach(this->_pins[1], 12800, 8);
  }
  void move(int speed)override{
    this->_speed = constrain(abs(speed), 0, 255) * sign(speed);
    ledcWrite(this->_pins[0], this->_speed>0?  this->_speed: 0);
    ledcWrite(this->_pins[1], this->_speed<0? -this->_speed: 0);
  }
};
using AnalogMotor = AnalogMotor_ESP32;

class AnalogMotor_3pin_ESP32: public AnalogMotor_Base{
  public:
  using AnalogMotor_Base::AnalogMotor_Base;
  void attach(const uint8_t pins[])override{
    this->_pins = pins;
    pinMode(this->_pins[0], OUTPUT);
    pinMode(this->_pins[1], OUTPUT);
    ledcAttach(this->_pins[2], 12800, 8);
  }
  void move(int speed)override{
    this->_speed = constrain(abs(speed), 0, 255) * sign(speed);
    digitalWrite(this->_pins[0], this->_speed>0);
    digitalWrite(this->_pins[1], this->_speed<0);
    ledcWrite(this->_pins[2], abs(this->_speed));
  }
};
using AnalogMotor_3pin = AnalogMotor_3pin_ESP32;

#endif
