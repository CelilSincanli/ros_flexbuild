#include "ros_flexbuild_test/ros_flexbuild_test.hpp"

namespace ros_flexbuild {

    FlexBuildTestNode::FlexBuildTestNode(ros_wrapper::NodeHandle node) : node_(node) {
        interface_ = std::make_shared<ros_wrapper::RosInterface<>>(node_);

        std::string greeting_target = interface_->DeclareAndGetParam("greeting_target", std::string("world"));
        ROS_LOG_WARN(node_, "Greeting target: " << greeting_target);
        std::string server_service_name = interface_->DeclareAndGetParam("service_server_name", std::string("trigger_server_service"));
        ROS_LOG_WARN(node_, "Server service name: " << server_service_name);
        std::string server_client_name = interface_->DeclareAndGetParam("server_client_name", std::string("trigger_client_service"));
        ROS_LOG_WARN(node_, "Server client name: " << server_client_name);

        // Get timer period from param, default to 100 ms
        int timer_period = interface_->DeclareAndGetParam("timer_period", 100);
        ROS_LOG_WARN(node_, "Timer period: " << timer_period << " ms");

        publisher_ = interface_->CreatePublisher<MsgType>("greeting_topic", 10);
        subscriber_ = interface_->CreateSubscriber<MsgType>(
            "greeting_topic", 10,
            ros_wrapper::bind(&FlexBuildTestNode::messageCallback, this, ros_wrapper::_1)
        );

        service_server_ = interface_->CreateService<SrvType>(
            server_service_name, 
            ros_wrapper::bind(&FlexBuildTestNode::triggerServiceCallback, this, ros_wrapper::_1, ros_wrapper::_2)
        );

        service_client_ = interface_->CreateServiceClient<SrvType>(server_client_name);

        timer_ = interface_->CreateTimer(
            ros_wrapper::getTimerDuration(timer_period),
            &FlexBuildTestNode::publishMessage,
            this
        );
    }

    void FlexBuildTestNode::publishMessage() {
        MsgType msg;
        msg.data = "Hello from ros_flexbuild!";
        ROS_LOG_WARN(node_, "Hello from ros_flexbuild!");
        #ifdef ROS2_BUILD
            publisher_->publish(msg);

            std::shared_ptr<std_srvs::srv::Trigger::Request> request = std::make_shared<std_srvs::srv::Trigger::Request>();
            
            if (service_client_->wait_for_service(std::chrono::milliseconds(10))) {
                auto result = service_client_->async_send_request(request);
            } else {
                ROS_LOG_WARN(node_, "Service not available. Skipping service call.");
            }

        #else
            publisher_->publish(msg);

            std_srvs::Trigger::Request request;
            std_srvs::Trigger::Response response;

            if (service_client_.waitForExistence(ros::Duration(1))) {
                // Call the service, but don't block the thread
                bool success = service_client_.call(request, response);  
                if (success) {
                    ROS_LOG_INFO(node_, "Service called successfully");
                } else {
                    ROS_LOG_WARN(node_, "Failed to call service");
                }
            } else {
                ROS_LOG_WARN(node_, "Service not available. Skipping service call.");
            }
        #endif
        ROS_LOG_INFO(node_, "end of the timer");
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