/*
 * Copyright (C) 2020 Agribot. All rights reserved.
 */

#ifndef HAL_LED_LIGHTS_LED_LIGHTS_CONSTANTS_H_
#define HAL_LED_LIGHTS_LED_LIGHTS_CONSTANTS_H_
#include <cstdint>

constexpr uint16_t pixels_per_contact = 64;
constexpr uint16_t max_pixels_number = 512;
constexpr std::uint8_t led_off = 0x00;
constexpr float intensity_scale = 100.0;
constexpr float max_intensity = 1.0;

enum startingPixelsIndexes : uint16_t
{
  contact_0 = 0,
  contact_1 = 64,
  contact_2 = 128,
  contact_3 = 192,
  contact_4 = 256,
  contact_5 = 320,
  contact_6 = 384,
  contact_7 = 448
};

// the content of this enum must be updated with the lamp control service file constants
enum colorTypes : std::uint8_t
{
  black = 0,
  red = 1,
  green = 2,
  blue = 3,
  yellow = 4,
  white = 5,
  orange = 6,
  light_blue = 7,
  purple = 8,
  color_count = 9
};

constexpr std::uint8_t channel = 0;
constexpr std::uint8_t pixel_command = 0;
constexpr std::uint16_t frame_data_length = 1536;
constexpr std::uint8_t color_off = 0x00;
constexpr std::uint16_t rgb_bytes_per_pixel = 0x03;
constexpr std::uint16_t frame_base_length = 0x04;
constexpr std::uint16_t red_byte_position = 0;
constexpr std::uint16_t green_byte_position = 1;
constexpr std::uint16_t blue_byte_position = 2;

struct LedSegment
{
  uint16_t first_pixel_index;
  uint16_t last_pixel_index;
};

struct colorInRGB
{
  std::uint8_t red = 0;
  std::uint8_t green = 0;
  std::uint8_t blue = 0;

  colorInRGB()
  {
    red = 0x00;
    green = 0x00;
    blue = 0x00;
  }

  colorInRGB(std::uint8_t r, std::uint8_t g, std::uint8_t b)
  {
    red = r;
    green = g;
    blue = b;
  }

  void setColorRGB(const colorTypes color)
  {
    switch (color)
    {
      case colorTypes::black:
        red = 0x00;
        green = 0x00;
        blue = 0x00;
        break;
      case colorTypes::red:
        red = 0xFF;
        green = 0x00;
        blue = 0x00;
        break;
      case colorTypes::green:
        red = 0x00;
        green = 0xFF;
        blue = 0x00;
        break;
      case colorTypes::blue:
        red = 0x00;
        green = 0x00;
        blue = 0xFF;
        break;
      case colorTypes::yellow:
        red = 0xFF;
        green = 0xFF;
        blue = 0x00;
        break;
      case colorTypes::white:
        red = 0xFF;
        green = 0xFF;
        blue = 0xFF;
        break;
      case colorTypes::orange:
        red = 0xFF;
        green = 0x8C;
        blue = 0x00;
        break;
      case colorTypes::light_blue:
        red = 0x00;
        green = 0xFF;
        blue = 0xFF;
        break;
      case colorTypes::purple:
        red = 0xFF;
        green = 0x00;
        blue = 0xFF;
        break;
      default:
        red = 0x00;
        green = 0x00;
        blue = 0x00;
        break;
    }
  }

  void setIntensity(float multiplier)
  {
    red = static_cast<uint8_t>(static_cast<float>(red) * multiplier);
    green = static_cast<uint8_t>(static_cast<float>(green) * multiplier);
    blue = static_cast<uint8_t>(static_cast<float>(blue) * multiplier);
  }
};

#endif  // HAL_LED_LIGHTS_LED_LIGHTS_CONSTANTS_H_
