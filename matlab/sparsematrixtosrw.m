function sparsematrixtosrw(n, density, filename) 
    %% Generate random sparse matrix:
    M = sprand(n,n,density); %+ (1+rand).*eye(n);
    % Also do for band matrices?
    
    %% Convert to Sparse RowWise
    [x,y,z] = find(M');
    n = length(x);
    
    i(1) = 0;
    row = 1;
    current_i = 0;
    iter = 1;
    
    while iter<=n
        if y(iter) == row
            current_i = current_i + 1;
            j(iter) = x(iter);
            v(iter) = z(iter);
            iter = iter + 1;
        else
            row = row + 1;
            i(row) = current_i;            
        end
    end
    
    i(row+1) = current_i;
    
    (length(i)-1)
    %j
    length(v)
    
    %% Save to file:
    data_dump = [i,j,v];
    
    fid = fopen(filename,'w');
    fprintf( fid,'%d ', i );
    fprintf(fid, '\n');
    fprintf( fid,'%d ', j );
    fprintf(fid, '\n');
    fprintf( fid,'%f ', v );
    fprintf(fid, '\n');
    fclose(fid);
end

