#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <time.h>

#include "../common/MarkerMessage.pb.h"
#include "../common/zhelpers.hpp"
extern "C" {
#include "serial.h"
}

using namespace std;


int main( int argc, char** argv )
{

	// Verify that the version of the library that we linked against is
	// compatible with the version of the headers we compiled against.
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	zmq::context_t context (1);
	zmq::socket_t s(context, ZMQ_SUB);
	s.connect ("tcp://192.168.50.83:5555");
  s.setsockopt( ZMQ_SUBSCRIBE, "", 0);

  int fd = serialInit("/dev/ttyUSB0");
  cout << "Started treadmill controller..." << endl;

  int pwm_value = 650;


	while(true)
	{
    std::string data = s_recv(s);
    treadmill::MarkerMessage marker;
    marker.ParseFromString(data);

	  cout << "markerFound: " << marker.markerfound() << endl;
    if(marker.markerfound())
    {
      int difference = 310 - marker.x();
      if(difference > 10)
        pwm_value += (difference/10)*(difference/10);
      else if(difference < -10)
        pwm_value -= (difference/10)*(difference/10);
      if(pwm_value < 600)
        pwm_value = 600;
      else if(pwm_value > 999)
        pwm_value = 999;
	    cout << "markerFound_x: " << marker.x() << endl;
	    cout << "markerFound_y: " << marker.y() << endl;
    }
    cout << "New PWM value: " << pwm_value << endl;
    serialWriteSpeed(fd,pwm_value);
	}
  serialClose(fd);

  return 0;
}

