/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include <hal_led_lights/LedLightsOpcClient/led_lights_opc_client.h>

constexpr std::uint16_t lower_byte = 0x00FF;
constexpr std::uint16_t higher_byte = 0xFF00;

LedLightsOpcClient::LedLightsOpcClient() : opc_()
{
  opc_host_resolved_ = false;
  // first byte of the light control frame is channel byte, always 0
  frame_data_.push_back(channel);
  // second byte is the command
  frame_data_.push_back(pixel_command);
  // third and fourth bytes are length
  // higher byte
  frame_data_.push_back(static_cast<std::uint8_t>((frame_data_length & higher_byte) >> 8));
  // lower byte
  frame_data_.push_back(static_cast<std::uint8_t>(frame_data_length & lower_byte));
  // then comes data bytes, 3 for every led/pixel
  for (int vector_bytes_counter = 0; vector_bytes_counter < frame_data_length; vector_bytes_counter++)
  {
    frame_data_.push_back(color_off);
  }
}

void LedLightsOpcClient::setPixelsData(
    const std::vector<std::unique_ptr<ColorInRgb>>& dataVector)
{
  ColorInRgb tempData;
  const auto index = getIndex(dataVector);
  for (auto it_pixel = dataVector.begin(); it_pixel != dataVector.end(); ++it_pixel)
  {
    tempData.red = (**it_pixel).red;
    tempData.green = (**it_pixel).green;
    tempData.blue = (**it_pixel).blue;
    setPixelFrameData(index(it_pixel), tempData);
  }
}

void LedLightsOpcClient::setPixelFrameData(const uint pixel_ID, const ColorInRgb& data)
{
  uint16_t pixel_base_byte_position = frame_base_length + pixel_ID * rgb_bytes_per_pixel;
  frame_data_[pixel_base_byte_position + red_byte_position] = data.red;
  frame_data_[pixel_base_byte_position + green_byte_position] = data.green;
  frame_data_[pixel_base_byte_position + blue_byte_position] = data.blue;
}

bool LedLightsOpcClient::checkOpcConnection()
{
  return (opc_host_resolved_ && opc_.isConnected());
}

bool LedLightsOpcClient::initOpcConnection()
{
  opc_host_resolved_ = false;
  // first init the OPC client connection
  if (opc_.resolve("localhost"))
  {
    opc_host_resolved_ = true;
    return true;
  }
  else
  {
    return false;
  }
}

bool LedLightsOpcClient::sendPixelsData()
{
  if (!checkOpcConnection())
  {
    if (!initOpcConnection())
    {
      return false;
    }
  }
  return opc_.write(frame_data_);
}
