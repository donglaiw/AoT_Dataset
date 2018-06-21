using namespace std;
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/gpu/gpu.hpp"
#include "opencv2/calib3d/calib3d.hpp"

#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
using namespace cv;
using namespace cv::gpu;

int main(int argc, char** argv){
	// 0. preparation
    // 0.1 read arg
	String vidFile(argv[1]);
	String hFile(argv[2]);
	String outFolder(argv[3]);
	int f_st = atoi(argv[4]);
	int f_md = atoi(argv[5]);
	int f_lt = atoi(argv[6]);
	int crop_thres = atoi(argv[7]);

    // 0.2 open inputs
	VideoCapture capture(vidFile);
	if(!capture.isOpened()) {
		printf("Error: can't open the video.");
        return -1;
	}
    ifstream hRead(hFile.c_str());
    if(!hRead.is_open()){
        printf("Error: can't open the homo file.");
        return -1;
    }

    // 1. first pass of H to estimate the bounding box
	Mat im0, im1;
    // 1.1 get frame size
    capture >> im0;
    Size sz = im0.size();
    float corner[4]={0,sz.width-1,0,sz.height-1};

    // 1.2 warp all the corner pts
    string outBB = outFolder.substr(0,outFolder.length()-1)+"_bb.txt";

    string line,tmpStr;
    Mat H0 = Mat::zeros(3, 3, CV_64F);
    double *H0_data = (double *)(H0.data);

    vector<Point2f> points[2];
    points[0].push_back(Point2f(0,0));
    points[0].push_back(Point2f(0,sz.height-1));
    points[0].push_back(Point2f(sz.width-1,0));
    points[0].push_back(Point2f(sz.width-1,sz.height-1));
    //cout<<"0: "<<points[0][0].x<<"-"<<points[0][0].y<<","<<points[0][1].x<<"-"<<points[0][1].y<<","<<points[0][2].x<<"-"<<points[0][2].y<<","<<points[0][3].x<<"-"<<points[0][3].y<<endl;
    double data[9];
    for(int i=f_st;i<f_lt;i++){
        getline(hRead,line);
        std::stringstream lineStream(line);
        for(int j = 0; j < 9; j++){
            getline(lineStream, tmpStr,','); 
            H0_data[j]=strtod(tmpStr.c_str(), NULL);
        }
        perspectiveTransform(points[0], points[1],H0);
        //cout<<i<<": "<<points[1][0].x<<"-"<<points[1][0].y<<","<<points[1][1].x<<"-"<<points[1][1].y<<","<<points[1][2].x<<"-"<<points[1][2].y<<","<<points[1][3].x<<"-"<<points[1][3].y<<endl;
        // shrink the corner point
        corner[0] = max(max(corner[0],points[1][0].x),points[1][1].x);
        corner[1] = min(min(corner[1],points[1][2].x),points[1][3].x);
        corner[2] = max(max(corner[2],points[1][0].y),points[1][2].y);
        corner[3] = min(min(corner[3],points[1][1].y),points[1][3].y);
    }
    hRead.close();
    corner[0]=ceil(corner[0]);corner[2]=ceil(corner[2]);
    corner[1]=floor(corner[1]);corner[3]=floor(corner[3]);

    // 1.3 output the bbox
    // outFolder: xxx/20/
    ofstream out(outBB.c_str());
    for(int j=0;j<4;j++){out<<corner[j]<<",";};
    out.close();

    capture.release();
	return 0;
}
