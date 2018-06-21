using namespace std;

#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/gpu/gpu.hpp"

#include <stdio.h>
#include <iostream>
using namespace cv;
using namespace cv::gpu;

int main(int argc, char** argv){
	// IO operation
	String vidFile(argv[1]);
	int step = atoi(argv[2]);
	
	//std::cout<<"db: "<<vidFile<<std::endl;
	//std::cout<<"db: "<<device_id<<std::endl;
	//std::cout<<"db: "<<step<<std::endl;
	VideoCapture capture(vidFile);
	if(!capture.isOpened()) {
		//printf("Could not initialize capturing..\n");
		printf("0");
		return -1;
	}

	Mat frame;
	int frame_num = 0;

	while(true) {
		capture >> frame;
        //cout<<"start: "<<frame_num<<endl;
		if(frame.empty())
			break;
        // GPU optical flow
        //cout<<"start back flo: "<<frame_num<<endl;
		/*
        alg_tvl1->calc(frame_1,frame_0,d_flow);
        split(d_flow, flow_uv);
		flow_uv[0].download(flow_x);
		flow_uv[1].download(flow_y);
        */

		frame_num = frame_num + 1;
		int step_t = step;
		while (step_t > 1){
			capture >> frame;
			step_t--;
		}
	}
    //cout<<"numF: "<<frame_num<<endl;
    cout<<frame_num;
    capture.release();
	return 0;
}
