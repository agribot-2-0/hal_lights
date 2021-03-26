/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */
#pragma once

#include <hal_led_lights/external/OpcClient/opc_client.h>
#include <hal_led_lights/common/led_lights_constants.h>
#include <vector>
#include <memory>
#include <tuple>
#include <cstdint>
#include <iostream>

class LedLightsOpcClient
{
  OpcClient opc_;
  bool opc_host_resolved_;
  std::vector<std::uint8_t> frame_data_;

public:
  LedLightsOpcClient();
  bool checkOpcConnection();
  bool initOpcConnection();
  // helping lambda encapsulation for the easier index acces in for range-based loops
  template <typename Collection>
  auto getIndex(Collection const& collection);
  void setPixelsData(const std::vector<std::unique_ptr<ColorInRgb>>& dataVector);
  void setPixelFrameData(const uint pixel_count, const ColorInRgb& data);
  bool sendPixelsData();
};

template <typename Collection>
auto LedLightsOpcClient::getIndex(Collection const& collection)
{
  return [&collection](auto const& iterator) { return std::distance(begin(collection), iterator); };
}
