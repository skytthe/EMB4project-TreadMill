#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <time.h>

#include "../common/MarkerMessage.pb.h"
#include "../common/zhelpers.hpp"

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


	while(true)
	{
    std::string data = s_recv(s);
    treadmill::MarkerMessage marker;
    marker.ParseFromString(data);

	  cout << "markerFound: " << marker.markerfound() << endl;
    if(marker.markerfound())
    {
	    cout << "markerFound_x: " << marker.x() << endl;
	    cout << "markerFound_y: " << marker.y() << endl;
    }
	}


  return 0;
}

