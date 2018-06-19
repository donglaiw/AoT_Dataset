Reverse Film Dataset
===
We collected clips from Hollywood films which are displayed in reverse deliberately. 
Thanks to the “trivia” section on the IMDB website, shots that use reverse action techniques are often pointed out by the fans as Easter eggs. 
With keyword matching (e.g. “reverse motion”) and manual refinement on the trivia database, 
we collected 67 clips from 25 popular movies, including ‘Mary Poppins’, ‘Brave Heart’ and ‘Pulp Fiction’.

[link to movie clips](https://youtu.be/1zfZhXkOzCw)


--
Collection Process (`T_rf.py`)
- IMDB top-250 movies/shows (`top250.html`, `vid_250.txt`)
- keyword matching "backward", "reverse" + manual pruning (`trivia_rv1023.txt`)
- manual download and extract clips
