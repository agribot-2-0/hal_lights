/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include <hal_lights/lights_ros_wrapper/lights_ros_wrapper.h>

using std::uint16_t;
using std::uint8_t;

constexpr uint8_t max_lamp_bit = 0x80;

LightsRosWrapper::LightsRosWrapper(int argc, char** argv)
{
  ros::init(argc, argv, "hal_lights");
  node_ptr_ = std::make_unique<ros::NodeHandle>();
  lamps_control_service_ = node_ptr_->advertiseService("agribot/hal_lights/change_lamps_configuration",
                                                       &LightsRosWrapper::lampControlServiceCallback, this);

  leds_controller_.connectWithOpcServer();
  addLamp("right_front", static_cast<uint16_t>(FadeCandyDataLinePixelIndex::data_line_0),
          static_cast<uint16_t>(FadeCandyDataLinePixelIndex::data_line_0) + fade_candy_features::pixels_per_data_line -
              1);
  addLamp("left_front", static_cast<uint16_t>(FadeCandyDataLinePixelIndex::data_line_2),
          static_cast<uint16_t>(FadeCandyDataLinePixelIndex::data_line_2) + fade_candy_features::pixels_per_data_line -
              1);
  addLamp("right_back", static_cast<uint16_t>(FadeCandyDataLinePixelIndex::data_line_4),
          static_cast<uint16_t>(FadeCandyDataLinePixelIndex::data_line_4) + fade_candy_features::pixels_per_data_line -
              1);
  addLamp("left_back", static_cast<uint16_t>(FadeCandyDataLinePixelIndex::data_line_6),
          static_cast<uint16_t>(FadeCandyDataLinePixelIndex::data_line_6) + fade_candy_features::pixels_per_data_line -
              1);
}

void LightsRosWrapper::addLamp(const std::string lamp_name, const uint16_t start_pixel, const uint16_t last_pixel)
{
  try
  {
    leds_controller_.addLamp(lamp_name, start_pixel, last_pixel);
  }
  catch (const std::exception* except)
  {
    ROS_ERROR_STREAM("[HalLights]" << std::string(except->what()) << std::endl);
  }
}

bool LightsRosWrapper::changeEveryLampConfiguration(
    const LampConfiguration& config,
    agribot_msgs::LampsControl::Response& res)  // NOLINT(runtime/references)
{
  leds_controller_.changeEveryLampConfiguration(config);
  res.success = true;
  return true;
}

bool LightsRosWrapper::changeLampConfiguration(const LampConfiguration& config)
{
  try
  {
    leds_controller_.changeLampConfiguration(config);
    return true;
  }
  catch (const std::exception* except)
  {
    ROS_ERROR_STREAM("[HalLights] " << std::string(except->what()) << '\n');
    return false;
  }
}

bool LightsRosWrapper::executeLampControlService(
    agribot_msgs::LampsControl::Request& req,   // NOLINT(runtime/references)
    agribot_msgs::LampsControl::Response& res)  // NOLINT(runtime/references)
{
  LampConfiguration config;
  config.setName(req.lamp);
  config.color_ = static_cast<Color>(req.color);
  config.intensity_ = req.intensity;
  config.period_flashing_ = req.flash_period_in_miliseconds;
  config.period_fading_ = req.fading_period_in_miliseconds;
  if (config.lamp_name_ == "ALL")
  {
    return changeEveryLampConfiguration(config, res);
  }
  else
  {
    uint16_t lamp_mask = 1;
    while (lamp_mask <= max_lamp_bit)
    {
      if ((req.lamp & lamp_mask) != 0)
      {
        config.setName(req.lamp & lamp_mask);
        if (config.lamp_name_.empty())
        {
          ROS_ERROR_STREAM("[HalLights] Requested lamp: " + std::to_string(req.lamp & lamp_mask) +
                           " doesn't exist on the list of possible lamp names");
          res.success = false;
          return false;
        }
        else
        {
          if (!changeLampConfiguration(config))
          {
            res.success = false;
            return false;
          }
        }
      }
      lamp_mask = lamp_mask << 1;
    }
    res.success = true;
    return true;
  }
}

bool LightsRosWrapper::lampControlServiceCallback(
    agribot_msgs::LampsControl::Request& req,   // NOLINT(runtime/references)
    agribot_msgs::LampsControl::Response& res)  // NOLINT(runtime/references)
{
  // if fading period greater than flashing, reject service call
  if (req.flash_period_in_miliseconds < req.fading_period_in_miliseconds)
  {
    ROS_ERROR_STREAM("[HalLights] Fading period can't be greater than flash period");
    res.success = false;
    return false;
  }
  // if intensity greater than fade_candy_features::intensity_scale
  if (req.intensity > static_cast<uint8_t>(fade_candy_features::intensity_scale))
  {
    ROS_ERROR_STREAM("[HalLights] Intensity can't be grater than " << fade_candy_features::intensity_scale);
    res.success = false;
    return false;
  }
  // if color is out of the enumerated value range, then
  if (req.color >= static_cast<uint8_t>(Color::color_count))
  {
    ROS_ERROR_STREAM("[HalLights] Requested color doesn't exist on the color list");
    res.success = false;
    return false;
  }
  return executeLampControlService(req, res);
}

bool LightsRosWrapper::execute()
{
  ros::Rate loop_rate(10);
  while (ros::ok())
  {
    if (!leds_controller_.execute())
    {
      ROS_ERROR_STREAM_THROTTLE(10, "[HalLights] Can't connect with OPC server");
    }
    ros::spinOnce();
    loop_rate.sleep();
  }
  return true;
}
