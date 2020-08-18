function [ time_mat, timestr ] = epoch2matlabDateTime( currMSecsSinceEpoch )
    %epoch2matlabDateTime This function converts unix 1970 epoch time to matlab
    %datenum format
    %   Detailed explanation goes here
    % solution found here: http://stackoverflow.com/questions/12661862/converting-epoch-to-date-in-matlab
    milisecsInDay = 24*3600*1000;

    time_ref = datenum('1970','yyyy');

    time_mat = time_ref + currMSecsSinceEpoch / milisecsInDay;

    timestr = datestr(time_mat, 'yyyymmdd HH:MM:SS.FFF');
end

