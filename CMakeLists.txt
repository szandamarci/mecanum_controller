cmake_minimum_required(VERSION 3.5)
project(mecanum)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(mecanum_common REQUIRED)

# Include directories
include_directories(
  include
  ScannerLib
  bcm2835
  ${tf2_ros_INCLUDE_DIRS}
)

# Controller executable
add_executable(controller
  src/controller.cpp
  src/Spi.cpp
  bcm2835/bcm2835.c
)
ament_target_dependencies(controller
  rclcpp
  geometry_msgs
  tf2_ros
  tf2
  mecanum_common
  tf2_geometry_msgs
)

# Install targets
install(TARGETS controller
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()