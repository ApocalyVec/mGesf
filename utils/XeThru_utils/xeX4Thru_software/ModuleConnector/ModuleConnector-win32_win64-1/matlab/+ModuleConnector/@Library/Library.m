classdef Library < handle
    %LIBRARY This class handles loading and unloading of the XeThru Module Communication Library. 
    %
    % See also LOADLIBRARY
    
    properties
        library_name = 'libModuleConnector';    % Default library name.
        library_includes = 'matlab_wrapper.h';  % Default library includes.
        library_recording_includes = 'matlab_recording_api.h';
        library_datatypes_includes = 'datatypes.h';
    end
    
    properties (Dependent)
        libLoaded;       % Returns 1 if library is open, otherwise 0
    end
    
    methods (Access = public)
        
        function obj = Library(library_name,library_includes)
            if nargin
                obj.library_name = library_name;
                obj.library_includes = library_includes;
            end
            obj.loadlib();
        end
        
        %% Radar Library Handling
        %
        function obj = loadlib(obj)
            % Load library
            if not(obj.libLoaded)
                [notfound,warnings] = loadlibrary(obj.library_name,obj.library_includes,'addheader',obj.library_recording_includes,'addheader',obj.library_datatypes_includes);
                if ~isempty(notfound)
                    if iscellstr(notfound)
                        error(notfound{1})
                    else 
                        error('Error using loadlibrary. File or library signature not found');
                    end
                end
                warning(warnings)
            else
                disp('Library is already loaded.');
            end
        end
        
        function obj = unloadlib(obj)
            % Unload library.
            if obj.libLoaded
                unloadlibrary(obj.library_name)
                disp('Radarlib unloaded');
            else
                warning('No library to unload');
            end
        end
        
        function libfunctions(obj)
            % List all functions provided by the library
            libfunctions(obj.library_name);
        end

    end

    methods 
        function is_loaded = get.libLoaded(obj)
            is_loaded = libisloaded(obj.library_name);
        end
    end
    
end

