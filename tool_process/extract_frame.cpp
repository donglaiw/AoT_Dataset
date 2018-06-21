using namespace std;

#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/gpu/gpu.hpp"

#include <stdio.h>
#include <iostream>
using namespace cv;
using namespace cv::gpu;

static void convertFlowToImage(const Mat &flow_x, const Mat &flow_y, Mat &img_x, Mat &img_y,
       double lowerBound, double higherBound) {
	#define CAST(v, L, H) ((v) > (H) ? 255 : (v) < (L) ? 0 : cvRound(255*((v) - (L))/((H)-(L))))
	for (int i = 0; i < flow_x.rows; ++i) {
		for (int j = 0; j < flow_y.cols; ++j) {
			float x = flow_x.at<float>(i,j);
			float y = flow_y.at<float>(i,j);
			img_x.at<uchar>(i,j) = CAST(x, lowerBound, higherBound);
			img_y.at<uchar>(i,j) = CAST(y, lowerBound, higherBound);
		}
	}
	#undef CAST
}

static void drawOptFlowMap(const Mat& flow, Mat& cflowmap, int step,double, const Scalar& color){
    for(int y = 0; y < cflowmap.rows; y += step)
        for(int x = 0; x < cflowmap.cols; x += step)
        {
            const Point2f& fxy = flow.at<Point2f>(y, x);
            line(cflowmap, Point(x,y), Point(cvRound(x+fxy.x), cvRound(y+fxy.y)),
                 color);
            circle(cflowmap, Point(x,y), 2, color, -1);
        }
}

int main(int argc, char** argv){
	// IO operation
	String vidFile(argv[1]);
	String imgFile(argv[2]);
	int startF = atoi(argv[3]);
	int endF = atoi(argv[4]);
	int step = atoi(argv[5]);
	float height = (float)atof(argv[6]);
	String suf(argv[7]);
	
	std::cout<<"db: "<<vidFile<<std::endl;
	//std::cout<<"db: "<<device_id<<std::endl;
	//std::cout<<"db: "<<step<<std::endl;
	VideoCapture capture(vidFile);
	if(!capture.isOpened()) {
		printf("Could not initialize capturing..\n");
		return -1;
	}

	Mat frame;
	int frame_num = 0;
    // frame_id: start from startF (1-index)
    vector<int> comp;
    comp.push_back(CV_IMWRITE_PNG_COMPRESSION);
    comp.push_back(0);
	while(true && frame_num<endF) {
		capture >> frame;frame_num = frame_num + 1;
        //cout<<"start: "<<frame_num<<endl;
		if(frame.empty())
			break;
        
        if(frame_num>=startF){
            char tmp[20];sprintf(tmp,"/image_%04d.",int(frame_num));
            Mat imgX_;
            int szNew;
            if(height>0){
                szNew=(int)((float)frame.cols*height/(float)frame.rows); 
                resize(frame,imgX_,cv::Size(szNew,height));
                imwrite(imgFile + tmp + suf,imgX_,comp);
            }else{
                imwrite(imgFile + tmp + suf,frame,comp);
            }
            int step_t = step;
            while (step_t > 1){
                capture >> frame;frame_num = frame_num + 1;
                step_t--;
            }
        }
	}
    cout<<"numF: "<<frame_num<<","<<endF<<endl;
    capture.release();
	return 0;
}
