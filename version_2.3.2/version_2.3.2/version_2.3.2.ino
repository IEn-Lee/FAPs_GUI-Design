#define LV_CONF_INCLUDE_SIMPLE

#include "Arduino_H7_Video.h"
#include "Arduino_GigaDisplayTouch.h"
#include "lvgl.h"
#include "pump.h"
#include "humidity.h"
#include "temperature.h"
#include "uvled.h"
#include "Adafruit_BMP280.h"
#include "Wire.h"
Adafruit_BMP280 bmp;
lv_obj_t* temperature_value_label = NULL;
lv_obj_t* temperature_chart = NULL;
lv_chart_series_t* temp_series = NULL;
int temp_index = 0;
lv_obj_t* chart_temp_label = NULL;

Arduino_H7_Video          Display(800, 480, GigaDisplayShield);
Arduino_GigaDisplayTouch  TouchDetector;

unsigned long start_time = 0;
bool is_running = false;
lv_obj_t *elapsed_time_label = NULL;
lv_obj_t *cycles_completed_label = NULL;
lv_obj_t *cycle_duration_slider = NULL;
lv_obj_t *micturition_slider = NULL;

// --- motor control ---
unsigned long last_cycle_time = 0;
bool motor_is_running = false;
int completed_cycles = 0; // 新增完成週期計數
unsigned long current_seconds = 0;

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

  if (strcmp(title, "Micturition cycles/day") == 0) {
    micturition_slider = slider;
  }
}

// ---------- add START long press logic ----------
void reset_chart() {
  if (temperature_chart && temp_series) {
    for (int i = 0; i < 20; ++i) {
      lv_chart_set_value_by_id(temperature_chart, temp_series, i, LV_CHART_POINT_NONE);
    }
    lv_chart_refresh(temperature_chart);
    temp_index = 0;  // 重置索引，從頭開始填資料
  }
}

lv_obj_t * confirm_dialog = NULL;

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
      int cycles_per_day = lv_slider_get_value(micturition_slider);
      int interval_between_cycles = 86400 / cycles_per_day;
      last_cycle_time = -interval_between_cycles;  // 讓 loop() 判斷立即啟動馬達

      motor_is_running = false;

      completed_cycles = 0;                      // 週期計數歸零
      lv_label_set_text(elapsed_time_label, "00:00:00");  // 時間顯示重設
      lv_label_set_text(cycles_completed_label, "0");     // 週期顯示重設

      reset_chart();   // 清空溫度圖表
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

// --- motor control ---
const int motorIn1 = 12;
const int motorIn2 = 9;
const int motorENA = 3;

lv_obj_t* pump_status_label = NULL;

void startMotor(bool forward = true) {
  digitalWrite(motorIn1, forward ? HIGH : LOW);
  digitalWrite(motorIn2, forward ? LOW : HIGH);
  analogWrite(motorENA, 200);
}

void stopMotor() {
  digitalWrite(motorIn1, LOW);
  digitalWrite(motorIn2, LOW);
  analogWrite(motorENA, 0);
}

void updatePumpStatus(bool is_on) {
  if (pump_status_label) {
    lv_label_set_text(pump_status_label, is_on ? "ON" : "OFF");
    lv_obj_set_style_text_color(pump_status_label, 
      is_on ? lv_palette_main(LV_PALETTE_BLUE) : lv_palette_main(LV_PALETTE_RED), 0);
  }
}

