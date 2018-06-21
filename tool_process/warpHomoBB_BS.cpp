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
	String inBB(argv[7]); // estimated bbox region
	int cropBB = atoi(argv[8]); // how much more to crop

    // 0.2 open inputs
    ifstream hRead(hFile.c_str());
    if(!hRead.is_open()){
        printf("Error: can't open the homo file.");
        return -1;
    }

    // 1. first pass of H to estimate the bounding box
    // 1.1 get frame size

    ifstream cornerRead(inBB.c_str());
    string line,tmpStr;
    // eat in 1 pix
    int corner_bs[6]={0,0,0,0,0,0};
    if(cornerRead.good()){
        getline(cornerRead,line);
        std::stringstream lineStream(line);
        for(int j = 0; j < 6; j++){
            getline(lineStream, tmpStr,','); 
            corner_bs[j]=atof(tmpStr.c_str());
        }    
    }
    cornerRead.close();
    // points index: 0->sz-1
    float corner[4]={0,corner_bs[5]-corner_bs[2]-corner_bs[3]-2*cropBB,
                     0,corner_bs[4]-corner_bs[0]-corner_bs[1]-2*cropBB};

    // 1.2 warp all the corner pts
    string outFormat = hFile.substr(hFile.rfind("."));
    string outBB = outFolder.substr(0,outFolder.length()-1)+"_bb"+outFormat;

    Mat H0 = Mat::zeros(3, 3, CV_64F);
    double *H0_data = (double *)(H0.data);

    vector<Point2f> points[2];
    points[0].push_back(Point2f(0,0));
    points[0].push_back(Point2f(0,corner[3]));
    points[0].push_back(Point2f(corner[1],0));
    points[0].push_back(Point2f(corner[1],corner[3]));

    double data[9];
    for(int i=f_st;i<f_lt;i++){
        getline(hRead,line);
        std::stringstream lineStream(line);
        for(int j = 0; j < 9; j++){
            getline(lineStream, tmpStr,','); 
            H0_data[j]=strtod(tmpStr.c_str(), NULL);
        }
        perspectiveTransform(points[0], points[1],H0);
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

	return 0;
}
