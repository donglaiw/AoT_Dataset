# AOT_Dataset
Tools for dataset collection for the CVPR 18 paper ["Learning and Using the Arrow of Time"](http://vision03.csail.mit.edu/manip/aot/aot.html) project

## Video Crawler
### Get Video URLs
1. By Query: get video ids by queries
- setup query, folder to save and number of pages to crawl: 
```
init_path;q='raw+footage';sn='rfootage/';numF=100;
``` 
- crawl Vimeo query result pages:
```
tid=0;crawl_by_query
```
- parse into video IDs:
```
tid=1;crawl_by_query
```

2. By Author Name: get video ids by authors

### Download Videos
For each video ID,
```
youtube-dl --no-overwrites --continue https://vimeo.com/ID -o v_%(id)s.%(ext)s > ID.log 2&1
```

