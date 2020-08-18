% Latest examples is located at https://github.com/xethru/XeThru_ModuleConnector_Examples or https://dev.azure.com/xethru/XeThruApps/_git/XeThru_ModuleConnector_Examples.
% 
% Target module: 
% X4M300
% 
% Introduction: 
% This is an example of how to print out application messages from X4M300 module.
%
% prerequisite:
% 1. this example should be placed in ModuleConnector\matlab\examples
% folder.
% 2. disp_module_info.m and disp_sensor_settings.m in the same folder.
%
% How to run:
% Change User configurations section according to your needs.

%add paths
add_ModuleConnector_path();
% if running on a 32-bit Windows system, instead run:
% add_ModuleConnector_path('win32');

clc
clear
%% User configurations:
% Select comport 
device_name = 'COM13';
% Enable/disable recording
rec = 0; 
% sensor configurations:
profile = hex2dec('014d4ab8'); %Presence profile
detection_zone_start=0.4;
detection_zone_end=4;
sensitivity = 5;
%noisemap_control = 7; %initialize noisemap everytime when get start (approximately 120s)
noisemap_control = 6; %use default noise map to start quickly (approximately 20s)
pres_movinglist_message = hex2dec('723bfa1f'); %movinglist message 
pres_signal_message = hex2dec('723bfa1e'); %presence signal message 
%% Configure X4M300
disp_module_info(device_name);
% Load the library
Lib = ModuleConnector.Library;
% Moduleconnector object and X4M300 interface
mc = ModuleConnector.ModuleConnector(device_name,0);
X4M300 = mc.get_x4m300;

if rec
    % Specify output directory
    output_dir = '.';
    % Get recorder 
    recorder = mc.get_data_recorder();
    % Set session id.
    recorder.set_session_id('Presence_recording');
    movinglist_datatype = ModuleConnector.DataRecorderInterface.DataType_PresenceMovingListDataType;
    recorder.start_recording(movinglist_datatype, output_dir);
end

% Empty buffer 
while X4M300.peek_message_presence_single > 0
    X4M300.read_message_presence_single(); 
end
while X4M300.peek_message_presence_movinglist > 0
    X4M300.read_message_presence_movinglist(); 
end


% sensor configuration
X4M300.set_sensor_mode('stop');

X4M300.load_profile(profile);
X4M300.set_output_control(pres_movinglist_message, 1);
X4M300.set_output_control(pres_signal_message, 1);
X4M300.set_detection_zone(detection_zone_start, detection_zone_end);
X4M300.set_sensitivity(sensitivity);
X4M300.set_noisemap_control(6);

X4M300.set_sensor_mode('run');

%% Figure config 
states = {'No Presence'; 'Presence'; 'Initializing'};
fh = figure(1);
clf(1);

% Configuring subplots for Presence state and Distance plots
p_ax1=subplot(4,1,1);
ph_ps1 = plot(NaN,NaN);
h1 = animatedline;
th_ax1 = title('Presence State');
grid on;
ylim([0,2]);
set(gca,'ytick',[0:2],'yticklabel',states);
set(gca,'XTickLabel',[]);

p_ax2=subplot(4,1,2);
ph_ps2 = plot(NaN,NaN);
h2 = animatedline;
th_ax2 = title('Distance');
grid on;
ylim([detection_zone_start-0.1,detection_zone_end+0.1]);
set(gca,'XTickLabel',[]);

% Configuring subplots for Movement vectors
subplot(4,1,3);
ph_mvs = plot(NaN,NaN);
th_mvs = title('');
grid on;
xlim([detection_zone_start, detection_zone_end]);
ylim([0,100]);
ylabel('Movement slow')

subplot(4,1,4);
ph_mvf = plot(NaN,NaN);
th_mvf = title('');
grid on;
xlim([detection_zone_start, detection_zone_end]);
ylim([0,100]);
ylabel('Movement fast')

%% Data Visualization 
[pml_message, status]=X4M300.read_message_presence_movinglist();

% Wait for the module to initialize 
disp("Start Initialization!")
count_second = 0;
while pml_message.presence_state == 2
    count_second = count_second + 1;
    disp(count_second + " Initialization will take around 20s or 120s!");
    [pml_message, status]=X4M300.read_message_presence_movinglist();
end 
disp("Initialization Complete!")

% Generate range axis using only active cells 
range_count=pml_message.movementIntervalCount;
range_axis=linspace(detection_zone_start,detection_zone_end, range_count);

% Set xaxis to the range vector generated 
ph_mvs.XData = range_axis;
ph_mvf.XData = range_axis; 


startTime = datetime('now');
while ishandle(fh)
    % Read presence single data from sensor
    [ps_message, status_2]= X4M300.read_message_presence_single();
    % Update distance
    dist=ps_message.distance;
      
    % Read presence movinglist data from sensor 
    [pml_message, status]=X4M300.read_message_presence_movinglist();

    % Update presence state
    ps=pml_message.presence_state;
    
    % Aquire current time
    t =  datetime('now') - startTime;
    
    % Stream presence state
    addpoints(h1,datenum(t),ps)
    p_ax1.XLim = datenum([t-seconds(90) t]);

    
    % Stream distance
    addpoints(h2,datenum(t),dist)
    p_ax2.XLim = datenum([t-seconds(90) t]);
    th_ax2.String = ['Distance: ' num2str(dist)];
    
    % Update moving slow/fast vectors 
    mv_s=pml_message.movementSlowItem;
    mv_f=pml_message.movementFastItem;
    
  
    % Stream movementdata 
    ph_mvs.YData = mv_s(1:range_count);
    ph_mvf.YData = mv_f(1:range_count); 
    th_mvs.String = ['Frame Count: ' num2str(pml_message.counter) ];
    % Aquire current time
    t =  datetime('now') - startTime;
    
    datetick('x','keeplimits')
    drawnow; 
   
end

% Stop recording and reset sensor 
if rec
    recorder.stop_recording(movinglist_datatype);
end
X4M300.set_sensor_mode('stop');

% Clean up.
clear;
