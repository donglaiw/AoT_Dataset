Tools for Video Pre-process [under construction] 
===
TODO: organize the files with description

We pre-process the videos as follows:
- <b>Black frame removal:</b> For each video, we select five uni- formly spaced frames and compute the mean RGB value for each row and column. To remove the black frame, we find the first or last consecutive rows or columns whose mean value are less than our manually set threshold.
- <b>Shot detection:</b> We compute the frame difference for each video and save the mean RGB difference for each frame. Then, we select 41-frame-long clips whose mean frame dif- ferences are not 0 (e.g. static frames) and are not large (e.g. shot transition or fast camera motion).
- <b>Clip stabilization:</b> For each 41-frame-long clip, we first compute the homography between each frame and the cen- tral frame independently, and then smooth the estimated ho- mography with outlier rejection. We stabilize each clip with these estimated homographies.
- <b>Clip stabilization:</b> For each 41-frame-long clip, we first compute the homography between each frame and the cen- tral frame independently, and then smooth the estimated ho- mography with outlier rejection. We stabilize each clip with these estimated homographies.
