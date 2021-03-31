/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include <hal_led_lights/led_lights_ros_wrapper/led_lights_ros_wrapper.h>

using std::uint16_t;

LedLightsRosWrapper::LedLightsRosWrapper(int argc, char** argv)
{
  ros::init(argc, argv, "hal_lights");
  node_ptr_ = std::make_unique<ros::NodeHandle>();
  lamps_control_service_ = node_ptr_->advertiseService("agribot/hal_lights/change_lamps_configuration",
                                                       &LedLightsRosWrapper::lampControlServiceCallback, this);

  leds_controller_.connectWithOpcServer();
  addLamp("right_front", static_cast<uint16_t>(FadeCandyDataLinePixelIndex::data_line_0),
          static_cast<uint16_t>(FadeCandyDataLinePixelIndex::data_line_0) + fade_candy_features::pixels_per_contact -
              1);
  addLamp("left_front", static_cast<uint16_t>(FadeCandyDataLinePixelIndex::data_line_2),
          static_cast<uint16_t>(FadeCandyDataLinePixelIndex::data_line_2) + fade_candy_features::pixels_per_contact -
              1);
  addLamp("right_back", static_cast<uint16_t>(FadeCandyDataLinePixelIndex::data_line_4),
          static_cast<uint16_t>(FadeCandyDataLinePixelIndex::data_line_4) + fade_candy_features::pixels_per_contact -
              1);
  addLamp("left_back", static_cast<uint16_t>(FadeCandyDataLinePixelIndex::data_line_6),
          static_cast<uint16_t>(FadeCandyDataLinePixelIndex::data_line_6) + fade_candy_features::pixels_per_contact -
              1);
}

void LedLightsRosWrapper::addLamp(const std::string lamp_name, const uint16_t start_pixel, const uint16_t last_pixel)
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

bool LedLightsRosWrapper::changeEveryLampConfiguration(
    const LampConfiguration& config,
    agribot_msgs::LampsControl::Response& res)  // NOLINT(runtime/references)
{
  leds_controller_.changeEveryLampConfiguration(config);
  res.success = true;
  return true;
}

bool LedLightsRosWrapper::changeLampConfiguration(
    const LampConfiguration& config,
    agribot_msgs::LampsControl::Response& res)  // NOLINT(runtime/references)
{
  try
  {
    leds_controller_.changeLampConfiguration(config);
    res.success = true;
    return true;
  }
  catch (const std::exception* except)
  {
    ROS_ERROR_STREAM("[HalLights] " << std::string(except->what()) << '\n');
    res.success = false;
    return false;
  }
}

bool LedLightsRosWrapper::lampControlServiceCallback(
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
  LampConfiguration config;
  config.lamp_name_ = req.lamp;
  config.color_ = static_cast<Color>(req.color);
  config.intensity_ = req.intensity;
  config.periodFlashing_ = req.flash_period_in_miliseconds;
  config.periodFading_ = req.fading_period_in_miliseconds;
  if (req.lamp == req.control_to_all_lamps)
  {
    return changeEveryLampConfiguration(config, res);
  }
  else
  {
    return changeLampConfiguration(config, res);
  }
}

bool LedLightsRosWrapper::execute()
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
