
%function matlab2opencv( variable, fileName, flag)
%int rows cols;
[rows,cols] = size(cameraParams);
cameraParams = cameraParams';
%if ( ~exist('flag','var') )
    %flag = 'w';
%end

%if ( ~exist(fileName,'file') || flag == 'w' )
 %   file = fopen( fileName, 'w');
  %  fprintf( file, '%%YAML:1.0\n');
%else
 %   file = fopen( fileName, 'a');
%end

fprintf( file, ' %s: !!opencv-matrix\n', inputname(1));
fprintf( file, ' rows: %d\n', rows);
fprintf( file, ' cols: %d\n', cols);
fprintf( file, ' dt: f\n');
fprintf( file, ' data: [ ');

for i=1:rows*cols
        fprintf( file, '%.6f', variable(i));
        if (i == rows*cols)
            break;
       end
        fprintf( file, ', ');
        if mod(i+1,4) == 0
        fprintf( file, '\n            ');
        end
end

fprintf( file, ']\n');
fclose(file);



