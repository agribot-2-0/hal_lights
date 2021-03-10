/*
 * Copyright (C) 2020 Agribot. All rights reserved.
 */

#include <hal_led_lights/led_lights_lamp.h>

LedLightsLamp::LedLightsLamp(const LedSegment& pixels_segment)
{
  // lamp off
  color_ = colorTypes::black;
  intensity_ = 1.0;
  flashing_period_ms_ = 0;
  fading_period_ms_ = 0;
  lamp_pixels_ = pixels_segment;
  flash_togle_ = true;
}

inline bool LedLightsLamp::checkIfTimerOverflown()
{
  // get the current time in miliseconds from the epoch
  uint64_t now = returnTimeInMiliseconds();
  if ((now - last_flash_time_) >= flashing_period_ms_)
  {
    last_flash_time_ = now;
    return true;
  }
  else
  {
    return false;
  }
}

bool LedLightsLamp::checkFlashTimer()
{
  if (checkIfTimerOverflown())
  {
    return true;
  }
  else
  {
    return false;
  }
}

inline void LedLightsLamp::flashToggle()
{
  if (flash_togle_)
  {
    flash_togle_ = false;
  }
  else
  {
    flash_togle_ = true;
  }
}

bool LedLightsLamp::checkFlashState()
{
  if (flashing_period_ms_ > 0)
  {
    // if timer overflown, change the current flash state
    if (checkFlashTimer())
    {
      flashToggle();
    }
    return flash_togle_;
  }
  // if flashing period is 0, then lamp is always ON
  else
  {
    return true;
  }
}

void LedLightsLamp::setColor(const colorTypes new_color)
{
  color_ = new_color;
}

void LedLightsLamp::setFlashingPeriod(const uint16_t period)
{
  // reset flash variables
  flash_togle_ = true;
  last_flash_time_ = returnTimeInMiliseconds();
  // set new period
  flashing_period_ms_ = period;
}

void LedLightsLamp::applyFading(colorInRGB* rgb_to_fade, const colorTypes color)
{
  // if the colour request have changed, then start fading timer
  if (faded_color_ != color)
  {
    last_fading_time_ = returnTimeInMiliseconds();
  }
  // if changed to black, then fade the color to black
  if (color == colorTypes::black)
  {
    rgb_to_fade->setIntensity(1 - checkFadingMultiplier());
    faded_color_ = color;
  }
  // if we come back to lamp colour then fadeback to it
  else if (color == color_)
  {
    rgb_to_fade->setIntensity(checkFadingMultiplier());
    faded_color_ = color;
  }
  // if it is not the lamp color, or black color, then just light up the requested colour
  else
  {
    rgb_to_fade->setColorRGB(color);
  }
}

void LedLightsLamp::setLedsRGBvalues(colorInRGB* rgb_color)
{
  rgb_color->setColorRGB(color_);
  // set the colours base intensity
  rgb_color->setIntensity(intensity_);
  if (checkFlashState())
  {
    applyFading(rgb_color, color_);
  }
  else
  {
    applyFading(rgb_color, colorTypes::black);
  }
}

float LedLightsLamp::checkFadingMultiplier()
{
  uint64_t now = returnTimeInMiliseconds();
  if (fading_period_ms_ > 0 && ((now - last_fading_time_) <= fading_period_ms_))
  {
    float multiplier = static_cast<float>(now - last_fading_time_) / static_cast<float>(fading_period_ms_);
    if (multiplier < max_intensity)
    {
      return multiplier;
    }
    else
    {
      return max_intensity;
    }
  }
  else
  {
    return max_intensity;
  }
}

uint64_t inline LedLightsLamp::returnTimeInMiliseconds()
{
  return static_cast<uint64_t>(
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
          .count());
}

void LedLightsLamp::setIntensity(const uint8_t new_intensity)
{
  intensity_ = static_cast<float>(new_intensity / intensity_scale);
}

void LedLightsLamp::setFadingPeriod(const uint16_t period)
{
  last_fading_time_ = returnTimeInMiliseconds();
  fading_period_ms_ = period;
  faded_color_ = color_;
}

void LedLightsLamp::setLampPixels(const LedSegment& new_led_segment)
{
  lamp_pixels_.first_pixel_index = new_led_segment.first_pixel_index;
  lamp_pixels_.last_pixel_index = new_led_segment.last_pixel_index;
}

colorTypes LedLightsLamp::returnColor() const
{
  return color_;
}

LedSegment LedLightsLamp::returnLampPixels() const
{
  return lamp_pixels_;
}

uint16_t LedLightsLamp::returnFlashingPeriod() const
{
  return flashing_period_ms_;
}

uint16_t LedLightsLamp::returnFadingPeriod() const
{
  return fading_period_ms_;
}
