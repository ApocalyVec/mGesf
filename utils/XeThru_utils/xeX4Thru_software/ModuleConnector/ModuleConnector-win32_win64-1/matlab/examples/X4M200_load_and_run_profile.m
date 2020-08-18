% This example demonstrates the use of ModuleConnector to load the adult
% respiration profile and read and plot the sleep message from the module.
%
% To complete the following example you need:
% - An X4M200 module
% - The ModuleConnector library
% - MATLAB
%

%add paths
addModuleConnectorPath();
% if running on a 32-bit Windows system, instead run:
% addModuleConnectorPath('win32');

clc
clear

% Device com-port
COMPORT = 'COM4';

% Load the library
Lib = ModuleConnector.Library;
% Display the functions available in the library
Lib.libfunctions;

% Create ModuleConnector object
mc = ModuleConnector.ModuleConnector(COMPORT,0);

% Get X4M200 interface
x4m200 = mc.get_x4m200();


% Display system info
disp(['ItemNumber = ' x4m200.get_system_info(0)]);
disp(['OrderCode = ' x4m200.get_system_info(1)]);
disp(['FirmWareID = ' x4m200.get_system_info(2)]);
disp(['Version = ' x4m200.get_system_info(3)]);
disp(['Build = ' x4m200.get_system_info(4)]);
disp(['SerialNumber = ' x4m200.get_system_info(6)]);
disp(['VersionList = ' x4m200.get_system_info(7)]);

% Read OrderCode
OrderCode = x4m200.get_system_info(1);
% Check that OrderCode of connected module is actually X4M200
try
    assert(strcmp(OrderCode,'X4M200'),['FW of connected module is ' OrderCode ' and not X4M200.']);
catch me
    disp(me.message);
    % Respiration adult profile is not supported on this module. Skip rest
    % of example.
    % Clean up.
    clear mc;
    clear x4m200;
    Lib.unloadlib;
    clear Lib;
    return;
end

%% Preparation

% Usually sensor autostarts, so first set sensor mode stop
x4m200.set_sensor_mode('stop');

% Clear message buffers
while x4m200.peek_message_respiration_legacy > 0
    x4m200.read_message_respiration_legacy;
end

while x4m200.peek_message_respiration_sleep > 0
    x4m200.read_message_respiration_sleep;
end

while x4m200.peek_message_respiration_movinglist > 0
    x4m200.read_message_respiration_movinglist;
end

while x4m200.peek_message_respiration_detectionlist > 0
    x4m200.read_message_respiration_detectionlist;
end

% Load adult respiration (respiration2) profile.
ProfileID_Respiration2 = hex2dec('064e57ad');
ret = x4m200.load_profile(ProfileID_Respiration2);

if ret
    error('Failed to load profile. Check ModuleConnector logs.');
end

%% Do some configuration

% Set detection zone from 0.4 to 2.0 meters
x4m200.set_detection_zone(0.4,2.0);

% Set sensitivity
x4m200.set_sensitivity(7);

%% Run

% Set sensor mode run
x4m200.set_sensor_mode('run');

%% Visualize data

% Most of the code in this section is to handle visualization of the sleep
% message data from the module. Reading of the sleep message from the
% module is done with the command x4m200.read_message_respiration_sleep().

% Read and plot data from module. Close figure to stop.
fh = figure(1);
clf(1);
maxHistory = 60*17; % 60 seconds * 17 frames per second -> 1 minute history
numFields = 6; % Actually 7, but first field is counter and is used for time vector.
for i = 1:numFields
    % Create handles for update of plots.
    subplot(numFields,1,i);
    ph(i) = plot(NaN,NaN);
    grid on;
    th(i) = title('');
    ax(i) = gca;
end

% Loop as long as figure is open.
while ishandle(fh)
    sleep_message = x4m200.read_message_respiration_sleep();
    fields = fieldnames(sleep_message);
    for i = 1:numFields
        % Update data for each subplot.
        ph(i).XData = [ph(i).XData sleep_message.counter];
        ph(i).YData = [ph(i).YData sleep_message.(fields{i+1})];
        th(i).String = [strrep(fields{i+1},'_','\_') ' - frameCounter = ' num2str(sleep_message.counter)];
        xlim(ax(i),[sleep_message.counter-maxHistory sleep_message.counter]);
    end
    drawnow;
end

%% Stop radar and close connection

% Set sensor mode stop
x4m200.set_sensor_mode('stop');
% Reset module
x4m200.module_reset();

% Clean up.
clear mc;
clear x4m200;
Lib.unloadlib;
clear Lib;
