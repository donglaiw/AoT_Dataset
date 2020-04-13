Tools for Video Pre-process [under construction] 
===
To save time and storage, we directly pre-process video clips from the video in C++, instead of extracting all frames. In practice, we first use the statistics computed below to automatically select good clips, and then use html/js visualization [demo](https://github.com/donglaiw/js-demo) for manual proofreading.


Below are some useful functionalities:

- <b>Basics:</b> 
```
count_frame.cpp  // video -> number of frames
```

- <b>Black border stripe removal:</b> For each video, we select five uniformly spaced frames and compute the mean RGB value for each row and column. To remove the black frame, we find the first or last consecutive rows or columns whose mean value are less than our manually set threshold.
```
extract_frame.cpp  // video -> extract certain range/step/size of frames
```

- <b>Shot detection:</b> We compute the frame difference for each video and save the mean RGB difference for each frame. Then, we select 41-frame-long clips whose mean frame differences are not 0 (e.g. static frames) and are not large (e.g. shot transition or fast camera motion).

```
meanDiff_frame.cpp  // video -> mean RGB difference
```

- <b>Homography computation:</b> For each 41-frame-long clip, we first compute the homography between each frame and the central frame independently, and then smooth the estimated homography with outlier rejection.
```
//compute homography
computeHomo.cpp // from a video: compute homography from each frame (f_st~f_lt) to the central frame (f_md)
computeHomoF.cpp // from a folder of frames: compute homography from each frame (f_st-f_lt) to the central frame (f_md)
computeHomoMask.cpp // computeHomo.cpp + mask constraint for keypoint locations. e.g., m_h<= pts <= H-m_h
computeHomo_BS.cpp // computeHomo.cpp + bounding box constraint
```

- <b>Clip stabilization output:</b> For the selected clips, we output the
  stabilized clips by warping frames by the estimated homographies. 
```
//warp frames by homography
warpHomo.cpp // from a video: output warped frames after stabilization
warpHomoResize.cpp // warpHomo.cpp + resize output
```

- <b>Optical flow computation:</b> Compute optical flow with GPU TV_L1
```
// do optical flow
doFlowFolder.cpp // from a folder: output optical flow
doFlow256Folder.cpp // doFlowFolder.cpp + quantize the flow to uint8
```

- <b> (Optional) Optical flow for stabilized frames:</b> Compute forward and backward flow after clip stabilization
```
//compute homography
stabFlow.cpp // from a video: compute optical flow after stabilizing the clip
stabFlowFolder.cpp // stabFlow.cpp + from a folder of frames
stabFlowStep.cpp // stabFlow.cpp + at a given step of frames
stabFlow_BS.cpp // stabFlow.cpp + within a given bounding box
```

