#ifndef ROS_INTERFACE_H_
#define ROS_INTERFACE_H_

#include <functional>
#include <memory>
#include <string>
#include <vector>

#ifdef ROS2_BUILD
    #include <rclcpp/rclcpp.hpp>
    #include <rcl_interfaces/msg/parameter_descriptor.hpp>
    #include <std_msgs/msg/string.hpp>
    #include <std_srvs/srv/trigger.hpp>
#else
    #include <ros/ros.h>
    #include <std_msgs/String.h>
    #include <std_srvs/Trigger.h>
#endif

namespace ros_wrapper {

#ifdef ROS2_BUILD

    #define ROS_WRAPPER_LOG_STREAM(level, node, stream_args) \
    do { \
      std::ostringstream __ros_stream__; \
      __ros_stream__ << stream_args; \
      RCLCPP_##level(node->get_logger(), "%s", __ros_stream__.str().c_str()); \
    } while(0)

    using NodeHandle = rclcpp::Node::SharedPtr;

    template<typename MsgT>
    using PublisherType = typename rclcpp::Publisher<MsgT>::SharedPtr;

    template<typename MsgT>
    using SubscriberType = typename rclcpp::Subscription<MsgT>::SharedPtr;

    template<typename ServiceT>
    using ServiceRequestType = std::shared_ptr<typename ServiceT::Request>;

    template<typename ServiceT>
    using ServiceResponseType = std::shared_ptr<typename ServiceT::Response>;

    using TimerType = rclcpp::TimerBase::SharedPtr;

    using DurationType = std::chrono::milliseconds;

    template<typename ServiceT>
    using ServiceHandleType = typename rclcpp::Service<ServiceT>::SharedPtr;


    
    class RosClient {
    public:
        explicit RosClient(NodeHandle node) : node_(node) {}

        template <typename T>
        bool GetParam(const std::string& name, T& value) {
            return node_->get_parameter(name, value);
        }

        template <typename T>
        T DeclareAndGetParam(const std::string& name, const T& default_value) {
            node_->declare_parameter<T>(name, default_value);
            return node_->get_parameter(name).get_value<T>();
        }

        template <typename T>
        T GetOrDeclareParam(const std::string& name, const T& default_value) {
            if (!node_->has_parameter(name)) {
                node_->declare_parameter(name, default_value);
            }
            return node_->get_parameter(name).get_value<T>();
        }

        template <typename T>
        void DeclareParamWithDescriptor(const std::string& name, const T& default_value,
                                        const rcl_interfaces::msg::ParameterDescriptor& descriptor) {
            node_->declare_parameter<T>(name, default_value, descriptor);
        }

        std::vector<std::string> GetStringListParam(const std::string& name) {
            return node_->get_parameter(name).as_string_array();
        }

        template <typename MsgT>
        PublisherType<MsgT> CreatePublisher(const std::string& topic, int queue_size) {
            return node_->create_publisher<MsgT>(topic, queue_size);
        }

        template <typename MsgT>
        SubscriberType<MsgT> CreateSubscriber(
            const std::string& topic, int queue_size,
            std::function<void(const typename MsgT::SharedPtr)> callback) {
            return node_->create_subscription<MsgT>(topic, queue_size, callback);
        }

        template <typename ServiceT>
        auto CreateService(
            const std::string& service_name,
            std::function<void(ServiceRequestType<ServiceT>, ServiceResponseType<ServiceT>)> callback) {
            return node_->create_service<ServiceT>(service_name, callback);
        }

        rclcpp::Time Now() const {
            return node_->now();
        }

        std::string GetNodeName() const {
            return node_->get_name();
        }

        template <typename CallbackT>
        TimerType CreateTimer(DurationType interval, CallbackT&& callback) {
            return node_->create_wall_timer(interval, std::forward<CallbackT>(callback));
        }

    private:
        NodeHandle node_;
    };

#else // ROS 1

    #define ROS_WRAPPER_LOG_STREAM(level, node, stream_args) \
    do { \
        std::ostringstream __ros_stream__; \
        __ros_stream__ << stream_args; \
        ROS_##level("%s", __ros_stream__.str().c_str()); \
    } while(0)

    using NodeHandle = ros::NodeHandle*;

    template<typename MsgT>
    using PublisherType = ros::Publisher;

    template<typename MsgT>
    using SubscriberType = ros::Subscriber;

    template<typename ServiceT>
    using ServiceRequestType = typename ServiceT::Request&;

    template<typename ServiceT>
    using ServiceResponseType = typename ServiceT::Response&;

    using TimerType = ros::Timer;

    using DurationType = ros::Duration;

    template<typename ServiceT>
    using ServiceHandleType = ros::ServiceServer;

