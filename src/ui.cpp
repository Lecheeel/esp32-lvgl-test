#include "ui.h"
#include "config.h"
#include <Arduino.h> // for millis(), delay()

// 全局变量定义
lv_obj_t *weight_label = nullptr;
lv_obj_t *unit_label = nullptr;
lv_obj_t *status_label = nullptr;
lv_obj_t *wifi_label = nullptr;
lv_obj_t *calib_step_label = nullptr;
lv_obj_t *calib_msg_label = nullptr;

float lastDisplayedWeight = -1.0;

// 辅助：绘制简易咖啡杯 (适配横屏位置)
void drawCoffeeCup(lv_obj_t *parent, int x, int y, int width, int height) {
    lv_obj_t *cup_body = lv_obj_create(parent);
    lv_obj_set_size(cup_body, width, height);
    lv_obj_set_pos(cup_body, x, y);
    lv_obj_set_style_bg_color(cup_body, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_radius(cup_body, 3, 0);
    lv_obj_set_style_border_width(cup_body, 0, 0);
    lv_obj_clear_flag(cup_body, LV_OBJ_FLAG_SCROLLABLE);

    // 咖啡液面
    lv_obj_t *coffee = lv_obj_create(parent);
    lv_obj_set_size(coffee, width - 4, 6);
    lv_obj_set_pos(coffee, x + 2, y + 4);
    lv_obj_set_style_bg_color(coffee, lv_color_hex(0x6F4E37), 0);
    lv_obj_set_style_border_width(coffee, 0, 0);
}

// 开机动画
void showBootAnimation() {
    lv_obj_t *screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x000000), 0);

    // 简单预热
    for(int i=0; i<5; i++) { lv_timer_handler(); delay(10); }

    int centerX = SCREEN_WIDTH / 2;
    int centerY = SCREEN_HEIGHT / 2;

    // 简单的淡入文字
    lv_obj_t *text = lv_label_create(screen);
    lv_label_set_text(text, "COFFEE\nSCALE");
    lv_obj_set_style_text_align(text, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(text, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(text, &lv_font_montserrat_14, 0);
    lv_obj_align(text, LV_ALIGN_CENTER, 0, 0);

    // 模拟加载条
    lv_obj_t *bar = lv_bar_create(screen);
    lv_obj_set_size(bar, 100, 6);
    lv_obj_align(bar, LV_ALIGN_CENTER, 0, 30);
    lv_bar_set_range(bar, 0, 100);
    lv_bar_set_value(bar, 0, LV_ANIM_OFF);

    for (int i = 0; i <= 100; i+=5) {
        lv_bar_set_value(bar, i, LV_ANIM_ON);
        lv_timer_handler();
        delay(30);
    }
    
    delay(500);
    lv_obj_clean(screen);
}

// 主界面：重量显示
void create_weight_display_ui(void) {
    lv_obj_t *screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x101018), 0);

    // 1. 顶部栏
    wifi_label = lv_label_create(screen);
    lv_label_set_text(wifi_label, LV_SYMBOL_WIFI " AP");
    lv_obj_set_style_text_color(wifi_label, lv_color_hex(0x00FF88), 0);
    lv_obj_set_style_text_font(wifi_label, &lv_font_montserrat_12, 0);
    lv_obj_align(wifi_label, LV_ALIGN_TOP_LEFT, 5, 5);

    lv_obj_t *title = lv_label_create(screen);
    lv_label_set_text(title, "SCALE");
    lv_obj_set_style_text_color(title, lv_color_hex(0x555555), 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_12, 0);
    lv_obj_align(title, LV_ALIGN_TOP_RIGHT, -5, 5);

    // 2. 中部：重量 (横向拉宽)
    lv_obj_t *weight_cont = lv_obj_create(screen);
    lv_obj_set_size(weight_cont, 140, 70);
    lv_obj_align(weight_cont, LV_ALIGN_CENTER, 0, -5);
    lv_obj_set_style_bg_color(weight_cont, lv_color_hex(0x1E1E2C), 0);
    lv_obj_set_style_border_color(weight_cont, lv_color_hex(0x00d4ff), 0);
    lv_obj_set_style_border_width(weight_cont, 2, 0);
    lv_obj_set_style_radius(weight_cont, 8, 0);
    lv_obj_clear_flag(weight_cont, LV_OBJ_FLAG_SCROLLABLE);

    weight_label = lv_label_create(weight_cont);
    lv_label_set_text(weight_label, "0.0");
    lv_obj_set_style_text_color(weight_label, lv_color_hex(0xFFFFFF), 0);
    // 建议在 lv_conf.h 中开启 montserrat_24 或 28 以获得更好效果
    lv_obj_set_style_text_font(weight_label, &lv_font_montserrat_14, 0); 
    lv_obj_align(weight_label, LV_ALIGN_CENTER, -5, 0);

    unit_label = lv_label_create(weight_cont);
    lv_label_set_text(unit_label, "g");
    lv_obj_set_style_text_color(unit_label, lv_color_hex(0xFFFF00), 0);
    lv_obj_set_style_text_font(unit_label, &lv_font_montserrat_12, 0);
    lv_obj_align_to(unit_label, weight_label, LV_ALIGN_OUT_RIGHT_BOTTOM, 4, 0);

    // 3. 底部状态
    status_label = lv_label_create(screen);
    lv_label_set_text(status_label, "READY");
    lv_obj_set_style_text_color(status_label, lv_color_hex(0x00FF00), 0);
    lv_obj_set_style_text_font(status_label, &lv_font_montserrat_12, 0);
    lv_obj_align(status_label, LV_ALIGN_BOTTOM_MID, 0, -5);
}

// 校准界面
void create_calibration_ui(void) {
    lv_obj_t *screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x1a1a2e), 0);

    calib_step_label = lv_label_create(screen);
    lv_label_set_text(calib_step_label, "CALIBRATION");
    lv_obj_set_style_text_color(calib_step_label, lv_color_hex(0xFF8800), 0);
    lv_obj_align(calib_step_label, LV_ALIGN_TOP_MID, 0, 10);

    calib_msg_label = lv_label_create(screen);
    lv_label_set_text(calib_msg_label, "Step 1");
    lv_obj_set_style_text_color(calib_msg_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_align(calib_msg_label, LV_ALIGN_CENTER, 0, 0);
}

void update_calibration_display(int state) {
    if (!calib_step_label) return;
    const char* msg = "";
    switch(state) {
        case 1: msg = "Empty Scale"; break;
        case 2: msg = "Place 100g"; break; // 请根据实际砝码修改提示
        case 3: msg = "Calculating..."; break;
        case 4: msg = "Success!"; break;
    }
    lv_label_set_text(calib_msg_label, msg);
}

void update_weight_display(float weight) {
    if (abs(weight - lastDisplayedWeight) < 0.1 && lastDisplayedWeight >= 0) return;
    if (!weight_label) return;

    char buf[16];
    dtostrf(weight, 1, 1, buf); // 格式化为1位小数
    lv_label_set_text(weight_label, buf);
    
    // 动态调整单位位置
    if (unit_label) {
        lv_obj_align_to(unit_label, weight_label, LV_ALIGN_OUT_RIGHT_BOTTOM, 4, 0);
    }
    lastDisplayedWeight = weight;
}
