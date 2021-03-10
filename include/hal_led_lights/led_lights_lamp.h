#ifndef HAL_LED_LIGHTS_LED_LIGHTS_LAMP_H_
#define HAL_LED_LIGHTS_LED_LIGHTS_LAMP_H_

/*
 * Copyright (C) 2020 Agribot. All rights reserved.
 */

#include <hal_led_lights/led_lights_constants.h>
#include <cstdint>
#include <chrono> // NOLINT [build/c++11]
#include <iostream>

class LedLightsLamp
{
  colorTypes color_;
  float intensity_;
  colorTypes faded_color_;
  uint64_t last_flash_time_;
  bool flash_togle_;
  uint16_t flashing_period_ms_;
  uint64_t last_fading_time_;
  uint16_t fading_period_ms_;
  LedSegment lamp_pixels_;

  inline bool checkIfTimerOverflown();
  bool checkFlashTimer();
  inline void flashToggle();
  bool checkFlashState();
  float checkFadingMultiplier();
  uint64_t inline returnTimeInMiliseconds();

public:
  explicit LedLightsLamp(const LedSegment& lamp_leds);
  void setLedsRGBvalues(colorInRGB* rgb_color);
  void applyFading(colorInRGB* rgb_to_fade, const colorTypes color);

  void setIntensity(const uint8_t new_intensity);
  void setColor(const colorTypes color);
  void setFlashingPeriod(const uint16_t period);
  void setFadingPeriod(const uint16_t period);
  void setLampPixels(const LedSegment& new_led_segmentlor);

  uint16_t returnFlashingPeriod() const;
  uint16_t returnFadingPeriod() const;
  colorTypes returnColor() const;
  LedSegment returnLampPixels() const;
};

#endif  // HAL_LED_LIGHTS_LED_LIGHTS_LAMP_H_
