#include "Arduino_H7_Video.h"
#include "Arduino_GigaDisplayTouch.h"
#include "lvgl.h"
#include "pump.h"
#include "humidity.h"
#include "temperature.h"
#include "uvled.h"

Arduino_H7_Video          Display(800, 480, GigaDisplayShield);
Arduino_GigaDisplayTouch  TouchDetector;

unsigned long start_time = 0;
bool is_running = false;
lv_obj_t *elapsed_time_label = NULL;
lv_obj_t *cycles_completed_label = NULL;
lv_obj_t *cycle_duration_slider = NULL;


typedef struct {
  lv_obj_t * slider;
  lv_obj_t * value_label;
  const char * unit;
  int step;
} slider_bundle_t;

static void slider_event_cb(lv_event_t * e) {
  slider_bundle_t * bundle = (slider_bundle_t *)lv_event_get_user_data(e);
  int value = lv_slider_get_value(bundle->slider);
  char buf[16];
  snprintf(buf, sizeof(buf), "%d%s", value, bundle->unit);
  lv_label_set_text(bundle->value_label, buf);
  lv_obj_align_to(bundle->value_label, bundle->slider, LV_ALIGN_OUT_BOTTOM_MID, 0, -75);
}

void update_slider_label(slider_bundle_t *bundle) {
  int value = lv_slider_get_value(bundle->slider);
  char buf[16];
  snprintf(buf, sizeof(buf), "%d%s", value, bundle->unit);
  lv_label_set_text(bundle->value_label, buf);
  lv_obj_align_to(bundle->value_label, bundle->slider, LV_ALIGN_OUT_BOTTOM_MID, 0, -75);
}

static void minus_event_cb(lv_event_t * e) {
  slider_bundle_t * bundle = (slider_bundle_t *)lv_event_get_user_data(e);
  int value = lv_slider_get_value(bundle->slider);
  int new_value = value - bundle->step;
  if(new_value < lv_slider_get_min_value(bundle->slider)) new_value = lv_slider_get_min_value(bundle->slider);
  lv_slider_set_value(bundle->slider, new_value, LV_ANIM_OFF);
  update_slider_label(bundle);
}

static void plus_event_cb(lv_event_t * e) {
  slider_bundle_t * bundle = (slider_bundle_t *)lv_event_get_user_data(e);
  int value = lv_slider_get_value(bundle->slider);
  int new_value = value + bundle->step;
  if(new_value > lv_slider_get_max_value(bundle->slider)) new_value = lv_slider_get_max_value(bundle->slider);
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
    lv_coord_t offset_x = (i == 0) ? 0 : (i == 1 ? (lv_obj_get_width(slider)-40)/2 : lv_obj_get_width(slider)-20);
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
  }

}

// ---------- add START long press logic ----------
lv_obj_t * confirm_dialog = NULL;

