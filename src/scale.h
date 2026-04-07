#ifndef SCALE_H
#define SCALE_H

#include <HX711.h>
#include "config.h"

// 校准状态枚举
enum CalibrationState {
  CALIB_NONE,           // 未在校准
  CALIB_STEP1_EMPTY,    // 步骤1：空载读数
  CALIB_STEP2_WEIGHT,   // 步骤2：200g砝码读数
  CALIB_STEP3_EMPTY2,   // 步骤3：再次空载读数
  CALIB_COMPLETE        // 校准完成
};

// 称重模块类
class Scale {
public:
  Scale();
  void init();
  bool isReady();
  float getWeight();
  void tare();  // 去皮功能
  
  // 校准相关方法
  void startCalibration();
  bool isCalibrating();
  CalibrationState getCalibrationState();
  void updateCalibration();  // 在主循环中调用，更新校准状态
  float getCalibrationFactor();
  long getOffset();
  
private:
  HX711 scale;
  long offset;
  float calibrationFactor;
  bool ready;
  
  // 校准相关
  CalibrationState calibState;
  unsigned long calibStartTime;
  long calibReading1;  // 第一次空载读数
  long calibReading2;  // 200g砝码读数
  long calibReading3;  // 第二次空载读数
  
  void calibrate();
  long readStableValue();  // 读取稳定值
};

#endif // SCALE_H

