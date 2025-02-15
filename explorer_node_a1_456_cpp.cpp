/*
 * explorer_node_a1_456.cpp
 *
 * BLG456E Assignment 1 skeleton
 *
 * Instructions: Change the laser_callback function to make the robot explore more intelligently, using its sensory data (the laser range array).
 *
 * Advanced: If you want to make use of the robot's mapping subsystem then you can make use of the map in the mapping_callback function.
 *
 */

//Common ROS headers.
#include "ros/ros.h"

//This is needed for the data structure containing the motor command.
#include "geometry_msgs/Twist.h"
//This is needed for the data structure containing the laser scan.
#include "sensor_msgs/LaserScan.h"
//This is needed for the data structure containing the map (which you may not use).
#include "nav_msgs/OccupancyGrid.h"
//This is for easy printing to console.
#include <iostream>
#include <cmath>
#include <chrono>
#include <ctime>


/*
 * Robotics
 * Homework 1
 * Student name: Sabrina Cara
 * Student number : 150160914
 * */

// For information on what publishing and subscribing is in ROS, look up the tutorials.
ros::Publisher motor_command_publisher;
ros::Subscriber laser_subscriber;
ros::Subscriber map_subscriber;

// For information on what a "message" is in ROS, look up the tutorials.
sensor_msgs::LaserScan laser_msg;
nav_msgs::OccupancyGrid map_msg;
geometry_msgs::Twist motor_command;

//The following function is a "callback" function that is called back whenever a new laser scan is available.
//That is, this function will be called for every new laser scan.
//
// --------------------------------------------------------------------------------------------
//CHANGE THIS FUNCTION TO MAKE THE ROBOT EXPLORE INTELLIGENTLY.
// --------------------------------------------------------------------------------------------
//

        
void laser_callback(const sensor_msgs::LaserScan::ConstPtr& msg)
{
    laser_msg=*msg;
    //data structure containing the command to drive the robot

    //for now let us just set it up to drive forward ...
    motor_command.linear.x=1;
    //...and left a bit
    motor_command.angular.z=0;

    //let's publish that command so that the robot follows it
    motor_command_publisher.publish(motor_command);

    //Alternatively we could have looked at the laser scan BEFORE we made this decision.
    //Well, let's see how we might use a laser scan.
    std::vector<float> laser_ranges;
    laser_ranges=laser_msg.ranges;
    double distance = 1.20; //this is a threshold value that robot cannot pass
    double distance_angular = 0.75;
    double right  = laser_ranges[0]; //we assign the laser values to doubles for simplicity in code reading
    double left = laser_ranges[laser_ranges.size()-1];
    double middle = laser_ranges[laser_ranges.size()/2];
    

	if(middle <= distance){ //if middle distance scanned by robot gets smaller than threshold, it means robot is getting closer to an obstacle.
		
		if ((right > left) && (!std::isnan(left))){ //if right distance is bigger than left distance, turn right	
			std::cout<<"Going Right"<<std::endl;
			motor_command.linear.x = -0.2; //We make linear negative, so it can go a little backwards
			motor_command.angular.z = -3.0; //we make angular negative, because negative is clockwise, which means it would turn right
		}
		else if((right < left) && (!std::isnan(right))) { //if right is smaller than left, turn left
			std::cout<<"Going Left"<<std::endl;
			motor_command.linear.x = -0.2;
			motor_command.angular.z = 3.0;//Angular vel positive means counterclockwise, which would turn it left
		}
	}
	else if(middle > distance){// This condition covers the case when the wall is not straight but inclined.
		//This means we would have to check also left distance and right distance when middle does not pass the threshold.
		if(left < distance_angular){ //If left wall is closer we turn right without going back
			std::cout<<"Going Right 2"<<std::endl;
			motor_command.angular.z = -1.0;
			motor_command.linear.x = 0;
		}
		else if (right < distance_angular){ //If right wall is closer we turn left without going back
			std::cout<<"Going Left 2"<<std::endl;
			motor_command.angular.z = 1.0;
			motor_command.linear.x = 0;
		}
		else { //If none of the conditions is satisfied, robot keeps walking straight
		std::cout<<"Going Straight"<<std::endl;
		motor_command.linear.x = 1.0;
		motor_command.angular.z = 0;
		}
	}
	
	motor_command_publisher.publish(motor_command);
    //the laser scan is an array (vector) of distances.
    std::cout<<"Number of points in laser scan is: "<<laser_ranges.size()<<std::endl;
    std::cout<<"The distance to the rightmost scanned point is "<<laser_ranges[0]<<std::endl;
    std::cout<<"The distance to the leftmost scanned point is "<<laser_ranges[laser_ranges.size()-1]<<std::endl;
    std::cout<<"The distance to the middle scanned point is "<<laser_ranges[laser_ranges.size()/2]<<std::endl;

    //You can use basic trignometry with the above scan array and the following information to find out exactly where the laser scan found something:
    std::cout<<"The minimum angle scanned by the laser is "<<laser_msg.angle_min<<std::endl;
    std::cout<<"The maximum angle scanned by the laser is "<<laser_msg.angle_max<<std::endl;
    std::cout<<"The increment in angles scanned by the laser is "<<laser_msg.angle_increment<<std::endl; //should be that angle_min+angle_increment*laser_ranges.size() is about angle_max
    std::cout<<"The minimum range (distance) the laser can perceive is "<<laser_msg.range_min<<std::endl;
    std::cout<<"The maximum range (distance) the laser can perceive is "<<laser_msg.range_max<<std::endl;

}
//
// --------------------------------------------------------------------------------------------
//