static void event_handler(lv_event_t * e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t * btn = (lv_obj_t *)lv_event_get_target(e);
  lv_obj_t * label = lv_obj_get_child(btn, 0);
  const char * text = lv_label_get_text(label);

  if (code == LV_EVENT_CLICKED) {
    if (strcmp(text, "STOP") == 0) {
      show_confirm_dialog(btn); // press STOP → show confirm window
    } else {
      lv_label_set_text(label, "STOP");
      lv_obj_set_style_bg_color(btn, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
      start_time = millis(); // start timing
      is_running = true;     // active time recoding
      lv_label_set_text(elapsed_time_label, "00:00:00");      // reset time
      lv_label_set_text(cycles_completed_label, "0");         // reset cycles count
    }
  }
}

void show_confirm_dialog(lv_obj_t * btn) {
  if (confirm_dialog) return; // aviod repeat

  // create container
  confirm_dialog = lv_obj_create(lv_scr_act());
  lv_obj_set_size(confirm_dialog, 500, 200);
  lv_obj_center(confirm_dialog);
  lv_obj_set_style_radius(confirm_dialog, 12, 0);
  lv_obj_set_style_bg_color(confirm_dialog, lv_palette_lighten(LV_PALETTE_GREY, 3), 0);
  lv_obj_set_style_pad_all(confirm_dialog, 20, 0);

  // text
  lv_obj_t * msg = lv_label_create(confirm_dialog);
  lv_label_set_text(msg, "Stop the operation?");
  lv_obj_set_style_text_font(msg, &lv_font_montserrat_40, 0);
  lv_obj_align(msg, LV_ALIGN_TOP_MID, 0, 10);

  // YES button
  lv_obj_t * btn_yes = lv_btn_create(confirm_dialog);
  lv_obj_set_size(btn_yes, 100, 50);
  lv_obj_set_style_bg_color(btn_yes, lv_palette_main(LV_PALETTE_RED), 0);
  lv_obj_align(btn_yes, LV_ALIGN_BOTTOM_LEFT, 40, -20);
  lv_obj_t * label_yes = lv_label_create(btn_yes);
  lv_label_set_text(label_yes, "YES");
  lv_obj_set_style_text_font(label_yes, &lv_font_montserrat_32, 0);
  lv_obj_center(label_yes);

  // NO button
  lv_obj_t * btn_no = lv_btn_create(confirm_dialog);
  lv_obj_set_size(btn_no, 100, 50);
  lv_obj_set_style_bg_color(btn_no, lv_palette_main(LV_PALETTE_BLUE), 0);
  lv_obj_align(btn_no, LV_ALIGN_BOTTOM_RIGHT, -40, -20);
  lv_obj_t * label_no = lv_label_create(btn_no);
  lv_label_set_text(label_no, "NO");
  lv_obj_set_style_text_font(label_no, &lv_font_montserrat_32, 0);
  lv_obj_center(label_no);

  // send START/STOP as user_data
  lv_obj_add_event_cb(btn_yes, [](lv_event_t * e) {
    lv_obj_t * original_btn = (lv_obj_t *)lv_event_get_user_data(e);
    lv_obj_t * label = lv_obj_get_child(original_btn, 0);
    lv_label_set_text(label, "START");
    lv_obj_set_style_bg_color(original_btn, lv_color_hex(0x0000FF), LV_PART_MAIN | LV_STATE_DEFAULT);
    is_running = false; // stop timing
    lv_obj_del(confirm_dialog);
    confirm_dialog = NULL;
  }, LV_EVENT_CLICKED, btn);

  lv_obj_add_event_cb(btn_no, [](lv_event_t * e) {
    lv_obj_del(confirm_dialog);
    confirm_dialog = NULL;
  }, LV_EVENT_CLICKED, NULL);
}

// ------------------------------------------------

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

void create_status_cell(lv_obj_t * parent, const void * icon_src, const char * label_text, const char * value_text, lv_color_t value_color, lv_coord_t x, lv_coord_t y) {
  lv_obj_t * cont = lv_obj_create(parent);
  lv_obj_set_size(cont, 350, 160);
  lv_obj_align(cont, LV_ALIGN_TOP_LEFT, x, y);
  lv_obj_set_style_radius(cont, 12, 0);
  lv_obj_set_style_bg_color(cont, lv_palette_lighten(LV_PALETTE_GREY, 3), 0);

  lv_obj_t * icon = lv_img_create(cont);
  lv_img_set_src(icon, icon_src);
  lv_obj_align(icon, LV_ALIGN_TOP_LEFT, 0, 0);

  lv_obj_t * label = lv_label_create(cont);
  lv_label_set_text(label, label_text);
  lv_obj_set_style_text_font(label, &lv_font_montserrat_40, 0);
  lv_obj_align(label, LV_ALIGN_TOP_RIGHT, 0, 0);

  lv_obj_t * value = lv_label_create(cont);
  lv_label_set_text(value, value_text);
  lv_obj_set_style_text_color(value, value_color, 0);
  lv_obj_set_style_text_font(value, &lv_font_montserrat_48, 0);
  lv_obj_align(value, LV_ALIGN_BOTTOM_RIGHT, 0, -10);
}

void creat_label(lv_obj_t * parent, const char * label_text, lv_align_t align, const lv_font_t * font, lv_coord_t x, lv_coord_t y){
  lv_obj_t * label = lv_label_create(parent);
  lv_label_set_text(label, label_text);
  lv_obj_set_style_text_font(label, font, 0);
  lv_obj_align(label, align, x, y);
}

void create_monitor_chart(lv_obj_t * parent, lv_coord_t x, lv_coord_t y) {
  lv_obj_t * chart = lv_chart_create(parent);
  lv_obj_set_size(chart, 700, 200);
  lv_obj_align(chart, LV_ALIGN_BOTTOM_RIGHT, x, y);
  lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 20, 40);
  lv_chart_set_div_line_count(chart, 5, 20);
  lv_obj_set_style_bg_opa(chart, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(chart, 2, 0);
  lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
  lv_chart_set_point_count(chart, 20);
  lv_chart_series_t * ser = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_BLUE), LV_CHART_AXIS_PRIMARY_Y);
  for (int i = 0; i < 20; ++i) {
    int value = 21 + rand() % 20;
    lv_chart_set_value_by_id(chart, ser, i, value);
  }
  lv_chart_refresh(chart);
}