    class RosClient {
    public:
        explicit RosClient(NodeHandle nh) : node_handle_(*nh) {}

        template <typename T>
        bool GetParam(const std::string& name, T& value) {
            return node_handle_.getParam(name, value);
        }

        template <typename T>
        T DeclareAndGetParam(const std::string& name, const T& default_value) {
            T value = default_value;
            node_handle_.param<T>(name, value, default_value);
            return value;
        }

        template <typename T>
        T GetOrDeclareParam(const std::string& name, const T& default_value) {
            T value = default_value;
            node_handle_.param<T>(name, value, default_value);
            return value;
        }

        template <typename T>
        void DeclareParamWithDescriptor(const std::string& name, const T& default_value,
                                        const rcl_interfaces::msg::ParameterDescriptor& /*descriptor*/) {
            (void)name; (void)default_value;
        }

        std::vector<std::string> GetStringListParam(const std::string& name) {
            std::vector<std::string> result;
            (void)name;
            return result;
        }

        template <typename MsgT>
        PublisherType<MsgT> CreatePublisher(const std::string& topic, int queue_size) {
            return node_handle_.advertise<MsgT>(topic, queue_size);
        }

        template <typename MsgT>
        SubscriberType<MsgT> CreateSubscriber(
            const std::string& topic, int queue_size,
            void (*callback)(const typename MsgT::ConstPtr&)) {
            return node_handle_.subscribe<MsgT>(topic, queue_size, callback);
        }

        template <typename ServiceT>
        auto CreateService(
            const std::string& service_name,
            bool (*callback)(ServiceRequestType<ServiceT>, ServiceResponseType<ServiceT>)) {
            return node_handle_.advertiseService(service_name, callback);
        }

        ros::Time Now() const {
            return ros::Time::now();
        }

        std::string GetNodeName() const {
            return node_handle_.getNamespace();
        }

        template <typename CallbackT>
        TimerType CreateTimer(DurationType interval, CallbackT callback) {
            return node_handle_.createTimer(interval, callback);
        }

    private:
        ros::NodeHandle node_handle_;
    };

#endif // ROS2_BUILD

    #define ROS_LOG_INFO(node, stream_args)  ROS_WRAPPER_LOG_STREAM(INFO, node, stream_args)
    #define ROS_LOG_WARN(node, stream_args)  ROS_WRAPPER_LOG_STREAM(WARN, node, stream_args)
    #define ROS_LOG_ERROR(node, stream_args) ROS_WRAPPER_LOG_STREAM(ERROR, node, stream_args)
    #define ROS_LOG_DEBUG(node, stream_args) ROS_WRAPPER_LOG_STREAM(DEBUG, node, stream_args)

    template <typename Impl = RosClient>
    class RosInterface {
    public:
        explicit RosInterface(NodeHandle node) : client_(node) {}

        template <typename T>
        bool GetParam(const std::string& name, T& value) {
            return client_.template GetParam<T>(name, value);
        }

        template <typename T>
        T DeclareAndGetParam(const std::string& name, const T& default_value) {
            return client_.template DeclareAndGetParam<T>(name, default_value);
        }

        template <typename T>
        T GetOrDeclareParam(const std::string& name, const T& default_value) {
            return client_.template GetOrDeclareParam<T>(name, default_value);
        }

        template <typename T>
        void DeclareParamWithDescriptor(const std::string& name, const T& default_value,
                                        const rcl_interfaces::msg::ParameterDescriptor& descriptor) {
            client_.template DeclareParamWithDescriptor<T>(name, default_value, descriptor);
        }

        std::vector<std::string> GetStringListParam(const std::string& name) {
            return client_.GetStringListParam(name);
        }

        template <typename MsgT>
        auto CreatePublisher(const std::string& topic, int queue_size) {
            return client_.template CreatePublisher<MsgT>(topic, queue_size);
        }

        template <typename MsgT, typename CallbackT>
        auto CreateSubscriber(const std::string& topic, int queue_size, CallbackT callback) {
            return client_.template CreateSubscriber<MsgT>(topic, queue_size, callback);
        }

        template <typename ServiceT, typename CallbackT>
        auto CreateService(const std::string& service_name, CallbackT callback) {
            return client_.template CreateService<ServiceT>(service_name, callback);
        }

        auto Now() const {
            return client_.Now();
        }

        std::string GetNodeName() const {
            return client_.GetNodeName();
        }

        template <typename CallbackT>
        TimerType CreateTimer(DurationType interval, CallbackT callback) {
            return client_.CreateTimer(interval, callback);
        }

    private:
        Impl client_;
    };

} // namespace ros_wrapper

#endif /* ROS_INTERFACE_H_ */