//You can also make use of the map which is being built by the "gslam_mapping" subsystem
//There is some code here to help but you can understand the API also
// by looking up the OccupancyGrid message and its members (this is the API for the message).
//If you want me to explain the data structure I will - just ask me in advance of class.
void map_callback(const nav_msgs::OccupancyGrid::ConstPtr& msg) {

    const bool chatty_map=true;

    map_msg=*msg;

    double map_width=map_msg.info.width;
    double map_height=map_msg.info.height;

    double map_origin_x = map_msg.info.origin.position.x;
    double map_origin_y = map_msg.info.origin.position.y;
    double map_orientation = acos(map_msg.info.origin.orientation.z);

    std::vector<signed char > map = map_msg.data;

    if(chatty_map)std::cout<<"------MAP:------"<<std::endl;
    // Here x and y are incremented by five to make the map fit in the terminal
    // Note that we have lost some map information by shrinking the data
    // this code is mostly to illustrate how the map grid data can be accessed
    for(unsigned int x=0; x<map_width; x+=5) {
        for(unsigned int y=0; y<map_height; y+=5) {

            unsigned int index = x + y*map_width;

            if(map[index] > 50) { // 0 – 100 represents how occupied
                //this square is occupied
                if(chatty_map)std::cout<<"X";
            } else if(map[index]>=0) {
                //this square is unoccupied
                if(chatty_map)std::cout<<" ";
            } else {
                //this square is unknown
                if(chatty_map)std::cout<<"?";
            }
        }
        if(chatty_map)std::cout<<std::endl;
    }
    if(chatty_map)std::cout<<"----------------"<<std::endl;


}

int main(int argc, char **argv)
{
    // must always do this when starting a ROS node - and it should be the first thing to happen
    ros::init(argc, argv, "amble");
    // the NodeHandle object is our access point to ROS
    ros::NodeHandle n;

    // Here we declare that we are going to publish "Twist" messages to the topic /cmd_vel_mux/navi
    motor_command_publisher = n.advertise<geometry_msgs::Twist>("/cmd_vel_mux/input/navi", 100);

    // Here we set the function laser_callback to receive new laser messages when they arrive
    laser_subscriber = n.subscribe("/scan", 1000,laser_callback);
    // Here we set the function map_callback to receive new map messages when they arrive from the mapping subsystem
    map_subscriber = n.subscribe("/map", 1000,map_callback);

    //now enter an infinite loop - the callback functions above will be called when new laser or map messages arrive.
    ros::Duration time_between_ros_wakeups(0.001);
    while(ros::ok()) {
        ros::spinOnce();
        time_between_ros_wakeups.sleep();
    }
    return 0;
}
