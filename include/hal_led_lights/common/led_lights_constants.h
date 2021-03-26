/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once
#include <cstdint>
#include <string>

namespace fade_candy_features
{
constexpr uint16_t pixels_per_contact = 64;
constexpr uint16_t max_pixels_number = 512;
constexpr std::uint8_t led_off = 0x00;
constexpr uint8_t intensity_scale = 100;
}  // namespace fade_candy_features

enum class FadeCandyContactPixelIndex
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
enum class Color
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

struct LampConfiguration
{
  std::string lamp_name;
  Color color;
  uint8_t intensity;
  uint16_t periodFlashing;
  uint16_t periodFading;

  LampConfiguration()
  {
    lamp_name = "";
    color = Color::black;
    intensity = 100;
    periodFlashing = 0;
    periodFading = 0;
  }
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

struct ColorInRgb
{
  std::uint8_t red = 0;
  std::uint8_t green = 0;
  std::uint8_t blue = 0;

  ColorInRgb()
  {
    red = 0x00;
    green = 0x00;
    blue = 0x00;
  }

  ColorInRgb(std::uint8_t r, std::uint8_t g, std::uint8_t b)
  {
    red = r;
    green = g;
    blue = b;
  }

  void setColorRgb(const Color color)
  {
    switch (color)
    {
      case Color::black:
        red = 0x00;
        green = 0x00;
        blue = 0x00;
        break;
      case Color::red:
        red = 0xFF;
        green = 0x00;
        blue = 0x00;
        break;
      case Color::green:
        red = 0x00;
        green = 0xFF;
        blue = 0x00;
        break;
      case Color::blue:
        red = 0x00;
        green = 0x00;
        blue = 0xFF;
        break;
      case Color::yellow:
        red = 0xFF;
        green = 0xFF;
        blue = 0x00;
        break;
      case Color::white:
        red = 0xFF;
        green = 0xFF;
        blue = 0xFF;
        break;
      case Color::orange:
        red = 0xFF;
        green = 0x8C;
        blue = 0x00;
        break;
      case Color::light_blue:
        red = 0x00;
        green = 0xFF;
        blue = 0xFF;
        break;
      case Color::purple:
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

  void setIntensity(uint8_t intensity)
  {
    if (intensity <= 100)
    {
      red = static_cast<uint8_t>(static_cast<float>(red) * (static_cast<float>(intensity) /
                                                            static_cast<float>(fade_candy_features::intensity_scale)));
      green =
          static_cast<uint8_t>(static_cast<float>(green) * (static_cast<float>(intensity) /
                                                            static_cast<float>(fade_candy_features::intensity_scale)));
      blue =
          static_cast<uint8_t>(static_cast<float>(blue) * (static_cast<float>(intensity) /
                                                           static_cast<float>(fade_candy_features::intensity_scale)));
    }
    else
    {
      red = 0;
      green = 0;
      blue = 0;
    }
  }
};
