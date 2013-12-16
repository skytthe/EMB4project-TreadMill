#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <time.h>

#include <opencv2/opencv.hpp>

#include "find_markers.hpp"
#include "Marker.hpp"

#include "MarkerMessage.pb.h"

using namespace std;
using namespace cv;

const int DESIRED_CAMERA_WIDTH = 640;
const int DESIRED_CAMERA_HEIGHT = 480;


int main( int argc, char** argv )
{

	// Verify that the version of the library that we linked against is
	// compatible with the version of the headers we compiled against.
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	VideoCapture camera;
	camera.open(0);
	if(!camera.isOpened())
	{
		cerr << "ERROR: could not access camera" << endl;
		exit(1);
	}
	camera.set(CV_CAP_PROP_FRAME_WIDTH, DESIRED_CAMERA_WIDTH);
	camera.set(CV_CAP_PROP_FRAME_HEIGHT, DESIRED_CAMERA_HEIGHT);

	system("v4l2-ctl --set-ctrl white_balance_temperature_auto=0");
	system("v4l2-ctl --set-ctrl brightness=30");
	system("v4l2-ctl --set-ctrl contrast=32");
	system("v4l2-ctl --set-ctrl saturation=56");
	system("v4l2-ctl --set-ctrl hue=0");
	system("v4l2-ctl --set-ctrl gamma=100");
	system("v4l2-ctl --set-ctrl white_balance_temperature=3000");
	system("v4l2-ctl --set-ctrl sharpness=2");
	system("v4l2-ctl --set-ctrl backlight_compensation=1");
	system("v4l2-ctl --set-ctrl exposure_auto=1");
	system("v4l2-ctl --set-ctrl exposure_absolute=150");


/*

bbb@emb4:~/workspace/marker_detection/build$ v4l2-ctl --list-ctrls-menus
                     brightness (int)    : min=-64 max=64 step=1 default=30 value=51
                       contrast (int)    : min=0 max=95 step=1 default=32 value=95
                     saturation (int)    : min=0 max=100 step=1 default=56 value=100
                            hue (int)    : min=-180 max=180 step=1 default=0 value=0
 white_balance_temperature_auto (bool)   : default=1 value=0
                          gamma (int)    : min=48 max=300 step=1 default=100 value=300
           power_line_frequency (menu)   : min=0 max=2 default=2 value=2
				0: Disabled
				1: 50 Hz
				2: 60 Hz
      white_balance_temperature (int)    : min=2800 max=6500 step=1 default=4600 value=2800
                      sharpness (int)    : min=1 max=7 step=1 default=2 value=4
         backlight_compensation (int)    : min=0 max=4 step=1 default=1 value=2
                  exposure_auto (menu)   : min=0 max=3 default=3 value=1
				0: Auto Mode
				1: Manual Mode
				2: Shutter Priority Mode
				3: Aperture Priority Mode
              exposure_absolute (int)    : min=10 max=1250 step=1 default=333 value=333

*/

	while(true)
	{
struct timespec start, end;
clock_gettime( CLOCK_REALTIME, &start );


		//capture image
		Mat cameraFrame;
		camera >> cameraFrame;

		std::vector<Marker> detectedMarkers = findMarkers(cameraFrame);
		
		cout << "list size " << detectedMarkers.size() << endl;


		treadmill::MarkerMessage markerMessage;
		

		
		if(detectedMarkers.size() < 1)
		{
			markerMessage.set_markerfound(false);
		} else {
		markerMessage.set_markerfound(true);
		
		for (int i=0; i < detectedMarkers.size(); i++) {
	                float x = 0.0;
	                float y = 0.0;

			std::vector<cv::Point2f> p  = detectedMarkers.at(i).points;
			for(int j=0; j < p.size(); j++) {
				x = x + p.at(j).x;
				y = y + p.at(j).y;
			}			
			x = x/4.0;
			y = y/4.0; 

		markerMessage.set_x((int)x);
		markerMessage.set_y((int)y);
		markerMessage.set_error_x((int)x);
		markerMessage.set_error_y((int)y);
		

			cout << "x=" << x << ",y=" << y << endl;
			/*
			stringstream ss;
			ss << "x=" << x << ",y=" << y;
       			s_send(socket, ss.str());
			string reply = s_recv(socket);
        		std::cout << "Received " << reply << std::endl;
			*/
		}
		}

string m;
markerMessage.SerializeToString(&m);

cout << m << endl;

treadmill::MarkerMessage newMarker;
newMarker.ParseFromString(m);



		cout << "markerFound: " << newMarker.markerfound() << endl;
if(newMarker.markerfound())
{
		cout << "markerFound_x: " << newMarker.x() << endl;
		cout << "markerFound_y: " << newMarker.y() << endl;
}
clock_gettime( CLOCK_REALTIME, &end );
double difference = (end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec)/1000000000.0d;
cout << "It took " << difference << " seconds to process frame - fps="<< (1.0/difference) << endl;


	}


  return 0;
}

