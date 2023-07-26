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
#include "tf2/LinearMath/Quaternion.h"

//tf2::Quaternion::EU

using std::cout, std::endl;
ros::Publisher pub;
bool flag = false;
float position;
float kp = 0.1; // Коэффициент пропорциональной составляющей 0.1
float kd = 0.2; // Коэффициент дифференциальной составляющей 0.2

  void pose(const geometry_msgs::PoseStamped& xyu) {
    position = xyu.pose.position.z;

  }

  //void subs(const geometry_msgs::Point& msg) {
  void subs(const sensor_msgs::LaserScan& msg) {


    geometry_msgs::TwistStamped vec;
    //vec.header.frame_id = "laser";
    //int left = msg.ranges.at(50);
    //int right = msg.ranges.at(130);
    float err, errOLD;
    err = 1 - position;
    float P  =  err * kp; // Вычисляем пропорциональную составляющую
    float D  = (err - errOLD) * kd; // Вычисляем дифференциальную составляющую
    errOLD = err;             
    float PD = P + D;                      // Получаем значение от ПД-регулятора

    vec.twist.linear.x = 0;
    vec.twist.linear.y = 0;
    vec.twist.linear.z = PD;

    /*vec.pose.point.x = 2;
    vec.pose.course = PD;
    //vec.pose.point.y = 0;
    vec.pose.point.z = 1;*/
    
    //vec.twist.linear.x = 0.2 - u;
    //vec.twist.angular.z = 0.2 * u; 
    //pub.publish(vec); 
    cout << "FRONT - " << msg.ranges.at(180) << endl; 
    cout << "L - " << msg.ranges.at(50) << endl; 
    cout << "R - " << msg.ranges.at(130) << endl; 
    //cout << "PD - " << PD << endl; 

}

void check() {
    ros::NodeHandle n;
    
   //ros::Subscriber sub = n.subscribe("Laser_nano", 1000, subs);
    ros::Subscriber sub2 = n.subscribe("/mavros/local_position/pose", 1000, pose);
    ros::Subscriber sub = n.subscribe("scan", 1000, subs);
    //ros::Subscriber sub2 = n.subscribe("", 1000, cmvel);
    pub = n.advertise<geometry_msgs::TwistStamped>("/mavros/setpoint_velocity/cmd_vel", 1);
    
    ros::Rate loop_rate(5);
    
    while (ros::ok()) {
        ros::spinOnce();
        loop_rate.sleep();
    }
}

int main(int argc, char **argv) {
    ros::init(argc, argv, "ahuet3");
    
    check();
    return 0;
}
