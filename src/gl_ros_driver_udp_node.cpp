#include <ros/ros.h>
#include <sensor_msgs/LaserScan.h>

#include "gl_driver.h"


int main(int argc, char** argv)
{
    // ros init
    std::string gl_ip = std::string("10.110.1.2");
    int gl_port = 2000;
    int pc_port = 3000;
    std::string frame_id = std::string("laser");
    std::string pub_topicname_lidar = std::string("scan");
    
    ros::init(argc, argv, "gl_ros_driver_udp_node");
    ros::NodeHandle nh;
    ros::NodeHandle nh_priv("~");

    nh_priv.param("gl_ip", gl_ip, gl_ip);
    nh_priv.param("gl_port", gl_port, gl_port);
    nh_priv.param("pc_port", pc_port, pc_port);
    nh_priv.param("frame_id", frame_id, frame_id);
    nh_priv.param("pub_topicname_lidar", pub_topicname_lidar, pub_topicname_lidar);

    ros::Publisher data_pub = nh.advertise<sensor_msgs::LaserScan>(pub_topicname_lidar, 10);

    // GL Init
    Gl gl(gl_ip,gl_port,pc_port);
    std::cout << "Serial Num : " << gl.GetSerialNum() << std::endl;
    gl.SetFrameDataEnable(true);

    // loop
    ros::Rate loop_rate(80);
    while(ros::ok())
    {
        sensor_msgs::LaserScan scan_msg;

        Gl::framedata_t frame_data;
        gl.ReadFrameData(frame_data);
        int num_data = frame_data.distance.size();
        if(num_data>0)
        {
            scan_msg.header.stamp = ros::Time::now();
            scan_msg.header.frame_id = frame_id;
            scan_msg.angle_min = frame_data.angle[0];
            scan_msg.angle_max = frame_data.angle[num_data-1];
            scan_msg.angle_increment = (scan_msg.angle_max - scan_msg.angle_min) / (double)(num_data-1);
            scan_msg.range_min = 0.001;
            scan_msg.range_max = 30.0;
            scan_msg.ranges.resize(num_data);
            for(int i=0; i<num_data; i++)
            {
                scan_msg.ranges[i] = frame_data.distance[i];
            }
            
            data_pub.publish(scan_msg);
        }

        ros::spinOnce();
        loop_rate.sleep();
    }

    gl.SetFrameDataEnable(false);

    return 0;
}
