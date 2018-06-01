addpath('/data/vision/billf/deep-learning/Manipulation/util')


% st_id
switch floor(tid)
    case 1;sn='user/';
    case 2;sn='channel/';
    case 3;sn='group/';
end

switch tid
case {1,2,3}
    % get user_id
    switch tid
    case 1;
        % expensive to use youtube-dl
        cmd='python do_user.py 1_0 0';
        % directly parse the html
    case 2; % manual input
    case 3; % manual input
    end
    system([cmd])
    fprintf('num: %d\n',numel(textread([sn 'name.txt'],'%s')))
case {1.1,2.1,3.1}
    % download main page to figure out number of videos
    switch floor(tid)
    case 1;un=@(x) [x '/videos/all/sort:date'];
    case 2;un=@(x) ['channels/' x '/videos'];
    case 3;un=@(x) ['groups/' x '/sort:date'];
    end
    a=textread([sn 'name.txt'],'%s');
    F = fopen([sn 'd1.sh'],'w');
    cc=0;
    for i = st_id:numel(a)
        todo=1;
        if exist([sn 'd1-' a{i} '.htm'],'file')
            tmp = textread([sn 'd1-' a{i} '.htm'],'%s');
            if numel(tmp)~=0;todo=0;end
        end
        if todo
            cmd_fmt = ['https://vimeo.com/' un(a{i})];
            cmd = sprintf(cmd_fmt, i);
            fprintf(F,'wget --no-check-certificate -U ''Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.8.1.6) Gecko/20070802 SeaMonkey/1.1.4'' %s -O %s/d1-%s.htm &\n', cmd,sn,a{i});
            cc=cc+1;
        end
        if mod(cc,16)==0
            fprintf(F,'wait \n');
        end
    end
    fclose(F);
    system(['sh ' sn '/d1.sh']);
case {1.2,2.2,3.2}
    switch floor(tid)
    case 1;kk='data-title';un=@(x,y) [x '/videos/all/page:' y '/sort:date'];
    case 2;kk='data-page';un=@(x,y) ['channels/' x '/videos/page:' y];
    case 3;kk='data-page';un=@(x,y) ['groups/' x '/page:' y '/sort:date'];
    end
    F = fopen([sn 'd2.sh'],'w');
    a=textread([sn 'name.txt'],'%s');
    numP=12;
    for i=st_id:numel(a)
        [~,s1]=system(['grep ' kk ' ' sn '/d1-' a{i} '.htm']);
        if numel(s1)==0
            numPage=1;
        else
            s2 = strsplit(s1,'\n');
            s2(cellfun(@isempty,s2))=[];
            switch tid
            case 1.2
                s2 = s2{end};
                ind = strfind(s2,kk);
                s3 = s2(ind:end);
                numV=str2num(strrep(s3(find(s3=='"',1,'first')+1:find(s3==' ',1,'first')-1),',',''));
                numPage = ceil(numV/numP);
            case {2.2,3.2}
                s2 = s2{end-1};% ..., last, next
                ind = strfind(s2,kk);
                s3 = s2(ind:end);
                numPage=str2num(s3(find(s3=='"',1,'first')+1:find(s3=='"',1,'last')-1));
            end
        end
        cc=0;
        for j=1:numPage
            if ~exist([sn 'd2-' a{i} '-' num2str(j) '.htm'],'file')
                cmd = ['https://vimeo.com/' un(a{i},num2str(j))];
                fprintf(F,'wget --no-check-certificate -U ''Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.8.1.6) Gecko/20070802 SeaMonkey/1.1.4'' %s -O %s/d2-%s-%d.htm &\n', cmd,sn,a{i},j);
                cc=cc+1;
                if mod(cc,10)==0
                    fprintf(F,'wait \n');
                end
            end
        end
        if cc>0
            fprintf(F,'wait \n');
        end
    end
    fclose(F);
    system(['sh ' sn 'd2.sh']);
case {1.3,2.3,3.3}
    % get video id
    F = fopen([sn 'download_all.sh'],'w');
    a=textread([sn 'name.txt'],'%s');
    for i=st_id:numel(a)
        fns = dir([sn 'd2-' a{i} '-*.htm']);
        for j=1:numel(fns)
            [~,s1]=system(['grep data-position ' sn fns(j).name]);
            s2 = strsplit(s1,'\n');
            for k=1:numel(s2)
                if numel(s2{k})>0
                    ind = strfind(s2{k},'clip_');
                    s3=s2{k}(ind+5:end);
                    fprintf(F,[s3(1:find(s3=='"',1,'first')-1) ' \n']);
                end
            end
        end
    end
    fclose(F);
    sn = [sn 'download_all.sh'];
    a=textread(sn,'%s');
    U_out(sn,unique(a));
case 3
    % get user id
    switch tid
    case 3.1
        % old method: expensive youtube-dl API
        vv=U_getM(1);
        vv = vv(randperm(numel(vv)));
        fid=fopen(['./do_user.sh'],'w');
        numF=1;
        for i=1:numel(vv)
            fprintf(fid,['ssh ' vv{i} ' ''for i in $(seq ' num2str((i-1)*numF) ' ' num2str(i*numF-1) ');do cd /data/vision/billf/deep-time/Data/vimeo/dw/script && python do_user.py ' num2str(numF*numel(vv)) '_${i-1} & done'' &\n']);
            fprintf(fid, 'sleep 2 \n');
        end
        fclose(fid);
        fprintf(['sh ./do_user.sh\n'])
        system(['chmod +x ./do_user.sh']);
        %fprintf(['./do_dw.sh 1_0 ' sn '/download_all.sh &\n']);
    case 3.2
        % get from html
    end
end

