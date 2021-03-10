/*
 * Copyright (C) 2020 Agribot. All rights reserved.
 */

#ifndef HAL_LED_LIGHTS_LED_LIGHTS_OPC_CLIENT_H_
#define HAL_LED_LIGHTS_LED_LIGHTS_OPC_CLIENT_H_

#include <hal_led_lights/opc_client.h>
#include <hal_led_lights/led_lights_constants.h>
#include <vector>
#include <memory>
#include <tuple>
#include <cstdint>
#include <iostream>

class LedLightsOPCClient
{
  OPCClient opc_;
  bool opc_inited_;
  std::vector<std::uint8_t> frame_data_;

public:
  LedLightsOPCClient();
  ~LedLightsOPCClient();
  bool checkOPCConnection();
  bool initOPCConnection();
  // helping lambda encapsulation for the easier index acces in for range-based loops
  template <typename Collection>
  auto getIndex(Collection const& collection);
  void setPixelsData(const std::vector<std::unique_ptr<colorInRGB>>& dataVector);
  void setPixelFrameData(const uint pixel_count, const colorInRGB& data);
  bool sendPixelsData();
};

template <typename Collection>
auto LedLightsOPCClient::getIndex(Collection const& collection)
{
  return [&collection](auto const& iterator) { return std::distance(begin(collection), iterator); };
}

#endif  // HAL_LED_LIGHTS_LED_LIGHTS_OPC_CLIENT_H_
