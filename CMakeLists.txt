cmake_minimum_required(VERSION 3.8)
project(ros_flexbuild)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(CMAKE_CXX_STANDARD 17)

string(ASCII 27 Esc)
set(ColourReset "${Esc}[m")
set(BoldRed     "${Esc}[1;31m")
set(BoldGreen   "${Esc}[1;32m")
set(BoldYellow  "${Esc}[1;33m")
set(BoldBlue    "${Esc}[1;34m")
set(BoldRed     "${Esc}[1;31m")

find_package(rclcpp QUIET)
find_package(catkin QUIET COMPONENTS roscpp)

if(catkin_FOUND)

    message("${BoldYellow}ROS detected.${ColourReset}")
    message("${BoldBlue}Using catkin build system.${ColourReset}")

    find_package(catkin REQUIRED COMPONENTS roscpp std_msgs std_srvs)

    catkin_package(
        INCLUDE_DIRS include
        LIBRARIES ros_flexbuild
        CATKIN_DEPENDS roscpp std_msgs std_srvs
    )

    include_directories(
        include
        ${catkin_INCLUDE_DIRS}
    )

    add_library(ros_flexbuild INTERFACE)
    target_include_directories(ros_flexbuild INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_link_libraries(ros_flexbuild INTERFACE ${catkin_LIBRARIES})

    add_executable(ros_test_node test/ros_flex_test.cpp)
    target_link_libraries(ros_test_node ${catkin_LIBRARIES})
    add_dependencies(ros_test_node ${catkin_EXPORTED_TARGETS})


    install(DIRECTORY include/
        DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
    )
    install(TARGETS ros_test_node
        RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
    )

elseif(rclcpp_FOUND)

    message("${BoldYellow}ROS 2 detected.${ColourReset}")
    message("${BoldBlue}Using ament build system.${ColourReset}")

    find_package(ament_cmake REQUIRED)
    find_package(rclcpp REQUIRED)
    find_package(std_msgs REQUIRED)
    find_package(std_srvs REQUIRED)

    add_library(ros_flexbuild INTERFACE)
    target_include_directories(ros_flexbuild INTERFACE include)
    target_compile_definitions(ros_flexbuild INTERFACE ROS2_BUILD)

    add_executable(ros2_test_node src/main.cpp src/ros_flexbuild_test.cpp)
    target_include_directories(ros2_test_node PRIVATE include)

    ament_target_dependencies(ros2_test_node
        rclcpp
        std_msgs
        std_srvs
    )

    target_link_libraries(ros2_test_node ros_flexbuild)
    target_compile_definitions(ros2_test_node PRIVATE ROS2_BUILD)

    install(TARGETS ros2_test_node
      DESTINATION lib/${PROJECT_NAME}
    )
    install(DIRECTORY include/
      DESTINATION include/
    )
    install(DIRECTORY launch/
      DESTINATION share/${PROJECT_NAME}/launch
    )

    ament_package()

else()

    message("${BoldRed}Neither ROS 1 (catkin) nor ROS 2 (rclcpp) was found!${ColourReset}")

endif()