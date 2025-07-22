#define LV_CONF_INCLUDE_SIMPLE
#include "ui_status_variable.h"
#include "Arduino_H7_Video.h"
#include "Arduino_GigaDisplayTouch.h"
#include "lvgl.h"
#include "pump.h"
#include "humidity.h"
#include "temperature.h"
#include "uvled.h"
#include "Wire.h"
#include "ui_slider.h"
#include "motor_control.h"
#include "led_control.h"
#include "sensor_bmp280.h"
#include "ui_monitor_display.h"
#include "ui_tabview.h"
#include "ui_control_button.h"
#include "ui_status_label.h"


Arduino_H7_Video          Display(800, 480, GigaDisplayShield);
Arduino_GigaDisplayTouch  TouchDetector;


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
  } else if (strcmp(label_text, "UV LED") == 0) {
      led_status_label = value;
  }
}

void setup() {
  Serial.begin(115200);
  Display.begin();
  TouchDetector.begin();
  Wire.begin();  // Restart Wire
  create_main_tabview();

  motor_init();
  led_init();

  if (cycles_completed_label) {
    lv_label_set_text(cycles_completed_label, "0");
  }
  bmp280_init();
}

unsigned int delay_loop_count = 0;

void loop() {
  unsigned int delay_time = 5;
  lv_timer_handler();
  delay(delay_time);

  unsigned long delay_compensation = delay_time * delay_loop_count;
  static unsigned long last_update = 0;
  unsigned long now_ms = millis();

  if (now_ms - (last_update - delay_compensation) >= 1000) {
    last_update = now_ms;

    // --- BMP280 sensor update ---
    bmp280_update();
    update_temperature_ui(bmp280_get_temperature());
    unsigned long elapsed = (now_ms - start_time) / 1000;
    // --- Elapsed time and cycle count update ---
    if (is_running) {
      update_elapsed_time(elapsed);
      update_cycles_count(completed_cycles);
    }
  }

  // --- motor control ---
  if (last_cycle_time == 0) {
    last_cycle_time = current_seconds;
  }

  if (is_running) {
    int cycles_per_day = lv_slider_get_value(micturition_slider);
    int cycle_duration = lv_slider_get_value(cycle_duration_slider);
    int interval_between_cycles = 86400 / cycles_per_day;

    current_seconds = millis() / 1000;

    if (!motor_is_running && (current_seconds - last_cycle_time >= interval_between_cycles)) {
      startMotor();
      updatePumpStatus(true);
      motor_is_running = true;
      last_cycle_time = current_seconds;
    }

    if (motor_is_running && (current_seconds - last_cycle_time >= cycle_duration)) {
      stopMotor();
      updatePumpStatus(false);
      motor_is_running = false;
      completed_cycles++;

      if (cycles_completed_label) {
        char buf[8];
        snprintf(buf, sizeof(buf), "%d", completed_cycles);
        lv_label_set_text(cycles_completed_label, buf);
      }
    }
  } else {
    if (motor_is_running) {
      stopMotor();
      updatePumpStatus(false);
      motor_is_running = false;
    }
  }
  ++delay_loop_count;
}


