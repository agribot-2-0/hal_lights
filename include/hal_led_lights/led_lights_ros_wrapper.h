/*
 * Copyright (C) 2020 Agribot. All rights reserved.
 */

#ifndef HAL_LED_LIGHTS_LED_LIGHTS_ROS_WRAPPER_H_
#define HAL_LED_LIGHTS_LED_LIGHTS_ROS_WRAPPER_H_
#include <agribot_msgs/LampsControl.h>
#include <hal_led_lights/led_lights_controller.h>
#include <ros/ros.h>
#include <ros/console.h>
#include <iostream>
#include <memory>
#include <string>

class LedLightsRosWrapper
{
  LedLightsController ledsController_;
  std::unique_ptr<ros::NodeHandle> node_ptr_;
  ros::ServiceServer lampsControlService_;

  bool lampControlServiceCallback(agribot_msgs::LampsControl::Request& req,      // NOLINT(runtime/references)
                                  agribot_msgs::LampsControl::Response& res);    // NOLINT(runtime/references)
  bool changeLampConfiguration(agribot_msgs::LampsControl::Request& req,         // NOLINT(runtime/references)
                               agribot_msgs::LampsControl::Response& res);       // NOLINT(runtime/references)
  bool changeAllLampsConfigurations(agribot_msgs::LampsControl::Request& req,    // NOLINT(runtime/references)
                                    agribot_msgs::LampsControl::Response& res);  // NOLINT(runtime/references)

public:
  LedLightsRosWrapper(int argc, char** argv);
  bool execute();
};

#endif  // HAL_LED_LIGHTS_LED_LIGHTS_ROS_WRAPPER_H_
