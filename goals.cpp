/**
* \file goals.cpp
* \brief software architecture for the control of the robot in the environment
* \author Martina Germani
* \date 27/07/2022
*
* \details
* The type of robot's control depends on user's input. In particular, if the user
* enters the letter "a", the robot has to autonomously reach a (x,y) coordinate inserted 
* by the user; if the user enters the letter "m", the user itself can manually drive the
* robot using the keyboard; if, instead, the user enters the letter "c", the user can 
* manually drive the robot using the keyboard, but there are some controls in order to
* avoid collision with the edges of the environment. 
*
* Subscribes to:<BR>
* /scan which is a topic that in the field "ranges" contains information about the distance
* between the robot and the obstacles
*
* Publishes to:<BR>
* /scan
* /cmd_vel
*
* Client:<BR>
* /client/simple_action_client
*
* Description :
*
* This node simulates a robot's control depending on the input inserted by the user.
* If the user chooses the "automatic guide" (letter "a"), the move_base package requires 
* goal to be sent to the move_base/goal topic; in this way, the move_base action accepts
* goal from client and attempts to move the robot to the specified position.
* If the user chooses the "manual guide" (letter "m"), the user can press some keyboard keys
* so as to move the robot in the desired direction; in particular, it can press "+" in order
* to move the robot on a straight line, "r" in order to turn right the robot, "l" in order 
* to turn left the robot, "s" in order to stop the robot or "." in order to change the guide
* mode. 
* If the user chooses the "controlled guide" (letter "c"), the user can move the robot as in
* the "manual guide", but there are some controls releted to the distance from the obstacles;
* for doing that there are a publisher and a subscriber to the topic /scan of the robot, in
* fact its field "ranges" contains information about the distance between the robot and the
* obstacles around it.
*
**/


// Assignment 3: software architecture for the control of the robot in the environment.
#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <iostream>
#include "geometry_msgs/Twist.h"
#include "sensor_msgs/LaserScan.h"

using namespace std;

// global variable definition:

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient; ///<global actionclient.

geometry_msgs::Twist base_cmd; ///<velocity of the robot.
sensor_msgs::LaserScan scan; ///<laser scan information.
ros::Publisher scan_pub; ///<global publisher to the /scan topic.
ros::Publisher pub; ///<global publisher to the /cmd_vel topic.


float x; ///<goal's coordinates inserted by the user.
float y; ///<goal's coordinates inserted by the user.
char cmd; ///<command inserted by keyboard for moving the robot. 
char input; ///<guide mode choosen by the user. 
float value_front; ///<distances from the obstacle on the robot's front. 
float value_right; ///<distances from the obstacle on the robot's right. 
float value_left; ///<distances from the obstacle on the robot's left. 


/**
* \brief callback for controlling the distance between the robot and the obstacles around it
* in order to avoid collision with them. 
* \param scan robot's laser
* 
* This function, while the user moves the robot using the keyboard, controls the distance
* between the robot and the obstacles around it: this information is contained in the field
* "ranges" of the topic /scan. More in detail, if the user enters "+", this function controls
* the distance between the robot and the obstacle in front of it: if it is close, the robot 
* stops, otherwise it moves on a straight line. If, instead, the user enters "l", this 
* function controls the distance between the robot and the obstacle on the left: if it is
* close, the robot turns right instead of left, otherwise it turns left. 
* If the user enters "r", this function controls the distance between the robot and the
* obstacle on the right: if it is close, the robot turns left instead of right, otherwise
* it turns right. 
**/

