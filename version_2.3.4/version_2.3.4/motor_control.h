#pragma once

#include "lvgl.h"

// initalization in setup
void motor_init();

// start & stop
void startMotor(bool forward = true);
void stopMotor();

// Update UI pump ON/OFF label
void updatePumpStatus(bool is_on);

// use for updating labe of UI
extern lv_obj_t* pump_status_label;
