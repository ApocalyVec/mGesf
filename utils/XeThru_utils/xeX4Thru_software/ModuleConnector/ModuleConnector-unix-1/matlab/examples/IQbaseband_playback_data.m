% This example demonstrates the use of ModuleConnector.DataPlayer to
% playback baseband iq data from file. If you wish to generate such a file,
% you can run the X4M200_X4M300_IQbaseband_record_data.m example.
%
% To complete the following example you need:
% - A dataset with recorded baseband iq data
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
% Display the functions available in the library
Lib.libfunctions;

% Name of metafile e.g. '.../xethru_recording_<timestamp>_<sessionID>/xethru_recording_meta.dat'
metafilename = './xethru_recording_20170913_150437_baseband_recording/xethru_recording_meta.dat';

% Create DataPlayer object
player = ModuleConnector.DataPlayer(metafilename);

% Create ModuleConnector object with DataPlayer as argument.
mc = ModuleConnector.ModuleConnector(player,0);

% Get application interface
app = mc.get_x4m300();

%% DataPlayer configuration

% Specify data type for playback.
baseband_iq_data_type = ModuleConnector.DataRecorderInterface.DataType_BasebandIqDataType;

% Set filter to only playback baseband iq data.
player.set_filter(baseband_iq_data_type);

% Set start position 0 (frankly not necessary)
player.set_position(0);

% Set playback rate
rate = 10.0; % 10 times normal speed
player.set_playback_rate(rate);

%% Run playback and visualize data

% Start playback
player.play();

% At this point, baseband iq data can be read from application interface as
% if it was coming from a module.

% Read and plot data from file. Close figure to stop.
fh = figure(1);
clf(1);

% Create handles for update of plots.
subplot(2,1,1);
ph_amp = plot(NaN,NaN);
th_amp = title('');
ax_amp = gca;
ylim([0 0.03]);
grid on;
xlabel('Range [m]');
subplot(2,1,2);
ph_phase = plot(NaN,NaN);
th_phase = title('');
ax_phase = gca;
ylim([-3*pi/2 3*pi/2]);
grid on;
xlabel('Range [m]');

range_vector = [];

% Loop through file
while app.peek_message_baseband_iq > 0 && ishandle(fh)
    [bb_iq, hdr] = app.read_message_baseband_iq;
    if isempty(range_vector)
        % First time, generate range vector.
        range_vector = (0:hdr.num_bins-1)*hdr.bin_length + hdr.range_offset;
        ph_amp.XData = range_vector;
        ph_phase.XData = range_vector;
    end
    ph_amp.YData = abs(bb_iq);
    ylim(ax_amp,[0 max([0.03 max(abs(bb_iq)) + 0.01])]);
    th_amp.String = ['Baseband amplitude - frameCounter = ' num2str(hdr.frame_counter)];
    ph_phase.YData = atan2(imag(bb_iq),real(bb_iq));
    th_phase.String = ['Baseband phase - frameCounter = ' num2str(hdr.frame_counter)];
    drawnow;
end

% Clean up.
clear app;
clear mc;
clear player;
Lib.unloadlib;
clear Lib;
