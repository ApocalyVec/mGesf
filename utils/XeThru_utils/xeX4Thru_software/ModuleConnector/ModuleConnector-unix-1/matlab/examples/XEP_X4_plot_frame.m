% This example demonstrates the use of the BasicRadarClassX4 example class
% to configure the chip and start streaming.
%
% To complete the following example you need:
% - An X4M200/X4M300/X4M03 module
% - The ModuleConnector library
% - MATLAB

%add paths
addModuleConnectorPath();
% if running on a 32-bit Windows system, instead run:
% addModuleConnectorPath('win32');

clc
clear

% Load the library
Lib = ModuleConnector.Library;
Lib.libfunctions

% Input parameters
COM = 'COM4';
FPS = 20;
dataType = 'bb';

% Chip settings
PPS = 26;
DACmin = 949;
DACmax = 1100;
Iterations = 16;
FrameStart = 0.2; % meters.
FrameStop = 9.4; % meters.

% Create BasicRadarClassX4 object
radar = BasicRadarClassX4(COM,FPS,dataType);

% Open radar.
radar.open();

% Use X4M300 interface to attempt to set sensor mode XEP (manual).
app = radar.mc.get_x4m300();

app.set_sensor_mode('stop');
try
    app.set_sensor_mode('XEP');
catch
    % Unable to set sensor mode. Assume only running XEP FW.
end

% Initialize radar.
radar.init();

% Configure X4 chip.
radar.radarInstance.x4driver_set_pulsesperstep(PPS);
radar.radarInstance.x4driver_set_dac_min(DACmin);
radar.radarInstance.x4driver_set_dac_max(DACmax);
radar.radarInstance.x4driver_set_iterations(Iterations);

% Configure frame area
radar.radarInstance.x4driver_set_frame_area(FrameStart,FrameStop);
% Read back actual set frame area
[frameStart, frameStop] = radar.radarInstance.x4driver_get_frame_area();

% Start streaming and subscribe to message_data_float.
radar.start();

tstart = tic;

fh = figure(5);
clf(fh);
ph = plot(0);
ylabel('Normalized amplitude');
xlabel('Range [m]');

th = title('');
grid on;

i = 0;

while ishandle(fh)
    % Peek message data float
    numPackets = radar.bufferSize();
    if numPackets > 0
        i = i+1;
        % Get frame (uses read_message_data_float)
        [frame, ctr] = radar.GetFrameNormalized();
        
        if i == 1
            numBins = length(frame);
            if strcmp('bb', dataType)
                numBins = numBins/2;
            end
            binLength = (frameStop-frameStart)/(numBins-1);
            rangeVec = (0:numBins-1)*binLength + frameStart;
            ph.XData = rangeVec;
        end
        
        switch dataType
            
            case 'rf'
                ph.YData = frame;
                ylim([-1.2 1.2]);
            case 'bb'
                frame = frame(1:end/2) + 1i*frame(end/2 + 1:end);
                ph.YData = abs(frame); 
                ylim([-0.01 0.1]);
        end
        
            
        
        th.String = ['FrameNo: ' num2str(i) ' - Length: ' num2str(length(frame)) ' - FrameCtr: ' num2str(ctr)];
        
        drawnow;
        if mod(i,100)==0
            disp(['Packets available: ' num2str(radar.bufferSize())]);
        end
        
    end
        
end

% Stop streaming.
radar.stop();

tspent = toc(tstart);

% Output short summary report.
framesRead = i;
totFramesFromChip = ctr;

FPS_est = framesRead/tspent;

framesDropped = ctr-i;

disp(['Read ' num2str(framesRead) ' frames. A total of ' num2str(totFramesFromChip) ' were sent from chip. Frames dropped: ' num2str(framesDropped)]);
disp(['Estimated FPS: ' num2str(FPS_est) ', should be: ' num2str(FPS)]);

radar.close();
clear radar frame