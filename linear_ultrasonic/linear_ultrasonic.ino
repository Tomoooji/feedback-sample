//#define ESP32

#include <PS4Controller.h>
#include "AnalogMotorDriver.h"
#include "UltraSonic.h"

constexpr char mac[] = "00:00:00:00:00:00";

namespace PINs {
uint8_t dcmotor[2] = { 19, 18 };
uint8_t limitswitch_a = 12;
uint8_t limitswitch_b = 4;
uint8_t trig = 13;
uint8_t echo = 5;
}

AnalogMotor DC{};

HC_SR04 Sensor{
  100000,  // timeout
  10,      // send time long
  200,     // wait time long
};

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

void setup() {
  DC.attach(PINs::dcmotor);
  pinMode(PINs::limitswitch_a, INPUT_PULLUP);
  pinMode(PINs::limitswitch_b, INPUT_PULLUP);
  Sensor.attach(PINs::trig, PINs::echo);
  Sensor.read();
  PS4.begin(mac);
}

void loop() {
  int speed = 0;
  if (PS4.isConnected()) {
    length_target = map_target(PS4.Circle(), PS4.Triangle(), PS4.Square());
    speed = (length_target - Sensor.readDist()) * 0.1;  // 脳筋P制御
    if (!digitalRead(PINs::limitswitch_a) && speed < 0) speed = 0;
    if (!digitalRead(PINs::limitswitch_b) && speed > 0) speed = 0;
  }
  DC.move(speed);
}