void distCallback(const sensor_msgs::LaserScan::ConstPtr& scan)
{
	// compute the distances fromt the obstacles on robot's front, robot's right, robot's left
	value_front = scan->ranges[360];
	value_right = scan->ranges[72];
	value_left = scan->ranges[650];
	
	if(cmd!='+' && cmd!='l' && cmd!='r' &&  cmd!='s' && cmd!='.')
	{
		cout<<"Unknown command: "<<cmd<<endl;
	}
			
	base_cmd.linear.x=base_cmd.linear.y=base_cmd.angular.z=0;
	
	// if the user enters "+" 	
	if(cmd=='+')
	{
		ROS_INFO("control the distance from the obstacles in front of the robot");
		
		// if the obstacle in front of the robot is near
		if(value_front< 1.5)
		{
			// stop the robot
			ROS_INFO("the robot has an obstacle in front of him: it can't move");
			base_cmd.linear.x =0;
			pub.publish(base_cmd);
		}
		
		// if there are no obstacles nearby in front of the robot
		else
		{
			// the robot moves on a straight line
			ROS_INFO("no obstacles in the front, proceed");
			base_cmd.linear.x=0.5;
			pub.publish(base_cmd);
		}
		
		cin>>cmd;
	}
	
	// if the user enters "l"
	else if(cmd=='l')
	{
		ROS_INFO("control the distance from the obstacles on the robot's left");
		
		// if there is an obstacle nearby on the robot's left
		if(value_left<1.5)
		{
			// the robot turns right
			ROS_INFO("The robot has an obtascle on the left, so it turns right");
			base_cmd.angular.z=-0.75;
			base_cmd.linear.x=0.1;
			pub.publish(base_cmd);
		}
		// if there are no obstacles nearby on the robot's left
		else
		{
			// the robot turns right
			ROS_INFO("no obstacles on the left, proceed");
			base_cmd.angular.z=0.75;
			base_cmd.linear.x=0.1;
			pub.publish(base_cmd);
		}
		
		cin>>cmd;
	}
	
	// if the user enters "r"
	else if(cmd=='r')
	{
		ROS_INFO("control the distance from the obstacles on the robot's right");
		
		// if there is an obstacle on the robot's right
		if(value_right <1.5)
		{
			// the robot turns left	
			ROS_INFO("The robot has an obtascle on the right, so it turns left");
			base_cmd.angular.z=0.75;
			base_cmd.linear.x=0.1;
			pub.publish(base_cmd);
		}
		
		// if there are no obstacles nearby on the robot's right
		else
		{
			// the robot turns right
			ROS_INFO("no obstacles on the right, proceed");
			base_cmd.angular.z = -0.75;
			base_cmd.linear.x = 0.1;
			pub.publish(base_cmd);
		}
		
		cin>>cmd;
	}
		
	// if the user enters "s"	
	else if(cmd=='s')
	{
		// stop the robot
		ROS_INFO("stop the robot");
		base_cmd.linear.x = 0;
		base_cmd.angular.z = 0;
		pub.publish(base_cmd);
		
		cin>>cmd;
			
	}
	
	
	
					
}
		


/**
* \brief main function which controls the robot's behaviour.
*
* This function creates a node, called "goal_node", two publishers (one to the topic
* /cmd_vel and one to the /scan topic) and a subscriber to the /scan topic. Once the user 
* chose how to move the robot, this function implements some controls. More in details, if 
* the user enters "a", it constructs an action client used to communicate with the 
* move_base action, then waits for the action server to come up and creates a goal to send 
* to the move_base; at this point, the user enters the desired position, and this function 
* controls if the inserted target is reachable or not: if it is not reachable, it will be 
* deleted, otherwise it waits for the goal to finish. 
* If the user enters "m", the user can move the robot using the keyboard, so this function
* contains the code which builds some commands for doing that. In particular, if the user
* enters "+", it moves the robot on a straight line modifying its linear velocity and 
* publishing it on the /cmd_vel topic; if the user enters "r", it turns right the robot
* modifying its angular velocity and publishing it on /cmd_vel topic; if the user enters 
* "l", it turns left the robot, modifying its angular velocity and publishing it on /cmd_vel * topic; if the user enters "s", it stops the robot by putting to zero both its linear and
* angular velocity and publishing them on the /cmd_vel topic; if, instead, the user enters
* ".", it changes the guide mode. 
* If the user enters "c", the distCallback function is executed.
**/


