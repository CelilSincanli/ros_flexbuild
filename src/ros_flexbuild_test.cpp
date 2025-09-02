#include "ros_flexbuild_test/ros_flexbuild_test.hpp"

namespace ros_flexbuild {

    FlexBuildTestNode::FlexBuildTestNode(ros_wrapper::NodeHandle node) : node_(node) {
        interface_ = std::make_shared<ros_wrapper::RosInterface<>>(node_);

        std::string greeting_target = interface_->DeclareAndGetParam("greeting_target", std::string("world"));
        ROS_LOG_WARN(node_, "Greeting target: " << greeting_target);

        publisher_ = interface_->CreatePublisher<MsgType>("greeting_topic", 10);
        subscriber_ = interface_->CreateSubscriber<MsgType>(
            "greeting_topic", 10,
            ros_wrapper::bind(&FlexBuildTestNode::messageCallback, this, ros_wrapper::_1)
        );

        service_server_ = interface_->CreateService<SrvType>(
            "trigger_server", 
            ros_wrapper::bind(&FlexBuildTestNode::triggerServiceCallback, this, ros_wrapper::_1, ros_wrapper::_2)
        );

        service_client_ = interface_->CreateServiceClient<SrvType>("trigger_server");

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

            // ROS2 service client async call
            std::shared_ptr<std_srvs::srv::Trigger::Request> request = std::make_shared<std_srvs::srv::Trigger::Request>();
            
            // Start async request without blocking the main thread
            if (service_client_->wait_for_service(std::chrono::milliseconds(10))) {
                auto result = service_client_->async_send_request(request);
                // Do not block, just initiate the request and keep going
            } else {
                ROS_LOG_WARN(node_, "Service not available. Skipping service call.");
            }

        #else // ROS1
            publisher_.publish(msg);

            // ROS1 service client call (non-blocking)
            std_srvs::Trigger::Request request;
            if (service_client_.exists()) {
                // In ROS1, we call synchronously, but you can modify this for async if needed
                service_client_.call(request);
            } else {
                ROS_LOG_WARN(node_, "Service not available. Skipping service call.");
            }
        #endif
    }


    #ifdef ROS2_BUILD
        void FlexBuildTestNode::messageCallback(const MsgType::SharedPtr msg) {
            ROS_LOG_INFO(node_, "Received: '" << msg->data << "'");
        }

        void FlexBuildTestNode::triggerServiceCallback(
            ros_wrapper::ServiceRequestType<SrvType> request,
            ros_wrapper::ServiceResponseType<SrvType> response) {
            (void)request;
            response->success = true;
            response->message = "Trigger service called!";
            ROS_LOG_INFO(node_, "Trigger service was called.");
        }
    #else
        void FlexBuildTestNode::messageCallback(const MsgType::ConstPtr& msg) {
            ROS_LOG_INFO(node_, "Received: '" << msg->data << "'");
        }

        bool FlexBuildTestNode::triggerServiceCallback(
            ros_wrapper::ServiceRequestType<SrvType> request,
            ros_wrapper::ServiceResponseType<SrvType> response) {
            (void)request;
            response.success = true;
            response.message = "Trigger service called!";
            ROS_LOG_INFO(node_, "Trigger service was called.");
            return true;
        }
    #endif

} // namespace ros_flexbuild