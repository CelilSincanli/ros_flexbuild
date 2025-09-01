# ros_flexbuild

## Description

`ros_flexbuild` is a header-only C++ interface wrapper that enables writing generic ROS node code compatible with both ROS 1 and ROS 2.  
It provides unified type aliases and helper functions so you can build and run your code in either ROS ecosystem with minimal changes.  
The project includes Dockerfiles for building and testing in isolated ROS 1 and ROS 2 environments.

---

## How to Build Docker Images

### ROS 1 (Noetic)

1. Build the Docker image:
    ```bash
    sudo docker build -f Dockerfile.ros -t ros_flex_build .
    ```

2. Run the container:
    ```bash
    sudo docker run -it --rm ros_flex_build
    ```

### ROS 2 (Humble)

1. Build the Docker image:
    ```bash
    sudo docker build -f Dockerfile.ros2 -t ros2_flex_build .
    ```

2. Run the container:
    ```bash
    sudo docker run -it --rm ros2_flex_build
    ```

---

## Notes

- The Dockerfiles automatically build the workspace and set up the environment.
- You can select the timer period in milliseconds using the provided API.
- Launch files and entrypoints are configured for both ROS 1 and ROS 2.

