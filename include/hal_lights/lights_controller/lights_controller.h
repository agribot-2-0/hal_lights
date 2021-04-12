/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */
#pragma once

#include <hal_lights/lamp/lamp.h>
#include <hal_lights/lights_opc_client/lights_opc_client.h>
#include <hal_lights/common/lights_constants.h>
#include <unordered_map>
#include <stdexcept>
#include <vector>
#include <memory>
#include <tuple>
#include <string>
#include <cstdint>
#include <iostream>

class LightsController
{
  std::vector<std::unique_ptr<ColorInRgb>> leds_state_;
  std::unordered_map<std::string, std::unique_ptr<Lamp>> lamps_;
  LightsOpcClient opc_client_;

  void changeLampLedsColor(const std::unique_ptr<Lamp> &lamp);
  void setLedsStates();

public:
  LightsController();

  bool addLamp(const std::string& lamp_name, const uint16_t start_led, const uint16_t stop_led);
  std::string debugPrintLampConfigurations();
  bool changeLampConfiguration(const LampConfiguration& conf);
  void changeEveryLampConfiguration(const LampConfiguration& conf);

  void changePixel(const std::uint8_t connector, const std::uint8_t pixel, const ColorInRgb& pixel_value);
  ColorInRgb getPixelValue(const std::uint8_t connector, const std::uint8_t pixel) const;
  bool connectWithOpcServer();
  bool execute();
};
