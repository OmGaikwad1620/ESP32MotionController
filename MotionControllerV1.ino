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

int jx = map(analogRead(35),0,4096,0,32767);
int jy = map(analogRead(34),0,4096,32767,0);

int16_t ax, ay, az, gx, gy, gz;
  
mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  
  float accX = ax / 16384.0;
  float accY = ay / 16384.0;
  float accZ = az / 16384.0;
  float gyroX = gx / 131.0;
  float gyroY = gy / 131.0;
  float gyroZ = gz / 131.0;

  unsigned long currentTime = millis();
  float dt = (currentTime - lastTime) / 1000.0;
  lastTime = currentTime;

  float accPitch = atan2(accY, sqrt(accX * accX + accZ * accZ)) * 180 / PI;
  float accRoll  = atan2(-accX, accZ) * 180 / PI;

  float alpha = 0.98;

  gyroXangle = alpha * (gyroXangle + gyroX * dt) + (1 - alpha) * accPitch;
  gyroYangle = alpha * (gyroYangle + gyroY * dt) + (1 - alpha) * accRoll;
  gyroZangle += gyroZ * dt;

  // Clamp angles to expected range
  gyroXangle = constrain(gyroXangle, -90, 90);   // pitch
  gyroYangle = constrain(gyroYangle, -90, 90);   // roll
  gyroZangle = fmod(gyroZangle + 180.0, 360.0) - 180.0;  // wrap yaw to -180 to 180

  // Map to 0 - 32767
  int pitchMapped = mapFloat(gyroXangle, -90, 90, 0, 32767);
  int rollMapped  = mapFloat(gyroYangle, -90, 90, 0, 32767);
  int yawMapped   = mapFloat(gyroZangle, -180, 180, 0, 65475);
if(controller.isConnected())
{
  digitalWrite(LED_BUILTIN,HIGH);
  controller.setAxes(jx, jy, pitchMapped, rollMapped, 0, rollMapped, 0, 0);       //(X, Y, Z, RX, RY, RZ)
      for (int i = 0; i < numButtons; i++) {
      int touchVal = touchRead(gpioPins[i]);
      bool isTouched = touchVal < threshold;
      if (isTouched && !buttonStates[i]) {
        controller.press(i + 1); // Gamepad buttons are 1-indexed
        buttonStates[i] = true;
      } else if (!isTouched && buttonStates[i]) {
        controller.release(i + 1);
        buttonStates[i] = false;
      }
    }
  controller.sendReport();
}
else
{
  digitalWrite(LED_BUILTIN,LOW);
}
}
