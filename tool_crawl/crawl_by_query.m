%q='prores';sn='prores/';numF=583;%
%q='magic+lantern';sn='magic/';numF=789;
%q='test+footage';sn='footage/';numF=1341;%d
% q='raw+footage';sn='rfootage/';numF=1077;%d
% sn='tmp2/';% just for remedy
% cat *.txt|grep -v h264|grep -v unavailable|grep -v msmpeg4v2| mpeg4mpeg4
%sn='channel/';

addpath('../util')
switch tid
case 0
    % get query html
    U_mkdir(sn)
    F = fopen([sn 'dw_page.sh'],'w');
    for i = 1:numF
        cmd_fmt = ['https://vimeo.com/search/page:%d/sort:relevance?q=' q];
        cmd = sprintf(cmd_fmt, i);
        fprintf(F,'wget --no-check-certificate -U ''Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.8.1.6) Gecko/20070802 SeaMonkey/1.1.4'' %s -O %s%d.htm &\n', cmd, sn,i);
        if mod(i,16)==0
            fprintf(F,'wait \n');
        end
    end
    fclose(F);
    system(['sh ' sn 'dw_page.sh']);
case 1
    % get ids
    % works in 2018
    system(['cat ' sn '*.htm|grep vimeo.config > ' sn 'all_id.txt']);
    F=fopen([sn 'all_id.txt'],'r');
    F2=fopen([sn 'dw_id.sh'],'w');
    i = 0;
    while ~feof(F)
        i = i + 1;
        line = fgetl(F);
        sp=regexp(line, 'uri', 'split');
        for j=2:numel(sp)
            line = sp{j}(7+find(sp{j}(7:end)=='/',1,'first'):find(sp{j}==',',1,'first')-2);
            fprintf(F2, [line '\n']);
        end
    end
    fclose(F);
    fclose(F2);

case 1.01
    % get ids
    % works in 2017
    system(['cat ' sn '*.htm|grep data-clip-id > ' sn 'all_id.txt']);
    F=fopen([sn 'all_id.txt'],'r');
    F2=fopen([sn 'download_all.sh'],'w');
    i = 0;
    while ~feof(F)
        i = i + 1;
        line = fgetl(F);
        sp=regexp(line, 'data-clip-id', 'split');
        line = sp{2}(3:end);
        sp=regexp(line, '"', 'split');
        line = sp{1};
        % output id
        fprintf(F2, [line '\n']);
    end
    fclose(F);
    fclose(F2);
end

