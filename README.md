# ESP32 与 LVGL 界面实践

## 摘要

在资源受限的 ESP32 平台上，将 **Light and Versatile Graphics Library（LVGL）** 与 SPI 小屏与网络功能结合，完成一套可运行的交互界面。笔记以 **LVGL 的集成方法、显示管线与内存权衡** 为主线，辅以项目结构与构建说明，供课程设计或毕业设计阶段回顾与复现。

---

## 1. 学习目的与笔记范围

| 主题 | 说明 |
|------|------|
| **核心** | 理解 LVGL 在 MCU 上的角色：渲染模型、显示驱动注册、时间基准、颜色与缓冲策略。 |
| **实践** | 在 Arduino + PlatformIO 环境下完成 `lv_init`、显示刷新回调、主循环中与 `lv_timer_handler()` 的配合。 |
| **延伸** | 与 TFT_eSPI、HX711、SoftAP Web 服务协同时的注意事项（非 LVGL 本体，但影响界面稳定性）。 |

本文档**不**替代官方文档，而是对「本项目中实际用到的 LVGL 概念」作结构化整理与个人理解记录。

---

## 2. 项目概览（便于对照代码）

- **硬件**：ESP32；ST7735（160×128）；HX711 称重模块；若干按键。  
- **软件栈**：PlatformIO；Arduino 框架；**LVGL 9.x**（置于 `lib/lvgl-9.4.0`）；TFT_eSPI；HX711 库。  
- **功能简述**：屏幕显示重量与状态；支持校准与去皮流程；设备开启 WiFi 热点，浏览器通过 HTTP 读取重量与控制计时（业务逻辑见 `src/`，界面集中在 `src/ui.cpp`）。

---

## 3. LVGL 学习

### 3.1 LVGL 在嵌入式系统中的位置

LVGL 是运行在 MCU 上的**用户界面库**：负责控件树、样式、动画与绘制，**不直接操作硬件**。开发者需要：

1. 提供 **显示设备**（display）：分辨率、像素格式、刷新回调；  
2. 周期性调用 **`lv_timer_handler()`** 以推进重绘与动画；  
3. 提供 **系统时间**（tick），使动画与超时正确（本项目中通过 `lv_tick_inc()` 与 `millis()` 对齐）。

理解这一点后，调试思路会清晰：**花屏、错位、卡顿** 往往来自显示回调与分辨率/旋转不一致，或 tick 与 handler 调用频率不当，而非单一控件 API 错误。

### 3.2 版本与配置：`lv_conf.h` 与编译宏

本项目使用 **LVGL 9**，全局配置位于 `include/lv_conf.h`，并在 `platformio.ini` 中通过 `-DLV_CONF_INCLUDE_SIMPLE` 等宏使构建系统包含该头文件。

笔者关注的几项配置及其含义：

- **`LV_COLOR_DEPTH 16`**：与 RGB565 一致，每像素 2 字节，适合 SPI TFT。  
- **`LV_COLOR_16_SWAP`**：是否交换 RGB565 高低字节；需与实际屏与驱动一致，否则会出现色偏或乱色。  
- **`LV_MEM_SIZE`**：LVGL 内部堆大小；控件与样式分配依赖于此，过小可能导致创建对象失败，过大则挤占应用其余内存。  
- **字体**：通过 `LV_FONT_MONTSERRAT_*` 在 `platformio.ini` 中按需启用，避免链接未使用的大字号以节省 Flash。

**学习小结**：`lv_conf.h` 是「资源与功能的总开关」，修改后应重新全量编译，并观察 RAM/Flash 占用变化。

### 3.3 显示驱动：从逻辑帧缓冲到屏

LVGL 9 中显示相关 API 以 **`lv_display_*`** 为前缀（与旧版 `lv_disp_*` 命名体系对应升级）。本项目中典型流程为：

1. `lv_display_create(宽, 高)`：逻辑分辨率与 ST7735 横屏后的可视区域一致（见 `config.h` 中 `SCREEN_WIDTH` / `SCREEN_HEIGHT`）。  
2. `lv_display_set_buffers(..., LV_DISPLAY_RENDER_MODE_PARTIAL)`：**部分刷新模式**，仅需提供**部分高度的行缓冲**，而非整屏，从而在 ESP32 RAM 受限条件下仍可运行。  
3. `lv_display_set_flush_cb(..., my_disp_flush)`：当 LVGL 完成一块区域的绘制后，调用用户实现的 `my_disp_flush`，将 `px_map` 推送到物理屏。  
4. `lv_display_set_color_format(..., LV_COLOR_FORMAT_RGB565)`：与缓冲区内数据格式一致。

