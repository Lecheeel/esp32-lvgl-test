#ifndef TIMER_H
#define TIMER_H

#include <Arduino.h>

// 计时器状态枚举
enum TimerState {
  TIMER_STOPPED,   // 已停止
  TIMER_RUNNING,   // 运行中
  TIMER_PAUSED     // 已暂停
};

// 分段计时结构
struct TimerSegment {
  String name;           // 分段名称
  unsigned long startTime;  // 开始时间（毫秒）
  unsigned long duration;   // 持续时间（毫秒）
};

// 计时器类
class Timer {
public:
  Timer();
  void start();           // 开始计时
  void pause();           // 暂停计时
  void resume();          // 恢复计时
  void stop();            // 停止计时
  void reset();           // 重置计时器
  void addSegment(const String& name);  // 添加分段
  
  TimerState getState() const;
  unsigned long getElapsedTime() const;  // 获取已用时间（毫秒）
  String getFormattedTime() const;        // 获取格式化的时间字符串 (MM:SS)
  String getFormattedTimeWithMs() const;  // 获取格式化的时间字符串，包含毫秒 (MM:SS.mmm)
  int getSegmentCount() const;
  TimerSegment getSegment(int index) const;
  
private:
  TimerState state;
  unsigned long startTime;      // 开始时间
  unsigned long pausedTime;     // 暂停时累计的时间
  unsigned long lastPauseTime;  // 上次暂停的时间
  
  static const int MAX_SEGMENTS = 20;
  TimerSegment segments[MAX_SEGMENTS];
  int segmentCount;
  unsigned long segmentStartTime;  // 当前分段的开始时间
};

#endif // TIMER_H