// --- motor control ---

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

  if (strcmp(label_text, "Temp") == 0) {
      temperature_value_label = value;
  } else if (strcmp(label_text, "Pump") == 0) {
      pump_status_label = value;
  } else if (strcmp(label_text, "Cycles Completed:") == 0) {
      cycles_completed_label = value;
  }
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
  // number of temperature points
  lv_chart_set_point_count(chart, 20);
  lv_chart_series_t * ser = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_BLUE), LV_CHART_AXIS_PRIMARY_Y);
  for (int i = 0; i < 20; ++i) {
    lv_chart_set_value_by_id(chart, ser, i, LV_CHART_POINT_NONE);
  }
  lv_chart_refresh(chart);

  temperature_chart = chart;
  temp_series = ser;
  temp_index = 0;

  chart_temp_label = lv_label_create(parent);
  lv_label_set_text(chart_temp_label, "Now: -- °C");
  lv_obj_set_style_text_font(chart_temp_label, &lv_font_montserrat_20, 0);
  lv_obj_align_to(chart_temp_label, chart, LV_ALIGN_TOP_RIGHT, -45, 0); 

  lv_obj_set_style_bg_color(chart_temp_label, lv_palette_lighten(LV_PALETTE_GREY, 3), 0);  // backgraound color
  lv_obj_set_style_bg_opa(chart_temp_label, LV_OPA_COVER, 0);  // Enable background coloring
  lv_obj_set_style_pad_all(chart_temp_label, 4, 0);  // Inner margins keep text from being pasted
  lv_obj_set_style_radius(chart_temp_label, 6, 0);   // Round corners


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
  create_status_cell(tab2, &temperature, "Temp", "NaN °C", lv_palette_main(LV_PALETTE_BLUE), 20, 200);
  create_status_cell(tab2, &humidity, "Humidity", "NaN %", lv_palette_main(LV_PALETTE_BLUE), 400, 200);

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

void i2c_bus_recovery() {
  const int SDA_PIN = 20;
  const int SCL_PIN = 21;

  pinMode(SDA_PIN, INPUT_PULLUP);
  pinMode(SCL_PIN, INPUT_PULLUP);
  delay(5);
  pinMode(SCL_PIN, OUTPUT);

  for (int i = 0; i < 9; i++) {
    digitalWrite(SCL_PIN, LOW);
    delayMicroseconds(5);
    digitalWrite(SCL_PIN, HIGH);
    delayMicroseconds(5);
  }

  pinMode(SDA_PIN, OUTPUT);
  digitalWrite(SDA_PIN, HIGH);
  delayMicroseconds(5);

  pinMode(SDA_PIN, INPUT_PULLUP);
  pinMode(SCL_PIN, INPUT_PULLUP);

  // Restart I2C
  Wire.end();
  delay(100);
  Wire.begin();
  Serial.println("I2C bus recovery and Wire restarted.");
}


bool bmp_error = false;
void setup() {
  Serial.begin(115200);
  Display.begin();
  TouchDetector.begin();
  Wire.begin();  // Restart Wire
  lv_example_tabview_1();
  pinMode(motorIn1, OUTPUT);
  pinMode(motorIn2, OUTPUT);
  pinMode(motorENA, OUTPUT);
  stopMotor();
  completed_cycles = 0;
  if (cycles_completed_label) {
    lv_label_set_text(cycles_completed_label, "0");
  }

  if (!bmp.begin(0x76)) {
    Serial.println("BMP280 not found");
    bmp_error = true;
  } else {
    bmp_error = false;
  }

}

