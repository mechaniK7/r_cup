#include <ros/ros.h>
#include <geometry_msgs/Point.h>
#include <sensor_msgs/PointCloud.h> 
#include <geometry_msgs/TwistStamped.h>
#include <geometry_msgs/PoseStamped.h>
#include <sensor_msgs/LaserScan.h>
#include <drone_msgs/Goal.h>
#include <math.h>
#include <vector>
#include <iostream>
#include "tf2/LinearMath/Matrix3x3.h"
#include "tf2/LinearMath/Quaternion.h"
#include "tf2/LinearMath/Quaternion.h"
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>

using std::cout, std::endl;

float front_l; // Датчик направленный вперёд
float right1_l; // Датчик направленный направо, передний
float right2_l; // Датчик направленный направо, задний


ros::Publisher field_vel;
ros::Publisher goal_pose;

geometry_msgs::PoseStamped position;
geometry_msgs::Point ardu_nano_msg;
drone_msgs::Goal vec;

// Класс для работы с Эйлеровыми углами и кватернионами
class EulerAngles {
public:
EulerAngles()
{
roll = 0;
pitch = 0;
yaw = 0;
}
double roll;
double pitch;
double yaw;
// Функция для установки Эйлеровых углов по переданным параметрам
void setRPY(float new_roll, float new_pitch, float new_yaw)
{
roll = new_roll;
pitch = new_pitch;
yaw = new_yaw;
}
// Функция для передачи Эйлеровых углов в вектор вращения
void setRPY_of_quaternion(tf2::Quaternion &q)
{
q.setRPY(roll, pitch, yaw);
}
// Функция для получения Эйлеровых углов из вектора вращения
void get_RPY_from_quaternion(tf2::Quaternion q)
{
tf2::Matrix3x3 m(q);
m.getRPY(roll, pitch, yaw);
}
// Функция для получения Эйлеровых углов из вектора вращения заданного geometry_msgs::Quaternion
void get_RPY_from_msg_quaternion(geometry_msgs::Quaternion q)
{
tf2::Quaternion q_tf;
tf2::convert(q, q_tf);
get_RPY_from_quaternion(q_tf);
}
};
EulerAngles current_angles;

////////////////////
void pose(const geometry_msgs::PoseStamped& xyu) {
    position = xyu;
    current_angles.get_RPY_from_msg_quaternion(position.pose.orientation);
}

void subs(const geometry_msgs::Point& msg) {
    ardu_nano_msg = msg;
}
   


void check() {
    ros::NodeHandle n;
    ros::Subscriber laser_nano = n.subscribe("Laser_nano", 1000, subs);
    ros::Subscriber mavros_local_pose = n.subscribe("/mavros/local_position/pose", 1000, pose);
    field_vel = n.advertise<geometry_msgs::TwistStamped>("/field_vel", 1);
    goal_pose = n.advertise<drone_msgs::Goal>("/goal_pose", 1);
    
    ros::Rate loop_rate(5);

    while (ros::ok()) {
        
        
        front_l = ardu_nano_msg.x;
        right1_l = ardu_nano_msg.y;
        right2_l = ardu_nano_msg.z;



        /*vec.pose.point.x = position.pose.position.x;
        vec.pose.point.y = position.pose.position.y;
        vec.pose.point.z = 1.0;
        vec.pose.course = current_angles.yaw;
        Goal_pose.publish(vec);*/

        geometry_msgs::TwistStamped speed;
       
        ros::spinOnce();
        loop_rate.sleep();
    }
}

int main(int argc, char **argv) {
    ros::init(argc, argv, "runALC");
    check();

    return 0;
}
