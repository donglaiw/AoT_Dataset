using namespace std;

#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/gpu/gpu.hpp"
#include "opencv2/calib3d/calib3d.hpp"

#include <stdio.h>
#include <iostream>
#include <fstream>
using namespace cv;
using namespace cv::gpu;
    

int main(int argc, char** argv){
	// IO operation
	String vidFile(argv[1]);
	String outFile(argv[2]);
	int f_st = atoi(argv[3]);
	int f_md = atoi(argv[4]);
	int f_lt = atoi(argv[5]);

    ofstream out(outFile.c_str());

    vector<String> filenames;
    glob(vidFile, filenames);
    if(filenames.size()==0) {
        cout<<"No files in "<<vidFile<<endl;
        return -1;
    }


    // image i/o
	Mat im0;
	Mat im0_gray,im1_gray;
	int frame_num = 0;

    // for sub-pix feat: only for corners
    // opencv-3.1.0/samples/cpp/lkdemo.cpp
    TermCriteria termcrit(TermCriteria::COUNT|TermCriteria::EPS,20,0.03);
    Size winSize(19,19);

    // feature to track
    vector<Point2f> points[3];
    double qualityLevel = 0.01;
    double minDistance = 5;
    double k = 0.04;
    int blockSize = 11;
    int maxCorners = 1000;
    bool useHarrisDetector = true;
    
    // fb-flow thres
    float fb_thres=1.0;
    Mat status, flowErr;
    float hErr;
    int hNum;

    // index: start from 1
    // first pass to get the central image
    im0 = imread(filenames[f_md-1]);
    cvtColor(im0, im0_gray, CV_BGR2GRAY);
	
    // second pass to compute HM
    bool Hbad;
    for(int i=f_st-1;i<f_lt;i++){
        im0 = imread(filenames[i]);
        if(i!=f_md-1){
            cvtColor(im0, im1_gray, CV_BGR2GRAY);
            goodFeaturesToTrack(im1_gray, points[0], maxCorners,qualityLevel,minDistance,
                            Mat(),blockSize,useHarrisDetector,k);
            Hbad=true;
            if(points[0].size()>0){
                //cornerSubPix(im1_gray, points[0], subPixWinSize, Size(-1,-1), termcrit);
                calcOpticalFlowPyrLK(im1_gray, im0_gray, points[0], points[1], status, flowErr, winSize,
                                3, termcrit, 0, 0.001);
                calcOpticalFlowPyrLK(im0_gray, im1_gray, points[1], points[2], status, flowErr, winSize,
                                3, termcrit, 0, 0.001);

                vector<Point2f> pointsH[3];
                for(int j=0;j<points[1].size();j++){
                    Point2f dif = points[0][j]-points[2][j];
                    if(abs(dif.x)+abs(dif.y)<=fb_thres){ 
                        pointsH[0].push_back(points[0][j]);
                        pointsH[1].push_back(points[1][j]);
                    }
                }
                
                //cout<<"hoho:"<<points[1].size()<<"_"<<pointsH[0].size()<<endl;
                if (pointsH[1].size()>=6){
                    vector<unsigned char> match_mask; 
                    //Mat H = findHomography(pointsH[0], pointsH[1], CV_LMEDS, 0.3, match_mask);
                    Mat H = findHomography(pointsH[0], pointsH[1], CV_RANSAC, 0.5, match_mask);
                    //for(int l=0;l<6;l++){ cout<<pointsH[0][l]<<",";} cout<<endl;
                    //for(int l=0;l<6;l++){ cout<<pointsH[1][l]<<",";} cout<<endl;
                    perspectiveTransform(pointsH[0], pointsH[2],H);
                    hErr=0;hNum=0;
                    for(int j=0;j<match_mask.size();j++){
                        if(match_mask[j]==1){
                            Point2f out = pointsH[1][j]-pointsH[2][j];
                            hErr+=out.x*out.x+out.y*out.y;
                            hNum+=1;
                        }
                    }
                    double *Hdata = (double *)(H.data);
                    // matching cost
                    for(int j=0;j<9;j++){out<<Hdata[j]<<",";};
                    out<<hErr/hNum<<","<<hNum<<endl;
                    Hbad=false;
                }
            }
            if(Hbad){
                for(int j=0;j<9;j++){out<<"0,";};
                out<<"0,0"<<endl;
            }
        }        
	}
    out.close();
	return 0;
}
