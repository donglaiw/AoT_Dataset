Reverse Film Dataset
===
We collected clips from Hollywood films which are displayed in reverse deliberately. 
Thanks to the “trivia” section on the IMDB website, shots that use reverse action techniques are often pointed out by the fans as Easter eggs. 
With keyword matching (e.g. “reverse motion”) and manual refinement on the trivia database, 
we collected 67 clips from 25 popular movies, including ‘Mary Poppins’, ‘Brave Heart’ and ‘Pulp Fiction’.

Visualization of movie clips [original version](http://vision03.csail.mit.edu/manip/data/imdb/T_movie_gif10.htm)
[10-frame version](http://vision03.csail.mit.edu/manip/data/imdb/T_movie_gif10.htm)
=======
### Donwload Clips
- [Black Stripe Removed]()
- [Stabilized 10-frame]()

### Data Files
- `test_vid23.txt`: a list of 23 movies using reverse film technique
- `test_clip67.txt`: list of 67 clips that have at least one well-stabilized 10-frame sequence


### Collection Process (`T_rf.py`)
- movie list: IMDB top-250 movies (`crawl_vid250.html`, `crawl_vid250.txt`)
- reverse film clip description: crawl trivia + keyword matching "backward", "reverse" + manual pruning (`crawl_trivial.sh`, `crawl_trivia_prune.txt`)
- reverse film clip: manual download and extract clips
