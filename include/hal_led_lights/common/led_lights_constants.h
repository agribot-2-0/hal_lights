/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once
#include <cstdint>
#include <string>

namespace fade_candy_features
{
constexpr std::uint16_t pixels_per_contact = 64;
constexpr std::uint16_t max_pixels_number = 512;
constexpr std::uint8_t led_off = 0x00;
constexpr std::uint8_t intensity_scale = 100;
}  // namespace fade_candy_features

enum class FadeCandyDataLinePixelIndex
{
  data_line_0 = 0 * fade_candy_features::pixels_per_contact,
  data_line_1 = 1 * fade_candy_features::pixels_per_contact,
  data_line_2 = 2 * fade_candy_features::pixels_per_contact,
  data_line_3 = 3 * fade_candy_features::pixels_per_contact,
  data_line_4 = 4 * fade_candy_features::pixels_per_contact,
  data_line_5 = 5 * fade_candy_features::pixels_per_contact,
  data_line_6 = 6 * fade_candy_features::pixels_per_contact,
  data_line_7 = 7 * fade_candy_features::pixels_per_contact
};

// the content of this enum must be updated with the lamp control service file constants
enum class Color
{
  black,
  red,
  green,
  blue,
  yellow,
  white,
  orange,
  light_blue,
  purple,
  color_count
};

struct LampConfiguration
{
  std::string lamp_name_;
  Color color_;
  std::uint8_t intensity_;
  std::uint16_t periodFlashing_;
  std::uint16_t periodFading_;

  LampConfiguration()
  {
    lamp_name_ = "";
    color_ = Color::black;
    intensity_ = 0;
    periodFlashing_ = 0;
    periodFading_ = 0;
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
  std::uint16_t first_pixel_index_;
  std::uint16_t last_pixel_index_;
};

struct ColorInRgb
{
  std::uint8_t red_ = 0;
  std::uint8_t green_ = 0;
  std::uint8_t blue_ = 0;

  ColorInRgb()
  {
    red_ = 0x00;
    green_ = 0x00;
    blue_ = 0x00;
  }

  ColorInRgb(std::uint8_t r, std::uint8_t g, std::uint8_t b)
  {
    red_ = r;
    green_ = g;
    blue_ = b;
  }

  void setColorRgb(const Color color)
  {
    switch (color)
    {
      case Color::black:
        red_ = 0x00;
        green_ = 0x00;
        blue_ = 0x00;
        break;
      case Color::red:
        red_ = 0xFF;
        green_ = 0x00;
        blue_ = 0x00;
        break;
      case Color::green:
        red_ = 0x00;
        green_ = 0xFF;
        blue_ = 0x00;
        break;
      case Color::blue:
        red_ = 0x00;
        green_ = 0x00;
        blue_ = 0xFF;
        break;
      case Color::yellow:
        red_ = 0xFF;
        green_ = 0xFF;
        blue_ = 0x00;
        break;
      case Color::white:
        red_ = 0xFF;
        green_ = 0xFF;
        blue_ = 0xFF;
        break;
      case Color::orange:
        red_ = 0xFF;
        green_ = 0x8C;
        blue_ = 0x00;
        break;
      case Color::light_blue:
        red_ = 0x00;
        green_ = 0xFF;
        blue_ = 0xFF;
        break;
      case Color::purple:
        red_ = 0xFF;
        green_ = 0x00;
        blue_ = 0xFF;
        break;
      default:
        red_ = 0x00;
        green_ = 0x00;
        blue_ = 0x00;
        break;
    }
  }

  void setIntensity(uint8_t intensity)
  {
    if (intensity <= fade_candy_features::intensity_scale)
    {
      red_ = static_cast<uint8_t>(static_cast<float>(red_) * (static_cast<float>(intensity) /
                                                            static_cast<float>(fade_candy_features::intensity_scale)));
      green_ =
          static_cast<uint8_t>(static_cast<float>(green_) * (static_cast<float>(intensity) /
                                                            static_cast<float>(fade_candy_features::intensity_scale)));
      blue_ =
          static_cast<uint8_t>(static_cast<float>(blue_) * (static_cast<float>(intensity) /
                                                           static_cast<float>(fade_candy_features::intensity_scale)));
    }
    else
    {
      red_ = 0;
      green_ = 0;
      blue_ = 0;
    }
  }
};
