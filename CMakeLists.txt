#############################################################################
## Set minimum required version of cmake, project name and compile options ##
#############################################################################
cmake_minimum_required(VERSION 2.8.3)
project(hal_lights)

## Compilation options
add_compile_options(-std=c++17 -Wall -Wextra -pedantic -Werror -Wno-unused-variable)


#############################################################################
## Find catkin packages and libraries for catkin and system dependencies   ##
#############################################################################
find_package(catkin REQUIRED COMPONENTS
  roscpp
  agribot_msgs
  agribot_common
)

################################################
## Declare ROS messages, services and actions ##
################################################

################################################
## Declare ROS dynamic reconfigure parameters ##
################################################

###################################
## catkin specific configuration ##
###################################
catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES can
#  CATKIN_DEPENDS other_catkin_pkg
#  DEPENDS system_lib
)

#############################################################################
## Build                                                                   ##
#############################################################################
include_directories(
 include
 ${catkin_INCLUDE_DIRS}
 ${agribot_common_SOURCE_DIR}/include
)

list(APPEND cpp_files src/lights_node/lights_node.cpp src/lights_ros_wrapper/lights_ros_wrapper.cpp)
list(APPEND cpp_files src/lights_controller/lights_controller.cpp)
list(APPEND cpp_files src/lights_opc_client/lights_opc_client.cpp)
list(APPEND cpp_files src/lamp/lamp.cpp)

link_directories("${PROJECT_SOURCE_DIR}")
add_executable(hal_lights_node ${cpp_files})
target_link_libraries(hal_lights_node ${agribot_common_LIBRARIES} ${catkin_LIBRARIES})

add_dependencies(hal_lights_node ${catkin_EXPORTED_TARGETS})

#############
## Install ##
#############

install(TARGETS
  hal_lights_node
  DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
)


install(DIRECTORY launch
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
  PATTERN ".svn" EXCLUDE
)

#############
## Testing ##
#############
