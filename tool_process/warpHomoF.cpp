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
	int step = atoi(argv[8]);
	int outSize = atof(argv[9]);
	String suf(argv[10]);

    // 0.2 open inputs
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

    // 1. first pass of H to estimate the bounding box
	Mat im0, im1, imSave;
    // 1.1 get frame size
    im0 = imread(filenames[0]);
    Size sz = im0.size();
    float corner[4]={0,sz.width-1,0,sz.height-1};

    // 1.2 warp all the corner pts
    string outBB = outFolder.substr(0,outFolder.length()-1)+"_bb.txt";
    ifstream cornerRead(outBB.c_str());
    string line,tmpStr;
    Mat H0 = Mat::zeros(3, 3, CV_64F);
    double *H0_data = (double *)(H0.data);

    if(cornerRead.good()){
        getline(cornerRead,line);
        std::stringstream lineStream(line);
        for(int j = 0; j < 4; j++){
            getline(lineStream, tmpStr,','); 
            corner[j]=atof(tmpStr.c_str());
        }    
        hRead.close();
    }else{
        vector<Point2f> points[2];
        points[0].push_back(Point2f(0,0));
        points[0].push_back(Point2f(0,sz.height-1));
        points[0].push_back(Point2f(sz.width-1,0));
        points[0].push_back(Point2f(sz.width-1,sz.height-1));

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
    }
    cornerRead.close();

    // cout<<corner[0]<<"_"<<corner[1]<<"_"<<corner[2]<<"_"<<corner[3]<<endl;
    // Rect stabCrop(corner[2], corner[0], corner[3]-corner[2]+1, corner[1]-corner[0]+1);
    Rect stabCrop(corner[0], corner[2], (corner[1]-corner[0]+1), (corner[3]-corner[2]+1));
    // Rect stabCrop((int)corner[0], (int)corner[2], (int)(corner[1]-corner[0]+1), (int)(corner[3]-corner[2]+1));
    if(min(stabCrop.width,stabCrop.height)<crop_thres){
        return 0;
    }
 
    // 2. second pass to warp the image
    // 2.1 prepare i/o
    // output non-compression png
    // too costly 
    //vector<int> comp;comp.push_back(CV_IMWRITE_PNG_COMPRESSION);comp.push_back(0);
    hRead.open(hFile.c_str());

    // 2.2 skip the initial frames
    
    for(int i=f_st-1;i<f_lt;i+=step){
        //cout<<i<<endl;
        im0 = imread(filenames[i]);
        // get Homography
        Mat imOut;
        if(i==f_md-1){
            // memset(H0_data, 0, sizeof(H0_data)); 
            // H0_data[0]=1.0;H0_data[4]=1.0;H0_data[8]=1.0;
            imOut = im0(stabCrop);
        }else{
            getline(hRead,line);
            std::stringstream lineStream(line);
            for(int j = 0; j < 9; j++){
                getline(lineStream, tmpStr,','); 
                H0_data[j]=strtod(tmpStr.c_str(),NULL);
            }
            // warp image
            warpPerspective(im0, im1, H0, sz);
            imOut = im1(stabCrop);
        }
        //Mat imOut = im1;
        char tmp[20];sprintf(tmp,"/image_%04d.",i+1);
        //imwrite(outFolder + tmp, imOut,comp);
        //cout<<outR<<","<<outR*(float)imOut.cols<<","<<outR*(float)imOut.rows<<","<<endl;
        // cout<<outR<<","<<(float)imOut.cols<<","<<(float)imOut.rows<<","<<endl;
        if(outSize!=1){
            //resize(imOut,imSave,cv::Size((int)(outR*(float)imOut.cols),(int)(outR*(float)imOut.rows)));
            resize(imOut,imSave,cv::Size((int)((float)outSize*(float)imOut.cols/(float)imOut.rows),outSize));
            imwrite(outFolder + tmp + suf , imSave);
        }else{
            imwrite(outFolder + tmp + suf , imOut);
        }

        //cout<<outFolder + tmp + suf<<endl;
        // to step over
        // careful with the middle frame ...
        if(step>1){
            for(int j=0;j<step-2;j++){
                getline(hRead,line);
            }
            if(i>=f_md-1 || i+step<=f_md-1){
                getline(hRead,line);
            }
        }
	}
    hRead.close();
	return 0;
}
