% Latest examples is located at https://github.com/xethru/XeThru_ModuleConnector_Examples or https://dev.azure.com/xethru/XeThruApps/_git/XeThru_ModuleConnector_Examples.
% 
% Target module: 
% X4M200
% X4M300
% 
% Introduction: 
% This is an example of how to get and plot movement_list messages from X4M200, X4M300.
%
% prerequisite:
% 1. this example should be placed in Moduleonnector\matlab\examples
% folder.
% 2. disp_module_info.m and disp_sensor_settings.m in the same folder.
%
% How to run:
% Change User configurations section according to your needs. 

%add paths
add_ModuleConnector_path();
% if running on a 32-bit Windows system, instead run:
% add_ModuleConnector_path('win32');

%%
% Device com-port
device_name = 'COM13';

% Whether to record data or not, set to 0 if you don't want to record.
rec = 0;
% Detection zone
detection_zone_start = 0.4;
detection_zone_end = 4;
%noisemap_control = 7; %initialize noisemap everytime when get start (approximately 120s)
noisemap_control = 6; %use default noise map to start quickly (approximately 20s)


% Load the library
OrderCode = disp_module_info(device_name);
Lib = ModuleConnector.Library;

% Create ModuleConnector object
mc = ModuleConnector.ModuleConnector(device_name,0);
% Get recorder
if rec 
    recorder = mc.get_data_recorder();
    % Output directory for recorded data
    output_dir = '.';
end 

% Need to do some preparation if the connected module is a X4M300 or X4M200
switch OrderCode
    case {'X4M300'}
        % Running X4M300 module.
        app = mc.get_x4m300();
        output=hex2dec('723bfa1f'); %movinglist output 
        profile=hex2dec('014d4ab8'); %Presence profile
        
        if rec
            recorder.set_session_id('X4M300_Movinglist_recording');
            datatype = ModuleConnector.DataRecorderInterface.DataType_PresenceMovingListDataType;
        end
    case {'X4M200'}
        % Running X4M200 module.
        app = mc.get_x4m200();
        output=hex2dec('610a3b00'); %movinglist output 
        profile=hex2dec('47fabeba'); %Respiration profile
        
        if rec
            recorder.set_session_id('X4M200_Movinglist_recording');  
            datatype = ModuleConnector.DataRecorderInterface.DataType_RespirationMovingListDataType;
        end
    otherwise
        error(['Unknown device connected. Returns Module' OrderCode]);
end
app.set_sensor_mode('stop');
% Clear buffers
if strcmp(OrderCode,'X4M300')
    while app.peek_message_presence_movinglist > 0
        app.read_message_presence_movinglist(); 
    end
else
    while app.peek_message_respiration_movinglist > 0
        app.read_message_respiration_movinglist(); 
    end
end

% Apply apropriate module settings 
app.load_profile(profile);
app.set_output_control(output, 1);
app.set_noisemap_control(noisemap_control);
app.set_detection_zone(detection_zone_start , detection_zone_end);
app.set_sensor_mode('run');
if rec
    recorder.start_recording(datatype, output_dir);
end 
% Configuring subplots for Movement vectors
fh = figure(1);
clf(1);

% Configuring subplots for Movement vectors
subplot(2,1,1);
fh_f = plot(NaN,NaN);
th_f = title('');
grid on;
xlim([detection_zone_start, detection_zone_end]);
ylim([0,100]);
ylabel('Movement Fast')

subplot(2,1,2);
fh_s = plot(NaN,NaN);
th_s = title('');
grid on;
xlim([detection_zone_start, detection_zone_end]);
ylim([0,100]);
ylabel('Movement Slow')

% Loop as long as figure is open.
while ishandle(fh)
    if strcmp(OrderCode,'X4M300')
        message=app.read_message_presence_movinglist();
    else
        message=app.read_message_respiration_movinglist();
    end
    % Aquire movement slow and fast items 
    mv_s=message.movementSlowItem;
    mv_f=message.movementFastItem;

    range_count = message.movementIntervalCount;
    range_axis = linspace(detection_zone_start,detection_zone_end, range_count);
    
    fh_f.XData = range_axis;
    fh_s.XData = range_axis;
    
    fh_f.YData = mv_f;
    fh_s.YData = mv_s;
    
    
    drawnow;
end

if rec
    % Stop recording.
    recorder.stop_recording(datatype);
end
% Stop and reset module
app.set_sensor_mode('stop');

% Clean up.

% Clean up.
clear;