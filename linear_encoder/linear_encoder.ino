//#define ESP32

#include <PS4Controller.h>
#include "AnalogMotorDriver.h"


constexpr char mac[] = "00:00:00:00:00:00";

namespace PINs {
constexpr uint8_t dcmotor[2] = { 19, 18 };
constexpr uint8_t limitswitch_a = 12;
constexpr uint8_t limitswitch_b = 4;
constexpr uint8_t encoder_a = 13;
constexpr uint8_t encoder_b = 5;
}

AnalogMotor DC{};

int length_target;

int map_target(bool pos1, bool pos2, bool pos3) {
  // 2進数に変換してるので必要なら他の組み合わせ(pos1 & pos2とか)も指定できる
  switch (pos1)
    | (pos2 << 1) | (pos3 << 2) {
      case 0:  // all pos is false
        return 0;
      case 1:  // only pos1 true
        return 10;
      case 3:  // only pos2 true
        return 20;
      case 5:  // only pos3 true
        return 30;
    }
}

constexpr float cm_per_pulse = 0.23;
constexpr float length_max;
volatile float length_now = length_max/2;
portMUX_TYPE mux = portMUX_INITALIZER_UNLOCKED;

void IRAM_ATTR measure_isr() {
  portENTER_CRITICAL_ISR(&mux);
  if (digitalRead(PINs::encorder_a) == digitalRead(PINs::encorder_b)) {
    length_now += cm_per_pulse;
  } else {
    length_now -= cm_per_pulse;
  }
  portEXIT_CRITICAL_ISR(&mux);
  ;
}

void setup() {
  DC.attach(PINs::dcmotor);
  pinMode(PINs::limitswitch_a, INPUT_PULLUP);
  pinMode(PINs::limitswitch_b, INPUT_PULLUP);
  pinMode(PINs::encoder_a, INPUT);
  pinMode(PINs::encoder_b, INPUT);
  attachInterrupt(digitalPinToInterrupt(PINs::encorder_a, ) measure_isr, CHANGE);
  PS4.begin(mac);
}

void loop() {
  int speed = 0;
  if (PS4.isConnected()) {
    length_target = map_target(PS4.Circle(), PS4.Triangle(), PS4.Square());
    portENTER_CRITICAL(&mux);
    speed = (length_target - length_now) * 0.1;  // 脳筋P制御
    if (!digitalRead(PINs::limitswitch_a)) {
      length_now = 0;
      if (speed < 0) speed = 0;
    }
    if (!digitalRead(PINs::limitswitch_b)) {
      length_now = length_max;
      if (speed > 0) speed = 0;
    }
    portEXIT_CRITICAL(&mux);
  }
  DC.move(speed);
}