#ifndef ROS_FLEXBUILD_TEST_HPP_
#define ROS_FLEXBUILD_TEST_HPP_

#include "ros_interface.hpp"

namespace ros_flexbuild {

#ifdef ROS2_BUILD
    using MsgType = std_msgs::msg::String;
    using SrvType = std_srvs::srv::Trigger;
    using MsgCallbackParamType = std_msgs::msg::String::SharedPtr;
#else
    using MsgType = std_msgs::String;
    using SrvType = std_srvs::Trigger;
    using MsgCallbackParamType = const std_msgs::String::ConstPtr&;
#endif

class FlexBuildTestNode {
public:
    FlexBuildTestNode(ros_wrapper::NodeHandle node);

private:
    void messageCallback(MsgCallbackParamType msg);

    #ifdef ROS2_BUILD
        void triggerServiceCallback(
            ros_wrapper::ServiceRequestType<SrvType> request,
            ros_wrapper::ServiceResponseType<SrvType> response);
    #else
        bool triggerServiceCallback(
            ros_wrapper::ServiceRequestType<SrvType> request,
            ros_wrapper::ServiceResponseType<SrvType> response);
    #endif

    void publishMessage();

    ros_wrapper::NodeHandle node_;
    std::shared_ptr<ros_wrapper::RosInterface<>> interface_;
    ros_wrapper::PublisherType<MsgType> publisher_;
    ros_wrapper::SubscriberType<MsgType> subscriber_;
    ros_wrapper::TimerType timer_;
    ros_wrapper::ServiceServer<SrvType> service_server_;
    ros_wrapper::ServiceClient<SrvType> service_client_;
};

}  // namespace ros_flexbuild

#endif  // ROS_FLEXBUILD_TEST_HPP_
