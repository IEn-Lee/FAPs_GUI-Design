#include "ui_tabview.h"
#include "ui_slider.h"
#include "lvgl.h"
#include "ui_control_button.h"
#include "ui_status_label.h"
#include "sensor_bmp280.h"
#include "ui_status_variable.h"

// from other modules
extern void create_start_button(lv_obj_t*, const char*, const lv_font_t*, lv_coord_t, lv_coord_t);

// --- UI helper functions (reuse from your code) ---
static void create_status_cell(lv_obj_t* parent, const void* icon_src, const char* label_text, const char* value_text, lv_color_t value_color, lv_coord_t x, lv_coord_t y);
static void create_monitor_chart(lv_obj_t* parent, lv_coord_t x, lv_coord_t y);
static void create_label(lv_obj_t* parent, const char* text, lv_align_t align, const lv_font_t* font, lv_coord_t x, lv_coord_t y);

void create_main_tabview() {
    lv_obj_t* tabview = lv_tabview_create(lv_screen_active());
    lv_obj_t* tab_btns = lv_tabview_get_tab_btns(tabview);
    lv_obj_set_style_text_font(tab_btns, &lv_font_montserrat_22, 0);

    lv_obj_t* tab1 = lv_tabview_add_tab(tabview, "SETTING");
    lv_obj_t* tab2 = lv_tabview_add_tab(tabview, "DEVICE STATUS");
    lv_obj_t* tab3 = lv_tabview_add_tab(tabview, "MONITOR");

    // --- Tab 1: Setting ---
    create_named_slider(tab1, "Micturition cycles/day", 1, 10, "", 20, 10, 1);
    create_named_slider(tab1, "Cycle Duration (sec)", 1, 120, "", 20, 160, 1);
    lv_obj_set_style_text_font(tab1, &lv_font_montserrat_34, 0);
    create_start_button(tab1, "START", &lv_font_montserrat_40, 0, 0);

    // --- Tab 2: Status ---
    extern const lv_img_dsc_t uvled;
    extern const lv_img_dsc_t pump;
    extern const lv_img_dsc_t temperature;
    extern const lv_img_dsc_t humidity;

    create_status_cell(tab2, &uvled, "UV LED", "OFF", lv_palette_main(LV_PALETTE_RED), 20, 20);
    create_status_cell(tab2, &pump, "Pump", "OFF", lv_palette_main(LV_PALETTE_RED), 400, 20);
    create_status_cell(tab2, &temperature, "Temp", "NaN °C", lv_palette_main(LV_PALETTE_BLUE), 20, 200);
    create_status_cell(tab2, &humidity, "Humidity", "NaN %", lv_palette_main(LV_PALETTE_BLUE), 400, 200);

    // --- Tab 3: Monitor ---
    create_label(tab3, "Elapsed Time:", LV_ALIGN_TOP_LEFT, &lv_font_montserrat_40, 0, 20);
    elapsed_time_label = lv_label_create(tab3);
    lv_label_set_text(elapsed_time_label, "00:00:00");
    lv_obj_set_style_text_font(elapsed_time_label, &lv_font_montserrat_48, 0);
    lv_obj_align(elapsed_time_label, LV_ALIGN_TOP_RIGHT, 0, 20);

    create_label(tab3, "Cycles Completed:", LV_ALIGN_TOP_LEFT, &lv_font_montserrat_40, 0, 90);
    cycles_completed_label = lv_label_create(tab3);
    lv_label_set_text(cycles_completed_label, "0");
    lv_obj_set_style_text_font(cycles_completed_label, &lv_font_montserrat_48, 0);
    lv_obj_align(cycles_completed_label, LV_ALIGN_TOP_RIGHT, 0, 90);

    create_monitor_chart(tab3, 0, 0);

    create_label(tab3, "40", LV_ALIGN_OUT_BOTTOM_LEFT, &lv_font_montserrat_20, 35, 185);
    create_label(tab3, "35", LV_ALIGN_OUT_BOTTOM_LEFT, &lv_font_montserrat_20, 35, 230);
    create_label(tab3, "30", LV_ALIGN_OUT_BOTTOM_LEFT, &lv_font_montserrat_20, 35, 270);
    create_label(tab3, "25", LV_ALIGN_OUT_BOTTOM_LEFT, &lv_font_montserrat_20, 35, 315);
    create_label(tab3, "20", LV_ALIGN_OUT_BOTTOM_LEFT, &lv_font_montserrat_20, 35, 360);
    create_label(tab3, "Temperature Chart", LV_ALIGN_OUT_BOTTOM_LEFT, &lv_font_montserrat_20, 70, 158);
}

// --- Internal helpers ---
static void create_status_cell(lv_obj_t* parent, const void* icon_src, const char* label_text, const char* value_text, lv_color_t value_color, lv_coord_t x, lv_coord_t y) {
    lv_obj_t* cont = lv_obj_create(parent);
    lv_obj_set_size(cont, 350, 160);
    lv_obj_align(cont, LV_ALIGN_TOP_LEFT, x, y);
    lv_obj_set_style_radius(cont, 12, 0);
    lv_obj_set_style_bg_color(cont, lv_palette_lighten(LV_PALETTE_GREY, 3), 0);

    lv_obj_t* icon = lv_img_create(cont);
    lv_img_set_src(icon, icon_src);
    lv_obj_align(icon, LV_ALIGN_TOP_LEFT, 0, 0);

    lv_obj_t* label = lv_label_create(cont);
    lv_label_set_text(label, label_text);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_40, 0);
    lv_obj_align(label, LV_ALIGN_TOP_RIGHT, 0, 0);

    lv_obj_t* value = lv_label_create(cont);
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

static void create_label(lv_obj_t* parent, const char* text, lv_align_t align, const lv_font_t* font, lv_coord_t x, lv_coord_t y) {
    lv_obj_t* label = lv_label_create(parent);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_font(label, font, 0);
    lv_obj_align(label, align, x, y);
}

static void create_monitor_chart(lv_obj_t* parent, lv_coord_t x, lv_coord_t y) {
    lv_obj_t* chart = lv_chart_create(parent);
    lv_obj_set_size(chart, 700, 200);
    lv_obj_align(chart, LV_ALIGN_BOTTOM_RIGHT, x, y);
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 20, 40);
    lv_chart_set_div_line_count(chart, 5, 20);
    lv_obj_set_style_bg_opa(chart, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(chart, 2, 0);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    lv_chart_set_point_count(chart, 20);
    lv_chart_series_t* ser = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_BLUE), LV_CHART_AXIS_PRIMARY_Y);
    for (int i = 0; i < 20; ++i) {
        lv_chart_set_value_by_id(chart, ser, i, LV_CHART_POINT_NONE);
    }
    lv_chart_refresh(chart);

    temperature_chart = chart;
    temp_series = ser;

    chart_temp_label = lv_label_create(parent);
    lv_label_set_text(chart_temp_label, "Now: -- °C");
    lv_obj_set_style_text_font(chart_temp_label, &lv_font_montserrat_20, 0);
    lv_obj_align_to(chart_temp_label, chart, LV_ALIGN_TOP_RIGHT, -45, 0);
    lv_obj_set_style_bg_color(chart_temp_label, lv_palette_lighten(LV_PALETTE_GREY, 3), 0);
    lv_obj_set_style_bg_opa(chart_temp_label, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(chart_temp_label, 4, 0);
    lv_obj_set_style_radius(chart_temp_label, 6, 0);
}