**双缓冲与缓冲高度**：本项目使用高度为 10 行的双缓冲（见 `main.cpp` 中注释），总占用约数 KB 量级。笔者体会：**缓冲高度**是在「内存占用」与「刷新次数/CPU 负载」之间的折中；高度过小会增加 flush 次数，过大则可能与其他全局对象（如 WiFi、栈）争抢内存导致异常。

### 3.4 刷新回调中的坐标与旋转

在 `setRotation()` 已设定屏幕坐标系的前提下，**flush 回调中应使用 LVGL 传入的 `area` 与像素缓冲区直接对应到 `setAddrWindow` / `pushColors`**。笔者在实践中遇到因手动交换 x/y 或与旋转假设不一致导致的错位，后统一为：**以 LVGL 逻辑坐标为准，与 TFT 旋转设置保持一致**，问题得以消除。

该经验可归纳为：**显示驱动的职责是「忠实搬运像素」**，旋转与镜像尽量在 TFT 驱动层一次性设好，避免在 flush 中重复变换。

### 3.5 时间基准：`lv_tick_inc` 与主循环

配置中 `LV_TICK_CUSTOM` 为 0 时，需手动维护 tick。本项目在 `loop()` 中按固定周期（如 5 ms）调用 `lv_tick_inc(LVGL_TICK_PERIOD)`，并与 `lv_timer_handler()`、`delay(5)` 协同，使动画与内部定时器行为稳定。

**学习小结**：tick 间隔不必与 `delay` 完全相同，但应**长期平均准确**；长时间阻塞（如不加分解的 `delay(数千)`）会导致动画停顿，宜拆分为带 `lv_timer_handler()` 的短延时（开机动画中已有类似用法）。

### 3.6 界面结构：屏幕、对象与样式

本项目界面代码在 `src/ui.cpp` 中，涉及：

- **屏幕与背景色**：`lv_scr_act()`、`lv_obj_set_style_bg_color`。  
- **标签与字体**：`lv_label_create`、`lv_label_set_text`、`lv_obj_set_style_text_font`（Montserrat 系列）。  
- **容器与对齐**：`lv_obj_create`、`lv_obj_align`、`lv_obj_align_to`，用于重量数字与单位「g」的相对布局。  
- **进度条**：`lv_bar_create` 用于启动阶段简单加载反馈。  
- **条件重绘**：`update_weight_display` 中对变化小于阈值的重量跳过刷新，减轻 SPI 与 CPU 负担。

**学习小结**：LVGL 的界面是**树状对象**；样式可逐对象设置。小屏项目尤应注意**字号与边距**，避免信息溢出或拥挤。

### 3.7 与业务逻辑的衔接

校准与称重状态由 `Scale` 与 `main.cpp` 状态机驱动；界面通过 `lv_obj_clean` 切换「主界面 / 校准界面」，并用 `update_calibration_display` 等函数更新文案。笔者体会：**界面层应尽量只接收「已处理好的显示数据」**，避免在 UI 文件中直接依赖硬件细节，有利于单独测试与后期替换控件。

---

## 4. 构建与烧录

环境要求：已安装 [PlatformIO](https://platformio.org/)（或 VS Code / Cursor 的 PlatformIO 扩展）。

```bash
cd esp32-lvgl-test
pio run -t upload
pio device monitor
```

首次编译会解析 `lib_deps` 与 `lib/` 下本地库；具体串口端口依本机环境在 `platformio.ini` 中配置（若需可添加 `upload_port` 等）。

---

## 5. 目录结构（摘录）

| 路径 | 说明 |
|------|------|
| `src/main.cpp` | 初始化 TFT、LVGL、外设；主循环；flush 与 tick。 |
| `src/ui.cpp` / `ui.h` | LVGL 界面与刷新逻辑。 |
| `include/lv_conf.h` | LVGL 全局配置。 |
| `platformio.ini` | 编译选项、TFT_eSPI 引脚宏、字体宏。 |
| `lib/lvgl-9.4.0` | LVGL 源码（版本以目录为准）。 |

---
