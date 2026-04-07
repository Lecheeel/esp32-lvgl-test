#include <Arduino.h>
#include <TFT_eSPI.h>
#include <lvgl.h>
#include "config.h"
#include "scale.h"
#include "timer.h"
#include "coffee_webserver.h"
#include "ui.h"

// --- 全局对象 ---
TFT_eSPI tft = TFT_eSPI();
Scale scale;
Timer timer;
CoffeeWebServer* webServer = nullptr;

// --- 显存缓冲区 (关键修复：减小高度以节省内存) ---
// 宽度 160 * 高度 10 = 1600 像素
// 1600 * 2字节(RGB565) = 3200 字节
// 双缓冲共占用 6.4KB，远小于之前的 12.8KB，解决内存溢出
const int BUF_HEIGHT = 10;
static lv_color_t buf1[SCREEN_WIDTH * BUF_HEIGHT];
static lv_color_t buf2[SCREEN_WIDTH * BUF_HEIGHT];

static unsigned long last_tick = 0;

// 按钮防抖
bool lastButtonState = HIGH;
bool buttonPressed = false;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

// 去皮按钮防抖
bool lastTareButton1State = HIGH;
bool tareButton1Pressed = false;
unsigned long lastTareButton1DebounceTime = 0;

bool lastTareButton2State = HIGH;
bool tareButton2Pressed = false;
unsigned long lastTareButton2DebounceTime = 0;

// --- 刷新回调函数 (关键修复：简化坐标映射) ---
void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.startWrite();
    // 因为 tft.setRotation(3) 已经设置好了逻辑坐标系
    // 这里直接使用 LVGL 的坐标，不要手动交换 x/y
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors((uint16_t *)px_map, w * h, true);
    tft.endWrite();

    lv_display_flush_ready(disp);
}

bool checkButton()
{
    bool reading = digitalRead(BUTTON_PIN);
    if (reading != lastButtonState) lastDebounceTime = millis();
    if ((millis() - lastDebounceTime) > debounceDelay) {
        if (reading == LOW && !buttonPressed) {
            buttonPressed = true;
            lastButtonState = reading;
            return true;
        }
    }
    if (reading == HIGH) buttonPressed = false;
    lastButtonState = reading;
    return false;
}

bool checkTareButton1()
{
    bool reading = digitalRead(TARE_BUTTON1);
    if (reading != lastTareButton1State) lastTareButton1DebounceTime = millis();
    if ((millis() - lastTareButton1DebounceTime) > debounceDelay) {
        if (reading == LOW && !tareButton1Pressed) {
            tareButton1Pressed = true;
            lastTareButton1State = reading;
            return true;
        }
    }
    if (reading == HIGH) tareButton1Pressed = false;
    lastTareButton1State = reading;
    return false;
}

bool checkTareButton2()
{
    bool reading = digitalRead(TARE_BUTTON2);
    if (reading != lastTareButton2State) lastTareButton2DebounceTime = millis();
    if ((millis() - lastTareButton2DebounceTime) > debounceDelay) {
        if (reading == LOW && !tareButton2Pressed) {
            tareButton2Pressed = true;
            lastTareButton2State = reading;
            return true;
        }
    }
    if (reading == HIGH) tareButton2Pressed = false;
    lastTareButton2State = reading;
    return false;
}

void setup()
{
    Serial.begin(SERIAL_BAUD_RATE);
    delay(100);
    Serial.println("System Starting...");

    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(TARE_BUTTON1, INPUT_PULLUP);
    pinMode(TARE_BUTTON2, INPUT_PULLUP);

    // 1. 初始化 TFT
    tft.init();
    tft.setRotation(3); // 横屏模式 (Cable 右侧)
    tft.fillScreen(TFT_BLACK);

    // 2. 初始化 LVGL
    lv_init();
    
    // 创建显示驱动，使用逻辑分辨率 160x128
    lv_display_t *disp = lv_display_create(SCREEN_WIDTH, SCREEN_HEIGHT);
    lv_display_set_buffers(disp, buf1, buf2, sizeof(buf1), LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(disp, my_disp_flush);
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB565);

    // 3. 硬件初始化
    scale.init();
    webServer = new CoffeeWebServer(&scale, &timer);
    webServer->begin();

    // 4. 显示 UI
    showBootAnimation();
    create_weight_display_ui();
    
    Serial.println("Setup Complete.");
    last_tick = millis();
}

void loop()
{
    unsigned long current_tick = millis();
    if (current_tick - last_tick >= LVGL_TICK_PERIOD) {
        lv_tick_inc(LVGL_TICK_PERIOD);
        last_tick = current_tick;
    }

    // 按钮按下 -> 开始校准
    if (checkButton() && !scale.isCalibrating()) {
        scale.startCalibration();
        lv_obj_clean(lv_scr_act());
        create_calibration_ui();
    }

    // 去皮按钮按下 -> 执行去皮功能
    if ((checkTareButton1() || checkTareButton2()) && !scale.isCalibrating() && scale.isReady()) {
        scale.tare();
        Serial.println("Tare button pressed - weight reset to zero");
    }

    if (scale.isCalibrating()) {
        scale.updateCalibration();
        update_calibration_display((int)scale.getCalibrationState());
        
        if (scale.getCalibrationState() == CALIB_COMPLETE) {
            update_calibration_display(4);
            delay(1000);
            lv_obj_clean(lv_scr_act());
            create_weight_display_ui();
            lastDisplayedWeight = -1.0;
        }
    }
    else if (scale.isReady()) {
        float weight = scale.getWeight();
        update_weight_display(weight);
        if (webServer) webServer->updateWeight(weight);
    }

    if (webServer) webServer->handleClient();

    lv_timer_handler();
    delay(5);
}
