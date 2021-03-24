/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include <hal_led_lights/led_lights_controller.h>

using ::uint8_t;

LedLightsController::LedLightsController()
{
  // maximum number of leds for one fadecandy
  for (uint pixel_iterator = 0; pixel_iterator < fade_candy_features::max_pixels_number; pixel_iterator++)
  {
    leds_state_.push_back(std::move(std::make_unique<ColorInRgb>(
        fade_candy_features::led_off, fade_candy_features::led_off, fade_candy_features::led_off)));
  }
}

bool LedLightsController::connectWithOPCServer()
{
  return opc_client_.initOPCConnection();
}

LedLightsController::~LedLightsController()
{
  leds_state_.clear();
}

ColorInRgb LedLightsController::getPixelValue(const std::uint8_t connector, const std::uint8_t pixel) const
{
  return *leds_state_[connector * fade_candy_features::pixels_per_contact + pixel];
}

void LedLightsController::changePixel(const std::uint8_t connector, const std::uint8_t pixel,
                                      const ColorInRgb& pixel_value)
{
  (*leds_state_[connector * fade_candy_features::pixels_per_contact + pixel]).red = pixel_value.red;
  (*leds_state_[connector * fade_candy_features::pixels_per_contact + pixel]).green = pixel_value.green;
  (*leds_state_[connector * fade_candy_features::pixels_per_contact + pixel]).blue = pixel_value.blue;
}

bool LedLightsController::addLamp(const std::string& lamp_name, const uint16_t start_led, const uint16_t stop_led)
{
  // check if lamp with such a name already exists
  if (lamps_.find(lamp_name) != lamps_.end())
  {
    std::cerr << "\nLamp name: " << lamp_name << " already exists!\n";
    return false;
  }
  // check if proper led indexing is achieved
  if (start_led > stop_led)
  {
    std::cerr << "\nLamp name: " << lamp_name << " Start led number must be lower than the stop led value!";
    return false;
  }
  // if both conditions are good then create a led segment for this lamp
  LedSegment lamp_segment = { start_led, stop_led };
  LedSegment temporary;
  // then check if any lamp have the same sets of leds
  for (const auto& lamp : lamps_)
  {
    temporary = lamp.second->getLampPixels();
    // check if lamps pixels dont converge
    if (temporary.first_pixel_index >= lamp_segment.first_pixel_index &&
        temporary.first_pixel_index <= lamp_segment.last_pixel_index)
    {
      std::cerr << "\nLamp name: " << lamp_name << " Lamps leds converge with lamp: " << lamp.first << "!\n";
      return false;
    }
    if (temporary.last_pixel_index >= lamp_segment.first_pixel_index &&
        temporary.last_pixel_index <= lamp_segment.last_pixel_index)
    {
      std::cerr << "\nLamp name: " << lamp_name << " Lamps leds converge with lamp: " << lamp.first << "!\n";
      return false;
    }
  }
  // if they dont converge, then add lamp to the unordered map
  lamps_.insert(std::make_pair(lamp_name, std::move(std::make_unique<LedLamp>(lamp_segment))));
}

bool LedLightsController::changeAllLampLightConfiguration(const Color color, const uint8_t intensity,
                                                          const uint16_t periodFlashing, const uint16_t periodFading)
{
  for (auto& lamp : lamps_)
  {
    lamp.second->setColor(color);
    lamp.second->setIntensity(intensity);
    lamp.second->setFlashingPeriod(periodFlashing);
    lamp.second->setFadingPeriod(periodFading);
  }
}

bool LedLightsController::changeLampLightConfiguration(const std::string& lamp_name, const Color color,
                                                       const uint8_t intensity, const uint16_t periodFlashing,
                                                       const uint16_t periodFading)
{
  auto lamp_iterator = lamps_.find(lamp_name);
  if (lamp_iterator == lamps_.end())
  {
    std::cerr << "\nLamp name: " << lamp_name << " doesnt exist!";
    return false;
  }
  //
  lamp_iterator->second->setColor(color);
  lamp_iterator->second->setFlashingPeriod(periodFlashing);
  lamp_iterator->second->setFadingPeriod(periodFading);
  return true;
}

void LedLightsController::debugWriteLampConfigurations()
{
  for (const auto& lamp : lamps_)
  {
    std::cout << "Lamp name: " << lamp.first << "\n";
    std::cout << "First pixel: " << static_cast<int>(lamp.second->getLampPixels().first_pixel_index)
              << " Last pixel: " << static_cast<int>(lamp.second->getLampPixels().last_pixel_index) << "\n";
    std::cout << "Color: " << static_cast<int>(lamp.second->getColor()) << "\n";
    std::cout << "Fading period: " << static_cast<int>(lamp.second->getFadingPeriod()) << "\n";
    std::cout << "Flashing period: " << static_cast<int>(lamp.second->getFlashingPeriod()) << "\n";
  }
}

void LedLightsController::setLedsStates()
{
  for (const auto& lamp : lamps_)
  {
    changeLampLedsColor(lamp.second);
  }
}

void LedLightsController::changeLampLedsColor(std::unique_ptr<LedLamp> const& lamp)
{
  uint16_t lamp_leds_base_index = lamp->getLampPixels().first_pixel_index;
  uint16_t lamp_leds_last_index = lamp->getLampPixels().last_pixel_index;
  ColorInRgb colorRGB;
  lamp->setLedsRGBvalues(&colorRGB);
  for (uint16_t led_iterator = lamp_leds_base_index; led_iterator < lamp_leds_last_index; led_iterator++)
  {
    (*leds_state_[led_iterator]).red = colorRGB.red;
    (*leds_state_[led_iterator]).green = colorRGB.green;
    (*leds_state_[led_iterator]).blue = colorRGB.blue;
  }
}

bool LedLightsController::execute()
{
  setLedsStates();
  opc_client_.setPixelsData(leds_state_);
  return opc_client_.sendPixelsData();
}
