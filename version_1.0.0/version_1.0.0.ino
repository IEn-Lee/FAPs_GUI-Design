#include "Arduino_H7_Video.h"
#include "Arduino_GigaDisplayTouch.h"
#include "lvgl.h"

Arduino_H7_Video          Display(800, 480, GigaDisplayShield);
Arduino_GigaDisplayTouch  TouchDetector;

// Global background section
static lv_obj_t * mid_left_area;
static lv_obj_t * bottom_area;
static lv_obj_t * mid_right_area;
static lv_obj_t * title_area;

void background_display(void)
{
    // Space setting
    title_area = lv_obj_create(lv_scr_act());
    lv_obj_set_size(title_area, 795, 50);
    lv_obj_align(title_area, LV_ALIGN_TOP_MID, 0, 5);

    bottom_area = lv_obj_create(lv_scr_act());
    lv_obj_set_size(bottom_area, 795, 200);
    lv_obj_align(bottom_area, LV_ALIGN_BOTTOM_MID, 0, -5);
    
    
    mid_left_area = lv_obj_create(lv_scr_act());
    lv_obj_set_size(mid_left_area, 395, 210);
    lv_obj_align(mid_left_area, LV_ALIGN_CENTER, -200, -75);

    mid_right_area = lv_obj_create(lv_scr_act());
    lv_obj_set_size(mid_right_area, 395, 210);
    lv_obj_align(mid_right_area, LV_ALIGN_CENTER, 200, -75);

    // Space label setting
    lv_obj_t * title_label = lv_label_create(title_area);
    lv_label_set_text(title_label, "Biofilm Test Bench Controller");
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_26, 0);
    lv_obj_align(title_label, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t * setting_label = lv_label_create(mid_left_area);
    lv_label_set_text(setting_label, "SETTINGS");
    lv_obj_set_style_text_font(setting_label, &lv_font_montserrat_24, 0);
    lv_obj_align(setting_label, LV_ALIGN_TOP_MID, 0, 0);

    lv_obj_t * device_status_label = lv_label_create(mid_right_area);
    lv_label_set_text(device_status_label, "DEVICE STATUS");
    lv_obj_set_style_text_font(device_status_label, &lv_font_montserrat_24, 0);
    lv_obj_align(device_status_label, LV_ALIGN_TOP_MID, 0, 0);

}

// Label
void setting_section_item_label(void)
{
    lv_obj_t * micturition = lv_label_create(mid_left_area);
    lv_label_set_text(micturition, "Micturition cycles/day");
    lv_obj_set_style_text_font(micturition, &lv_font_montserrat_18, 0);
    lv_obj_align(micturition, LV_ALIGN_LEFT_MID, 0, -30);

    lv_obj_t * cycle_duration = lv_label_create(mid_left_area);
    lv_label_set_text(cycle_duration, "Cycle duration (s)");
    lv_obj_set_style_text_font(cycle_duration, &lv_font_montserrat_18, 0);
    lv_obj_align(cycle_duration, LV_ALIGN_LEFT_MID, 0, 20);
    
}

