// ui_slider.cpp
#include "ui_slider.h"
#include "ui_status_variable.h"
#include "ui_status_label.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


typedef struct {
  lv_obj_t * slider;
  lv_obj_t * value_label;
  const char * unit;
  int step;
} slider_bundle_t;

static void update_slider_label(slider_bundle_t *bundle) {
  int value = lv_slider_get_value(bundle->slider);
  char buf[16];
  snprintf(buf, sizeof(buf), "%d%s", value, bundle->unit);
  lv_label_set_text(bundle->value_label, buf);
  lv_obj_align_to(bundle->value_label, bundle->slider, LV_ALIGN_OUT_BOTTOM_MID, 0, -75);
}

static void slider_event_cb(lv_event_t * e) {
  slider_bundle_t * bundle = (slider_bundle_t *)lv_event_get_user_data(e);
  update_slider_label(bundle);
}

static void minus_event_cb(lv_event_t * e) {
  slider_bundle_t * bundle = (slider_bundle_t *)lv_event_get_user_data(e);
  int value = lv_slider_get_value(bundle->slider);
  int new_value = value - bundle->step;
  if (new_value < lv_slider_get_min_value(bundle->slider)) {
    new_value = lv_slider_get_min_value(bundle->slider);
  }
  lv_slider_set_value(bundle->slider, new_value, LV_ANIM_OFF);
  update_slider_label(bundle);
}

static void plus_event_cb(lv_event_t * e) {
  slider_bundle_t * bundle = (slider_bundle_t *)lv_event_get_user_data(e);
  int value = lv_slider_get_value(bundle->slider);
  int new_value = value + bundle->step;
  if (new_value > lv_slider_get_max_value(bundle->slider)) {
    new_value = lv_slider_get_max_value(bundle->slider);
  }
  lv_slider_set_value(bundle->slider, new_value, LV_ANIM_OFF);
  update_slider_label(bundle);
}

void create_named_slider(lv_obj_t * parent, const char * title, int min, int max, const char * unit, lv_coord_t x, lv_coord_t y, int step) {
  lv_obj_t * title_label = lv_label_create(parent);
  lv_obj_set_height(title_label, 70);
  lv_label_set_text(title_label, title);
  lv_obj_align(title_label, LV_ALIGN_TOP_LEFT, x, y);

  lv_obj_t * slider = lv_slider_create(parent);
  lv_obj_set_size(slider, 699, 20);
  lv_obj_align_to(slider, title_label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 30);
  lv_slider_set_range(slider, min, max);
  lv_slider_set_value(slider, min, LV_ANIM_OFF);

  lv_obj_t * value_label = lv_label_create(parent);
  char buf[16];
  snprintf(buf, sizeof(buf), "%d%s", min, unit);
  lv_label_set_text(value_label, buf);
  lv_obj_set_style_text_font(value_label, &lv_font_montserrat_48, 0);
  lv_obj_align_to(value_label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, -75);

  int values[3] = { min, (min + max) / 2, max };
  for (int i = 0; i < 3; ++i) {
    lv_obj_t * hint = lv_label_create(parent);
    char hint_buf[8];
    snprintf(hint_buf, sizeof(hint_buf), "%d", values[i]);
    lv_label_set_text(hint, hint_buf);
    lv_obj_set_style_text_color(hint, lv_palette_main(LV_PALETTE_GREY), 0);
    lv_obj_set_style_text_font(hint, &lv_font_montserrat_22, 0);
    lv_coord_t offset_x = (i == 0) ? 0 : (i == 1 ? (lv_obj_get_width(slider) - 40) / 2 : lv_obj_get_width(slider) - 20);
    lv_obj_align_to(hint, slider, LV_ALIGN_OUT_BOTTOM_LEFT, offset_x, 8);
  }

  lv_obj_t * btn_minus = lv_btn_create(parent);
  lv_obj_set_size(btn_minus, 58, 58);
  lv_obj_align_to(btn_minus, slider, LV_ALIGN_OUT_TOP_RIGHT, -100, -15);
  lv_obj_t * label_minus = lv_label_create(btn_minus);
  lv_label_set_text(label_minus, "-");
  lv_obj_set_style_text_font(label_minus, &lv_font_montserrat_34, 0);
  lv_obj_center(label_minus);

  lv_obj_t * btn_plus = lv_btn_create(parent);
  lv_obj_set_size(btn_plus, 58, 58);
  lv_obj_align_to(btn_plus, slider, LV_ALIGN_OUT_TOP_RIGHT, 0, -15);
  lv_obj_t * label_plus = lv_label_create(btn_plus);
  lv_label_set_text(label_plus, "+");
  lv_obj_set_style_text_font(label_plus, &lv_font_montserrat_34, 0);
  lv_obj_center(label_plus);

  slider_bundle_t * bundle = (slider_bundle_t *)malloc(sizeof(slider_bundle_t));
  bundle->slider = slider;
  bundle->value_label = value_label;
  bundle->unit = unit;
  bundle->step = step;

  lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, bundle);
  lv_obj_add_event_cb(btn_minus, minus_event_cb, LV_EVENT_CLICKED, bundle);
  lv_obj_add_event_cb(btn_plus, plus_event_cb, LV_EVENT_CLICKED, bundle);

  if (strcmp(title, "Cycle Duration (sec)") == 0) {
    cycle_duration_slider = slider;
  } else if (strcmp(title, "Micturition cycles/day") == 0) {
    micturition_slider = slider;
  }
}
