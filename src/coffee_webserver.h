#ifndef COFFEE_WEBSERVER_H
#define COFFEE_WEBSERVER_H

#include <WiFi.h>
#include <WebServer.h>
#include "config.h"
#include "scale.h"
#include "timer.h"

// Web服务器类
class CoffeeWebServer {
public:
  CoffeeWebServer(Scale* scale, Timer* timer);
  void begin();
  void handleClient();
  void updateWeight(float weight);  // 更新缓存的重量值
  
private:
  WebServer server;
  Scale* scale;
  Timer* timer;
  
  // 设置参数
  float coffeeWeight;      // 咖啡粉重量（克）
  float targetWater;       // 目标水量（克）
  String brewMethod;        // 冲煮方法（Japanese/American）
  
  // 缓存当前重量值（由主循环更新，避免频繁访问HX711）
  float currentWeight;
  
  // HTTP处理函数
  void handleRoot();
  void handleWeight();
  void handleTare();
  void handleTimer();
  void handleTimerStart();
  void handleTimerPause();
  void handleTimerStop();
  void handleTimerReset();
  void handleTimerSegment();
  void handleTimerSegments();
  void handleSetCoffeeWeight();
  void handleSetTargetWater();
  void handleSetBrewMethod();
  void handleNotFound();
  
  // 辅助函数
  String formatSegmentTime(unsigned long milliseconds);
};

#endif // COFFEE_WEBSERVER_H

