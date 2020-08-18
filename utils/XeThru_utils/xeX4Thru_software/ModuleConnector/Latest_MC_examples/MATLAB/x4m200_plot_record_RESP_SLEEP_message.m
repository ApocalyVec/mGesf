% Latest examples is located at https://github.com/xethru/XeThru_ModuleConnector_Examples or https://dev.azure.com/xethru/XeThruApps/_git/XeThru_ModuleConnector_Examples.
% 
% Target module:
% X4M200
% 
% Introduction:
% This is an example of how to print out application messages from X4M200 module.
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
% addModuleConnectorPath('win32');

clc
clear
%% User configurations:
% Select comport 
device_name = 'COM4';
% Enable/disable recording
rec = 0; 
% sensor configurations:
profile = hex2dec('064e57ad'); %Respiration profile
detection_zone_start = 0.4;
detection_zone_end = 4;
sensitivity = 5;
%noisemap_control = 7; %initialize noisemap everytime when get start (approximately 120s)
noisemap_control = 6; %use default noise map to start quickly (approximately 20s)
resp_movinglist_message = hex2dec('610a3b00'); %movinglist output 
sleep_status_message = hex2dec('2375a16c'); %sleep output 
resp_status_message = hex2dec('2375fe26'); %sleep output 
%% Configure X4M200
disp_module_info(device_name);
% Load the library
Lib = ModuleConnector.Library;
% Moduleconnector object and X4M200 interface
mc = ModuleConnector.ModuleConnector(device_name,0);
X4M200 = mc.get_x4m200;

% Start sensor 
if rec
    % Specify output directory
    output_dir = '.';
    % Get recorder 
    recorder = mc.get_data_recorder();
     % Set session id.
    recorder.set_session_id('Respiration_recording');
    recording_datatype = ModuleConnector.DataRecorderInterface.DataType_AllDataTypes;
    recorder.start_recording(recording_datatype, output_dir);
end

% Empty buffer 
while X4M200.peek_message_respiration_sleep() > 0
    X4M200.read_message_respiration_sleep(); 
end
while X4M200.peek_message_respiration_legacy() > 0
    X4M200.read_message_respiration_legacy(); 
end
while X4M200.peek_message_respiration_movinglist() > 0
    X4M200.read_message_respiration_movinglist(); 
end

% sensor configuration
X4M200.set_sensor_mode('stop');

X4M200.load_profile(profile);
X4M200.set_output_control(resp_movinglist_message, 1);
X4M200.set_output_control(sleep_status_message, 1);
X4M200.set_output_control(resp_status_message, 1);
X4M200.set_detection_zone(detection_zone_start , detection_zone_end);
X4M200.set_sensitivity(sensitivity);
X4M200.set_noisemap_control(noisemap_control);

X4M200.set_sensor_mode('run');

%% Figure config 
states = {'Breathing'; 'Movement'; 'Movement Tracking'; 'No Movement'; 'Initializing'};
fh = figure(1);
clf(1);

% Configuring subplots for Presence state and Distance plots
p_ax1=subplot(6,1,1);
ph_ps1 = plot(NaN,NaN);
h1 = animatedline;
th_ax1 = title('Respiration State');
grid on;
ylim([0,4]);
set(gca,'ytick',[0:4],'yticklabel',states);
set(gca,'XTickLabel',[]);

p_ax2=subplot(6,1,2);
ph_ps2 = plot(NaN,NaN);
h2 = animatedline;
th_ax2 = title('Distance');
grid on;
ylim([detection_zone_start-0.1,detection_zone_end+0.1]);
set(gca,'XTickLabel',[])

p_ax3=subplot(6,1,3);
ph_ps3 = plot(NaN,NaN);
h3 = animatedline;
th_ax3 = title('RPM');
grid on;
ylim([8,65]);
set(gca,'XTickLabel',[])

p_ax4=subplot(6,1,4);
ph_ps4 = plot(NaN,NaN);
h4 = animatedline;
th_ax4 = title('BreathPattern');
grid on;
%ylim([8,65]);
set(gca,'XTickLabel',[])

% Configuring subplots for Movement vectors
subplot(6,1,5);
%r_mvs = plot(NaN,NaN,'ob');
r_mvs = bar(NaN,NaN,'b');
th_mvs = title('');
grid on;
xlim([detection_zone_start, detection_zone_end]);
ylim([0,100]);
xlabel('detection zone');
ylabel('Movement slow');

subplot(6,1,6);
%r_mvf = plot(NaN,NaN,'og');
r_mvf = bar(NaN,NaN,'b');
th_mvf = title('');
grid on;
xlim([detection_zone_start, detection_zone_end]);
ylim([0,100]);
xlabel('detection zone');
ylabel('Movement fast')

% Wait for the module to initialize
%[s_message, status] = X4M200.read_message_respiration_sleep();
s_message.respiration_state = 4;
disp("Start Initialization!")
count_second = 0;
while s_message.respiration_state == 4
    count_second = count_second + 1;
    disp(count_second + " Initialization will take around 20s or 120s!");
    [s_message, status]=X4M200.read_message_respiration_sleep();
end 
disp("Initialization Complete! See plotting. ")

%% Data Visualization 
rml_message = X4M200.read_message_respiration_movinglist();

% Generate range axis using only active cells 
range_count=rml_message.movementIntervalCount;
range_axis=linspace(detection_zone_start,detection_zone_end, range_count);

% Set xaxis to the range vector generated 
r_mvs.XData = range_axis;
r_mvf.XData = range_axis; 


startTime = datetime('now');
while ishandle(fh)
    % Read messages from sensor
    while X4M200.peek_message_respiration_sleep() > 0
        [s_message, status] = X4M200.read_message_respiration_sleep(); 
    end
    while X4M200.peek_message_respiration_legacy() > 0
        [r_message, status] = X4M200.read_message_respiration_legacy(); 
    end
    
    % Update sleep status
    sleep_state = s_message.respiration_state;
    % Update distance
    dist = s_message.distance;
    rpm = s_message.RPM;
    bp = r_message.movement;
    
    % Aquire current time
    t =  datetime('now') - startTime;
    % Stream respiration state
    addpoints(h1,datenum(t),sleep_state)
    p_ax1.XLim = datenum([t-seconds(90) t]);
    % Stream distance
    addpoints(h2,datenum(t),dist)
    p_ax2.XLim = datenum([t-seconds(90) t]);
    th_ax2.String = ['Distance: ' num2str(dist)];
    % Stream RPM
    addpoints(h3,datenum(t),rpm)
    p_ax3.XLim = datenum([t-seconds(90) t]);   
    th_ax3.String = ['RPM: ' num2str(rpm)];
    % Stream breath pattern
    addpoints(h4,datenum(t),bp)
    p_ax4.XLim = datenum([t-seconds(90) t]);   
    th_ax4.String = ['BreathPattern: ' num2str(bp)];
    
    while X4M200.peek_message_respiration_movinglist() > 0
        [rml_message, status_2] = X4M200.read_message_respiration_movinglist();
    end
    % Update moving slow/fast vectors 
    mv_s=rml_message.movementSlowItem;
    mv_f=rml_message.movementFastItem;
    
    % Stream movementdata 
    r_mvs.YData = mv_s(1:range_count);
    r_mvf.YData = mv_f(1:range_count); 
    th_mvs.String = ['Frame Count: ' num2str(s_message.counter) ];
    datetick('x','keeplimits')
    drawnow; 
   
end

% Stop recording and reset sensor 
if rec
    recorder.stop_recording(recording_datatype);
end
X4M200.set_sensor_mode('stop');

% Clean up.
clear;
