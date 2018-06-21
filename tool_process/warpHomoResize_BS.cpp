
using namespace std;
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"

#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
using namespace cv;
using namespace std;

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
	int outSize = atoi(argv[9]);
	String suf(argv[10]);
	String inBB(argv[11]); // estimated bbox region
	int cropBB = atoi(argv[12]); // how much more to crop

    // 0.2 open inputs
	VideoCapture capture(vidFile);
	if(!capture.isOpened()) {
		printf("Error: can't open the video.");
        return -1;
	}
    ifstream hRead(hFile.c_str());
    if(!hRead.good()){
        printf("Error: can't open the H file.");
        return -1;
    }
    
	Mat im0, im1, imSave, im0_bs;
    string line,tmpStr;

    string outFormat = hFile.substr(hFile.rfind("."));
    string outBB = outFolder.substr(0,outFolder.length()-1)+"_bb"+outFormat;
    ifstream cornerRead(outBB.c_str());
    if(!cornerRead.good()){
        printf("Error: can't open warping bbox file.");
        cout<<outBB<<endl;
        return -1;
    }
    // 1. first pass of H to estimate the bounding box
    // 1.1 get frame size
    float corner[4]={0,0,0,0};
    // 1.2 warp all the corner pts
    Mat H0 = Mat::zeros(3, 3, CV_64F);
    double *H0_data = (double *)(H0.data);
    // pre-computed
    getline(cornerRead,line);
    std::stringstream lineStream_c(line);
    for(int j = 0; j < 4; j++){
        getline(lineStream_c, tmpStr,','); 
        corner[j]=atof(tmpStr.c_str());
    }    
    cornerRead.close();
    Rect stabCrop(corner[0], corner[2], (corner[1]-corner[0]+1), (corner[3]-corner[2]+1));

    // Rect stabCrop((int)corner[0], (int)corner[2], (int)(corner[1]-corner[0]+1), (int)(corner[3]-corner[2]+1));
    if(min(stabCrop.width,stabCrop.height)<crop_thres){
        return 0;
    }

    ifstream bsRead(inBB.c_str());
    float corner_bs[6]={0,0,0,0,0,0};
    if(! bsRead.good()){
        printf("Error: can't open bs bbox file.");
        return -1;
    }
    getline(bsRead,line);
    std::stringstream lineStream_bs(line);
    for(int j = 0; j < 6; j++){
        getline(lineStream_bs, tmpStr,','); 
        corner_bs[j]=atof(tmpStr.c_str());
    }    
    bsRead.close();
    Rect stabCrop_bs(corner_bs[2]+cropBB, corner_bs[0]+cropBB, 
            (corner_bs[5]-corner_bs[2]-corner_bs[3]-2*cropBB+1), (corner_bs[4]-corner_bs[0]-corner_bs[1]-2*cropBB+1));


    // 2. second pass to warp the image
    // 2.1 prepare i/o
    // output non-compression png
    // too costly 
    //vector<int> comp;comp.push_back(CV_IMWRITE_PNG_COMPRESSION);comp.push_back(0);

    // 2.2 skip the initial frames
    capture >> im0;
    im0_bs = im0(stabCrop_bs);
    Size sz = im0_bs.size();
    for(int i=1;i<f_st-1;i++){capture >> im0;}
    
    for(int i=f_st-1;i<f_lt;i+=step){
        // for i=0, im0 is already read before
        if(i!=0){ capture >> im0;}
        im0_bs = im0(stabCrop_bs);
        // get Homography
        Mat imOut;
        if(i==f_md-1){
            // memset(H0_data, 0, sizeof(H0_data)); 
            // H0_data[0]=1.0;H0_data[4]=1.0;H0_data[8]=1.0;
            imOut = im0_bs(stabCrop);
        }else{
            getline(hRead,line);
            std::stringstream lineStream(line);
            for(int j = 0; j < 9; j++){
                getline(lineStream, tmpStr,','); 
                H0_data[j]=strtod(tmpStr.c_str(),NULL);
            }
            // warp image
            warpPerspective(im0_bs, im1, H0, sz);
            //cout<<im1.rows<<"_"<<im1.cols<<"_"<<im0_bs.rows<<"_"<<im0_bs.cols<<"_"<<sz.height<<"_"<<sz.width<<endl;
            imOut = im1(stabCrop);
        }
        //Mat imOut = im1;
        char tmp[20];sprintf(tmp,"/image_%04d",i+1);
        //imwrite(outFolder + tmp, imOut,comp);
        if(outSize!=1){
            if(imOut.cols>imOut.rows){
                resize(imOut,imSave,cv::Size((int)((float)outSize*(float)imOut.cols/(float)imOut.rows),outSize));
            }else{
                resize(imOut,imSave,cv::Size(outSize,(int)((float)outSize*(float)imOut.rows/(float)imOut.cols)));
            }
            imwrite(outFolder + tmp + suf , imSave);
        }else{
            imwrite(outFolder + tmp + suf , imOut);
        }
        // to step over
        // careful with the middle frame ...
        if(step>1){
            for(int j=0;j<step-2;j++){
                getline(hRead,line);
                capture >> im0;
            }
            if(i>=f_md-1 || i+step<=f_md-1){
                getline(hRead,line);
            }
            capture >> im0;
        }
	}
    hRead.close();
    capture.release();
	return 0;
}
