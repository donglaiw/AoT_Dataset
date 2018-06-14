Reverse Film Dataset
===

### Donwload
- [Original frames]()
- [Stabilized 20-frame clips]()

### Data Files
- `video.txt`: a list of 25 movies using reverse film technique
- `test_clip20.txt`: list of the stabilized clips with video name and starting frame index


### Collection Process (`T_rf.py`)
- movie list: IMDB top-250 movies (`top250.html`, `vid_250.txt`)
- reverse film clip description: crawl trivia + keyword matching "backward", "reverse" + manual pruning (`trivia_rv1023.txt`)
- reverse film clip: manual download and extract clips
