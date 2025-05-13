#include "rclcpp/rclcpp.hpp"
#include "tf2_ros/transform_broadcaster.h"
#include "geometry_msgs/msg/transform_stamped.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"
#include "Scanner.h"

using namespace Leuze;
using namespace Leuze::ROD4;

class LaserScannerNode : public rclcpp::Node {
public:
    LaserScannerNode() : Node("mecanum_laser_scanner"), tf_broadcaster_(this) {
        // Create Publisher
        scanner_publisher_ = this->create_publisher<sensor_msgs::msg::LaserScan>(
            "mecanum/laserScannerData", 10
        );

        // Trying to connect to LaserScanner
        while (!scanner_.connect()) {
            RCLCPP_WARN(this->get_logger(), "Failed to connect to scanner, retrying in 3 seconds...");
            rclcpp::sleep_for(std::chrono::seconds(3));
        }

        scanner_.enableMeasurement(true);
        timer_ = this->create_wall_timer(std::chrono::milliseconds(20),
                                         std::bind(&LaserScannerNode::scan_callback, this));
    }

private:
    void scan_callback() {
        auto scan_msg = sensor_msgs::msg::LaserScan();
        scan_msg.header.frame_id = "mecanum/laserScanner";
        scan_msg.header.stamp = this->get_clock()->now();
        scan_msg.angle_min = -0.08726646259;
        scan_msg.angle_max = 3.22885911619;
        scan_msg.angle_increment = 2 * 1.65806278939 / (529 - 1);
        scan_msg.range_min = 0;
        scan_msg.range_max = 65;
        scan_msg.scan_time = 0.02;
        scan_msg.ranges.resize(529, 0);

        // Receive scanner data
        Datagram* pDatagram = new Datagram();
        while (!scanner_.receiveDatagram(pDatagram)) {
            rclcpp::sleep_for(std::chrono::milliseconds(3));
        }

        for (int i = 0; i < 529; i++) {
            scan_msg.ranges[i] = static_cast<double>(pDatagram->distances[528 - i]) / 1000.0;
        }
        delete pDatagram;

        // Publish scan message
        scanner_publisher_->publish(scan_msg);

        // Broadcast transform
        geometry_msgs::msg::TransformStamped transform;
        transform.header.stamp = this->get_clock()->now();
        transform.header.frame_id = "mecanum/robot";
        transform.child_frame_id = "mecanum/laserScanner";
        transform.transform.translation.x = 0.0;
        transform.transform.translation.y = 0.0;
        transform.transform.translation.z = 0.2;
        transform.transform.rotation.w = 1.0;  // No rotation

        tf_broadcaster_.sendTransform(transform);
    }

    Scanner scanner_;
    rclcpp::Publisher<sensor_msgs::msg::LaserScan>::SharedPtr scanner_publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
    tf2_ros::TransformBroadcaster tf_broadcaster_;
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<LaserScannerNode>());
    rclcpp::shutdown();
    return 0;
}
