#pragma once

#include "lvgl.h"

// initalization in setup
void led_init();

// control LED ON/OFF
void startLED();
void stopLED();

// UI status update
void updateLEDStatus(bool is_on);

// use for status label updating
extern lv_obj_t* led_status_label;