static int error_count = 0;
const int error_threshold = 3;
static int repeated_count = 0;
static float last_temp = NAN;
const float stuck_temp = 25.23; // Stop judging temperature
const int stuck_threshold = 8;  // Continuous times threshold
void loop() {
  lv_timer_handler();
  delay(5);

  static unsigned long last_update = 0;
  static int nan_count = 0;        // Continuous NaN count
  static int repeated_count = 0;   // Continuous counting of stuck temperature

  if (millis() - last_update >= 1000) {
    last_update = millis();

    // If an error occurs with BMP280, try resetting
    if (bmp_error) {
      Serial.println("Attempting BMP280 recovery...");
      i2c_bus_recovery();
      delay(100);
      if (bmp.begin(0x76)) {
        bmp_error = false;
        Serial.println("BMP280 recovered after I2C reset.");
      } else {
        Serial.println("BMP280 still not responding.");
      }
    }

    float temp = NAN;
    if (!bmp_error) {
      temp = bmp.readTemperature();
    }

    bool temp_valid = !isnan(temp) && temp > -40 && temp < 85;

    if (temp_valid) {
      nan_count = 0;

      // Determine if the temperature is stuck
      if (temp == stuck_temp) {
        repeated_count++;
        Serial.print("溫度與卡住值相同 ");
        Serial.print(temp);
        Serial.print("，重複次數 = ");
        Serial.println(repeated_count);
      } else {
        repeated_count = 0;
      }

      // Update Tab2 Temperature Label
      if (temperature_value_label) {
        char buf[16];
        snprintf(buf, sizeof(buf), "%.2f°C", temp);
        lv_label_set_text(temperature_value_label, buf);
      }

      // Update Tab3 Chart Label
      if (chart_temp_label) {
        char chart_buf[32];
        snprintf(chart_buf, sizeof(chart_buf), "Now: %.2f °C", temp);
        lv_label_set_text(chart_temp_label, chart_buf);
      }

      // Update the temperature graph (if running)
      if (is_running && temperature_chart && temp_series) {
        lv_chart_set_next_value(temperature_chart, temp_series, (int)temp);
        lv_chart_refresh(temperature_chart);
      }

    } else {
      nan_count++;
      Serial.print("Temperature read NaN, count = ");
      Serial.println(nan_count);

      // Update UI when NaN
      if (temperature_value_label) {
        lv_label_set_text(temperature_value_label, "NaN °C");
      }
      if (chart_temp_label) {
        lv_label_set_text(chart_temp_label, "Now: NaN °C");
      }
    }

    // Override the error threshold, reset the I2C and BMP280
    if (nan_count >= 3 || repeated_count >= stuck_threshold) {
      Serial.println("嘗試重置 I2C 和 BMP280...");
      i2c_bus_recovery();
      delay(500);
      if (bmp.begin(0x76)) {
        bmp_error = false;
        Serial.println("BMP280 重置成功");
      } else {
        bmp_error = true;
        Serial.println("BMP280 重置失敗");
      }
      nan_count = 0;
      repeated_count = 0;
    }

    // Calculate and update the elapsed time and the number of completed cycles
    unsigned long elapsed = (millis() - start_time) / 1000;
    int hours = elapsed / 3600;
    int minutes = (elapsed % 3600) / 60;
    int seconds = elapsed % 60;

    if (is_running) {
      if (elapsed_time_label) {
        char buf[16];
        snprintf(buf, sizeof(buf), "%02d:%02d:%02d", hours, minutes, seconds);
        lv_label_set_text(elapsed_time_label, buf);
      }

      if (cycles_completed_label) {
        char buf[8];
        snprintf(buf, sizeof(buf), "%d", completed_cycles);
        lv_label_set_text(cycles_completed_label, buf);
      }
    }

    // Serial output temperature or NaN
    if (temp_valid) {
      Serial.println(temp);
    } else {
      Serial.println("Temperature: NaN");
    }
  }

  // try to reconnect BMP280 for each 10 second (avoid never read the data)
  static unsigned long last_retry_time = 0;
  if (bmp_error && millis() - last_retry_time >= 10000) {
    last_retry_time = millis();
    Serial.println("定時重試 BMP280 初始化...");
    i2c_bus_recovery();
    delay(500);
    if (bmp.begin(0x76)) {
      bmp_error = false;
      Serial.println("BMP280重新偵測成功");
    } else {
      Serial.println("BMP280仍無法初始化");
    }
  }

  // --- motor control ---
  if (last_cycle_time == 0) {
    last_cycle_time = current_seconds;
  }

  if (is_running) {
    int cycles_per_day = lv_slider_get_value(micturition_slider);
    int cycle_duration = lv_slider_get_value(cycle_duration_slider);

    int interval_between_cycles = 86400 / cycles_per_day; // 週期間隔（秒）
    current_seconds = (millis() - start_time) / 1000;

    if (!motor_is_running && (current_seconds - last_cycle_time >= interval_between_cycles)) {
      // 開始新一個cycle
      startMotor();
      updatePumpStatus(true);
      motor_is_running = true;
      last_cycle_time = current_seconds; // 更新開始時間
    }

    if (motor_is_running && (current_seconds - last_cycle_time >= cycle_duration)) {
      // 完成這個cycle，停止馬達
      stopMotor();
      updatePumpStatus(false);
      motor_is_running = false;

      // 確實完成一個cycle後，才增加計數
      completed_cycles++;
      if (cycles_completed_label) {
        char buf[8];
        snprintf(buf, sizeof(buf), "%d", completed_cycles);
        lv_label_set_text(cycles_completed_label, buf);
      }
    }
  } else {
      // is_running為false，保證馬達一定停
      if (motor_is_running) {
        stopMotor();
        updatePumpStatus(false);
        motor_is_running = false;
      }
    }

  // --- motor control ---
}




