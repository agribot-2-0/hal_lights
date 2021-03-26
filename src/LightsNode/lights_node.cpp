/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include <hal_led_lights/LedLightsRosWrapper/led_lights_ros_wrapper.h>

int main(int argc, char** argv)
{
  LedLightsRosWrapper ros_wrapper(argc, argv);

  ros_wrapper.execute();
  return 0;
}
