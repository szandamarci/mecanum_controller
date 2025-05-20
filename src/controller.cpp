#include <memory>
#include <chrono>
#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/transform_stamped.hpp>
#include "geometry_msgs/msg/twist.hpp"
#include <tf2/LinearMath/Transform.h>
#include <tf2_ros/transform_broadcaster.h>
#include <tf2_ros/transform_listener.h>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2_ros/buffer.h>
#include <tf2/utils.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>

#include "mecanum_common/srv/set_configuration.hpp"
#include "mecanum_common/srv/set_speed.hpp"
#include "Spi.h"

using namespace mecanum;
using std::placeholders::_1;
using std::placeholders::_2;
Spi spi;
SpiMessage TxMsg, RxMsg;
std::shared_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster;
std::shared_ptr<tf2_ros::Buffer> tf_buffer;
std::shared_ptr<tf2_ros::TransformListener> tf_listener;
rclcpp::Node::SharedPtr node;
float temp=0.0;

void updateRobotPose()
{   
    try {
    geometry_msgs::msg::TransformStamped transformStamped;
    transformStamped = tf_buffer->lookupTransform("odom", "base_footprint", tf2::TimePointZero);

    TxMsg.conf.x = static_cast<float>(transformStamped.transform.translation.x);
    TxMsg.conf.y = static_cast<float>(transformStamped.transform.translation.y);
    TxMsg.conf.theta = static_cast<float>(tf2::getYaw(transformStamped.transform.rotation));
    }
    catch (tf2::TransformException &ex){
        RCLCPP_ERROR(node->get_logger(), "TF error: %s", ex.what());
    }

}

void cmdVelCallback(const geometry_msgs::msg::Twist::SharedPtr msg)
{
    if(msg->linear.x>0.1)
    {
        msg->linear.x=0.1;
    }
    if(msg->linear.x<-0.1)
    {
        msg->linear.x=-0.1;
    }
    if(msg->linear.y>0.1)
    {
        msg->linear.y=0.1;
    }
    if(msg->linear.y<-0.1)
    {
        msg->linear.y=-0.1;
    }
    if(msg->angular.z>0.1)
    {
        msg->angular.z=0.1;
    }
    if(msg->angular.z<-0.1)
    {
        msg->angular.z=-0.1;
    }
    TxMsg.speed.y = static_cast<float>((msg->linear.x));
    TxMsg.speed.x = static_cast<float>(-1*(msg->linear.y));
    TxMsg.speed.w = static_cast<float>(msg->angular.z);
    
    RCLCPP_INFO(node->get_logger(), "Speed: x=%.3f, y=%.3f, theta=%.3f", TxMsg.speed.x, TxMsg.speed.y, TxMsg.speed.w);

}

int main(int argc, char** argv)
{

    rclcpp::init(argc, argv);
    node = std::make_shared<rclcpp::Node>("mecanum_controller", rclcpp::NodeOptions().arguments({"--ros-args", "--log-level", "debug"}));
    RCLCPP_INFO(node->get_logger(), "Eddig jut el");
    
    if (!spi.init())
    {
        RCLCPP_ERROR(node->get_logger(), "Failed to initialize SPI");
        rclcpp::shutdown();
        return 1;

    }
    TxMsg.speed = {0, 0, 0};
    TxMsg.conf = {0, 0, 0};
    TxMsg.sync = MESSAGE_SYNC_VALUE;
    
    RxMsg.speed = {0, 0, 0};
    RxMsg.conf = {0, 0, 0};
    RxMsg.sync = MESSAGE_SYNC_VALUE;

    tf_buffer = std::make_shared<tf2_ros::Buffer>(node->get_clock());
    tf_listener = std::make_shared<tf2_ros::TransformListener>(*tf_buffer);
    tf_broadcaster = std::make_shared<tf2_ros::TransformBroadcaster>(node);
    geometry_msgs::msg::TransformStamped transformStamped;

    auto cmd_vel_subscriber = node->create_subscription<geometry_msgs::msg::Twist>("/cmd_vel", 10, cmdVelCallback);
    RCLCPP_INFO(node->get_logger(), "Subscribed to /cmd_vel");
    
    rclcpp::Rate loop_rate(10);
    while (rclcpp::ok())
    {
        

        if(spi.transferSignal())
        {
            if(spi.transfer(TxMsg, RxMsg))
            {
                RCLCPP_INFO(node->get_logger(), "Transmitted speed: x=%.3f, y=%.3f, w=%.3f", TxMsg.speed.x, TxMsg.speed.y, TxMsg.speed.w);
                transformStamped.header.stamp = node->get_clock()->now();
                transformStamped.header.frame_id = "odom";
                transformStamped.child_frame_id = "base_footprint";
                transformStamped.transform.translation.y = -RxMsg.conf.x;
                transformStamped.transform.translation.x= RxMsg.conf.y;
                transformStamped.transform.translation.z = 0.0;
                
                if (abs(RxMsg.conf.theta - temp)>0.004)
                {
                tf2::Quaternion q;
                q.setRPY(0, 0, RxMsg.conf.theta);
                transformStamped.transform.rotation.x = q.x();
                transformStamped.transform.rotation.y = q.y();
                transformStamped.transform.rotation.z = q.z();
                transformStamped.transform.rotation.w = q.w();
                
                
                updateRobotPose();
                }
                
                temp=RxMsg.conf.theta;
                tf_broadcaster->sendTransform(transformStamped);
                RCLCPP_INFO(node->get_logger(), "Received pose: x=%.3f, y=%.3f, theta=%.3f", RxMsg.conf.x, RxMsg.conf.y, RxMsg.conf.theta);
                
            }
            else
            {
                RCLCPP_INFO(node->get_logger(), "SPI Error");
            }
        }
        else{

            RCLCPP_INFO(node->get_logger(), "SPI transferSignalError");

        }

        rclcpp::spin_some(node);
        loop_rate.sleep();
    }
    tf_buffer.reset();
    tf_listener.reset();
    rclcpp::shutdown();
    return 0;

    
    

}
