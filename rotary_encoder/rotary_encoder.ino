//#define ESP32

#include <PS4Controller.h>
#include "AnalogMotorDriver.h"


constexpr char mac[] = "00:00:00:00:00:00";

namespace PINs {
constexpr uint8_t dcmotor[2] = { 19, 18 };
constexpr uint8_t limitswitch = 12;
constexpr uint8_t encoder_a = 13;
constexpr uint8_t encoder_b = 5;
}

AnalogMotor DC{};

int angle_target;
constexpr int ignore_range = 10;

int clip360(int deg) {
  return deg > 360 ? deg - 360 : (deg < 0 ? deg + 360 : deg);
}

int map_target(int8_t val_x, int8_t val_y) {
  return sq(val_x) + sq(val_y) > sq(ignore_range) ? clip360(degrees(atan2(val_y, val_x))) : -1;
}

constexpr float deg_per_pulse = 500 / 17 * 72;
volatile float angle_now;
portMUX_TYPE mux = portMUX_INITALIZER_UNLOCKED;

void IRAM_ATTR count_isr() {
  portENTER_CRITICAL_ISR(&mux);
  if (digitalRead(PINs::encorder_a) == digitalRead(PINs::encorder_b)) {
    angle_now += deg_per_pulse;
    if (angle_now > 360) angle_now -= 360;
  } else {
    angle_now -= deg_per_pulse;
    if (angle_now < 0) angle_now += 360;
  }
  portEXIT_CRITICAL_ISR(&mux);
  ;
}

void setup() {
  DC.attach(PINs::dcmotor);
  pinMode(PINs::limitswitch, INPUT_PULLUP);
  pinMode(PINs::encoder_a, INPUT);
  pinMode(PINs::encoder_b, INPUT);
  attachInterrupt(digitalPinToInterrupt(PINs::encorder_a, ) measure_isr, CHANGE);
  PS4.begin(mac);
}

void loop() {
  int speed = 0;
  if (PS4.isConnected()) {
    target = map_target(PS4.RStickX(), PS4.RStickY());
    if (target > 0) {
      portENTER_CRITICAL(&mux);
      speed = (angle_target - angle_now) * 5;  // 脳筋P制御
      if (!digitalRead(PINs::limitswitch)) angle_now = 0;
      portEXIT_CRITICAL(&mux);
    }
  }
  DC.move(speed);
}