#pragma once

#include "lvgl.h"

// global UI state variables
extern lv_obj_t* micturition_slider;
extern lv_obj_t* cycle_duration_slider;
extern lv_obj_t* temperature_chart;
extern lv_chart_series_t* temp_series;

extern int temp_index;

extern bool is_running;
extern bool motor_is_running;
extern unsigned long last_cycle_time;
extern unsigned long start_time;
extern int completed_cycles;
extern unsigned long current_seconds;