void lv_example_tabview_1(void) {
  lv_obj_t * tabview = lv_tabview_create(lv_screen_active());
  lv_obj_t * tab_btns = lv_tabview_get_tab_btns(tabview);
  lv_obj_set_style_text_font(tab_btns, &lv_font_montserrat_22, 0);

  lv_obj_t * tab1 = lv_tabview_add_tab(tabview, "SETTING");
  lv_obj_t * tab2 = lv_tabview_add_tab(tabview, "DEVICE STATUS");
  lv_obj_t * tab3 = lv_tabview_add_tab(tabview, "MONITOR");

  create_named_slider(tab1, "Micturition cycles/day", 1, 10, "", 20, 10, 1);
  create_named_slider(tab1, "Cycle Duration (sec)", 1, 120, "", 20, 160, 1);
  lv_obj_set_style_text_font(tab1, &lv_font_montserrat_34, 0);
  create_start_button(tab1, "START", &lv_font_montserrat_40, 0, 0);

  create_status_cell(tab2, &uvled, "UV LED", "ON", lv_palette_main(LV_PALETTE_BLUE), 20, 20);
  create_status_cell(tab2, &pump, "Pump", "OFF", lv_palette_main(LV_PALETTE_RED), 400, 20);
  create_status_cell(tab2, &temperature, "Temp", "37 °C", lv_palette_main(LV_PALETTE_BLUE), 20, 200);
  create_status_cell(tab2, &humidity, "Humidity", "60%", lv_palette_main(LV_PALETTE_BLUE), 400, 200);

  creat_label(tab3, "Elapsed Time:", LV_ALIGN_TOP_LEFT, &lv_font_montserrat_40, 0, 20);
  elapsed_time_label = lv_label_create(tab3);
  lv_label_set_text(elapsed_time_label, "00:00:00");
  lv_obj_set_style_text_font(elapsed_time_label, &lv_font_montserrat_48, 0);
  lv_obj_align(elapsed_time_label, LV_ALIGN_TOP_RIGHT, 0, 20);
  creat_label(tab3, "Cycles Completed:", LV_ALIGN_TOP_LEFT, &lv_font_montserrat_40, 0, 90);
  cycles_completed_label = lv_label_create(tab3);
  lv_label_set_text(cycles_completed_label, "0");
  lv_obj_set_style_text_font(cycles_completed_label, &lv_font_montserrat_48, 0);
  lv_obj_align(cycles_completed_label, LV_ALIGN_TOP_RIGHT, 0, 90);

  create_monitor_chart(tab3, 0, 0);
  creat_label(tab3, "40", LV_ALIGN_OUT_BOTTOM_LEFT, &lv_font_montserrat_20, 35, 185);
  creat_label(tab3, "35", LV_ALIGN_OUT_BOTTOM_LEFT, &lv_font_montserrat_20, 35, 230);
  creat_label(tab3, "30", LV_ALIGN_OUT_BOTTOM_LEFT, &lv_font_montserrat_20, 35, 270);
  creat_label(tab3, "25", LV_ALIGN_OUT_BOTTOM_LEFT, &lv_font_montserrat_20, 35, 315);
  creat_label(tab3, "20", LV_ALIGN_OUT_BOTTOM_LEFT, &lv_font_montserrat_20, 35, 360);
  creat_label(tab3, "Temperature Chart", LV_ALIGN_OUT_BOTTOM_LEFT, &lv_font_montserrat_20, 70, 158);
}

void setup() {
  Display.begin();
  TouchDetector.begin();
  lv_example_tabview_1();
}

void loop() {
  lv_timer_handler();
  delay(5);

  static unsigned long last_update = 0;
  if (is_running && millis() - last_update >= 1000) {
    last_update = millis();
    unsigned long elapsed = (millis() - start_time) / 1000;
    int hours = elapsed / 3600;
    int minutes = (elapsed % 3600) / 60;
    int seconds = elapsed % 60;

    if (elapsed_time_label) {
      char buf[16];
      snprintf(buf, sizeof(buf), "%02d:%02d:%02d", hours, minutes, seconds);
      lv_label_set_text(elapsed_time_label, buf);
    }

    if (cycle_duration_slider && cycles_completed_label) {
      int cycle_duration_sec = lv_slider_get_value(cycle_duration_slider);
      if (cycle_duration_sec > 0) {
        int cycles_completed = elapsed / cycle_duration_sec;
        char buf[8];
        snprintf(buf, sizeof(buf), "%d", cycles_completed);
        lv_label_set_text(cycles_completed_label, buf);
      }
    }
  }
}

