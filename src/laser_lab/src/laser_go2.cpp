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
ros::Publisher pub;
ros::Publisher pub2;
bool flag = false;

geometry_msgs::PoseStamped position;
float kp = 0.4; // Коэффициент пропорциональной составляющей 0.1
float kd = 0.2; // Коэффициент дифференциальной составляющей 0.2

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




//void subs(const geometry_msgs::Point& msg) {

  void pose(const geometry_msgs::PoseStamped& xyu) {
    position = xyu;
  }

  void subs(const sensor_msgs::LaserScan& msg) {

    EulerAngles angles;
    drone_msgs::Goal vec;

    angles.get_RPY_from_msg_quaternion(position.pose.orientation);
    vec.pose.point.x = position.pose.position.x;
    vec.pose.point.y = position.pose.position.y;
    vec.pose.point.z = 1.0;
    vec.pose.course = angles.yaw;
    pub2.publish(vec);

    geometry_msgs::TwistStamped speed;
    int left = msg.ranges.at(50);
    int right = msg.ranges.at(130);
    float err, errOLD;
    err = left - right;
    float P  =  err * kp; // Вычисляем пропорциональную составляющую
    float D  = (err - errOLD) * kd; // Вычисляем дифференциальную составляющую
    errOLD = err;             
    float PD = P + D; // Получаем значение от ПД-регулятора


    if (msg.ranges.at(180) <= 1) {
        /*angles.get_RPY_from_msg_quaternion(position.pose.orientation);
        vec.pose.point.x = position.pose.position.x;
        vec.pose.point.y = position.pose.position.y;
        vec.pose.point.z = 1.0;
        vec.pose.course = angles.yaw;
        pub2.publish(vec);*/
        if (flag == false) {
            flag = true;
            speed.twist.linear.x = 0.0;
            speed.twist.linear.y = 0.0;
            pub.publish(speed);
            ros::Duration(1).sleep();
            vec.pose.course += 1;
            pub2.publish(vec);
            speed.twist.linear.x = 0.0;
            speed.twist.linear.y = 0.0;
            pub.publish(speed);
            ros::Duration(1).sleep();
        }
        else {
            speed.twist.linear.x = 0;
            pub.publish(speed);
            vec.pose.course += 1.5;
            pub2.publish(vec);
        }
        

        
    }
    else {
        if (flag == false) { speed.twist.linear.x = 0.2; speed.twist.linear.y = 0.0; }
        if (flag == true) { speed.twist.linear.x = 0.0; speed.twist.linear.y = 0.2; }
        //speed.twist.angular.z = PD*3;
        vec.pose.course += PD;
        pub.publish(speed);
        pub2.publish(vec);
    }

    
    cout << "FRONT - " << msg.ranges.at(180) << endl; 
    cout << "L - " << msg.ranges.at(50) << endl; 
    cout << "R - " << msg.ranges.at(130) << endl; 
    cout << "PD - " << PD << endl; 

}
   


void check() {
    ros::NodeHandle n;
    //startTime = ros::Time::now().sec;
    //ros::Subscriber sub = n.subscribe("Laser_nano", 1000, subs);
    ros::Subscriber sub = n.subscribe("scan", 1000, subs);
    ros::Subscriber sub2 = n.subscribe("/mavros/local_position/pose", 1000, pose);
    pub = n.advertise<geometry_msgs::TwistStamped>("/field_vel", 1);
    pub2 = n.advertise<drone_msgs::Goal>("/goal_pose", 1);
    
    ros::Rate loop_rate(5);
    
    while (ros::ok()) {
        ros::spinOnce();
        loop_rate.sleep();
    }
}

int main(int argc, char **argv) {
    ros::init(argc, argv, "ahuet2");
    
    check();
    return 0;
}
