/*
 * Copyright (C) 2020 Agribot. All rights reserved.
 */
#include <hal_led_lights/led_lights_ros_wrapper.h>

int main(int argc, char** argv)
{
  LedLightsRosWrapper ros_wrapper(argc, argv);

  ros_wrapper.execute();
  return 0;
}
