% This example illustrates how to manipulate stored noisemaps on the module.
%
% To complete the following example you need:
% - An X4M200 or X4M300 module
% - The ModuleConnector library
% - MATLAB


%add paths
addModuleConnectorPath();
% if running on a 32-bit Windows system, instead run:
% addModuleConnectorPath('win32');

clc
clear


% Specify parameters
COMPORT             = 'COM4';   % Device com-port
options.interface   = 'x4m300';         % Interface to use. x4m300 or x4m200

Lib = ModuleConnector.Library;

clear mc; mc = ModuleConnector.ModuleConnector(COMPORT, 0);
pause(1);

% Get the XEP interface
xep = mc.get_xep();

% Get app interface and set profile
if strcmp( options.interface, 'x4m200' )
    app = mc.get_x4m200();
    % Adult respiration (respiration2) profile.
    profile = hex2dec('064e57ad');
elseif strcmp( options.interface, 'x4m300' )
    app = mc.get_x4m300();
    % Adult presence (presence2) profile.
    profile = hex2dec('014d4ab8');
else
    error('Interface not recognized.\n')
end

% Load a profile. Noisemap controls are stored associated with this profile,
% meaning setting/getting it witout loading a profile doesn't work.
status = app.load_profile(profile);
if status, error('Failed loading profile.'); end

% Set a detection zone. The noisemap requires a matching detection zone.
status = app.set_detection_zone(1, 3);
if status, error('Failed setting detection zone.'); end

[ctrl, status] = app.get_noisemap_control();
if status, error('Failed getting noisemap control.'); end

% Enable use of noisemap
ctrl = bitor(ctrl, 1);
% Use adaptive
ctrl = bitor(ctrl, 2);
% Don't reinitialize noisemap if stored, and store it after initialization if
% not present.
ctrl = bitand(ctrl, bitcmp(uint32(4)));
status = app.set_noisemap_control(ctrl);
if status, error('Failed setting noisemap control.'); end

% Delete old noisemap if any, forcing it to initialize and store a new one.
files = xep.find_all_files();
if ismember(xep.XTFILE_TYPE_NOISEMAP_FAST, files)
    xep.delete_file(xep.XTFILE_TYPE_NOISEMAP_FAST, 0);
end
if ismember(xep.XTFILE_TYPE_NOISEMAP_SLOW, files)
    xep.delete_file(xep.XTFILE_TYPE_NOISEMAP_SLOW, 0);
end

disp('Initializing a noisemap. Vacate the premises.');
pause(5);
status = app.set_sensor_mode('run');
if status, error('Failed starting sensor.'); end
disp('waiting for 130s ...');
pause(130); % 10 seconds margin of error
status = app.set_sensor_mode('stop');
if status, error('Failed stopping sensor.'); end
disp('Done.')
pause(1);

files = xep.find_all_files();
if ~ismember(xep.XTFILE_TYPE_NOISEMAP_FAST, files)
    error('Did not store a fast noisemap');
end
if ~ismember(xep.XTFILE_TYPE_NOISEMAP_SLOW, files)
    error('Did not store a slow noisemap');
end

nm_fast = xep.get_file(xep.XTFILE_TYPE_NOISEMAP_FAST, 0);
nm_slow = xep.get_file(xep.XTFILE_TYPE_NOISEMAP_SLOW, 0);

% Do it once more, but with a smaller detection zone.

status = app.load_profile(profile);
if status, error('Failed loading profile.'); end
status = app.set_detection_zone(1, 2);
if status, error('Failed setting detection zone.'); end
xep.delete_file(xep.XTFILE_TYPE_NOISEMAP_FAST, 0);
xep.delete_file(xep.XTFILE_TYPE_NOISEMAP_SLOW, 0);

disp('Initializing another one.');
status = app.set_sensor_mode('run');
if status, error('Failed starting sensor.'); end
disp('waiting for 130s ...');
pause(130);
status = app.set_sensor_mode('stop');
if status, error('Failed stopping sensor.'); end
disp('Done.')
pause(1);

files = xep.find_all_files();
if ~ismember(xep.XTFILE_TYPE_NOISEMAP_FAST, files)
    error('Did not store a fast noisemap');
end
if ~ismember(xep.XTFILE_TYPE_NOISEMAP_SLOW, files)
    error('Did not store a slow noisemap');
end

short_nm_fast = xep.get_file(xep.XTFILE_TYPE_NOISEMAP_FAST, 0);
short_nm_slow = xep.get_file(xep.XTFILE_TYPE_NOISEMAP_SLOW, 0);

xep.delete_file(xep.XTFILE_TYPE_NOISEMAP_FAST, 0);
xep.delete_file(xep.XTFILE_TYPE_NOISEMAP_SLOW, 0);

% Once more, with a restored previous noisemap.

% Upload noisemap to module flash.
xep.set_file(xep.XTFILE_TYPE_NOISEMAP_FAST, 0, nm_fast);
xep.set_file(xep.XTFILE_TYPE_NOISEMAP_SLOW, 0, nm_slow);
pause(1);

disp('Starting profile with the first noisemap.');
status = app.load_profile(profile);
if status, error('Failed loading profile.'); end

% This must match the detection zone the noisemap was made with for the
% restoring to work.
status = app.set_detection_zone(1, 3);
if status, error('Failed setting detection zone.'); end

display('Initialization should now only be around 20 seconds.');
status = app.set_sensor_mode('run');
if status, error('Failed starting sensor.'); end
pause(20);
display('Initialization done.');


display('Getting a list of noisemaps as they evolve over time.');
for ix=[1:5]
    pause(50);
    status = app.store_noisemap();
    if status, error('Failed storing noisemap.'); end
    % Store noisemap takes around a second to complete, pluss a small varying
    % amount of time for the flash writing to catch up.
    pause(10);
    nms_fast{ix} = xep.get_file(xep.XTFILE_TYPE_NOISEMAP_FAST, 0);
    nms_slow{ix} = xep.get_file(xep.XTFILE_TYPE_NOISEMAP_SLOW, 0);
end

status = app.set_sensor_mode('stop');
if status, error('Failed stopping sensor.'); end
