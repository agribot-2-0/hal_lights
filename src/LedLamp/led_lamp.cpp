/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include <hal_led_lights/LedLamp/led_lamp.h>

LedLamp::LedLamp(const LedSegment& pixels_segment)
{
  // lamp off
  color_ = Color::black;
  intensity_ = 100;
  flashing_period_ms_ = 0;
  fading_period_ms_ = 0;
  lamp_pixels_ = pixels_segment;
  flash_togle_ = true;
}

inline bool LedLamp::checkIfTimerOverflown()
{
  // get the current time in miliseconds from the epoch
  uint64_t now = getTimeInMiliseconds();
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

inline void LedLamp::flashToggle()
{
  flash_togle_ = !flash_togle_;
}

bool LedLamp::checkFlashState()
{
  if (flashing_period_ms_ > 0)
  {
    // if timer overflown, change the current flash state
    if (checkIfTimerOverflown())
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

void LedLamp::setColor(const Color new_color)
{
  color_ = new_color;
}

void LedLamp::setFlashingPeriod(const uint16_t period)
{
  // reset flash variables
  flash_togle_ = true;
  last_flash_time_ = getTimeInMiliseconds();
  // set new period
  flashing_period_ms_ = period;
}

void LedLamp::applyFading(ColorInRgb* rgb_to_fade, const Color color)
{
  // if the color request have changed, then start fading timer
  if (faded_color_ != color)
  {
    last_fading_time_ = getTimeInMiliseconds();
  }
  // if changed to black, then fade the color to black
  if (color == Color::black)
  {
    rgb_to_fade->setIntensity(fade_candy_features::intensity_scale - getFadingMultiplier());
    faded_color_ = color;
  }
  // if we come back to lamp color then fadeback to it
  else if (color == color_)
  {
    rgb_to_fade->setIntensity(getFadingMultiplier());
    faded_color_ = color;
  }
  // if it is not the lamp color, or black color, then just light up the requested colour
  else
  {
    rgb_to_fade->setColorRgb(color);
  }
}

void LedLamp::setLedsRGBvalues(ColorInRgb* rgb_color)
{
  rgb_color->setColorRgb(color_);
  // set the colors base intensity
  rgb_color->setIntensity(intensity_);
  if (checkFlashState())
  {
    applyFading(rgb_color, color_);
  }
  else
  {
    applyFading(rgb_color, Color::black);
  }
}

uint8_t LedLamp::getFadingMultiplier()
{
  uint64_t now = getTimeInMiliseconds();
  if (fading_period_ms_ > 0 && ((now - last_fading_time_) <= fading_period_ms_))
  {
    uint8_t multiplier =
        static_cast<uint8_t>((static_cast<float>(now - last_fading_time_) / static_cast<float>(fading_period_ms_)) *
                             static_cast<float>(fade_candy_features::intensity_scale));
    if (multiplier < fade_candy_features::intensity_scale)
    {
      return multiplier;
    }
    else
    {
      return fade_candy_features::intensity_scale;
    }
  }
  else
  {
    return fade_candy_features::intensity_scale;
  }
}

uint64_t inline LedLamp::getTimeInMiliseconds()
{
  return static_cast<uint64_t>(
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
          .count());
}

void LedLamp::setIntensity(const uint8_t new_intensity)
{
  intensity_ = new_intensity;
}

void LedLamp::setFadingPeriod(const uint16_t period)
{
  last_fading_time_ = getTimeInMiliseconds();
  fading_period_ms_ = period;
  faded_color_ = color_;
}

void LedLamp::setLampPixels(const LedSegment& new_led_segment)
{
  lamp_pixels_ = new_led_segment;
}

Color LedLamp::getColor() const
{
  return color_;
}

LedSegment LedLamp::getLampPixels() const
{
  return lamp_pixels_;
}

uint16_t LedLamp::getFlashingPeriod() const
{
  return flashing_period_ms_;
}

uint16_t LedLamp::getFadingPeriod() const
{
  return fading_period_ms_;
}
