% Latest examples is located at https://github.com/xethru/XeThru_ModuleConnector_Examples or https://dev.azure.com/xethru/XeThruApps/_git/XeThru_ModuleConnector_Examples.
% 
% Target:
% baseband IQ recording file, for example 'C:\Users\xxx\Documents\XeThru\xethru_recording_20170913_150437_baseband_recording\xethru_baseband_iq_20170913_150437.dat'
% 
% Introduction:
% This is an example showing how to paser and read recording files directly
% with out using ModuleConnector. This example just show how to paser Baseband I/Q dat recording file.
% User and mordify this example to paser other recording files by refering
% XeThru file formats document: https://www.xethru.com/community/resources/xethru-file-formats.55/ 
% 
% How to run:
% Change baseband_IQ_dat with the address of your own recording file. 

clc
clear

baseband_IQ_dat = 'C:\Users\xxx\Documents\XeThru\xethru_recording_20170913_150437_baseband_recording\xethru_baseband_iq_20170913_150437.dat';

NumHeader = 6; % item number in header

fid = fopen(baseband_IQ_dat, 'rb');
if fid < 3
    disp(['couldnt read file ' baseband_IQ_dat]);
    return
end

f = dir(baseband_IQ_dat);
fsize = f.bytes;

% read first frame
ctr = 0;
HeaderMat = [];
FrameMat = [];

while (1)
    if feof(fid)
        break
    end
    %read header
    %frame counter
    FrameCounter = fread(fid, 1,'uint32');
    NumOfBins = fread(fid, 1,'uint32');
    BinLength = fread(fid, 1,'single');
    % sampling frequency which defines the range resolution through
    % BinLength = C/Fs/2, where C is the speed of light in the medium. 
    Fs = fread(fid, 1,'single');
    % carrier frequency
    Fc = fread(fid, 1,'single');
    RangeOffset = fread(fid, 1, 'single');
    
    % check valid header read
    if isempty(FrameCounter) || isempty(NumOfBins) || isempty(BinLength) || isempty(Fs) ... 
            || isempty(Fc) || isempty(RangeOffset)
        break;
    end
    
    % read data
    data = fread(fid, 2*NumOfBins, 'single');
    if ctr==0
        % 2 because it's complex values and 4 because 'single' is 4 bytes. 
       NumFrames = fsize / (4*(NumHeader + 2*NumOfBins)); 
       HeaderMat = zeros(NumFrames, NumHeader);
       FrameMat = zeros(2*NumOfBins, NumFrames);
    end
    ctr = ctr + 1;
 
    HeaderMat(ctr,:) = [double(FrameCounter) double(NumOfBins) BinLength Fs Fc RangeOffset];
    FrameMat(:,ctr) = data;
    
end

[n,m] = size(HeaderMat);
disp([baseband_IQ_dat 'frame data has been read into FrameMat.']);
disp(['NumFrames=' num2str(n)]);
fclose(fid);
