% This example demonstrates the use of ModuleConnector.XEPs file API to
% read the parameter file from a module's flash storage.
%
% To complete the following example you need:
% - An X4M200/X4M300/X4M03 module(XEP for X4)
% - The ModuleConnector library
% - MATLAB
%

%add paths
addModuleConnectorPath();
% if running on a 32-bit Windows system, instead run:
% addModuleConnectorPath('win32');


clc
clear
% Load the library
Lib = ModuleConnector.Library;
Lib.libfunctions;

COMPORT = 'COM4';

mc = ModuleConnector.ModuleConnector(COMPORT,0);
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
clear xep;
clear mc;
Lib.unloadlib;
clear Lib;
