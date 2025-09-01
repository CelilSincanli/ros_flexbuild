#include "ros_flexbuild_test/ros_flexbuild_test.hpp"

namespace ros_flexbuild {

    FlexBuildTestNode::FlexBuildTestNode(ros_wrapper::NodeHandle node) : node_(node) {
        interface_ = std::make_shared<ros_wrapper::RosInterface<>>(node_);

        std::string greeting_target = interface_->DeclareAndGetParam("greeting_target", std::string("world"));
        ROS_LOG_WARN(node_, "Greeting target: " << greeting_target);

#ifdef ROS2_BUILD
        publisher_ = interface_->CreatePublisher<MsgType>("greeting_topic", 10);
        subscriber_ = interface_->CreateSubscriber<MsgType>(
            "greeting_topic", 10,
            std::bind(&FlexBuildTestNode::messageCallback, this, std::placeholders::_1)
        );
        // trigger_service_ = interface_->CreateService<SrvType>(
        //     "trigger_service",
        //     std::bind(&FlexBuildTestNode::triggerServiceCallback, this, std::placeholders::_1, std::placeholders::_2)
        // );
#else
        publisher_ = interface_->CreatePublisher<MsgType>("greeting_topic", 10);
        subscriber_ = interface_->CreateSubscriber<MsgType>(
            "greeting_topic", 10,
            boost::bind(&FlexBuildTestNode::messageCallback, this, _1)
        );
        
        // trigger_service_ = interface_->CreateService<SrvType>(
        //     "trigger_service",
        //     &FlexBuildTestNode::triggerServiceCallback,
        //     this
        // );
#endif
        timer_ = interface_->CreateTimer(
            ros_wrapper::getTimerDuration(100),
            &FlexBuildTestNode::publishMessage,
            this
        );
    }

    void FlexBuildTestNode::publishMessage() {
        MsgType msg;
        msg.data = "Hello from ros_flexbuild!";
        #ifdef ROS2_BUILD
                publisher_->publish(msg);
        #else
                publisher_.publish(msg);
        #endif
    }

#ifdef ROS2_BUILD
    void FlexBuildTestNode::messageCallback(const MsgType::SharedPtr msg) {
        ROS_LOG_INFO(node_, "Received: '" << msg->data << "'");
    }

    // ros_wrapper::ServiceCallbackReturnType FlexBuildTestNode::triggerServiceCallback(
    //     ros_wrapper::ServiceRequestType<SrvType> request,
    //     ros_wrapper::ServiceResponseType<SrvType> response)
    // {
    //     (void)request;
    //     response->success = true;
    //     response->message = "Trigger service called!";
    //     ROS_LOG_INFO(node_, "Trigger service was called.");
    // }
#else
    void FlexBuildTestNode::messageCallback(const MsgType::ConstPtr& msg) {
        ROS_LOG_INFO(node_, "Received: '" << msg->data << "'");
    }

    // ros_wrapper::ServiceCallbackReturnType FlexBuildTestNode::triggerServiceCallback(
    //     ros_wrapper::ServiceRequestType<SrvType> request,
    //     ros_wrapper::ServiceResponseType<SrvType> response)
    // {
    //     (void)request;
    //     response.success = true;
    //     response.message = "Trigger service called!";
    //     ROS_INFO("Trigger service was called.");
    //     return true;
    // }
#endif

} // namespace ros_flexbuild