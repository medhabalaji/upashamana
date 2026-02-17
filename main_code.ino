#include "BluetoothSerial.h"
BluetoothSerial SerialBT;

// Motor driver pins
const int M1_A = 25;
const int M1_B = 26;

const int M2_A = 27;
const int M2_B = 33;

const int M3_A = 14;
const int M3_B = 12;

const int M4_A = 32;
const int M4_B = 13;

// PWM parameters
const int pwmFreq = 2000;
const int pwmResolution = 8;

void setup() {
  Serial.begin(115200);

  pinMode(M1_A, OUTPUT); pinMode(M1_B, OUTPUT);
  pinMode(M2_A, OUTPUT); pinMode(M2_B, OUTPUT);
  pinMode(M3_A, OUTPUT); pinMode(M3_B, OUTPUT);
  pinMode(M4_A, OUTPUT); pinMode(M4_B, OUTPUT);

  digitalWrite(M1_A, LOW); digitalWrite(M1_B, LOW);
  digitalWrite(M2_A, LOW); digitalWrite(M2_B, LOW);
  digitalWrite(M3_A, LOW); digitalWrite(M3_B, LOW);
  digitalWrite(M4_A, LOW); digitalWrite(M4_B, LOW);

  delay(300);

  // PWM attach (ESP32 Core v3)
  ledcAttach(M1_A, pwmFreq, pwmResolution);
  ledcAttach(M2_A, pwmFreq, pwmResolution);
  ledcAttach(M3_A, pwmFreq, pwmResolution);
  ledcAttach(M4_A, pwmFreq, pwmResolution);

  ledcWrite(M1_A, 0);
  ledcWrite(M2_A, 0);
  ledcWrite(M3_A, 0);
  ledcWrite(M4_A, 0);

  SerialBT.begin("ESP32_Upashamana");
  Serial.println("READY | Manual + Presets");
}

// ------------------------------------------------

void setMotor(int motor, int value) {
  value = constrain(value, 0, 255);

  if (motor == 1) {
    ledcWrite(M1_A, value);
    digitalWrite(M1_B, LOW);
  }
  else if (motor == 2) {
    ledcWrite(M2_A, value);
    digitalWrite(M2_B, LOW);
  }
  else if (motor == 3) {
    ledcWrite(M3_A, value);
    digitalWrite(M3_B, LOW);
  }
  else if (motor == 4) {
    ledcWrite(M4_A, value);
    digitalWrite(M4_B, LOW);
  }
}

// ------------------------------------------------

void stopAll() {
  setMotor(1, 0);
  setMotor(2, 0);
  setMotor(3, 0);
  setMotor(4, 0);
}

// ------------------------------------------------

void processCommand(String cmd, bool fromBT) {
  cmd.trim();

  // -------- MANUAL CONTROL (UNCHANGED) --------
  if (cmd.startsWith("M1:")) {
    setMotor(1, cmd.substring(3).toInt());
  }
  else if (cmd.startsWith("M2:")) {
    setMotor(2, cmd.substring(3).toInt());
  }
  else if (cmd.startsWith("M3:")) {
    setMotor(3, cmd.substring(3).toInt());
  }
  else if (cmd.startsWith("M4:")) {
    setMotor(4, cmd.substring(3).toInt());
  }

  // -------- PRESET MODES --------
  else if (cmd == "NECK") {
    setMotor(1, 80);
    setMotor(2, 80);
    setMotor(3, 40);
    setMotor(4, 40);
  }

  else if (cmd == "SHOULDER") {
    setMotor(1, 110);
    setMotor(2, 50);
    setMotor(3, 110);
    setMotor(4, 50);
  }

  else if (cmd == "BACK") {
    setMotor(1, 120);
    setMotor(2, 120);
    setMotor(3, 130);
    setMotor(4, 130);
  }

  else if (cmd == "KNEE") {
    setMotor(1, 40);
    setMotor(2, 40);
    setMotor(3, 150);
    setMotor(4, 150);
  }

  else if (cmd == "OFF") {
    stopAll();
  }

  fromBT ? SerialBT.println("OK") : Serial.println("OK");
}

// ------------------------------------------------

void loop() {
  if (Serial.available()) {
    processCommand(Serial.readStringUntil('\n'), false);
  }

  if (SerialBT.available()) {
    processCommand(SerialBT.readStringUntil('\n'), true);
  }
}
