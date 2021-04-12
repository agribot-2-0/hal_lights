/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include <hal_lights/lights_controller/lights_controller.h>

using std::uint16_t;
using std::uint8_t;

LightsController::LightsController()
{
  // maximum number of leds for one fadecandy
  for (std::size_t pixel_iterator = 0; pixel_iterator < fade_candy_features::max_pixels_number; pixel_iterator++)
  {
    leds_state_.push_back(std::make_unique<ColorInRgb>(fade_candy_features::led_off, fade_candy_features::led_off,
                                                       fade_candy_features::led_off));
  }
}

bool LightsController::connectWithOpcServer()
{
  return opc_client_.initOpcConnection();
}

ColorInRgb LightsController::getPixelValue(const uint8_t connector, const uint8_t pixel) const
{
  return *leds_state_[connector * fade_candy_features::pixels_per_data_line + pixel];
}

void LightsController::changePixel(const uint8_t connector, const uint8_t pixel, const ColorInRgb& pixel_value)
{
  (leds_state_[connector * fade_candy_features::pixels_per_data_line + pixel])->red_ = pixel_value.red_;
  (leds_state_[connector * fade_candy_features::pixels_per_data_line + pixel])->green_ = pixel_value.green_;
  (leds_state_[connector * fade_candy_features::pixels_per_data_line + pixel])->blue_ = pixel_value.blue_;
}

bool LightsController::addLamp(const std::string& lamp_name, const uint16_t start_led, const uint16_t stop_led)
{
  // check if lamp with such a name already exists
  if (lamps_.find(lamp_name) != lamps_.end())
  {
    throw std::out_of_range("Lamp name: " + lamp_name + " already exists!");
  }
  // check if proper led indexing is achieved
  if (start_led > stop_led)
  {
    throw std::invalid_argument("Lamp name: " + lamp_name + " Start led number must be lower than the stop led value!");
  }
  // if both conditions are good then create a led segment for this lamp
  LedSegment led_segment = { start_led, stop_led };
  LedSegment temporary;
  // then check if any lamp have the same sets of leds
  for (const auto& [key, lamp] : lamps_)
  {
    temporary = lamp->getLampSegment();
    // check if lamps pixels don't overlap
    if (led_segment.last_pixel_index_ >= temporary.first_pixel_index_)
    {
      if (temporary.last_pixel_index_ >= led_segment.first_pixel_index_)
      {
        throw std::invalid_argument(
            "Lamp name: " + lamp_name + " " + std::to_string(led_segment.first_pixel_index_) + " " +
            std::to_string(led_segment.last_pixel_index_) + " Lamps leds overlap with lamp: " + key + " " +
            std::to_string(temporary.first_pixel_index_) + " " + std::to_string(led_segment.first_pixel_index_));
      }
    }
  }
  // if they don't overlap, then add lamp to the unordered map
  lamps_[lamp_name] = std::make_unique<Lamp>(led_segment);
  return true;
}

void LightsController::changeEveryLampConfiguration(const LampConfiguration& config)
{
  for (auto& [key, lamp] : lamps_)
  {
    static_cast<void>(key);
    lamp->setColor(config.color_);
    lamp->setIntensity(config.intensity_);
    lamp->setFlashingPeriod(config.period_flashing_);
    lamp->setFadingPeriod(config.period_fading_);
  }
}

bool LightsController::changeLampConfiguration(const LampConfiguration& config)
{
  auto lamp_iterator = lamps_.find(config.lamp_name_);
  if (lamp_iterator == lamps_.end())
  {
    throw std::out_of_range("Lamp name: " + config.lamp_name_ + " doesnt exist!");
    return false;
  }
  lamp_iterator->second->setColor(config.color_);
  lamp_iterator->second->setIntensity(config.intensity_);
  lamp_iterator->second->setFlashingPeriod(config.period_flashing_);
  lamp_iterator->second->setFadingPeriod(config.period_fading_);
  return true;
}

std::string LightsController::debugPrintLampConfigurations()
{
  std::string printed_msg = "";
  for (const auto& [key, lamp] : lamps_)
  {
    printed_msg = printed_msg + "Lamp name: " + key + "\n";
    printed_msg =
        printed_msg + "First pixel: " + std::to_string(static_cast<int>(lamp->getLampSegment().first_pixel_index_));
    printed_msg = printed_msg +
                  " Last pixel: " + std::to_string(static_cast<int>(lamp->getLampSegment().last_pixel_index_)) + "\n";
    printed_msg = printed_msg + "Color: " + std::to_string(static_cast<int>(lamp->getColor())) + "\n";
    printed_msg = printed_msg + "Fading period: " + std::to_string(static_cast<int>(lamp->getFadingPeriod())) + "\n";
    printed_msg =
        printed_msg + "Flashing period: " + std::to_string(static_cast<int>(lamp->getFlashingPeriod())) + "\n";
  }
  return printed_msg;
}

void LightsController::setLedsStates()
{
  for (auto& [key, lamp] : lamps_)
  {
    static_cast<void>(key);
    changeLampLedsColor(lamp);
  }
}

void LightsController::changeLampLedsColor(const std::unique_ptr<Lamp> &lamp)
{
  uint16_t lamp_leds_base_index = lamp->getLampSegment().first_pixel_index_;
  uint16_t lamp_leds_last_index = lamp->getLampSegment().last_pixel_index_;
  ColorInRgb color_rgb;
  lamp->setLedsRGBvalues(&color_rgb);
  for (std::size_t led_position = lamp_leds_base_index; led_position < lamp_leds_last_index; led_position++)
  {
    leds_state_[led_position]->red_ = color_rgb.red_;
    leds_state_[led_position]->green_ = color_rgb.green_;
    leds_state_[led_position]->blue_ = color_rgb.blue_;
  }
}

bool LightsController::execute()
{
  setLedsStates();
  opc_client_.setPixelsData(leds_state_);
  return opc_client_.sendPixelsData();
}
