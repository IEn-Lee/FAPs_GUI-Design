#include "Arduino_H7_Video.h"
#include "Arduino_GigaDisplayTouch.h"
#include "lvgl.h"
#include "pump.h"
#include "humidity.h"
#include "temperature.h"
#include "uvled.h"

Arduino_H7_Video          Display(800, 480, GigaDisplayShield);
Arduino_GigaDisplayTouch  TouchDetector;

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
}

static void event_handler(lv_event_t * e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t * btn = (lv_obj_t *)lv_event_get_target(e);

  if(code == LV_EVENT_CLICKED) {
    lv_obj_t * label = lv_obj_get_child(btn, 0);
    if(strcmp(lv_label_get_text(label), "START") == 0) {
      lv_label_set_text(label, "STOP");
      lv_obj_set_style_bg_color(btn, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
    } else {
      lv_label_set_text(label, "START");
      lv_obj_set_style_bg_color(btn, lv_color_hex(0x0000FF), LV_PART_MAIN | LV_STATE_DEFAULT);
    }
  }
}

void create_start_button(lv_obj_t * parent, const char * title, const lv_font_t * font, lv_coord_t x, lv_coord_t y) {
  lv_obj_t * btn = lv_btn_create(parent);
  lv_obj_set_size(btn, 400, 55);
  lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);
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
  lv_obj_set_size(chart, 700, 200);  // 調整大小
  lv_obj_align(chart, LV_ALIGN_BOTTOM_RIGHT, x, y);

  // 設定 Y 軸範圍：下限20，上限40
  lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 20, 40);

  // Y 軸分5條水平線（含邊界）
  lv_chart_set_div_line_count(chart, 5, 20);  // (水平線, 垂直線) = (5-1, 20-1)
  //lv_chart_set_y_tick_texts(chart, "40\n35\n30\n25\n20", 5, LV_CHART_AXIS_DRAW_LAST_TICK);

  // 顯示所有格線
  lv_obj_set_style_bg_opa(chart, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(chart, 2, 0);  // 外框

  lv_chart_set_type(chart, LV_CHART_TYPE_LINE);  // 折線圖
  lv_chart_set_point_count(chart, 20);  // 20 個資料點

  lv_chart_series_t * ser = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_BLUE), LV_CHART_AXIS_PRIMARY_Y);

  // 建立20個模擬的資料點 (25~40之間)
  for (int i = 0; i < 20; ++i) {
    int value = 21 + rand() % 20;  // 25~40
    lv_chart_set_value_by_id(chart, ser, i, value);
  }

  lv_chart_refresh(chart);  // 更新圖表
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

  // creat_label(parent, label_text, align, font, x, y)
  creat_label(tab3, "Elapsed Time:", LV_ALIGN_TOP_LEFT, &lv_font_montserrat_40, 0, 20);
  creat_label(tab3, "01:45:23", LV_ALIGN_TOP_RIGHT, &lv_font_montserrat_48, 0, 20);
  creat_label(tab3, "Cycles Completed:", LV_ALIGN_TOP_LEFT, &lv_font_montserrat_40, 0, 90);
  creat_label(tab3, "5", LV_ALIGN_TOP_RIGHT, &lv_font_montserrat_48, 0, 90);

  create_monitor_chart(tab3, 0, 0);  // 放置在 tab3 底部中央
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
}
