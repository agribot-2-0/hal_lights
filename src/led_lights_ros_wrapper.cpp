/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include <hal_led_lights/led_lights_ros_wrapper.h>

LedLightsRosWrapper::LedLightsRosWrapper(int argc, char** argv)
{
  ros::init(argc, argv, "led_lights");
  node_ptr_ = std::move(std::make_unique<ros::NodeHandle>());
  lamps_control_service_ = node_ptr_->advertiseService("agribot/hal_led_lights/change_lamps_configuration",
                                                     &LedLightsRosWrapper::lampControlServiceCallback, this);

  leds_controller_.connectWithOPCServer();
  leds_controller_.addLamp("right_front", static_cast<uint16_t>(FadeCandyContactPixelIndex::contact_0),
                          static_cast<uint16_t>(FadeCandyContactPixelIndex::contact_0) +
                              fade_candy_features::pixels_per_contact - 1);
  leds_controller_.addLamp("left_front", static_cast<uint16_t>(FadeCandyContactPixelIndex::contact_2),
                          static_cast<uint16_t>(FadeCandyContactPixelIndex::contact_2) +
                              fade_candy_features::pixels_per_contact - 1);
  leds_controller_.addLamp("right_back", static_cast<uint16_t>(FadeCandyContactPixelIndex::contact_4),
                          static_cast<uint16_t>(FadeCandyContactPixelIndex::contact_4) +
                              fade_candy_features::pixels_per_contact - 1);
  leds_controller_.addLamp("left_back", static_cast<uint16_t>(FadeCandyContactPixelIndex::contact_6),
                          static_cast<uint16_t>(FadeCandyContactPixelIndex::contact_6) +
                              fade_candy_features::pixels_per_contact - 1);
}

bool LedLightsRosWrapper::changeAllLampsConfigurations(
    agribot_msgs::LampsControl::Request& req,   // NOLINT(runtime/references)
    agribot_msgs::LampsControl::Response& res)  // NOLINT(runtime/references)
{
  leds_controller_.changeAllLampLightConfiguration(static_cast<Color>(req.color), req.intensity,
                                                  req.flash_period_in_miliseconds, req.fading_period_in_miliseconds);
  res.success = true;
  return true;
}

bool LedLightsRosWrapper::changeLampConfiguration(
    agribot_msgs::LampsControl::Request& req,   // NOLINT(runtime/references)
    agribot_msgs::LampsControl::Response& res)  // NOLINT(runtime/references)
{
  if (leds_controller_.changeLampLightConfiguration(req.lamp, static_cast<Color>(req.color), req.intensity,
                                                   req.flash_period_in_miliseconds, req.fading_period_in_miliseconds))
  {
    res.success = true;
    return true;
  }
  else
  {
    ROS_ERROR_STREAM("[hal_led_lights] Lamp doesnt exist");
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
    ROS_ERROR_STREAM("[hal_led_lights] Fading period cant be greater than flash period");
    res.success = false;
    return false;
  }
  // if intensity lower than 0.0 or greater than 1.0
  if (!(req.intensity <= static_cast<uint8_t>(fade_candy_features::intensity_scale)))
  {
    ROS_ERROR_STREAM("[hal_led_lights] Intensity cant be grater than 100");
    res.success = false;
    return false;
  }
  // if color is out of the enumerated value range, then
  if (req.color >= static_cast<uint8_t>(Color::color_count))
  {
    ROS_ERROR_STREAM("[hal_led_lights] Requested color doesnt exist on the color list");
    res.success = false;
    return false;
  }
  if (req.lamp == req.control_to_all_lamps)
  {
    return changeAllLampsConfigurations(req, res);
  }
  else
  {
    return changeLampConfiguration(req, res);
  }
}

bool LedLightsRosWrapper::execute()
{
  ros::Rate loop_rate(10);
  while (ros::ok())
  {
    if (!leds_controller_.execute())
    {
      ROS_ERROR_STREAM("[hal_led_lights] Cant connect with OPC server");
    }
    ros::spinOnce();
    loop_rate.sleep();
  }
}
