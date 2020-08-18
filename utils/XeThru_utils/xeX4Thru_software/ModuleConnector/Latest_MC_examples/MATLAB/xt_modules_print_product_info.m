% Latest examples is located at https://github.com/xethru/XeThru_ModuleConnector_Examples or https://dev.azure.com/xethru/XeThruApps/_git/XeThru_ModuleConnector_Examples.
% 
% Target module: 
% X4M200
% X4M300
% X4M03(XEP)
% 
% Introduction: 
% This is an example of how to print out XeThru Module infromation.
%
% prerequisite:
% this example should be placed in ModuleConnector\matlab\examples folder,
% check XeThruSensorsIntroduction application note to get details.
%
% How to run:
% Change device_name with your device name and run.

%add paths
add_ModuleConnector_path();
% if running on a 32-bit Windows system, instead run:
% add_ModuleConnector_path('win32');
clc
clear

% Device com-port
device_name = 'COM13';
disp_module_info(device_name);
clear;
    

