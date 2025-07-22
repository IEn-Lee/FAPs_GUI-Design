#pragma once

#include "lvgl.h"

// extern slider control object
extern lv_obj_t* cycle_duration_slider;
extern lv_obj_t* micturition_slider;

// name slider function
void create_named_slider(
  lv_obj_t * parent,
  const char * title,
  int min,
  int max,
  const char * unit,
  lv_coord_t x,
  lv_coord_t y,
  int step
);

