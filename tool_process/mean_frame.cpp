using namespace std;

#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/gpu/gpu.hpp"

#include <stdio.h>
#include <iostream>
#include <fstream>
using namespace cv;
using namespace cv::gpu;

int main(int argc, char** argv){
	// IO operation
	String vidFile(argv[1]);
	String outFile(argv[2]);
	int step = atoi(argv[3]);

    ofstream out(outFile.c_str());
	
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
		if(frame.empty()) {break;}
   
        Scalar tmp=mean(frame);
        out<<(int)tmp[2]<<","<<(int)tmp[1]<<","<<(int)tmp[0]<<endl;
        
		frame_num = frame_num + 1;
		int step_t = step;
		while (step_t > 1){
			capture >> frame;
			step_t--;
		}
	}
    out.close();
    capture.release();
	return 0;
}
