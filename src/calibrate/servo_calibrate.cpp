// Калибровка сервопривода: вводите в Serial Monitor импульс в мкс (500..2500)
// или угол (0..180) и подбирайте положения «открыто» и «закрыто».
#include <Arduino.h>
#include <ESP32Servo.h>

const int SERVO_PIN = 13; // сигнальный провод MG996R

Servo calServo;

void setup() {
  Serial.begin(115200);

  calServo.setPeriodHertz(50);
  calServo.attach(SERVO_PIN, 500, 2500);
  calServo.write(90);

  Serial.println("Введите импульс в мкс (500..2500) или угол (0..180)");
}

void loop() {
  if (Serial.available()) {
    // Читаем строку целиком: parseInt() после Enter возвращал 0 и уводил серво в 0°
    String line = Serial.readStringUntil('\n');
    line.trim();
    if (line.length() == 0 || !isDigit(line[0])) {
      return;
    }
    int v = line.toInt();
    if (v >= 500 && v <= 2500) {
      calServo.writeMicroseconds(v);
      Serial.printf("us = %d\n", v);
    } else if (v >= 0 && v <= 180) {
      calServo.write(v);
      Serial.printf("angle = %d -> us = %d\n", v, calServo.readMicroseconds());
    }
  }
}
