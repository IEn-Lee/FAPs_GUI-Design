#include "ui_status_variable.h"
#include "ui_status_label.h"
#include "ui_control_button.h"
#include "led_control.h"
#include "motor_control.h"
#include "sensor_bmp280.h"

// public access
static lv_obj_t * confirm_dialog = NULL;

// clear chart
static void reset_chart() {
  if (temperature_chart && temp_series) {
    for (int i = 0; i < 20; ++i) {
      lv_chart_set_value_by_id(temperature_chart, temp_series, i, LV_CHART_POINT_NONE);
    }
    lv_chart_refresh(temperature_chart);
  }
}

// Anti-accidental touch window
static void show_confirm_dialog(lv_obj_t * btn) {
  if (confirm_dialog) return;

  confirm_dialog = lv_obj_create(lv_scr_act());
  lv_obj_set_size(confirm_dialog, 500, 200);
  lv_obj_center(confirm_dialog);
  lv_obj_set_style_radius(confirm_dialog, 12, 0);
  lv_obj_set_style_bg_color(confirm_dialog, lv_palette_lighten(LV_PALETTE_GREY, 3), 0);
  lv_obj_set_style_pad_all(confirm_dialog, 20, 0);

  lv_obj_t * msg = lv_label_create(confirm_dialog);
  lv_label_set_text(msg, "Stop the operation?");
  lv_obj_set_style_text_font(msg, &lv_font_montserrat_40, 0);
  lv_obj_align(msg, LV_ALIGN_TOP_MID, 0, 10);

  lv_obj_t * btn_yes = lv_btn_create(confirm_dialog);
  lv_obj_set_size(btn_yes, 100, 50);
  lv_obj_set_style_bg_color(btn_yes, lv_palette_main(LV_PALETTE_RED), 0);
  lv_obj_align(btn_yes, LV_ALIGN_BOTTOM_LEFT, 40, -20);
  lv_obj_t * label_yes = lv_label_create(btn_yes);
  lv_label_set_text(label_yes, "YES");
  lv_obj_set_style_text_font(label_yes, &lv_font_montserrat_32, 0);
  lv_obj_center(label_yes);

  lv_obj_t * btn_no = lv_btn_create(confirm_dialog);
  lv_obj_set_size(btn_no, 100, 50);
  lv_obj_set_style_bg_color(btn_no, lv_palette_main(LV_PALETTE_BLUE), 0);
  lv_obj_align(btn_no, LV_ALIGN_BOTTOM_RIGHT, -40, -20);
  lv_obj_t * label_no = lv_label_create(btn_no);
  lv_label_set_text(label_no, "NO");
  lv_obj_set_style_text_font(label_no, &lv_font_montserrat_32, 0);
  lv_obj_center(label_no);

  lv_obj_add_event_cb(btn_yes, [](lv_event_t * e) {
    lv_obj_t * original_btn = (lv_obj_t *)lv_event_get_user_data(e);
    lv_obj_t * label = lv_obj_get_child(original_btn, 0);
    lv_label_set_text(label, "START");
    lv_obj_set_style_bg_color(original_btn, lv_color_hex(0x0000FF), LV_PART_MAIN | LV_STATE_DEFAULT);
    is_running = false;
    stopLED();
    updateLEDStatus(false);
    lv_obj_del(confirm_dialog);
    confirm_dialog = NULL;
  }, LV_EVENT_CLICKED, btn);

  lv_obj_add_event_cb(btn_no, [](lv_event_t * e) {
    lv_obj_del(confirm_dialog);
    confirm_dialog = NULL;
  }, LV_EVENT_CLICKED, NULL);
}

// handle button event
static void event_handler(lv_event_t * e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t * btn = (lv_obj_t *)lv_event_get_target(e);
  lv_obj_t * label = lv_obj_get_child(btn, 0);
  const char * text = lv_label_get_text(label);

  if (code == LV_EVENT_CLICKED) {
    if (strcmp(text, "STOP") == 0) {
      show_confirm_dialog(btn);
    } else {
      lv_label_set_text(label, "STOP");
      lv_obj_set_style_bg_color(btn, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
      start_time = millis();
      is_running = true;
      startLED();
      updateLEDStatus(true);

      int interval_between_cycles = 86400 / lv_slider_get_value(micturition_slider);
      last_cycle_time = -interval_between_cycles;

      motor_is_running = false;
      completed_cycles = 0;
      if (elapsed_time_label) lv_label_set_text(elapsed_time_label, "00:00:00");
      if (cycles_completed_label) lv_label_set_text(cycles_completed_label, "0");

      reset_chart();
    }
  }
}

// START/STOP button function
void create_start_button(lv_obj_t * parent, const char * title, const lv_font_t * font, lv_coord_t x, lv_coord_t y) {
  lv_obj_t * btn = lv_btn_create(parent);
  lv_obj_set_size(btn, 400, 55);
  lv_obj_add_event_cb(btn, event_handler, LV_EVENT_ALL, NULL);
  lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, x, y);
  lv_obj_set_style_bg_color(btn, lv_color_hex(0x0000FF), LV_PART_MAIN | LV_STATE_DEFAULT);

  lv_obj_t * label = lv_label_create(btn);
  lv_label_set_text(label, title);
  lv_obj_set_style_text_font(label, font, 0);
  lv_obj_center(label);
}
