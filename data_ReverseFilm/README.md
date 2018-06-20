Reverse Film Dataset
===
We collected clips from Hollywood films which are displayed in reverse deliberately. 
Thanks to the “trivia” section on the IMDB website, shots that use reverse action techniques are often pointed out by the fans as Easter eggs. 
With keyword matching (e.g. “reverse motion”) and manual refinement on the trivia database, 
we collected 67 clips from 25 popular movies, including ‘Mary Poppins’, ‘Brave Heart’ and ‘Pulp Fiction’.

Visualization of movie clips: [[original version]](http://vision03.csail.mit.edu/manip/data/imdb/T_movie_gif10.htm) [[10-frame version]](http://vision03.csail.mit.edu/manip/data/imdb/T_movie_gif10.htm)

=======
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
