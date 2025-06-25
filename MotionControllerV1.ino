#include <Wire.h>
#include <MPU6050.h>
#include <Arduino.h>
#include <BleGamepad.h>

BleGamepad controller;

MPU6050 mpu;//SCL(GPIO22),SDA(GPIO21).

uint16_t mapFloat(float x, float in_min, float in_max, int out_min, int out_max) {
  return (uint16_t)((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
}
float gyroXangle = 0, gyroYangle = 0, gyroZangle = 0;
unsigned long lastTime;

const int touchPins[6] = {T0, T1, T2, T3, T4, T5}; 
const int gpioPins[6]  = {13, 32, 33, 12, 4, 15};  
const int numButtons = 6;
const int threshold = 60;

bool buttonStates[6] = {false, false, false, false, false, false};

void setup(){
  controller.begin();
  mpu.begin();
  lastTime = millis();
  pinMode(LED_BUILTIN, OUTPUT);
}
void loop() {
  
}
