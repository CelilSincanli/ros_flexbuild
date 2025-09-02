from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        Node(
            package='ros_flexbuild',
            executable='ros2_test_node',
            name='flexbuild_node',
            output='screen',
            parameters=[{
                'greeting_target': 'world',
                'service_server_name': 'trigger_server_service',
                'server_client_name': 'trigger_client_service',
                'timer_period': 100 #ms
            }]
        )
    ])
