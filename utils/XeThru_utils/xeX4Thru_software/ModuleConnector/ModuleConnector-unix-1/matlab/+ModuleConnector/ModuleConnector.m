classdef ModuleConnector < handle
    %MODULECONNECTOR Establishes a connection to a module on a given comm port (device_name).
    %   mc = MODULECONNECTOR(device_name)
    %   mc = MODULECONNECTOR(device_name,log_level)
    %   mc = MODULECONNECTOR(device_name,log_level,lib_name)
    %
    %   Unless specified in the constructor call, the class properties default_log_level and
    %   lib_name will be used.
    %
    % % Example
    %   LIB = ModuleConnector.Library;                % Load and link runtime library
    %   mc = ModuleConnector.ModuleConnector('COM3'); % Attach module connector to port COM3
    %   x2 = mc.get_x2();                   % Create an interface to the X2 chip
    %   ChipID = x2.get_x2_register(2);               % Read X2 chip register
    %   clear x2 mc                                   % Clean up
    %   LIB.unloadlib();                              % Unload library
    %
    %
    % See also Library, X2M200, X2
    
    % Argument properties
    properties
        device_name                       % Device comm port. Set by constructor.
        default_log_level = 5;         	  % Default log level unless set by constructor.
        lib_name = 'libModuleConnector';  % Default library name unless set by constructor.
    end
    
    % Interface properties
    properties
        radarInterface % Layer one wrapper class
        dataRecorderInterface   % Layer one wrapper class for recording API
        
        mcInstance     % Libpointer to C/C++ library instance
    end
    
    methods
        
        function mc = ModuleConnector(device_name,log_level,lib_name)
            %MODULECONNECTOR Establishes a connection to a module on a
            %given comm port (device_name). Input device_name can also be a
            %DataPlayer object for playback of recorded data, or tcp://ip:port
            %for TCP/IP connection.
            %   mc = MODULECONNECTOR(device_name)
            %   mc = MODULECONNECTOR(device_name,log_level)
            %   mc = MODULECONNECTOR(device_name,log_level,lib_name)
            %
            % Unless specified in the constructor call, the class properties default_log_level and
            % lib_name will be used.
            
            
            if nargin > 2, mc.lib_name = lib_name;          end
            if nargin > 1, mc.default_log_level = log_level;end
            if nargin == 0
                error('No device specified');
            end
            
            if libisloaded(mc.lib_name)
                
                if ischar(device_name)
                    if ~isempty(strfind(device_name, 'tcp://'))
                        % Called with device name tcp. Parse IP/port and call
                        % tcp constructor.
                        rest = strsplit(device_name(7:end), ':');
                        ip = strsplit(rest{1}, '.');
                        ip = bitor(bitor(bitshift(uint32(str2double(ip{4})), 24), ...
                                         bitshift(uint32(str2double(ip{3})), 16)), ...
                                   bitor(bitshift(uint32(str2double(ip{2})), 8), ...
                                         uint32(str2double(ip{1}))));
                        port = swapbytes(uint16(str2double(rest{2})));
                        mc.device_name = device_name;
                        mc.mcInstance = calllib(mc.lib_name, 'nva_create_module_connector_tcpip', ip, port, mc.device_name, mc.default_log_level);
                        assert(~mc.mcInstance.isNull, strcat('nva_create_module_connector_tcpip failed (',mc.device_name,'/',mc.lib_name,'), verify IP and check the logs'));
                    else
                        % Called with device name COM port -> call regular
                        % constructor.
                        mc.device_name = device_name;
                        mc.mcInstance = calllib(mc.lib_name,'nva_create_module_connector',mc.device_name,mc.default_log_level);
                        assert(~mc.mcInstance.isNull, strcat('nva_create_module_connector failed (',mc.device_name,'/',mc.lib_name,'), verify COM-port and check the logs'));
                    end
                elseif isa(device_name,'ModuleConnector.DataPlayer')
                    % Called with DataPlayer argument -> call with playback
                    % constructor.
                    mc.device_name = 'playback';
                    mc.mcInstance = calllib(mc.lib_name,'nva_create_module_connector_playback',device_name.dataPlayer_instance,mc.default_log_level);
                    assert(~mc.mcInstance.isNull, strcat('nva_create_module_connector_playback failed (',mc.device_name,'/',mc.lib_name,', check the logs'));
                end
                mc.radarInterface = ModuleConnector.RadarInterface(mc.device_name,mc.default_log_level,mc.lib_name);
                
            else
                disp('ModuleConnector requires pre-loading of the dynamically linked library.')
                error(['Cannot find library ',mc.lib_name]);
            end
        end
        
        function log_level(this,new_log_level)
            %LOG_LEVEL Sets log level for ModuleConnector. Controls the
            % amount of logging the ModuleConnector will do.
            %
            calllib(this.lib_name,'nva_set_log_level',this.mcInstance,new_log_level);
        end
        
        function raw_interface = get_raw_interface(mc)
            %GET_RAW_INTERFACE Returns a Transport.
            % raw_interface = GET_RAW_INTERFACE(mc)
            raw_interface = ModuleConnector.Transport(mc);
        end
        
        function x2m200_interface = get_x2m200(mc)
            %GET_X2M200 Returns a X2M200 interface.
            % x2m200_interface = GET_X2M200(mc)
            x2m200_interface = ModuleConnector.X2M200(mc);
        end
        
        function x4m200_interface = get_x4m200(mc)
            %GET_X4M200 Returns a X4M200 interface.
            % x4m200_interface = GET_X4M200(mc)
            x4m200_interface = ModuleConnector.X4M200(mc);
        end
        
        function x4m300_interface = get_x4m300(mc)
            %GET_X4M300 Returns a X4M300 interface.
            % x4m300_interface = GET_X4M300(mc)
            x4m300_interface = ModuleConnector.X4M300(mc);
        end
        
        function x2_interface = get_x2(mc)
            %GET_X2 Returns a X2 interface.
            % x2_interface = GET_X2(mc)
            x2_interface = ModuleConnector.X2(mc);
        end
        
        function not_supported_interface = get_not_supported(mc)
            %GET_NOT_SUPPORTED Returns a not_supported_interface.
            % not_supported_interface = GET_NOT_SUPPORTED_INTERFACE(mc)
            not_supported_interface = ModuleConnector.NotSupported(mc);
        end
        
        function xep_interface = get_xep(mc)
            %GET_XEP Returns a XEP interface.
            % not_supported_interface = GET_XEP(mc)
            xep_interface = ModuleConnector.XEP(mc);
        end
        
        function x4m_common_interface = get_x4m_common(mc)
            %GET_X4M_COMMON Returns a X4M_Common_interface.
            % not_supported_interface = GET_X4M_COMMON(mc)
            x4m_common_interface = ModuleConnector.X4M_Common_Interface(mc);
        end
        
        function data_recorder = get_data_recorder(mc)
            %GET_DATA_RECORDER Returns a DataRecorder interface.
            % data_recorder = GET_DATA_RECORDER(mc)
            data_recorder = ModuleConnector.DataRecorder(mc.lib_name,mc);
        end
        
        function data_reader = get_data_reader(mc)
            %GET_DATA_READER Returns a DataReader interface.
            % data_reader = GET_DATA_READER(mc)
            data_reader = ModuleConnector.DataReader(mc);
        end
        
        function data_player = get_data_player(mc,metafilename,depth)
            %GET_DATA_PLAYER Returns a DataPlayer interface to play from
            %the specified metafilename.
            % data_player = GET_DATA_PLAYER(mc,metafilename)
            if nargin < 3, depth = -1; end
            data_player = ModuleConnector.DataPlayer(mc,metafilename,depth);
        end
        %%
        function delete(mc)
            % Destructor
            % DELETE(mc)
            clear('mc.radarInterface')
            calllib(mc.lib_name,'nva_destroy_module_connector',mc.mcInstance);
            clear('mc.mcInstance')
        end
        
    end
    
end
