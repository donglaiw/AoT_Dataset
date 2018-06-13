U_cf=@(f,v) cellfun(f,v,'UniformOutput',false);
U_af=@(f,v) arrayfun(f,v,'UniformOutput',false);
U_tr=@(x,y) textread(x,'%s','delimiter',y);
U_inter=@(x,y) numel(intersect(U_tr(x,'\n'),U_tr(y,'\n')));
U_fl=@(x,y) find(x==y,1,'last');
U_ff=@(x,y) find(x==y,1,'first');
