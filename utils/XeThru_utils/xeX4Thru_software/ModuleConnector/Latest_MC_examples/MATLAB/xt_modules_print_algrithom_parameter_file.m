% Latest examples is located at https://github.com/xethru/XeThru_ModuleConnector_Examples or https://dev.azure.com/xethru/XeThruApps/_git/XeThru_ModuleConnector_Examples.
% 
% Target module:
% X4M200
% X4M300
% X4M03(XEP)
% 
% Introduction: 
%  This example demonstrates the use of ModuleConnector.XEPs file API to read the parameter file from a module's flash storage.
%
% prerequisite:
% 1. this example should be placed in ModuleConnector\matlab\examples
% folder.
%
% How to run:
% Change User configurations section according to your needs. 

%add paths
add_ModuleConnector_path();
% if running on a 32-bit Windows system, instead run:
% add_ModuleConnector_path('win32');

clc
clear

%%
%% User configurations:
device_name         = 'COM13';           % Device com-port

% Load the library
Lib = ModuleConnector.Library;
mc = ModuleConnector.ModuleConnector(device_name,0);
xep = mc.get_xep();

% Search for parfile by type
parfileType = ModuleConnector.XEP.XTFILE_TYPE_PARFILE;
fileIDs = xep.search_for_file_by_type(parfileType);

if isempty(fileIDs)
    disp('No parameter file found in module flash storage.');
else
    % Read and display first parfile.
    parfileData = xep.get_file(parfileType,fileIDs(1));
    
    % Display content of parfile.
    disp('Parfile contents:');
    disp(native2unicode(parfileData)');
end

% Clean up.
clear;
