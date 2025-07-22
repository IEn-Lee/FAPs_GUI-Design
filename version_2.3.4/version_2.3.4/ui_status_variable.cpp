// ui_status_variable.cpp
#include "ui_status_variable.h"

lv_obj_t* micturition_slider = NULL;
lv_obj_t* cycle_duration_slider = NULL;
lv_obj_t* temperature_chart = NULL;
lv_chart_series_t* temp_series = NULL;

int temp_index = 0;

bool is_running = false;
bool motor_is_running = false;
unsigned long last_cycle_time = 0;
unsigned long start_time = 0;
int completed_cycles = 0;
unsigned long current_seconds = 0;
