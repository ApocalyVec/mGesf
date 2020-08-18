% Latest examples is located at https://github.com/xethru/XeThru_ModuleConnector_Examples or https://dev.azure.com/xethru/XeThruApps/_git/XeThru_ModuleConnector_Examples.
% 
% Target module: 
% X4M200
% X4M300
% 
% Introduction: 
% This is an example of how to get and plot baseband IQ or AP messages from X4M200, X4M300.
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

%% User configurations:
% File or Port from where to stream data. e.g./xethru_recording_20180622_113743_Float_Data_recording/xethru_recording_meta.dat or COM3 
%input = './xethru_recording_20181116_151220_Sensor_Baseband_Data_recording/xethru_recording_meta.dat';
input = 'COM13';
% Whether to output IQ og AP baseband data,0 if you want IQ-baseband
AP_output = 1;

% 1. When input is recording data
% Playback speed
playback_rate = 1.0; % X times normal speed


% 2. When input is real XeThru module
% Whether to record data or not, set to 0 if you don't want to record.
rec = 1;
% Detection zone
start=0.4;
stop=5;

% Load the library
Lib = ModuleConnector.Library;

if AP_output
    data_type = ModuleConnector.DataRecorderInterface.DataType_BasebandApDataType;
else
    data_type = ModuleConnector.DataRecorderInterface.DataType_BasebandIqDataType;
end

% Create DataPlayer object
if contains(input, 'xethru_recording_meta.dat')
    %% Dataplayer configurations 
    playback=1;
    player = ModuleConnector.DataPlayer(input);
    % Create ModuleConnector object with DataPlayer as argument.
    mc = ModuleConnector.ModuleConnector(player,0);
    app = mc.get_x4m200();%% x4m200 and x4m300 has same baseband IQ and AP message types.
    % Filter for Float data
    % Specify data type for playback/recording.
    player.set_filter(data_type);
    player.set_position(0);
    % Set playback rate
    player.set_playback_rate(playback_rate);
    % Enable loop 
    player.set_loop_mode_enabled(1);
else 
    playback=0;
    % Create ModuleConnector object
    OrderCode = disp_module_info(input);
    mc = ModuleConnector.ModuleConnector(input,0);
    % Get recorder
    if rec 
        recorder = mc.get_data_recorder();
        % Set session id.
        recorder.set_session_id('Sensor_Baseband_Data_recording');
        % Output directory for recorded data
        output_dir = '.';
        recorder.start_recording(data_type, output_dir);
    end 

    % Need to do some preparation if the connected module is a X4M300 or X4M200
    switch OrderCode
        case {'X4M300'}
            % Running X4M300 module.
            app = mc.get_x4m300();
            profile=hex2dec('014d4ab8'); %Presence profile
        case {'X4M200'}
            % Running X4M200 module.
            app = mc.get_x4m200();
            profile=hex2dec('47fabeba'); %Respiration profile
        otherwise
            error(['Unknown device connected. Returns Module ' module]);
    end
    app.set_sensor_mode('stop');
    % Configure for either outputs 
    if AP_output
        % flush buffer
        while app.peek_message_baseband_ap > 0
            app.read_message_baseband_ap(); 
        end
        output=hex2dec('0000000d'); %AP-baseband output 
        if rec
            datatype = ModuleConnector.DataRecorderInterface.DataType_BasebandApDataType;
            recorder.set_session_id('AP_Baseband_recording');  

        end
    else
        output=hex2dec('0000000c'); %IQ-baseband output
        % flush buffer
        while app.peek_message_baseband_iq > 0
            app.read_message_baseband_iq();
        end
        if rec
            datatype = ModuleConnector.DataRecorderInterface.DataType_BasebandIqDataType;
            recorder.set_session_id('IQ_Baseband_recording');  
        end
    end
end


% Set module settings and start recording/playback
if playback
    % Start playback
    player.play();
else 
    % Apply apropriate module settings 
    app.load_profile(profile);
    app.set_output_control(output, 1);
    app.set_detection_zone(start , stop);
    app.set_sensor_mode('run');
end
% Configuring subplots for Movement vectors
fh = figure(1);
clf(1);

% Configuring subplots for Movement vectors
subplot(2,1,1);
fh_f = plot(NaN,NaN);
th_f = title('');
grid on;
ax_1 = gca;
xlim([start, stop]);
ylim([0,0.1]);


subplot(2,1,2);
fh_s = plot(NaN,NaN);
th_s = title('');
grid on;
ax_2 = gca;
xlim([start, stop]);
ylim([0,0.1]);


% Loop as long as figure is open.
while ishandle(fh)
    if AP_output
        [A_I, P_Q, header] = app.read_message_baseband_ap();
        ylim(ax_1,[0 max([0 max(A_I)])]);
        ylabel(ax_1, 'Amplitude')
        ylim(ax_2,[-3*pi/2 3*pi/2]);
        ylabel(ax_2, 'Phase')
    else
        [message, header] = app.read_message_baseband_iq();
        A_I = real(message);
        ylim(ax_1, [min(A_I) max(A_I)]);
        ylabel(ax_1, 'I-Vectors')
        P_Q = imag(message);
        ylim(ax_2, [min(P_Q) max(P_Q)]);
        ylabel(ax_2, 'Q-Vectors')
    end
    count = header.frame_counter;

    th_f.String = ['FrameNo: ' num2str(count)];
    message_length = length(A_I);
    range_axis = linspace(start,stop, message_length);
    
    fh_f.XData = range_axis;
    fh_s.XData = range_axis;
    
    fh_f.YData = A_I;
    fh_s.YData = P_Q;
    
    
    drawnow;
end

if playback
    player.stop;
    clear player;
else 
    % Stop and reset module
    app.set_sensor_mode('stop');
    app.module_reset();
    if rec
        % Stop recording.
        recorder.stop_recording(datatype);
    end
end


% Clean up.
clear;