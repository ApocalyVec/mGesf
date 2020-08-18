% This example demonstrates the use of ModuleConnector.DataRecorder to
% record the baseband iq message to file.
%
% To complete the following example you need:
% - An X4M200/X4M300 module
% - The ModuleConnector library
% - MATLAB
%

%add paths
addModuleConnectorPath();
% if running on a 32-bit Windows system, instead run:
% addModuleConnectorPath('win32');

clc
clear


% Specify output directory
output_dir = '.';
% Specify recording duration
recording_duration = 30; % seconds

% Device com-port
COMPORT = 'COM4';

% Load the library
Lib = ModuleConnector.Library;
% Display the functions available in the library
Lib.libfunctions;

% Create ModuleConnector object
mc = ModuleConnector.ModuleConnector(COMPORT,0);

% Get application interface
app = mc.get_x4m300();

% Read FW version
OrderCode = app.get_system_info(1);
clear app;
%%
switch OrderCode
    case 'X4M300'
        app = mc.get_x4m300();
        profileID = hex2dec('014d4ab8'); % Presence
    case 'X4M200'
        app = mc.get_x4m200();
        profileID = hex2dec('064e57ad'); % Respiration adult (respiration2)
    otherwise
        error(['Connected module does not support this example. Must be X4M300 or X4M200. OrderCode ' OrderCode]);
end

% Display system info
disp(['ItemNumber = ' app.get_system_info(0)]);
disp(['OrderCode = ' app.get_system_info(1)]);
disp(['FirmWareID = ' app.get_system_info(2)]);
disp(['Version = ' app.get_system_info(3)]);
disp(['Build = ' app.get_system_info(4)]);
disp(['SerialNumber = ' app.get_system_info(6)]);
disp(['VersionList = ' app.get_system_info(7)]);

%% DataRecorder configuration

% Get DataRecorder
recorder = mc.get_data_recorder();

% Specify data type for recording
baseband_iq_data_type = ModuleConnector.DataRecorderInterface.DataType_BasebandIqDataType;

% Set session id.
recorder.set_session_id('baseband_recording');

% Set file split duration 10 min (600 seconds)
recorder.set_file_split_duration(600);

%% Preparation

% Usually sensor autostarts, so first set sensor mode stop
app.set_sensor_mode('stop');

% Load profile.
app.load_profile(profileID);

% Enable baseband iq output
output_control_bbiq = hex2dec('0000000c');
app.set_output_control(output_control_bbiq,1);

% Set detection zone from 0.4 to 3.0 meters
app.set_detection_zone(0.4,3.0);

% Start recording.
recorder.start_recording(baseband_iq_data_type,output_dir);

%% Run

% Set sensor mode run
app.set_sensor_mode('run');

% Let radar run for specified duration.
pause(recording_duration);

%% Stop radar and close connection

% Set sensor mode stop
app.set_sensor_mode('stop');

% Stop recording.
recorder.stop_recording(baseband_iq_data_type);

% Reset module
app.module_reset();

% Clean up.
clear app;
clear mc;
clear recorder;
Lib.unloadlib;
clear Lib;
