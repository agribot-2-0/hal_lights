/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include <hal_lights/lights_ros_wrapper/lights_ros_wrapper.h>

int main(int argc, char** argv)
{
  LightsRosWrapper ros_wrapper(argc, argv);

  ros_wrapper.execute();
  return 0;
}
