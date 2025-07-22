#pragma once

#include "lvgl.h"

extern lv_obj_t* temperature_value_label;
extern lv_obj_t* chart_temp_label;
extern lv_obj_t* temperature_chart;
extern lv_chart_series_t* temp_series;
extern lv_obj_t* elapsed_time_label;
extern lv_obj_t* cycles_completed_label;

void update_temperature_ui(float temp);
void update_elapsed_time(unsigned long ms);
void update_cycles_count(int count);

