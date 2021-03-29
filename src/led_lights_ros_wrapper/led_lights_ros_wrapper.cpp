/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include <hal_led_lights/led_lights_ros_wrapper/led_lights_ros_wrapper.h>

LedLightsRosWrapper::LedLightsRosWrapper(int argc, char** argv)
{
  ros::init(argc, argv, "hal_lights");
  node_ptr_ = std::move(std::make_unique<ros::NodeHandle>());
  lamps_control_service_ = node_ptr_->advertiseService("agribot/hal_led_lights/change_lamps_configuration",
                                                       &LedLightsRosWrapper::lampControlServiceCallback, this);

  leds_controller_.connectWithOpcServer();
  attemptToAddLamp("right_front", static_cast<uint16_t>(FadeCandyContactPixelIndex::contact_0),
                   static_cast<uint16_t>(FadeCandyContactPixelIndex::contact_0) +
                       fade_candy_features::pixels_per_contact - 1);
  attemptToAddLamp("left_front", static_cast<uint16_t>(FadeCandyContactPixelIndex::contact_2),
                   static_cast<uint16_t>(FadeCandyContactPixelIndex::contact_2) +
                       fade_candy_features::pixels_per_contact - 1);
  attemptToAddLamp("right_back", static_cast<uint16_t>(FadeCandyContactPixelIndex::contact_4),
                   static_cast<uint16_t>(FadeCandyContactPixelIndex::contact_4) +
                       fade_candy_features::pixels_per_contact - 1);
  attemptToAddLamp("left_back", static_cast<uint16_t>(FadeCandyContactPixelIndex::contact_6),
                   static_cast<uint16_t>(FadeCandyContactPixelIndex::contact_6) +
                       fade_candy_features::pixels_per_contact - 1);
}

void LedLightsRosWrapper::attemptToAddLamp(const std::string lamp_name, const uint16_t start_pixel,
                                           const uint16_t last_pixel)
{
  try
  {
    leds_controller_.addLamp(lamp_name, start_pixel, last_pixel);
  }
  catch (const char* err_msg)
  {
    ROS_ERROR_STREAM("[hal_led_lights]" << std::string(err_msg) << std::endl);
  }
}

bool LedLightsRosWrapper::changeAllLampsConfigurations(
    const LampConfiguration& config,
    agribot_msgs::LampsControl::Response& res)  // NOLINT(runtime/references)
{
  leds_controller_.changeAllLampLightConfiguration(config);
  res.success = true;
  return true;
}

bool LedLightsRosWrapper::changeLampConfiguration(
    const LampConfiguration& config,
    agribot_msgs::LampsControl::Response& res)  // NOLINT(runtime/references)
{
  try
  {
    leds_controller_.changeLampLightConfiguration(config);
    res.success = true;
    return true;
  }
  catch(const char* err_msg)
  {
    ROS_ERROR_STREAM("[hal_led_lights] " << err_msg << '\n');
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
    ROS_ERROR_STREAM("[hal_led_lights] Fading period can't be greater than flash period");
    res.success = false;
    return false;
  }
  // if intensity lower than 0.0 or greater than 1.0
  if (!(req.intensity <= static_cast<uint8_t>(fade_candy_features::intensity_scale)))
  {
    ROS_ERROR_STREAM("[hal_led_lights] Intensity can't be grater than 100");
    res.success = false;
    return false;
  }
  // if color is out of the enumerated value range, then
  if (req.color >= static_cast<uint8_t>(Color::color_count))
  {
    ROS_ERROR_STREAM("[hal_led_lights] Requested color doesn't exist on the color list");
    res.success = false;
    return false;
  }
  LampConfiguration config;
  config.lamp_name = req.lamp;
  config.color = static_cast<Color>(req.color);
  config.intensity = req.intensity;
  config.periodFlashing = req.flash_period_in_miliseconds;
  config.periodFading = req.fading_period_in_miliseconds;
  if (req.lamp == req.control_to_all_lamps)
  {
    return changeAllLampsConfigurations(config, res);
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
      ROS_ERROR_STREAM_THROTTLE(10, "[hal_led_lights] Can't connect with OPC server");
    }
    ros::spinOnce();
    loop_rate.sleep();
  }
  return true;
}
