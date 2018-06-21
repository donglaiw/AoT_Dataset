MJPEG-AoT Dataset
===
Unlike [Youtube](https://www.youtube.com/channel/UCVfwlh9XpX2Y_tQfjeln9QA), where videos are mosty H.264 compressed, 
[Vimeo](http://vimeo.com/) hosts many professional videos in a variety of original formats, which are known. 
To download videos without temporal codec compression (e.g. uncompressed or using  intra-frame codecs likes MJPEG and ProRes), 
we search on Vimeo with key- words such as “mjpeg”, “prores”, and “cannon+raw” etc. 
We verify the codec of the downloaded video with “ffmpeg”. 

We initially obtain around <b>7k/b> videos before pre-processing and <b>16.9k</b> individual shots from <b>3.5k</b> video after pre-processing.
