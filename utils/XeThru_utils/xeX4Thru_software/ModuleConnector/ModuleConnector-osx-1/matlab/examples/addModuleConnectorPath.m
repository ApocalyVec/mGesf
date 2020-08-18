function addModuleConnectorPath(optarg)

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
    elseif ismac || islinux
        addpath('../../lib/');
    end

end