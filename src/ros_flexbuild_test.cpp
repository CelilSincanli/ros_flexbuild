#include "ros_flexbuild_test/ros_flexbuild_test.hpp"

namespace ros_flexbuild {

    FlexBuildTestNode::FlexBuildTestNode(ros_wrapper::NodeHandle node) : node_(node) {
        interface_ = std::make_shared<ros_wrapper::RosInterface<>>(node_);

        std::string greeting_target = interface_->DeclareAndGetParam("greeting_target", std::string("world"));
        ROS_LOG_WARN(node_, "Greeting target: " << greeting_target);

        publisher_ = interface_->CreatePublisher<std_msgs::msg::String>("greeting_topic", 10);

        subscriber_ = interface_->CreateSubscriber<std_msgs::msg::String>(
            "greeting_topic", 10,
            std::bind(&FlexBuildTestNode::messageCallback, this, std::placeholders::_1)
        );

        trigger_service_ = interface_->CreateService<std_srvs::srv::Trigger>(
            "trigger_service",
            std::bind(&FlexBuildTestNode::triggerServiceCallback, this, std::placeholders::_1, std::placeholders::_2)
        );
    
        timer_ = interface_->CreateTimer(
            ros_wrapper::DurationType(1000),
            std::bind(&FlexBuildTestNode::publishMessage, this)
        );
    }

    void FlexBuildTestNode::publishMessage() {
        auto msg = std::make_shared<std_msgs::msg::String>();
        msg->data = "Hello from ros_flexbuild!";
        publisher_->publish(*msg);
    }

    void FlexBuildTestNode::messageCallback(const std_msgs::msg::String::SharedPtr msg) {
        ROS_LOG_INFO(node_, "Received: '" << msg->data << "'");
    }

    void FlexBuildTestNode::triggerServiceCallback(
        ros_wrapper::ServiceRequestType<std_srvs::srv::Trigger> request,
        ros_wrapper::ServiceResponseType<std_srvs::srv::Trigger> response)
    {
        (void)request;
        response->success = true;
        response->message = "Trigger service called!";
        ROS_LOG_INFO(node_, "Trigger service was called.");
    }

} // namespace ros_flexbuild