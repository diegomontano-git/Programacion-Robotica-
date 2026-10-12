cmake_minimum_required(VERSION 3.8)
project(act3)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(interfaces REQUIRED)                      # CHANGE

add_executable(emulador src/nodo1.cpp)
ament_target_dependencies(emulador rclcpp interfaces)    # CHANGE

add_executable(monitor src/nodo2.cpp)
ament_target_dependencies(monitor rclcpp interfaces)  # CHANGE

add_executable(operador src/nodo3.cpp)
ament_target_dependencies(operador rclcpp interfaces)  # CHANGE

install(TARGETS
  emulador
  monitor
  operador
  DESTINATION lib/${PROJECT_NAME})

ament_package()
