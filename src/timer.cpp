#include "timer.h"

Timer::Timer() : state(TIMER_STOPPED), startTime(0), pausedTime(0), 
                 lastPauseTime(0), segmentCount(0), segmentStartTime(0) {
}

void Timer::start() {
  if (state == TIMER_STOPPED) {
    // 重新开始
    startTime = millis();
    pausedTime = 0;
    segmentCount = 0;
    segmentStartTime = startTime;
  } else if (state == TIMER_PAUSED) {
    // 从暂停恢复（等同于resume）
    resume();
    return;
  }
  state = TIMER_RUNNING;
}

void Timer::pause() {
  if (state == TIMER_RUNNING) {
    lastPauseTime = millis();
    // 如果有正在进行的分段，需要保存暂停时的segmentStartTime
    // 这样恢复时可以正确计算分段持续时间
    state = TIMER_PAUSED;
  }
}

void Timer::resume() {
  if (state == TIMER_PAUSED) {
    unsigned long pauseDuration = millis() - lastPauseTime;
    pausedTime += pauseDuration;
    // 调整segmentStartTime，补偿暂停的时间
    if (segmentStartTime > 0) {
      segmentStartTime += pauseDuration;
    }
    state = TIMER_RUNNING;
  }
}

void Timer::stop() {
  state = TIMER_STOPPED;
  startTime = 0;
  pausedTime = 0;
  lastPauseTime = 0;
  segmentStartTime = 0;
}

void Timer::reset() {
  stop();
  segmentCount = 0;
}

void Timer::addSegment(const String& name) {
  if (state == TIMER_RUNNING && segmentCount < MAX_SEGMENTS) {
    unsigned long currentTime = millis();
    unsigned long segmentDuration = 0;
    
    if (segmentCount > 0) {
      // 计算上一个分段的持续时间
      segmentDuration = currentTime - segmentStartTime;
      segments[segmentCount - 1].duration = segmentDuration;
    }
    
    // 添加新分段
    segments[segmentCount].name = name;
    segments[segmentCount].startTime = currentTime - startTime - pausedTime;
    segments[segmentCount].duration = 0;  // 当前分段还未结束
    segmentStartTime = currentTime;
    segmentCount++;
  }
}

TimerState Timer::getState() const {
  return state;
}

unsigned long Timer::getElapsedTime() const {
  if (state == TIMER_STOPPED) {
    return 0;
  } else if (state == TIMER_PAUSED) {
    return lastPauseTime - startTime - pausedTime;
  } else {
    // TIMER_RUNNING
    return millis() - startTime - pausedTime;
  }
}

String Timer::getFormattedTime() const {
  unsigned long elapsed = getElapsedTime();
  unsigned long seconds = elapsed / 1000;
  unsigned long minutes = seconds / 60;
  seconds = seconds % 60;
  
  char buffer[10];
  snprintf(buffer, sizeof(buffer), "%02lu:%02lu", minutes, seconds);
  return String(buffer);
}

String Timer::getFormattedTimeWithMs() const {
  unsigned long elapsed = getElapsedTime();
  unsigned long totalSeconds = elapsed / 1000;
  unsigned long minutes = totalSeconds / 60;
  unsigned long seconds = totalSeconds % 60;
  unsigned long milliseconds = elapsed % 1000;
  
  char buffer[13];
  snprintf(buffer, sizeof(buffer), "%02lu:%02lu.%03lu", minutes, seconds, milliseconds);
  return String(buffer);
}

int Timer::getSegmentCount() const {
  return segmentCount;
}

TimerSegment Timer::getSegment(int index) const {
  if (index >= 0 && index < segmentCount) {
    TimerSegment seg = segments[index];
    // 如果是最后一个分段且计时器正在运行，更新持续时间
    if (index == segmentCount - 1 && state == TIMER_RUNNING && segmentStartTime > 0) {
      seg.duration = millis() - segmentStartTime;
    }
    return seg;
  }
  TimerSegment empty;
  empty.name = "";
  empty.startTime = 0;
  empty.duration = 0;
  return empty;
}

