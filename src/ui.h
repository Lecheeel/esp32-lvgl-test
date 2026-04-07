#ifndef UI_H
#define UI_H

#include <lvgl.h>
#include "config.h"

// LVGL 配置常量
#define LVGL_TICK_PERIOD 5 // LVGL tick period in milliseconds

// UI相关全局变量声明
extern lv_obj_t *weight_label;
extern lv_obj_t *unit_label;
extern lv_obj_t *status_label;
extern lv_obj_t *calib_step_label;
extern lv_obj_t *calib_msg_label;
extern float lastDisplayedWeight;
extern bool bootAnimationComplete;

// UI函数声明
void showBootAnimation();
void create_weight_display_ui(void);
void create_calibration_ui(void);
void update_weight_display(float weight);
void update_calibration_display(int state); // 使用int代替CalibrationState以避免包含scale.h

#endif // UI_H
