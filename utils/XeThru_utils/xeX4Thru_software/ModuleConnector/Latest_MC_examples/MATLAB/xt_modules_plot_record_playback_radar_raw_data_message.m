% Latest examples is located at https://github.com/xethru/XeThru_ModuleConnector_Examples or https://dev.azure.com/xethru/XeThruApps/_git/XeThru_ModuleConnector_Examples.
% 
% Target module:
% X4M200
% X4M300
% X4M03(XEP)
%
% Introduction:
% XeThru modules support both RF and baseband data output. This is an example of radar raw data manipulation. 
% Developer can use Module Connecter API to read, record radar raw data, and also playback recorded data. 
% 
% prerequisite:
% 1. this example should be placed in ModuleConnector\matlab\examples
% folder.
% 2. disp_module_info.m and disp_sensor_settings.m in the same folder.
%
% How to run:
% Change User configurations section according to your needs. 

% add paths
add_ModuleConnector_path();
% if running on a 32-bit Windows system, instead run:
% add_ModuleConnector_path('win32');

clc
clear
%% User configurations:
% File or Port from where to stream data. e.g./xethru_recording_20180622_113743_Float_Data_recording/xethru_recording_meta.dat or COM3 
% input = './xethru_recording_20181114_154128_Float_Data_recording/xethru_recording_meta.dat'; 
 input = 'com11';
% Specify data type for playback/recording.
data_type = ModuleConnector.DataRecorderInterface.DataType_FloatDataType;
downconversion = 1; %0: RF data output. 1: baseband IQ data ouput. For data play back, you need you konw which kind data it is and configure correctly here.

% 1. When input is recording data
% Playback speed
playback_rate = 1.0; % X times normal speed

% 2. When input is real XeThru module
rec = 0;
% The configuration below is tuned for a duty
% cycle of approximately 80% at 17 FPS, which is the same as used in the
% predefined XeThru profiles. Before changing these parameters, it's
% recommended to aquire an understanding of how the sampling works on the
% X4 chip.
dac_min = 949;
dac_max = 1100;
iterations = 16;
tx_center_frequency = 3;
tx_power = 2;
pulses_per_step = 300;
frame_area_offset = 0.18;
frame_area_start = 0;
frame_area_stop = 5;
fps = 17;

%% Configure player or module

% Load the library
Lib = ModuleConnector.Library;

% Create DataPlayer object
if contains(input, 'xethru_recording_meta.dat')
    %% Dataplayer configurations 
    playback=1;
    player = ModuleConnector.DataPlayer(input);
    % Create ModuleConnector object with DataPlayer as argument.
    mc = ModuleConnector.ModuleConnector(player,0);
    % Filter for Float data
    player.set_filter(data_type);
    player.set_position(0);
    % Set playback rate
    player.set_playback_rate(playback_rate);
    % Enable loop 
    player.set_loop_mode_enabled(1);
else 
    disp_module_info(input);
    %% Recorder and Module Configurations 
    playback=0;
    mc = ModuleConnector.ModuleConnector(input,0);
    % Start sensor 
    if rec
        % Specify output directory
        output_dir = '.';
        % Get recorder 
        recorder = mc.get_data_recorder();
         % Set session id.
        recorder.set_session_id('Float_Data_recording');
        recorder.start_recording(data_type, output_dir);
    end
    app = mc.get_x4m300();
    manual = hex2dec('12');
    try
        app.set_sensor_mode('stop');
    catch 
        error('Module could not be stopped');
    end
    try 
        app.set_sensor_mode(manual);
    catch
        error('Module could not be set to manual');
    end
end 


% Get application interface
xep = mc.get_xep();

% Set module settings and start recording/playback
if playback
    % Start playback
    player.play();
else 
    while xep.peek_message_data_float > 0
    xep.read_message_data_float();
    end

    xep.x4driver_set_downconversion(downconversion);
    xep.x4driver_set_tx_center_frequency (tx_center_frequency);
    xep.x4driver_set_tx_power(tx_power); 
    xep.x4driver_set_iterations(iterations);
    xep.x4driver_set_pulsesperstep(pulses_per_step);
    xep.x4driver_set_dac_min(dac_min);
    xep.x4driver_set_dac_max(dac_max);

    % Set frame area
    xep.x4driver_set_frame_area_offset(frame_area_offset);
    xep.x4driver_set_frame_area(frame_area_start, frame_area_stop);
    % Start streaming data
    xep.x4driver_set_fps(fps);
    disp_x4_settings(xep);
end 

%% Visualize data
% Read and plot data from input.
fh = figure(1);
clf(1);
ph_rf = plot(NaN,NaN);
th_rf = title('Rada raw data plot');
ax_rf = gca;
xlabel('Range [m]');

if downconversion == 1
    ylim([0 0.08]);
else 
    ylim([-0.08 0.08]);
end  


range_vector = [];
% Close figure to stop.
 while ishandle(fh)
   [int, len, frame_count, data]= xep.read_message_data_float;   
   if downconversion == 1
      % Generate IQ vector.
      len = len/2;
      i_vec = data(1:len);
      q_vec = data(len+1:len*2);
      iq_vec = i_vec + 1i*q_vec;
      data = abs(iq_vec);
   end   
   if isempty(range_vector)
   % First time, generate range vector.
         range_vector = linspace(frame_area_start, frame_area_stop, len);
         ph_rf.XData = range_vector;
    end
    ph_rf.YData = data;
    th_rf.String = ['Frame Count: ' num2str(frame_count)];
    %ylim(ax_rf,[min(data) max(data)]);
    drawnow;
 end
 
if playback
    player.stop;
    clear player;
else 
    if rec   
        recorder.stop_recording(data_type);
    end 
    app.set_sensor_mode('stop');
end


% Clean up.
clear;
