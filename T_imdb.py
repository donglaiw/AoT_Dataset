import sys
opt=sys.argv[1]

D0='ReverseFilm/'

def getT250():
    v=open(D0+'vid_250.txt')
    vv=v.readlines()
    v.close()
    return [x[x.find(',')+1:-1].replace('_',' ') for x in vv]

if opt=='0':
    # parse downloaded html into movie name
    a=open(D0+'top250.html','r')
    b=open(D0+'vid_250.txt','w')
    line=a.readline()
    cc=1
    while line:
        if 'title' in line and '</a>' in line:
            b.write(str(cc)+','+line[line.find('>')+1:-5].replace(' ','_')+'\n')
            cc+=1
        line=a.readline()
    a.close()
    b.close()

elif opt=='1':
    # get trivia urls
    if not os.path.exists(D0+'trivia/'):
        os.mkdir(D0+'trivia/')
    a=open(D0+'top250.html','r')
    b=open(D0+'dw_trivia.sh','w')
    line=a.readline()
    cc=1
    while line:
        if '<a href=' in line:
            cc+=1
            if cc%2==0:
                #print 'wget -o www.imdb.com'+line[line.find('"')+1:line.find('?')]+'trivia -O '+D0+'trivia/top_'+str(cc)+'.html'
                b.write('wget www.imdb.com'+line[line.find('"')+1:line.find('?')]+'trivia -O '+D0+'trivia/top_'+str(cc/2)+'.html\n')
                if cc%16==0:
                    #print "wait\n"
                    b.write("wait\n")
        line=a.readline()
    a.close()
    b.close()
elif opt=='2':
    # parse the html to txt
    for i in range(1,251):
        a=open(D0+'trivia/top_'+str(i)+'.html','r')
        b=open(D0+'trivia/top_'+str(i)+'.txt','w')
        line=a.readline()
        st=0
        # fixed format: next line is the text
        while line:
            if st==1:
                b.write(line[:-7]+'\n')
                st=0
            if 'sodatext' in line:
                st=1
            line=a.readline()
        b.close()
        a.close()
elif opt=='3':
    # found 43 movies
    # search keyword
    # reverse,back
    c=open(D0+'vid_250.txt')
    cc=c.readlines()
    c.close()
    dd=[None]*250
    for i,line in enumerate(cc):
        dd[i]=line[line.find(',')+1:-1]
    b=open(D0+'vid_250_rv.txt','w')
    cc_c=0
    vv=[]
    for i in range(1,251):
        a=open(D0+'trivia/top_'+str(i)+'.txt','r')
        line=a.readline()
        # fixed format: next line is the text
        pre_c=0
        while line:
            if 'backward' in line or 'reverse' in line:
                b.write(str(i)+'.'+dd[i-1]+':'+line)
                pre_c+=1
                #b.write(str(i)+':'+line)
            line=a.readline()
        a.close()
        if pre_c>0:
            cc_c+=1
            vv.append(dd[i-1])
    b.close()
    print '#movies:',cc_c
    vv.sort()
    for v in vv:
        print v
elif opt=='4':
    # parse txt to keyword
    a=open(D0+'trivia.txt','r')
    b=open(D0+'trivia_rv.txt','w')
    line=a.readline()
    st=0
    # fixed format: next line is the text
    # lower complexity: find reverse first and remove top 250 films
    cc=0
    preN=''
    preL=''
    doL=0
    preC=0
    while line:
        if line=='\n':
            # come to new line
            if doL:
                b.write(preL+'\n')
            preL='';doL=0;
        else:
            preL=preL+line[:-1]

        if line[:2]=='# ':
            cc+=1
            preN = line;preC = 0
            preL='';doL=0;
        # role, spell, name, decisioni,voice
        if ('backward' in line or 'reverse' in line) and '(VG)' not in preN:
            if preC==0:
                b.write(preN)
            preC=1
            doL=1
            #  movie title or has keyword
        line=a.readline()
    b.close()
    a.close()
    print cc
