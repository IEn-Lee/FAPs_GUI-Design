// sensor_bmp280.cpp
#include "sensor_bmp280.h"
#include "ui_status_label.h"
#include "ui_control_button.h"
#include <Adafruit_BMP280.h>
#include <Wire.h>
#include <lvgl.h>

// --- BMP280 instance and status ---
static Adafruit_BMP280 bmp;
static bool bmp_error = false;
static float last_temp = NAN;

// --- Temperature stuck detection ---
const float stuck_temp = 25.23;
const int stuck_threshold = 8;
static int repeated_count = 0;
static int nan_count = 0;

// --- I2C bus recovery ---
void i2c_bus_recovery() {
  const int SDA_PIN = 20;
  const int SCL_PIN = 21;

  pinMode(SDA_PIN, INPUT_PULLUP);
  pinMode(SCL_PIN, INPUT_PULLUP);
  delay(5);
  pinMode(SCL_PIN, OUTPUT);

  for (int i = 0; i < 9; i++) {
    digitalWrite(SCL_PIN, LOW);
    delayMicroseconds(5);
    digitalWrite(SCL_PIN, HIGH);
    delayMicroseconds(5);
  }

  pinMode(SDA_PIN, OUTPUT);
  digitalWrite(SDA_PIN, HIGH);
  delayMicroseconds(5);
  pinMode(SDA_PIN, INPUT_PULLUP);
  pinMode(SCL_PIN, INPUT_PULLUP);

  Wire.end();
  delay(100);
  Wire.begin();
  Serial.println("I2C bus recovered");
}

// --- Seneor Initialization ---
void bmp280_init() {
  Wire.begin();
  if (!bmp.begin(0x76)) {
    Serial.println("BMP280 not found");
    bmp_error = true;
  } else {
    bmp_error = false;
  }
}

// --- Periodic update (called in loop) ---
void bmp280_update() {
  static unsigned long last_update = 0;
  static unsigned long last_retry = 0;

  if (millis() - last_update < 1000) return;
  last_update = millis();

  float temp = NAN;
  if (!bmp_error) {
    temp = bmp.readTemperature();
  }

  bool valid = !isnan(temp) && temp > -40 && temp < 85;

  if (valid) {
    nan_count = 0;

    if (temp == stuck_temp) {
      repeated_count++;
    } else {
      repeated_count = 0;
    }

    last_temp = temp;
  } else {
    nan_count++;
    repeated_count = 0;
    last_temp = NAN;
  }

  // Automatic recovery if sensor fails or temperature gets stuck
  if (nan_count >= 3 || repeated_count >= stuck_threshold) {
    Serial.println("Reset I2C & BMP280");
    i2c_bus_recovery();
    delay(500);
    if (bmp.begin(0x76)) {
      bmp_error = false;
      Serial.println("BMP280 Reset successfully");
    } else {
      bmp_error = true;
      Serial.println("BMP280 Reset failed");
    }
    nan_count = 0;
    repeated_count = 0;
  }

  // Timed retry every 10 seconds (fail-safe)
  if (bmp_error && millis() - last_retry >= 10000) {
    last_retry = millis();
    Serial.println("Try timed BMP280 initialization...");
    i2c_bus_recovery();
    delay(500);
    if (bmp.begin(0x76)) {
      bmp_error = false;
      Serial.println("BMP280 initialization successfully");
    } else {
      Serial.println("BMP280 still cannot be initialized");
    }
  }
}

// --- Public accessors ---
float bmp280_get_temperature() {
  return last_temp;
}

bool bmp280_has_error() {
  return bmp_error;
}
