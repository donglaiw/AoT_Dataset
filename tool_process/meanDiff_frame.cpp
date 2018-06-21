using namespace std;

#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

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

	Mat frame,frame2;
	int frame_num = 0;

	while(true) {
		capture >> frame;
        frame.convertTo(frame, CV_64FC3);
		if(frame.empty()) {break;}
        if(frame_num!=0){
            Scalar tmp=mean(abs(frame-frame2));
            out<<(int)tmp[2]<<","<<(int)tmp[1]<<","<<(int)tmp[0]<<endl;
        }
        frame.copyTo(frame2);
   
       
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
