#include "coffee_webserver.h"
#include "webserver_html.h"

CoffeeWebServer::CoffeeWebServer(Scale* scale, Timer* timer) 
  : scale(scale), timer(timer), coffeeWeight(0.0), targetWater(0.0), brewMethod("Japanese"), currentWeight(0.0) {
}

void CoffeeWebServer::begin() {
  // 创建WiFi热点
  Serial.println("Setting up WiFi Access Point...");
  WiFi.mode(WIFI_AP);
  WiFi.softAP(WIFI_AP_SSID, WIFI_AP_PASSWORD, WIFI_AP_CHANNEL, false, WIFI_AP_MAX_CONN);
  
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  Serial.print("SSID: ");
  Serial.println(WIFI_AP_SSID);
  Serial.print("Password: ");
  Serial.println(WIFI_AP_PASSWORD);
  
  // 设置Web服务器路由
  server.on("/", HTTP_GET, [this]() { this->handleRoot(); });
  server.on("/api/weight", HTTP_GET, [this]() { this->handleWeight(); });
  server.on("/api/tare", HTTP_POST, [this]() { this->handleTare(); });
  server.on("/api/timer", HTTP_GET, [this]() { this->handleTimer(); });
  server.on("/api/timer/start", HTTP_POST, [this]() { this->handleTimerStart(); });
  server.on("/api/timer/pause", HTTP_POST, [this]() { this->handleTimerPause(); });
  server.on("/api/timer/stop", HTTP_POST, [this]() { this->handleTimerStop(); });
  server.on("/api/timer/reset", HTTP_POST, [this]() { this->handleTimerReset(); });
  server.on("/api/timer/segment", HTTP_POST, [this]() { this->handleTimerSegment(); });
  server.on("/api/timer/segments", HTTP_GET, [this]() { this->handleTimerSegments(); });
  server.on("/api/setCoffeeWeight", HTTP_POST, [this]() { this->handleSetCoffeeWeight(); });
  server.on("/api/setTargetWater", HTTP_POST, [this]() { this->handleSetTargetWater(); });
  server.on("/api/setBrewMethod", HTTP_POST, [this]() { this->handleSetBrewMethod(); });
  server.onNotFound([this]() { this->handleNotFound(); });
  
  // 启动服务器
  server.begin();
  Serial.println("Web server started");
}

void CoffeeWebServer::handleClient() {
  server.handleClient();
}

void CoffeeWebServer::handleRoot() {
  String html = String(HTML_HEAD) + String(HTML_SCRIPT);
  server.send(200, "text/html", html);
}

void CoffeeWebServer::updateWeight(float weight) {
  currentWeight = weight;
}

void CoffeeWebServer::handleWeight() {
  // 使用缓存的重量值，避免频繁访问HX711导致读取失败
  float weight = currentWeight;
  float ratio = 0.0;
  if (coffeeWeight > 0) {
    ratio = weight / coffeeWeight;
  }
  
  // 与屏幕显示一致：重量显示1位小数
  String json = "{";
  json += "\"weight\":" + String(weight, 1) + ",";
  json += "\"coffeeWeight\":" + String(coffeeWeight, 1) + ",";
  json += "\"ratio\":" + String(ratio, 2);
  json += "}";
  
  server.send(200, "application/json", json);
}

void CoffeeWebServer::handleTare() {
  scale->tare();
  String json = "{\"success\":true}";
  server.send(200, "application/json", json);
}

void CoffeeWebServer::handleTimer() {
  String timeStr = timer->getFormattedTimeWithMs();  // 使用带毫秒的格式
  TimerState state = timer->getState();
  
  String json = "{";
  json += "\"time\":\"" + timeStr + "\",";
  json += "\"state\":" + String(state) + ",";
  json += "\"segmentCount\":" + String(timer->getSegmentCount());
  json += "}";
  
  server.send(200, "application/json", json);
}

void CoffeeWebServer::handleTimerStart() {
  timer->start();
  String json = "{\"success\":true}";
  server.send(200, "application/json", json);
}

void CoffeeWebServer::handleTimerPause() {
  timer->pause();
  String json = "{\"success\":true}";
  server.send(200, "application/json", json);
}

void CoffeeWebServer::handleTimerStop() {
  timer->stop();
  String json = "{\"success\":true}";
  server.send(200, "application/json", json);
}

void CoffeeWebServer::handleTimerReset() {
  timer->reset();
  String json = "{\"success\":true}";
  server.send(200, "application/json", json);
}

void CoffeeWebServer::handleTimerSegment() {
  if (server.hasArg("name")) {
    String name = server.arg("name");
    if (name.length() > 0) {
      timer->addSegment(name);
      String json = "{\"success\":true}";
      server.send(200, "application/json", json);
      return;
    }
  }
  String json = "{\"success\":false,\"error\":\"Missing segment name\"}";
  server.send(400, "application/json", json);
}

void CoffeeWebServer::handleTimerSegments() {
  String json = "{";
  json += "\"count\":" + String(timer->getSegmentCount()) + ",";
  json += "\"segments\":[";
  
  for (int i = 0; i < timer->getSegmentCount(); i++) {
    if (i > 0) json += ",";
    TimerSegment seg = timer->getSegment(i);
    json += "{";
    json += "\"name\":\"" + seg.name + "\",";
    json += "\"time\":\"" + formatSegmentTime(seg.duration) + "\"";
    json += "}";
  }
  
  json += "]}";
  server.send(200, "application/json", json);
}

void CoffeeWebServer::handleSetCoffeeWeight() {
  if (server.hasArg("weight")) {
    float weight = server.arg("weight").toFloat();
    if (weight > 0) {
      coffeeWeight = weight;
      String json = "{\"success\":true}";
      server.send(200, "application/json", json);
      return;
    }
  }
  String json = "{\"success\":false,\"error\":\"Invalid weight\"}";
  server.send(400, "application/json", json);
}

void CoffeeWebServer::handleSetTargetWater() {
  if (server.hasArg("weight")) {
    float weight = server.arg("weight").toFloat();
    if (weight > 0) {
      targetWater = weight;
      String json = "{\"success\":true}";
      server.send(200, "application/json", json);
      return;
    }
  }
  String json = "{\"success\":false,\"error\":\"Invalid weight\"}";
  server.send(400, "application/json", json);
}

void CoffeeWebServer::handleSetBrewMethod() {
  if (server.hasArg("method")) {
    brewMethod = server.arg("method");
    String json = "{\"success\":true}";
    server.send(200, "application/json", json);
    return;
  }
  String json = "{\"success\":false,\"error\":\"Missing method\"}";
  server.send(400, "application/json", json);
}

void CoffeeWebServer::handleNotFound() {
  server.send(404, "text/plain", "Not Found");
}

String CoffeeWebServer::formatSegmentTime(unsigned long milliseconds) {
  unsigned long totalSeconds = milliseconds / 1000;
  unsigned long minutes = totalSeconds / 60;
  unsigned long seconds = totalSeconds % 60;
  unsigned long ms = milliseconds % 1000;
  
  char buffer[13];
  snprintf(buffer, sizeof(buffer), "%02lu:%02lu.%03lu", minutes, seconds, ms);
  return String(buffer);
}

