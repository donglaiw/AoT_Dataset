using namespace std;
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/gpu/gpu.hpp"

#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>

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

int main(int argc, char** argv){
	// 1. IO operation
	String vidFile(argv[1]);
	String hFile(argv[2]);
	String hbbFile(argv[3]);
	String outFolder(argv[4]);
	int bound = atoi(argv[5]);
    int device_id = atoi(argv[6]);
    int step = atoi(argv[7]);
	String suf(argv[8]);
	int f_st = atoi(argv[9]);
	int f_md = atoi(argv[10]);
	int f_lt = atoi(argv[11]);
    setDevice(device_id);

	// std::cout<<"db: "<<vidFile<<std::endl;
    // 2. open inputs
    vector<String> filenames;
    glob(vidFile, filenames);
    if(filenames.size()==0) {
        cout<<"No files in "<<vidFile<<endl;
        return -1;
    }

    ifstream hRead(hFile.c_str());
    if(!hRead.is_open()){
        printf("Error: can't open the H file.");
        return -1;
    }
    ifstream cornerRead(hbbFile.c_str());
     if(!cornerRead.is_open()){
        printf("Error: can't open the bounding box file.");
        return -1;
    }   
    float corner[4]={0,0,0,0};
    string line,tmpStr;

    getline(cornerRead,line);
    std::stringstream lineStream(line);
    for(int j = 0; j < 4; j++){
        getline(lineStream, tmpStr,','); 
        corner[j]=atof(tmpStr.c_str());
    }
    Rect stabCrop(corner[0], corner[2], (corner[1]-corner[0]+1), (corner[3]-corner[2]+1));
    cout<<corner[0]<<"_"<<corner[1]<<"_"<<corner[2]<<"_"<<corner[3]<<endl;
    cout<<min(stabCrop.width,stabCrop.height)<<endl;

    Size sz = stabCrop.size();
    Size szNew;
    if(sz.width>sz.height){
        szNew = Size((int)((float)sz.width*256.0/(float)sz.height),256);
    }else{
        szNew = Size(256,(int)((float)sz.height*256.0/(float)sz.width));
    }

    // 3. main algo
	Mat prev_grey, grey, grey_, frame, flow_x, flow_y, imgX, imgY, im0, im1;
    //Mat imgX_, imgY_;
    GpuMat frame_0, frame_1, flow_u, flow_v;
    OpticalFlowDual_TVL1_GPU alg_tvl1;
    char tmp[20];
    Mat H0 = Mat::zeros(3, 3, CV_64F);
    double *H0_data = (double *)(H0.data);

    for(int i=f_st-1;i<f_lt;i+=step){
        //cout<<i<<":"<<filenames[i]<<endl;
        im0 = imread(filenames[i]);
        if(i==f_md-1){
            frame = im0(stabCrop);
        }else{
            getline(hRead,line);
            std::stringstream lineStream(line);
            for(int j = 0; j < 9; j++){
                getline(lineStream, tmpStr,','); 
                H0_data[j]=strtod(tmpStr.c_str(),NULL);
                //if( (i-f_st+1)%13==0 || (i-f_st+1)%13==1 ){cout<<H0_data[j]<<",";}
            }
            // if( (i-f_st+1)%13==0 || (i-f_st+1)%13==1 ){cout<<endl;}
            // warp image
            warpPerspective(im0, im1, H0, im0.size());
            frame = im1(stabCrop);
        }
		if(i == f_st-1) {
			grey_.create(sz, CV_8UC1);
			grey.create(szNew, CV_8UC1);
			prev_grey.create(szNew, CV_8UC1);
            imgX.create(szNew, CV_8UC1);
            imgY.create(szNew, CV_8UC1);
			cvtColor(frame, grey_, CV_BGR2GRAY);
			resize(grey_, prev_grey, szNew);
			continue;
		}

        //cout<<"start: "<<filenames[frame_num]<<endl;
		cvtColor(frame, grey_, CV_BGR2GRAY);
		resize(grey_, grey, szNew);

        frame_0.upload(prev_grey);
        frame_1.upload(grey);
        
        //foward flow
        alg_tvl1(frame_0,frame_1,flow_u,flow_v);
        flow_u.download(flow_x);
        flow_v.download(flow_y);
        convertFlowToImage(flow_x,flow_y, imgX, imgY, -bound, bound);
        //resize(imgX,imgX_,szNew);
        //resize(imgY,imgY_,szNew);
        sprintf(tmp,"_%04d.",i-f_st+1);
        //imwrite(outFolder + "/f/flow_x" + tmp + suf,imgX_);
        //imwrite(outFolder + "/f/flow_y" + tmp + suf,imgY_);
        imwrite(outFolder + "/f/flow_x" + tmp + suf,imgX);
        imwrite(outFolder + "/f/flow_y" + tmp + suf,imgY);

        // backward flow
        alg_tvl1(frame_1,frame_0,flow_u,flow_v);
        flow_u.download(flow_x);
        flow_v.download(flow_y);
        convertFlowToImage(flow_x,flow_y, imgX, imgY, -bound, bound);
        //resize(imgX,imgX_,szNew);
        //resize(imgY,imgY_,szNew);
        sprintf(tmp,"_%04d.",f_lt-i);
        //imwrite(outFolder + "b/flow_x" + tmp + suf,imgX_);
        //imwrite(outFolder + "b/flow_y" + tmp + suf,imgY_);
        imwrite(outFolder + "/b/flow_x" + tmp + suf,imgX);
        imwrite(outFolder + "/b/flow_y" + tmp + suf,imgY);

		std::swap(prev_grey, grey);
	}
	return 0;
}
