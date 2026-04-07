#include <Arduino.h>
#include "scale.h"

Scale::Scale() : ready(false), offset(DEFAULT_OFFSET), calibrationFactor(CALIBRATION_FACTOR), 
                 calibState(CALIB_NONE), calibStartTime(0),
                 calibReading1(0), calibReading2(0), calibReading3(0) {
}

void Scale::init() {
  scale.begin(HX711_DT, HX711_SCK);
  
  // 等待 HX711 稳定
  delay(500);
  
  // 使用已校准的默认值（不再重新校准零点）
  // 如果需要重新校准，可以按下按钮启动校准程序
  ready = true;
}

void Scale::calibrate() {
  // 读取零点（空载时的读数）
  long sum = 0;
  for (int i = 0; i < SCALE_READINGS; i++) {
    if (scale.is_ready()) {
      sum += scale.read();
    }
    delay(SCALE_DELAY);
  }
  offset = sum / SCALE_READINGS;
}

bool Scale::isReady() {
  return ready && scale.is_ready();
}

float Scale::getWeight() {
  if (!isReady()) {
    return 0.0;
  }
  
  // 读取原始值
  long reading = scale.read();
  
  // 计算重量（减去零点偏移，应用校准系数）
  float weight = (reading - offset) / calibrationFactor;
  
  // 确保重量不为负（空载时显示0）
  if (weight < 0) {
    weight = 0;
  }
  
  // 重量小于1g时显示0
  if (weight < 1.0) {
    weight = 0;
  }
  
  // 限制最大显示重量
  if (weight > MAX_WEIGHT) {
    weight = MAX_WEIGHT;
  }
  
  return weight;
}

void Scale::tare() {
  // 读取当前值作为新的零点
  long sum = 0;
  int count = 0;
  for (int i = 0; i < SCALE_READINGS; i++) {
    if (scale.is_ready()) {
      sum += scale.read();
      count++;
    }
    delay(SCALE_DELAY);
  }
  if (count > 0) {
    offset = sum / count;
    Serial.println("Tare completed - new offset: " + String(offset));
  }
}

void Scale::startCalibration() {
  calibState = CALIB_STEP1_EMPTY;
  calibStartTime = millis();
  Serial.println("=== 开始校准 ===");
  Serial.println("步骤1: 请确保称重器为空，等待10秒稳定...");
}

bool Scale::isCalibrating() {
  return calibState != CALIB_NONE && calibState != CALIB_COMPLETE;
}

CalibrationState Scale::getCalibrationState() {
  return calibState;
}

long Scale::readStableValue() {
  // 读取多次取平均值
  long sum = 0;
  int count = 0;
  for (int i = 0; i < CALIBRATION_READINGS; i++) {
    if (scale.is_ready()) {
      sum += scale.read();
      count++;
    }
    delay(SCALE_DELAY);
  }
  return count > 0 ? sum / count : 0;
}

void Scale::updateCalibration() {
  if (calibState == CALIB_NONE || calibState == CALIB_COMPLETE) {
    return;
  }
  
  unsigned long currentTime = millis();
  unsigned long elapsed = currentTime - calibStartTime;
  
  switch (calibState) {
    case CALIB_STEP1_EMPTY:
      // 等待10秒后读取空载值
      if (elapsed >= CALIBRATION_STABLE_TIME) {
        calibReading1 = readStableValue();
        Serial.print("步骤1完成 - 空载读数: ");
        Serial.println(calibReading1);
        calibState = CALIB_STEP2_WEIGHT;
        calibStartTime = currentTime;
        Serial.println("步骤2: 请放置200g砝码，等待10秒稳定...");
      }
      break;
      
    case CALIB_STEP2_WEIGHT:
      // 等待10秒后读取200g砝码值
      if (elapsed >= CALIBRATION_STABLE_TIME) {
        calibReading2 = readStableValue();
        Serial.print("步骤2完成 - 200g砝码读数: ");
        Serial.println(calibReading2);
        calibState = CALIB_STEP3_EMPTY2;
        calibStartTime = currentTime;
        Serial.println("步骤3: 请拿开砝码，等待10秒稳定...");
      }
      break;
      
    case CALIB_STEP3_EMPTY2:
      // 等待10秒后读取第二次空载值
      if (elapsed >= CALIBRATION_STABLE_TIME) {
        calibReading3 = readStableValue();
        Serial.print("步骤3完成 - 再次空载读数: ");
        Serial.println(calibReading3);
        
        // 计算校准系数
        // 使用两次空载的平均值作为零点
        long avgEmpty = (calibReading1 + calibReading3) / 2;
        offset = avgEmpty;
        
        // 计算校准系数：砝码重量 / (砝码读数 - 零点)
        long weightDiff = calibReading2 - avgEmpty;
        if (weightDiff != 0) {
          calibrationFactor = (float)weightDiff / CALIBRATION_WEIGHT;
        }
        
        calibState = CALIB_COMPLETE;
        
        // 打印校准结果
        Serial.println("\n=== 校准完成 ===");
        Serial.print("零点偏移 (offset): ");
        Serial.println(offset);
        Serial.print("校准系数 (calibrationFactor): ");
        Serial.println(calibrationFactor, 6);
        Serial.print("空载读数1: ");
        Serial.println(calibReading1);
        Serial.print("200g砝码读数: ");
        Serial.println(calibReading2);
        Serial.print("空载读数2: ");
        Serial.println(calibReading3);
        Serial.print("平均空载读数: ");
        Serial.println(avgEmpty);
        Serial.print("重量差值: ");
        Serial.println(weightDiff);
        Serial.println("================\n");
      }
      break;
      
    default:
      break;
  }
}

float Scale::getCalibrationFactor() {
  return calibrationFactor;
}

long Scale::getOffset() {
  return offset;
}