int main(int argc, char** argv){

	ros::init(argc, argv, "goal_node");
	
	ros::NodeHandle n;
	ros::spinOnce();
	
	
	pub = n.advertise<geometry_msgs::Twist>("/cmd_vel", 10);
	
	// create a publisher and a subscriber to the topic /scan
	
	scan_pub = n.advertise<sensor_msgs::LaserScan>("/scan",1);
	
	ros::Subscriber scan_sub = n.subscribe("/scan", 10, distCallback);
		
	
	while(n.ok())
	{
	// the user has to choose from keyboard how to move the robot
	cout<<"Choose how to move: a for automatic guide, m for manual guide, c if you want an assisted guide"<<endl;
	cin>>input;
    
    	// if the user chooses the automatic guide:
    	if (input=='a')
	{
     		// construct an action client used to communicate with the action named "move_base"
       	MoveBaseClient ac("move_base", true);
   
     		// wait for the action server to come up
    		while(!ac.waitForServer(ros::Duration(5.0))){
      			 ROS_INFO("Waiting for the move_base action server to come up");
     		}
   
   		// create a goal to send to the move_base
     		move_base_msgs::MoveBaseGoal goal;
   
		// set the coordinate frame w.r.t the robot moves
     		goal.target_pose.header.frame_id = "map";
     		goal.target_pose.header.stamp = ros::Time::now();
     		goal.target_pose.pose.orientation.w = 1.0;
     		
     		// the user enters the desired target:
     		cout<<"where do you want the robot to go?"<<endl;
		cout<<"x= "<<endl;
		cin>>x;
		goal.target_pose.pose.position.x=x;
		cout<<"y= "<<endl;
		cin>>y;
		goal.target_pose.pose.position.y=y;
   		
   		// control on the target entered
   		// if the target entered is not reachable, it will be deleted
   		if ((x<-5.0) || (x>5.0) || (y>8.2) || (y<-8.0) || (x<1 && y<-6) || (x==1 && y>0.0) || (x>2.5 && y>2.5) || ((x>=0 && x<=3) && y>-0.1) || (x<=0 && y<=-5.3) || ((x>=2 && x<=3) && (y>=-4 && y<=-1.3)) || (x>=2 && (y>=-2.2 && y<=-1.3)) || (x>=1.5 && (y>-6.2 && y<-5.3)) || (x<=-2 && (y>0 && y<0.8)) || (x<=-2 &&(y>2 && y<3.5)) || ((x>=-2 && x<=-1.5) && (y>=2 && y<=3.3)))
   		{
   			ROS_INFO("goal non reachable: the entered target will be deleted");
   			ac.cancelAllGoals();
   			ros::Duration(1).sleep(); // wait 1 sec
   			
   		}
   		
   		// otherwise, if the entered target is reachable
   		else
   		{
   			// send the goal to the move_base
     			ROS_INFO("Sending goal");
     			ac.sendGoal(goal);
   
   			// wait for the goal to finish: wait for up to 30.0
     			bool finished_before_timeout = ac.waitForResult(ros::Duration(30.0));
     				
   			// check if the goal succeeded or failed
     			if(finished_before_timeout)
     			{
     				actionlib::SimpleClientGoalState state = ac.getState();
       			ROS_INFO("Hooray, the robot has reached the target");
       		}
     			else
       			ROS_INFO("The robot has not succeeded in reaching the target");
       			ac.cancelAllGoals();
       	}
   	}
   	
   	// if the user chooses the manual guide:
   	else if (input=='m')
   	{
   		// the user has to move manually the robot using the keyboard
   		cout<<"Type a command and then press enter: use '+' to more forward, 'l' to turn left, 'r' to turn right, 's' to stop the robot, '.' to exit"<<endl;
   		cin>>cmd;
   	
   		while(n.ok())
		{
			if(cmd!='+' && cmd!='l' && cmd!='r' && cmd!='s' && cmd!='.')
			{
				cout<<"Unknown command: "<<cmd<<endl;
			}
			
			base_cmd.linear.x=base_cmd.linear.y=base_cmd.angular.z=0;
			
			// if the user enters "+"
			if(cmd=='+')
			{
				// move the robot on a straight line
				base_cmd.linear.x=0.5;
				pub.publish(base_cmd);
				cin>>cmd;
			}
			
			// if the user enters "l"
			else if(cmd=='l')
			{
				// turn left
				base_cmd.angular.z=0.75;
				base_cmd.linear.x=0.5;
				pub.publish(base_cmd);
				cin>>cmd;
			}
			
			// if the user enters "r"
			else if(cmd=='r')
			{
				// turn right
				base_cmd.angular.z = -0.75;
				base_cmd.linear.x = 0.5;
				pub.publish(base_cmd);
				cin>>cmd;
			}
			
			// if the user enters "s"
			else if(cmd=='s')
			{
				// stop the robot
				base_cmd.linear.x = 0;
				base_cmd.angular.z = 0;
				pub.publish(base_cmd);
				cin>>cmd;
			
			}
			
			// if the user enters "."
			else if(cmd=='.')
			{
				// change the guide mode
				break;
			}
			
		}
	}
	
	// if the user chooses the controlled guide:
   	else if (input =='c')
   	{
   		
   		// the user has to move manually the robot using the keyboard, but there are some controls in order to avoid obstacles
   		cout<<"Type a command and then press enter: use '+' to more forward, 'l' to turn left, 'r' to turn right, 's' to stop the robot, '.' to exit"<<endl;
   		cin>>cmd;
   	
   	
   		while(n.ok())
		{
			// execute the callback
			ros::spinOnce();
			
			// if the user enters "."
			if(cmd=='.')
			{
				// exit from this modality
				base_cmd.linear.x = 0;
				base_cmd.angular.z = 0;
				pub.publish(base_cmd);
				break;
			}
   		}
   	}
   	
	}

	return 0;

}

