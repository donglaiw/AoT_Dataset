# Video Crawler

## Step 1: Get Video URLs
- By Query: get video ids by queries
*  setup query, folder to save and number of pages to crawl: 
```
init_path;q='raw+footage';sn='rfootage/';numF=100;
``` 
* crawl Vimeo query result pages:
```
tid=0;crawl_by_query
```
* parse into video IDs:
```
tid=1;crawl_by_query
```

- By Author Name: get video ids by authors

## Step 2: Download Videos
For each video ID,
```
youtube-dl --no-overwrites --continue https://vimeo.com/ID -o v_%(id)s.%(ext)s > ID.log 2&1
```

