% Latest examples is located at https://github.com/xethru/XeThru_ModuleConnector_Examples or https://dev.azure.com/xethru/XeThruApps/_git/XeThru_ModuleConnector_Examples.
%
% Function of adding ModuleConnector library to path.
%
% Prerequisite:
% This example should be placed in ModuleConnector\matlab\examples
% folder.
% 
% How to run: 
% Run addModuleConnectorPath() on 64-bit Windows system
% if running on a 32-bit Windows system, instead run:
% addModuleConnectorPath('win32');

function add_ModuleConnector_path(optarg)
    addpath('../../matlab/');
    addpath('../../include/');
    if nargin == 1
        if strcmp(optarg,'win32')
            addpath('../../lib32/');
            return;
        end
    end
    if ispc
        addpath('../../lib64/');
    elseif ismac || isunix
        addpath('../../lib/');
    end
end