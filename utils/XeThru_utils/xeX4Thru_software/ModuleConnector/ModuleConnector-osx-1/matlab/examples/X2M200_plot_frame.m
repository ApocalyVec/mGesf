% This example demonstrates the use of ModuleConnector to
% get frame and plot out.
%
% To complete the following example you need:
% - An X2M200 module
% - The ModuleConnector library
% - MATLAB
%

%add paths
addModuleConnectorPath();
% if running on a 32-bit Windows system, instead run:
% addModuleConnectorPath('win32');

clc
clear

COMPORT = 'COM15';

% Load the library
Lib = ModuleConnector.Library;
Lib.libfunctions

%% Using ModuleConnector
%
mc = ModuleConnector.ModuleConnector(COMPORT,0);
x2 = mc.get_x2();

frame = x2.capture_single_normalized_frame();
figure;plot(frame)

clear x2 frame
clear mc

%% Using BasicRadarClass
%
radar = BasicRadarClass(COMPORT);
radar.open();

frame = radar.GetSingleFrameNormalized();
figure;plot(frame)

radar.close();
clear radar frame

%% Using FunctionalRadarClass
%
radar = FunctionalRadarClass(COMPORT);
radar.open();

radar.killstream_sec = 5; % plot 5 seconds of data 
radar.start();

figure;plot(radar)

radar.close();
clear radar frame

Lib.unloadlib();
