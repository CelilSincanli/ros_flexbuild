#ifndef ROS_FLEXBUILD_TEST_HPP_
#define ROS_FLEXBUILD_TEST_HPP_

#include "ros_interface.hpp"

namespace ros_flexbuild {

// Use type aliases from ros_interface.hpp for generic code
using MsgType = ros_wrapper::MsgType;
using SrvType = ros_wrapper::SrvType;

class FlexBuildTestNode {
public:
    FlexBuildTestNode(ros_wrapper::NodeHandle node);

private:
    void messageCallback(ros_wrapper::MsgCallbackParamType msg);
    static bool triggerServiceCallback(
        ros_wrapper::ServiceRequestType<SrvType> request,
        ros_wrapper::ServiceResponseType<SrvType> response);

    void publishMessage();

    ros_wrapper::NodeHandle node_;
    std::shared_ptr<ros_wrapper::RosInterface<>> interface_;
    ros_wrapper::PublisherType<MsgType> publisher_;
    ros_wrapper::SubscriberType<MsgType> subscriber_;
    ros_wrapper::ServiceHandleType<SrvType> trigger_service_;
    ros_wrapper::TimerType timer_;
};

}  // namespace ros_flexbuild

#endif  // ROS_FLEXBUILD_TEST_HPP_
