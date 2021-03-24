/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */
#pragma once

#include <hal_led_lights/led_lamp.h>
#include <hal_led_lights/led_lights_opc_client.h>
#include <hal_led_lights/led_lights_constants.h>
#include <unordered_map>
#include <vector>
#include <memory>
#include <tuple>
#include <string>
#include <cstdint>
#include <iostream>

class LedLightsController
{
  std::vector<std::unique_ptr<ColorInRgb>> leds_state_;
  std::unordered_map<std::string, std::unique_ptr<LedLamp>> lamps_;
  LedLightsOPCClient opc_client_;

  void changeLampLedsColor(std::unique_ptr<LedLamp> const& lamp);
  void setLedsStates();

public:
  LedLightsController();
  ~LedLightsController();

  bool addLamp(const std::string& lamp_name, const uint16_t start_led, const uint16_t stop_led);
  void debugWriteLampConfigurations();
  bool changeLampLightConfiguration(const std::string& lamp_name, const Color color, const uint8_t intensity,
                                    const uint16_t periodFlashing, const uint16_t periodFading);
  bool changeAllLampLightConfiguration(const Color color, const uint8_t intensity, const uint16_t periodFlashing,
                                       const uint16_t periodFading);

  void changePixel(const std::uint8_t connector, const std::uint8_t pixel, const ColorInRgb& pixel_value);
  ColorInRgb getPixelValue(const std::uint8_t connector, const std::uint8_t pixel) const;
  bool connectWithOPCServer();
  bool execute();
};
