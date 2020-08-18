classdef LibraryCompiled < ModuleConnector.Library
    
    % Compiled MATLAB executables requires generation of a loader mfile to
    % load libraries. This in turn requires a different syntax when using
    % loadlib(). This class is created to handle the syntax differences
    % when using libraries in a compiled executable.
    %
    % How to use in a compiled executable:
    %
    % 1) Generate matlab_wrapper.m and *thunk*.dll needed by running
    %    generateLoaderMfileAndThunk(). Note that the /lib*/ and /include/
    %    folders need to be in the MATLAB path for this to work (MATLAB
    %    might display a warning after this step).
    % 2) Several files should now appear in your current folder. The
    %    important ones amongst them are matlab_wrapper.h and
    %    libModuleConnector_thunk_*.dll.
    % 3) In your compiled application, all calls to ModuleConnector.Library
    %    must be replaced by ModuleConnector.LibraryCompiled.
    % 4) Make sure that the script you want to compile adds the /lib*/ and
    %    /include/ folders to the path.
    % 5) Add '-a libModuleConnector_thunk_*.dll' (replace the * so the name
    %    is equal to the actual name of the file) as an extra argument to
    %    deploytool or mcc. We recommend using deploytool, since it
    %    automatically detects which *.m files to include in the
    %    application.
    %
    % Additional note: Make sure that the ModuleConnector path is not on
    % your global MATLAB path when running the executable, since the
    % executable actually uses the MATLAB path and might confuse the *.m
    % files in the package with the ones located on the MATLAB path, and
    % the program will crash.
    %
    % Simple example using mcc:
    %
    % lib = ModuleConnector.LibraryCompiled;
    % lib.generateLoaderMfileAndThunk();
    %
    % % matlab_wrapper.m and libModuleConnector_thunk_*.dll should now have
    % % been created.
    %
    % mcc -m my_script.m -a matlab_wrapper.m -a libModuleConnector_thunk_pcwin64.dll
    methods
    
        function obj = loadlib(obj)
            %% Load library
            if not(libisloaded(obj.library_name))
                if ~isdeployed
                    obj.generateLoaderMfileAndThunk();
                end
                [notfound,warnings] = loadlibrary(obj.library_name, @matlab_wrapper, 'alias', obj.library_name);
                if ~isempty(notfound)
                    error(notfound)
                end
            else
                disp('Library is already loaded.');
            end
        end
        
        function obj = generateLoaderMfileAndThunk(obj)
            %% Generate loader mfile and thunk dll
            % This is done by loading library with 'mfilename' input.
            [~, mfile_name] = fileparts(obj.library_includes);
            [notfound,warnings] = loadlibrary(obj.library_name,obj.library_includes,'addheader','matlab_recording_api.h',...
                'addheader','datatypes.h','mfilename',[mfile_name, '.m']);
            if ~isempty(notfound)
                error(notfound)
            end
            % The mfile and thunk dll is now created. Unload library to
            % avoid conflict.
            obj.unloadlib();
        end
    
    end
    
end