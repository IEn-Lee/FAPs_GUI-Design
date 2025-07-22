// motor_control.cpp
#include "motor_control.h"
#include "ui_status_label.h"
#include "led_control.h"
#include <Arduino.h>

// motor shiedl board pin
const int motorIn1 = 12;
const int motorIn2 = 9;
const int motorENA = 3;

void motor_init() {
  pinMode(motorIn1, OUTPUT);
  pinMode(motorIn2, OUTPUT);
  pinMode(motorENA, OUTPUT);
  stopMotor();
}

void startMotor(bool forward) {
  digitalWrite(motorIn1, forward ? HIGH : LOW);
  digitalWrite(motorIn2, forward ? LOW : HIGH);
  analogWrite(motorENA, 200); 
  // output voltage = 12 * (200/255)
}

void stopMotor() {
  digitalWrite(motorIn1, LOW);
  digitalWrite(motorIn2, LOW);
  analogWrite(motorENA, 0);
}

void updatePumpStatus(bool is_on) {
  if (pump_status_label) {
    lv_label_set_text(pump_status_label, is_on ? "ON" : "OFF");
    lv_obj_set_style_text_color(pump_status_label,
      is_on ? lv_palette_main(LV_PALETTE_BLUE) : lv_palette_main(LV_PALETTE_RED), 0);
  }
}
