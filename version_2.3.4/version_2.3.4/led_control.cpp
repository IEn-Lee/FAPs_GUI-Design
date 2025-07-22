// led_control.cpp
#include "led_control.h"
#include "ui_status_label.h"
#include "ui_tabview.h"
#include <Arduino.h>

// UV LED pin
const int ledIn1 = 13;
const int ledIn2 = 8;
const int ledENA = 11;

void led_init() {
  pinMode(ledIn1, OUTPUT);
  pinMode(ledIn2, OUTPUT);
  pinMode(ledENA, OUTPUT);
  stopLED();
}

void startLED() {
  digitalWrite(ledIn1, HIGH);
  digitalWrite(ledIn2, LOW);
  analogWrite(ledENA, 255);
  // Lightness = 12 * (255/255)
}

void stopLED() {
  digitalWrite(ledIn1, LOW);
  digitalWrite(ledIn2, LOW);
  analogWrite(ledENA, 0);
}

void updateLEDStatus(bool is_on) {
  if (led_status_label) {
    lv_label_set_text(led_status_label, is_on ? "ON" : "OFF");
    lv_obj_set_style_text_color(led_status_label,
      is_on ? lv_palette_main(LV_PALETTE_BLUE) : lv_palette_main(LV_PALETTE_RED), 0);
  }
}
