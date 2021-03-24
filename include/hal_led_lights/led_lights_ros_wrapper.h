/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once

#include <agribot_msgs/LampsControl.h>
#include <hal_led_lights/led_lights_controller.h>
#include <ros/ros.h>
#include <ros/console.h>
#include <iostream>
#include <memory>
#include <string>

class LedLightsRosWrapper
{
  LedLightsController leds_controller_;
  std::unique_ptr<ros::NodeHandle> node_ptr_;
  ros::ServiceServer lamps_control_service_;

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
