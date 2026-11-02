#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <uri/UriBraces.h>
#include <ESP32Servo.h>
#include "secrets.h" // WIFI_SSID, WIFI_PASSWORD — см. include/secrets.h.example

const int LED_PIN = 2; // встроенный LED
const int SERVO_PIN = 13; // сигнальный провод MG996R

const int SERVO_OPEN_ANGLE = 40;
const int SERVO_CLOSED_ANGLE = 125;

const long MIN_OPEN_MS = 300;    // нижний предел: серва MG996R проходит 85° примерно за 0,3 с
const long MAX_OPEN_MS = 300000; // верхний предел для /open/{ms}

WebServer server(80);
Servo doorServo;

// Таймер автозакрытия: 0 — таймер не запущен
unsigned long autoCloseStartMs = 0;
unsigned long autoCloseDurationMs = 0;

void openDoor() {
  doorServo.write(SERVO_OPEN_ANGLE);
  digitalWrite(LED_PIN, HIGH);
  Serial.printf("open: servo -> %d\n", SERVO_OPEN_ANGLE);
}

void closeDoor() {
  doorServo.write(SERVO_CLOSED_ANGLE);
  digitalWrite(LED_PIN, LOW);
  Serial.printf("close: servo -> %d\n", SERVO_CLOSED_ANGLE);
}

void cancelAutoClose() {
  autoCloseDurationMs = 0;
}

void handleOpen() {
  cancelAutoClose();
  openDoor();
  server.sendHeader("Connection", "close");
  server.send(200, "text/plain", "opened");
}

void handleOpenFor() {
  String arg = server.pathArg(0);
  bool isNumber = arg.length() > 0;
  for (char c : arg) {
    if (!isDigit(c)) {
      isNumber = false;
      break;
    }
  }
  // Не больше 6 цифр, чтобы toInt() не переполнился
  long ms = (isNumber && arg.length() <= 6) ? arg.toInt() : 0;

  server.sendHeader("Connection", "close");
  if (ms < MIN_OPEN_MS || ms > MAX_OPEN_MS) {
    server.send(400, "text/plain", "ms must be " + String(MIN_OPEN_MS) + ".." + String(MAX_OPEN_MS));
    return;
  }

  openDoor();
  autoCloseStartMs = millis();
  autoCloseDurationMs = ms;
  Serial.printf("auto close in %ld ms\n", ms);
  server.send(200, "text/plain", "opened for " + String(ms) + " ms");
}

void handleClose() {
  cancelAutoClose();
  closeDoor();
  server.sendHeader("Connection", "close");
  server.send(200, "text/plain", "closed");
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  doorServo.setPeriodHertz(50);
  doorServo.attach(SERVO_PIN, 500, 2500); // тот же диапазон, что в калибровке
  doorServo.write(SERVO_CLOSED_ANGLE);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  WiFi.setSleep(false);

  Serial.println();
  Serial.print("Connected, IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("MAC address: ");
  Serial.println(WiFi.macAddress());

  server.on("/open", HTTP_POST, handleOpen);
  server.on(UriBraces("/open/{}"), HTTP_POST, handleOpenFor);
  server.on("/close", HTTP_POST, handleClose);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();

  // Разность millis() корректна и при переполнении счётчика
  if (autoCloseDurationMs > 0 && millis() - autoCloseStartMs >= autoCloseDurationMs) {
    cancelAutoClose();
    closeDoor();
  }
}
