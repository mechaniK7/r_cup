#include <ros/ros.h>
#include <geometry_msgs/Point.h>
#include <sensor_msgs/PointCloud.h> 
#include <geometry_msgs/TwistStamped.h>
#include <geometry_msgs/PoseStamped.h>
#include <math.h>
#include <vector>
#include <iostream>
#include "tf2/LinearMath/Quaternion.h"

//tf2::Quaternion::EU

using std::cout, std::endl;
ros::Publisher pub;
bool flag = false;
geometry_msgs::PoseStamped drone_pose;

void pose(const geometry_msgs::PoseStamped& ps) 
{
    drone_pose = ps;
}   

void subs(const geometry_msgs::Point& msg) {
   
    geometry_msgs::TwistStamped vec;
    vec.header.frame_id = "laser";
    float front, right1, right2;
    front = msg.x;
    right1 = msg.y;
    right2 = msg.z;
    float front_n = 1;
    float right_n_sred = (right1 + right2) / 2;

    // Start run
    if (flag == false) {
    if (front <= front_n) {
        if (right1 <= right_n_sred) { // A
            vec.twist.angular.x = 1.0; // Поворот против часовой стрелки
            pub.publish(vec);
            flag = true;
        }
        else { // C
            vec.twist.angular.x = -1.0; // Поворот по часовой стрелке
            pub.publish(vec);

            while (right1 > right_n_sred) { vec.twist.linear.x = 1.0; pub.publish(vec);} // Движемся вперёд пока не увидим стену сбоку
            flag = true;
        }
    }
    else {
        if (right1 <= right_n_sred) { // B
            // Стоим на месте
            flag = true;
        }
        else { // D
            while (right1 > right_n_sred) { vec.twist.linear.x = 1.0; pub.publish(vec);} // Движемся вперёд пока не увидим стену сбоку
            flag = true;
        }
    }
    }
    // Algoritm
    while (flag == true) {
        if (right1 <= right_n_sred) {  // Если правый видит
            if (front <= front_n) { // B
                vec.twist.angular.x = 1.0; // Поворот против часовой стрелки 
                pub.publish(vec);
            }
            else { // Основное движение вперёд по лабиринту 
                ///// Алгоритм движения по 2 датчикам сбоку
                
                vec.twist.linear.x = 1.0;
                pub.publish(vec);
            }
        }
        else { // Если правый не видит
                vec.twist.angular.x = 1.0; // Поворот против часовой стрелки 
                pub.publish(vec);
        }
    }





    
   // vec.twist.linear.x = 1.0;
   // vec.twist.linear.y = 0.0;
   // pub.publish(vec); 
   
}
   


void check() {
    ros::NodeHandle n;
    
    ros::Subscriber sub2 = n.subscribe("/mavros/local_position/pose", 1000, pose);
    ros::Subscriber sub = n.subscribe("Laser_nano", 1000, subs);
    pub = n.advertise<geometry_msgs::TwistStamped>("Markers", 1);
    
    ros::Rate loop_rate(5);
    
    while (ros::ok()) {
        ros::spinOnce();
        loop_rate.sleep();
    }
}

int main(int argc, char **argv) {
    ros::init(argc, argv, "ahuet");
   
    check();
    return 0;
}
