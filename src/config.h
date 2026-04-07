#ifndef CONFIG_H
#define CONFIG_H

// =========================
// 显示屏引脚定义
// =========================
#define TFT_CS    5   // 片选引脚
#define TFT_RST   14  // 复位引脚
#define TFT_DC    15  // 数据/命令选择引脚
#define TFT_SCLK  18  // SPI 时钟线
#define TFT_MOSI  23  // SPI 数据线

// =========================
// HX711 称重模块引脚定义
// =========================
#define HX711_DT  13  // 数据引脚
#define HX711_SCK 12  // 时钟引脚

// =========================
// 按钮引脚定义
// =========================
#define BUTTON_PIN 0     // 校准按钮引脚（io0）
#define TARE_BUTTON1 16  // 去皮按钮1（io16）
#define TARE_BUTTON2 17  // 去皮按钮2（io17）

// =========================
// 显示屏参数
// =========================
#define SCREEN_WIDTH  160
#define SCREEN_HEIGHT 128

// =========================
// 称重模块参数
// =========================
#define CALIBRATION_FACTOR  425.635010  // 校准系数（已校准值）
#define DEFAULT_OFFSET      -98285      // 默认零点偏移（已校准值）
#define MAX_WEIGHT          2000.0      // 最大显示重量（克）
#define WEIGHT_THRESHOLD    0.1        // 重量变化阈值（小于此值不更新显示）
#define SCALE_READINGS      10          // 初始化时读取次数
#define SCALE_DELAY         10          // 读取间隔（毫秒）

// =========================
// 校准参数
// =========================
#define CALIBRATION_WEIGHT  200.0   // 校准砝码重量（克）
#define CALIBRATION_STABLE_TIME 10000  // 稳定等待时间（毫秒，10秒）
#define CALIBRATION_READINGS 50     // 校准时的读取次数

// =========================
// 串口参数
// =========================
#define SERIAL_BAUD_RATE       9600
#define SERIAL_OUTPUT_INTERVAL 100  // 串口输出间隔（毫秒）

// =========================
// WiFi 热点参数
// =========================
#define WIFI_AP_SSID           "CoffeeScale"
#define WIFI_AP_PASSWORD       "12345678"
#define WIFI_AP_CHANNEL        1
#define WIFI_AP_MAX_CONN       4

// =========================
// Web服务器参数
// =========================
#define WEB_SERVER_PORT        80

#endif // CONFIG_H

