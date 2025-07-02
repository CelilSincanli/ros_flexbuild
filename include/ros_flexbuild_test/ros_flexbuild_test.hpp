#ifndef ROS_FLEXBUILD_TEST_HPP_
#define ROS_FLEXBUILD_TEST_HPP_

#include "ros_interface.hpp"

namespace ros_flexbuild {

class FlexBuildTestNode {
public:
    FlexBuildTestNode(ros_wrapper::NodeHandle node);

private:
    void publishMessage();
    void messageCallback(const std_msgs::msg::String::SharedPtr msg);

    void triggerServiceCallback(
        ros_wrapper::ServiceRequestType<std_srvs::srv::Trigger> request,
        ros_wrapper::ServiceResponseType<std_srvs::srv::Trigger> response);

    ros_wrapper::NodeHandle node_;
    std::shared_ptr<ros_wrapper::RosInterface<>> interface_;
    ros_wrapper::PublisherType<std_msgs::msg::String> publisher_;
    ros_wrapper::SubscriberType<std_msgs::msg::String> subscriber_;
    ros_wrapper::TimerType timer_;

    ros_wrapper::ServiceHandleType<std_srvs::srv::Trigger> trigger_service_;
};

}  // namespace ros_flexbuild

#endif  // ROS_FLEXBUILD_TEST_HPP_
