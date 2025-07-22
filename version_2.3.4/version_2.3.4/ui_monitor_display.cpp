#include "ui_monitor_display.h"
#include "ui_status_variable.h"
#include <cstdio>  // for snprintf
#include <math.h>  // for isnan()

void update_temperature_ui(float temp) {
    if (!isnan(temp)) {
        if (temperature_value_label) {
            char buf[16];
            snprintf(buf, sizeof(buf), "%.2f°C", temp);
            lv_label_set_text(temperature_value_label, buf);
        }

        if (chart_temp_label) {
            char chart_buf[32];
            snprintf(chart_buf, sizeof(chart_buf), "Now: %.2f °C", temp);
            lv_label_set_text(chart_temp_label, chart_buf);
        }

        if (temperature_chart && temp_series) {
            lv_chart_set_next_value(temperature_chart, temp_series, (int)temp);
            lv_chart_refresh(temperature_chart);
        }
    } else {
        if (temperature_value_label) {
            lv_label_set_text(temperature_value_label, "NaN °C");
        }
        if (chart_temp_label) {
            lv_label_set_text(chart_temp_label, "Now: NaN °C");
        }
    }
}

void update_elapsed_time(unsigned long elapsed) {
    int hours = elapsed / 3600;
    int minutes = (elapsed % 3600) / 60;
    int secs = elapsed % 60;

    if (elapsed_time_label) {
        char buf[16];
        snprintf(buf, sizeof(buf), "%02d:%02d:%02d", hours, minutes, secs);
        lv_label_set_text(elapsed_time_label, buf);
    }
}

void update_cycles_count(int count) {
    if (cycles_completed_label) {
        char buf[8];
        snprintf(buf, sizeof(buf), "%d", count);
        lv_label_set_text(cycles_completed_label, buf);
    }
}
