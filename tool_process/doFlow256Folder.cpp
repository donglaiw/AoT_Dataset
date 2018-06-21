using namespace std;
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/gpu/gpu.hpp"
//#include "opencv2/core.hpp"
//#include "opencv2/core/utility.hpp"
//#include "opencv2/highgui.hpp"
//#include "opencv2/cudaoptflow.hpp"
//#include "opencv2/cudaarithm.hpp"


#include <stdio.h>
#include <iostream>
using namespace cv;
//using namespace cv::cuda;
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
	String outFolder(argv[2]);
	int bound = atoi(argv[3]);
    int device_id = atoi(argv[4]);
    int step = atoi(argv[5]);
	String suf(argv[6]);
	int f_st = atoi(argv[7]);
	int f_md = atoi(argv[8]);
	int f_lt = atoi(argv[9]);
    
    vector<String> filenames; 
    glob(vidFile, filenames); 
    int numF = filenames.size();
	if(numF==0) {
		cout<<"No files in "<<vidFile<<endl;
		return -1;
	}
    //cout<<"1. #files "<<filenames.size()<<endl;

	Mat prev_grey, grey, frame, flow_x, flow_y, imgX, imgY;
    GpuMat frame_0, frame_1, flow_u, flow_v;

    setDevice(device_id);
    //Ptr<cuda::OpticalFlowDual_TVL1> alg_tvl1 = cuda::OpticalFlowDual_TVL1::create();
    OpticalFlowDual_TVL1_GPU alg_tvl1;

    char tmp[20];
    //cout<<"2. start "<<endl;
    // ffmpeg has an extra first frame ...
    int szNew;Size imSize;
    Mat grey_, prev_grey_;

	for(int frame_num=f_st-1;frame_num<f_lt;frame_num+=step) {
        frame = imread(filenames[frame_num],CV_LOAD_IMAGE_COLOR);
		if(frame_num == 0) {
			grey.create(frame.size(), CV_8UC1);
			prev_grey.create(frame.size(), CV_8UC1);
			cvtColor(frame, prev_grey, CV_BGR2GRAY);

            if(frame.cols>frame.rows){
                szNew=(int)((float)frame.cols*256.0/(float)frame.rows);
                imSize=cv::Size(szNew,256);
            }else{
                szNew = (int)((float)frame.rows*256.0/(float)frame.cols);
                imSize=cv::Size(256,szNew);
            }
            resize(prev_grey,prev_grey_,imSize);
            imgX.create(imSize, CV_8UC1);
            imgY.create(imSize, CV_8UC1);
            //cout<<imSize<<endl;
			continue;
		}

        //cout<<"start: "<<imSize<<endl;
		cvtColor(frame, grey, CV_BGR2GRAY);
        resize(grey,grey_,imSize);
		frame_0.upload(prev_grey_);
		frame_1.upload(grey_);


        // GPU optical flow
        alg_tvl1(frame_0,frame_1,flow_u,flow_v);
        flow_u.download(flow_x);
        flow_v.download(flow_y);
        convertFlowToImage(flow_x,flow_y, imgX, imgY, -bound, bound);
        sprintf(tmp,"_%04d.",frame_num-f_st+1);
        imwrite(outFolder + "/f/flow_x" + tmp + suf,imgX);
        imwrite(outFolder + "/f/flow_y" + tmp + suf,imgY);

        // backward flow
        alg_tvl1(frame_1,frame_0,flow_u,flow_v);
        flow_u.download(flow_x);
        flow_v.download(flow_y);
        convertFlowToImage(flow_x,flow_y, imgX, imgY, -bound, bound);
        sprintf(tmp,"_%04d.",f_lt-frame_num);
        imwrite(outFolder + "/b/flow_x" + tmp + suf,imgX);
        imwrite(outFolder + "/b/flow_y" + tmp + suf,imgY);
        
        //cout<<imgX.cols<<","<<imgX.rows<<endl;
        std::swap(prev_grey_, grey_);
	}
	return 0;
}