void device_status_section_item_label(void)
{
    lv_obj_t * uvled_label = lv_label_create(mid_right_area);
    lv_label_set_text(uvled_label, "UV LED");
    lv_obj_set_style_text_font(uvled_label, &lv_font_montserrat_22, 0);
    lv_obj_align(uvled_label, LV_ALIGN_LEFT_MID, 0, -30);

    lv_obj_t * pump_label = lv_label_create(mid_right_area);
    lv_label_set_text(pump_label, "Pump");
    lv_obj_set_style_text_font(pump_label, &lv_font_montserrat_22, 0);
    lv_obj_align(pump_label, LV_ALIGN_LEFT_MID, 0, 0);

    lv_obj_t * temp_label = lv_label_create(mid_right_area);
    lv_label_set_text(temp_label, "Temperature");
    lv_obj_set_style_text_font(temp_label, &lv_font_montserrat_22, 0);
    lv_obj_align(temp_label, LV_ALIGN_LEFT_MID, 0, 30);

    lv_obj_t * sensor_label = lv_label_create(mid_right_area);
    lv_label_set_text(sensor_label, "Sensor");
    lv_obj_set_style_text_font(sensor_label, &lv_font_montserrat_22, 0);
    lv_obj_align(sensor_label, LV_ALIGN_LEFT_MID, 0, 60);


    lv_obj_t * uvled_status = lv_label_create(mid_right_area);
    lv_label_set_text(uvled_status, "ON");
    lv_obj_set_style_text_font(uvled_status, &lv_font_montserrat_22, 0);
    lv_obj_set_style_text_color(uvled_status, lv_color_hex(0x0000FF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(uvled_status, LV_ALIGN_RIGHT_MID, 0, -30);

    lv_obj_t * pump_status = lv_label_create(mid_right_area);
    lv_label_set_text(pump_status, "OFF");
    lv_obj_set_style_text_font(pump_status, &lv_font_montserrat_22, 0);
    lv_obj_set_style_text_color(pump_status, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(pump_status, LV_ALIGN_RIGHT_MID, 0, 0);

    lv_obj_t * temp_status = lv_label_create(mid_right_area);
    lv_label_set_text(temp_status, "32 °C");
    lv_obj_set_style_text_font(temp_status, &lv_font_montserrat_22, 0);
    lv_obj_align(temp_status, LV_ALIGN_RIGHT_MID, 0, 30);

    lv_obj_t * sensor_status = lv_label_create(mid_right_area);
    lv_label_set_text(sensor_status, "OK");
    lv_obj_set_style_text_font(sensor_status, &lv_font_montserrat_22, 0);
    lv_obj_set_style_text_color(sensor_status, lv_color_hex(0x0000FF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(sensor_status, LV_ALIGN_RIGHT_MID, 0, 60);   
}

void bottom_section_item_label(void)
{
    lv_obj_t * elapsed_time_labed = lv_label_create(bottom_area);
    lv_label_set_text(elapsed_time_labed, "Elapsed time");
    lv_obj_set_style_text_font(elapsed_time_labed, &lv_font_montserrat_22, 0);
    lv_obj_align(elapsed_time_labed, LV_ALIGN_LEFT_MID, 0, -70);

    lv_obj_t * elapsed_time = lv_label_create(bottom_area);
    lv_label_set_text(elapsed_time, "01:45:23");
    lv_obj_set_style_text_font(elapsed_time, &lv_font_montserrat_40, 0);
    lv_obj_align(elapsed_time, LV_ALIGN_LEFT_MID, 0, -30);

    lv_obj_t * cycle_completed = lv_label_create(bottom_area);
    lv_label_set_text(cycle_completed, "Cycle completed");
    lv_obj_set_style_text_font(cycle_completed, &lv_font_montserrat_22, 0);
    lv_obj_align(cycle_completed, LV_ALIGN_LEFT_MID, 0, 20);

    lv_obj_t * cycle_completed_num = lv_label_create(bottom_area);
    lv_label_set_text(cycle_completed_num, "5");
    lv_obj_set_style_text_font(cycle_completed_num, &lv_font_montserrat_40, 0);
    lv_obj_align(cycle_completed_num, LV_ALIGN_LEFT_MID, 0, 60);

    // Temporary temperature label
    lv_obj_t * temp_30 = lv_label_create(bottom_area);
    lv_label_set_text(temp_30, "30");
    lv_obj_set_style_text_font(temp_30, &lv_font_montserrat_20, 0);
    lv_obj_align(temp_30, LV_ALIGN_CENTER, -65, 0);

    lv_obj_t * temp_40 = lv_label_create(bottom_area);
    lv_label_set_text(temp_40, "40");
    lv_obj_set_style_text_font(temp_40, &lv_font_montserrat_20, 0);
    lv_obj_align(temp_40, LV_ALIGN_CENTER, -65, -70);

    lv_obj_t * temp_20 = lv_label_create(bottom_area);
    lv_label_set_text(temp_20, "20");
    lv_obj_set_style_text_font(temp_20, &lv_font_montserrat_20, 0);
    lv_obj_align(temp_20, LV_ALIGN_CENTER, -65, 70);

    lv_obj_t * temp_25 = lv_label_create(bottom_area);
    lv_label_set_text(temp_25, "25");
    lv_obj_set_style_text_font(temp_25, &lv_font_montserrat_20, 0);
    lv_obj_align(temp_25, LV_ALIGN_CENTER, -65, 35);

    lv_obj_t * temp_35 = lv_label_create(bottom_area);
    lv_label_set_text(temp_35, "35");
    lv_obj_set_style_text_font(temp_35, &lv_font_montserrat_20, 0);
    lv_obj_align(temp_35, LV_ALIGN_CENTER, -65, -35);
    // Temporary temperature label   
}
// Label

static void event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * btn = (lv_obj_t *)lv_event_get_target(e);

    if(code == LV_EVENT_CLICKED) {
        // choose index of label
        lv_obj_t * label = lv_obj_get_child(btn, 0);
        
        // check current text is "START" or not
        if(strcmp(lv_label_get_text(label), "START") == 0) {
            lv_label_set_text(label, "STOP");
            // 0xFF0000 => red
            lv_obj_set_style_bg_color(btn, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
        } else {
            lv_label_set_text(label, "START");
            // 0x0000FF => blue
            lv_obj_set_style_bg_color(btn, lv_color_hex(0x0000FF), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
}

void start_botton(void)
{
    lv_obj_t * btn1 = lv_button_create(lv_scr_act());
    lv_obj_add_event_cb(btn1, event_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_align_to(btn1, mid_left_area, LV_ALIGN_BOTTOM_MID, 0, -10);
    
    lv_obj_set_style_bg_color(btn1, lv_color_hex(0x0000FF), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_t * label = lv_label_create(btn1);
    lv_label_set_text(label, "START");
    lv_obj_set_style_text_font(label, &lv_font_montserrat_16, 0);
    lv_obj_center(label);
}

// SpinBox //
static lv_obj_t * micturition_spinBox;
static lv_obj_t * cycle_duration_spinBox;
static int32_t custom_step = 1;

static void lv_spinbox_increment_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_SHORT_CLICKED || code  == LV_EVENT_LONG_PRESSED_REPEAT) {
        lv_spinbox_set_value(micturition_spinBox, 
        lv_spinbox_get_value(micturition_spinBox) + custom_step);
    }
}
static void lv_spinbox_increment_event_cb_2(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_SHORT_CLICKED || code  == LV_EVENT_LONG_PRESSED_REPEAT) {
        lv_spinbox_set_value(cycle_duration_spinBox,
        lv_spinbox_get_value(cycle_duration_spinBox) + custom_step);
    }
}

static void lv_spinbox_decrement_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_SHORT_CLICKED || code == LV_EVENT_LONG_PRESSED_REPEAT) {
        lv_spinbox_set_value(micturition_spinBox, 
        lv_spinbox_get_value(micturition_spinBox) - custom_step);
    }
}
static void lv_spinbox_decrement_event_cb_2(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_SHORT_CLICKED || code == LV_EVENT_LONG_PRESSED_REPEAT) {
        lv_spinbox_set_value(cycle_duration_spinBox,
        lv_spinbox_get_value(cycle_duration_spinBox) - custom_step);
    }
}

void micturition_spinbox(void)
{
    micturition_spinBox = lv_spinbox_create(lv_screen_active());
    lv_spinbox_set_range(micturition_spinBox, 0, 99);
    lv_spinbox_set_digit_format(micturition_spinBox, 2, 0);
    lv_obj_set_width(micturition_spinBox, 50);
    lv_obj_align_to(micturition_spinBox, mid_left_area, LV_ALIGN_RIGHT_MID, -40, -30);

    int32_t h = lv_obj_get_height(micturition_spinBox);

    lv_obj_t * btn = lv_button_create(lv_screen_active());
    lv_obj_set_size(btn, h, h);
    lv_obj_align_to(btn, micturition_spinBox, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
    lv_obj_set_style_bg_image_src(btn, LV_SYMBOL_PLUS, 0);
    lv_obj_add_event_cb(btn, lv_spinbox_increment_event_cb, LV_EVENT_ALL,  NULL);

    btn = lv_button_create(lv_screen_active());
    lv_obj_set_size(btn, h, h);
    lv_obj_align_to(btn, micturition_spinBox, LV_ALIGN_OUT_LEFT_MID, -5, 0);
    lv_obj_set_style_bg_image_src(btn, LV_SYMBOL_MINUS, 0);
    lv_obj_add_event_cb(btn, lv_spinbox_decrement_event_cb, LV_EVENT_ALL, NULL);
}

void cycle_duration_spinbox(void)
{
    cycle_duration_spinBox = lv_spinbox_create(lv_screen_active());
    lv_spinbox_set_range(cycle_duration_spinBox, 0, 99);
    lv_spinbox_set_digit_format(cycle_duration_spinBox, 2, 0);
    lv_obj_set_width(cycle_duration_spinBox, 50);
    lv_obj_align_to(cycle_duration_spinBox, mid_left_area, LV_ALIGN_RIGHT_MID, -40, 20);

    int32_t h = lv_obj_get_height(cycle_duration_spinBox);

    lv_obj_t * btn = lv_button_create(lv_screen_active());
    lv_obj_set_size(btn, h, h);
    lv_obj_align_to(btn, cycle_duration_spinBox, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
    lv_obj_set_style_bg_image_src(btn, LV_SYMBOL_PLUS, 0);
    lv_obj_add_event_cb(btn, lv_spinbox_increment_event_cb_2, LV_EVENT_ALL,  NULL);

    btn = lv_button_create(lv_screen_active());
    lv_obj_set_size(btn, h, h);
    lv_obj_align_to(btn, cycle_duration_spinBox, LV_ALIGN_OUT_LEFT_MID, -5, 0);
    lv_obj_set_style_bg_image_src(btn, LV_SYMBOL_MINUS, 0);
    lv_obj_add_event_cb(btn, lv_spinbox_decrement_event_cb_2, LV_EVENT_ALL, NULL);
}
// SpinBox //

// Line Chart with Faded area //
static void hook_division_lines(lv_event_t * e);
static void add_faded_area(lv_event_t * e);
static void draw_event_cb(lv_event_t * e);

/**
 * Add a faded area effect to the line chart and make some division lines ticker
 */
void temp_chart(void)
{
    /*Create a chart*/
    lv_obj_t * chart = lv_chart_create(lv_screen_active());
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);   /*Show lines and points too*/
    lv_obj_set_size(chart, 400, 150);
    lv_obj_set_style_pad_all(chart, 0, 0);
    lv_obj_set_style_radius(chart, 0, 0);
    lv_obj_align_to(chart, bottom_area, LV_ALIGN_CENTER, 150, 0);

    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 20, 40);
    lv_chart_set_div_line_count(chart, 9, 7);

    lv_obj_add_event_cb(chart, draw_event_cb, LV_EVENT_DRAW_TASK_ADDED, NULL);
    lv_obj_add_flag(chart, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);

    lv_chart_series_t * ser = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_BLUE), LV_CHART_AXIS_PRIMARY_Y);

    int32_t values[10] = {28, 32, 27, 35, 28, 31, 37, 29, 30, 32};
    uint32_t i;
    for(i = 0; i < 10; i++) {
        lv_chart_set_next_value(chart, ser, values[i]);
    }
}

static void draw_event_cb(lv_event_t * e)
{
    lv_draw_task_t * draw_task = lv_event_get_draw_task(e);
    lv_draw_dsc_base_t * base_dsc = (lv_draw_dsc_base_t *)lv_draw_task_get_draw_dsc(draw_task);

    if(base_dsc->part == LV_PART_ITEMS && lv_draw_task_get_type(draw_task) == LV_DRAW_TASK_TYPE_LINE) {
        add_faded_area(e);

    }
    /*Hook the division lines too*/
    if(base_dsc->part == LV_PART_MAIN && lv_draw_task_get_type(draw_task) == LV_DRAW_TASK_TYPE_LINE) {
        hook_division_lines(e);
    }
}

static void add_faded_area(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target_obj(e);
    lv_area_t coords;
    lv_obj_get_coords(obj, &coords);

    lv_draw_task_t * draw_task = lv_event_get_draw_task(e);
    lv_draw_dsc_base_t * base_dsc = (lv_draw_dsc_base_t *)lv_draw_task_get_draw_dsc(draw_task);

    const lv_chart_series_t * ser = lv_chart_get_series_next(obj, NULL);
    lv_color_t ser_color = lv_chart_get_series_color(obj, ser);

    /*Draw a triangle below the line witch some opacity gradient*/
    lv_draw_line_dsc_t * draw_line_dsc = (lv_draw_line_dsc_t *)lv_draw_task_get_draw_dsc(draw_task);
    lv_draw_triangle_dsc_t tri_dsc;

    lv_draw_triangle_dsc_init(&tri_dsc);
    tri_dsc.p[0].x = draw_line_dsc->p1.x;
    tri_dsc.p[0].y = draw_line_dsc->p1.y;
    tri_dsc.p[1].x = draw_line_dsc->p2.x;
    tri_dsc.p[1].y = draw_line_dsc->p2.y;
    tri_dsc.p[2].x = draw_line_dsc->p1.y < draw_line_dsc->p2.y ? draw_line_dsc->p1.x : draw_line_dsc->p2.x;
    tri_dsc.p[2].y = LV_MAX(draw_line_dsc->p1.y, draw_line_dsc->p2.y);
    tri_dsc.bg_grad.dir = LV_GRAD_DIR_VER;

    int32_t full_h = lv_obj_get_height(obj);
    int32_t fract_uppter = (int32_t)(LV_MIN(draw_line_dsc->p1.y, draw_line_dsc->p2.y) - coords.y1) * 255 / full_h;
    int32_t fract_lower = (int32_t)(LV_MAX(draw_line_dsc->p1.y, draw_line_dsc->p2.y) - coords.y1) * 255 / full_h;
    tri_dsc.bg_grad.stops[0].color = ser_color;
    tri_dsc.bg_grad.stops[0].opa = (lv_opa_t)(255 - fract_uppter);
    tri_dsc.bg_grad.stops[0].opa = 255 - fract_uppter;
    tri_dsc.bg_grad.stops[0].frac = 0;
    tri_dsc.bg_grad.stops[1].color = ser_color;
    tri_dsc.bg_grad.stops[1].opa = (lv_opa_t)(255 - fract_lower);
    tri_dsc.bg_grad.stops[1].frac = 255;

    lv_draw_triangle(base_dsc->layer, &tri_dsc);

    /*Draw rectangle below the triangle*/
    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.bg_grad.dir = LV_GRAD_DIR_VER;
    rect_dsc.bg_grad.stops[0].color = ser_color;
    rect_dsc.bg_grad.stops[0].frac = 0;
    rect_dsc.bg_grad.stops[0].opa = (lv_opa_t)(255 - fract_lower);
    rect_dsc.bg_grad.stops[1].color = ser_color;
    rect_dsc.bg_grad.stops[1].frac = 255;
    rect_dsc.bg_grad.stops[1].opa = 0;

    lv_area_t rect_area;
    rect_area.x1 = (int32_t)draw_line_dsc->p1.x;
    rect_area.x2 = (int32_t)draw_line_dsc->p2.x - 1;
    rect_area.y1 = (int32_t)LV_MAX(draw_line_dsc->p1.y, draw_line_dsc->p2.y) - 1;
    rect_area.y2 = (int32_t)coords.y2;
    lv_draw_rect(base_dsc->layer, &rect_dsc, &rect_area);
}

static void hook_division_lines(lv_event_t * e)
{
    lv_draw_task_t * draw_task = lv_event_get_draw_task(e);
    lv_draw_dsc_base_t * base_dsc = (lv_draw_dsc_base_t *)lv_draw_task_get_draw_dsc(draw_task);
    lv_draw_line_dsc_t * line_dsc = (lv_draw_line_dsc_t *)lv_draw_task_get_draw_dsc(draw_task);

    /*Vertical line*/
    if(line_dsc->p1.x == line_dsc->p2.x) {
        line_dsc->color  = lv_palette_lighten(LV_PALETTE_GREY, 1);
        if(base_dsc->id1 == 3) {
            line_dsc->width = 2;
            line_dsc->dash_gap = 0;
            line_dsc->dash_width = 0;
        }
        else {
            line_dsc->width = 1;
            line_dsc->dash_gap = 6;
            line_dsc->dash_width = 6;
        }
    }
    /*Horizontal line*/
    else {
        if(base_dsc->id1 == 2) {
            line_dsc->width  = 2;
            line_dsc->dash_gap  = 0;
            line_dsc->dash_width  = 0;
        }
        else {
            line_dsc->width = 1;
            line_dsc->dash_gap  = 6;
            line_dsc->dash_width  = 6;
        }

        if(base_dsc->id1 == 1  || base_dsc->id1 == 3) {
            line_dsc->color  = lv_palette_main(LV_PALETTE_GREY);
        }
        else {
            line_dsc->color  = lv_palette_lighten(LV_PALETTE_GREY, 1);
        }
    }
}

// Line Chart with Faded area //

void setup() {
    Display.begin();
    TouchDetector.begin();
    background_display();
    start_botton();
    setting_section_item_label();
    device_status_section_item_label();
    micturition_spinbox();
    cycle_duration_spinbox();
    bottom_section_item_label();
    temp_chart();
}

void loop() {
    lv_task_handler();
    delay(5);
}
