/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once

#include <hal_lights/common/lights_constants.h>
#include <cstdint>
#include <chrono>  // NOLINT [build/c++11]
#include <iostream>

class Lamp
{
  Color color_;
  uint8_t intensity_;
  Color faded_color_;
  uint64_t last_flash_time_;
  bool flash_togle_;
  uint16_t flashing_period_ms_;
  uint64_t last_fading_time_;
  uint16_t fading_period_ms_;
  LedSegment led_segment_;

  inline bool checkIfTimerOverflown();
  inline void flashToggle();
  bool checkFlashState();
  uint8_t getFadingMultiplier();
  uint64_t inline getTimeInMiliseconds();

public:
  explicit Lamp(const LedSegment& lamp_leds);
  void setLedsRGBvalues(ColorInRgb* rgb_color);
  void applyFading(ColorInRgb* rgb_to_fade, const Color color);

  void setIntensity(const uint8_t new_intensity);
  void setColor(const Color color);
  void setFlashingPeriod(const uint16_t period);
  void setFadingPeriod(const uint16_t period);
  void setLampSegment(const LedSegment& new_led_segment);

  uint16_t getFlashingPeriod() const;
  uint16_t getFadingPeriod() const;
  Color getColor() const;
  LedSegment getLampSegment() const;
};
