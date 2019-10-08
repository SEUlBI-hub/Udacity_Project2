#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>
#include <numeric>
#include <iostream>

using namespace std;

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot
    
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;
    client.call(srv);
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    // TODO: Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera


    int height = img.height;
    int step = img.width;
    int img_size = height * step * 3;
    int xposition = 0;
    int meanX = 0;
    int numwhite = 0;
    float x = 0.0;
    float z = 0.0;
 

  
    // for (int i = 0; i < height ; i++) {
    //     for (int j = 0; j < step; j++) {
    //         if (img.data[i*3*step + 3*j] == 255 && img.data[i*3*step + 3*j+1] == 255 && img.data[i*3*step + 3*j+2] == 255) {
    //             xposition = xposition +j; numwhite = numwhite+1;
    //         }
    //     }
    // }
    ROS_INFO_STREAM("while start");
    int i = 0; 
    while(i < img_size - 2){
        // ROS_INFO_STREAM("while start");
        // cout << i << endl;

        if (img.data[i] == 255 && img.data[i+1] == 255 && img.data[i+2] == 255) {
                // ROS_INFO_STREAM("if start");
                xposition = xposition + (i%(step*3))/3; numwhite = numwhite+1;
                // cout << xposition << endl;
                // cout << numwhite << endl;
                // ROS_INFO_STREAM("if end");
        }
        i+=3;
        
    }
    ROS_INFO_STREAM("while end");
    cout << xposition << endl;
    cout << meanX << endl;
    cout << numwhite << endl;
    cout << step << endl;


    
    if(numwhite == 0) {
        drive_robot(0.0,0.3);
    }else{
        meanX = xposition/numwhite;
        if(meanX<step/3) {
            drive_robot(0.1,0.3);

        }
        else if(meanX>step/3) {
            drive_robot(0.1,-0.3);
        }
        else
        {
            drive_robot(0.1,0.0);
        }
    }
    
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
