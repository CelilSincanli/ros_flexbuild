#ifdef ROS2_BUILD
#include "ros_flexbuild_test/ros_flexbuild_test.hpp"

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<rclcpp::Node>("flexbuild_node");
    ros_flexbuild::FlexBuildTestNode test(node);
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
#else
#include "ros_flexbuild_test/ros_flexbuild_test.hpp"
#include <ros/ros.h>

int main(int argc, char** argv) {
    ros::init(argc, argv, "flexbuild_node");
    ros::NodeHandle nh;
    ros_flexbuild::FlexBuildTestNode test(&nh);
    ros::spin();
    return 0;
}
#endif