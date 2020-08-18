classdef BasicRadarClass < handle
    % BasicRadarClass provides aggregated calls to ModuleConnector.
    % 
    % Example usage: 
    %   Lib = ModuleConnector.Library;         % Load library
    %   radar = BASICRADARCLASS('COM3');       % Init connection to radar
    %   radar.open;                            % Open radar connection 
    %   frame = radar.GetSingleFrameNormalized;% Capture a frame
    %   plot(plot)                             % Plot frame
    %   radar.close;                           % Close radar connection
    % 
    %
    % BasicRadarClass methods:
    %   disp                        - 
    %   open                        - Open radar connection.
    %   close                       - Close radar connection.
    %   start                       - Start C++ library frame buffering
    %   stop                        - Stop  C++ library frame buffering
    %   clear                       - Clear C++ library frame buffer
    %   bufferSize                  - Count C++ library frames buffered
    %   GetFrameNormalized          - Fetch a radar frame from library buffer
    %   GetSingleFrameNormalized    - Capture a radar frame
    %   GetVariable                 - Return a chip or firmware variable
    %   SetVariable                 - Set a chip or firmware variable
    %   RunTimingMeas               - Updates FW timing measurement data 
    %   GetAllVariables             - Return a settings map
    %    
    % See also FUNCTIONALRADARCLASS, MODULECONNECTOR
    %
    %   Copyright 2016 Novelda AS
    
    properties (SetAccess = protected)
        var = ModuleConnector.NVA_X2_Variable;
        debugLvl = 5;
    end
    
   properties
        FPS = 20
        COM
    end

    % States    -- Do not edit --
    properties (SetAccess = private) 
        isOpen = 0;
        isStreaming = 0;
        libLoaded = 0;
    end
    
    % Pointers  -- Do not edit --
    properties (SetAccess = private,Hidden)
        radarInstance % radar interface
        mc % module connector
    end

    methods

        %% Constructors and Destructors
        %
        function r = BasicRadarClass(COM)
            % Constructor
            r.COM = COM;
        end
        
        function delete(obj)
            % Destructor
            disp('Closing radar')
            obj.close();
        end
    end

    methods

        function disp(r)
            libstatus = {'Not Loaded', 'Loaded'};
            displ.isOpen=r.isOpen;
            if isstruct(r.mc) || r.isOpen
            displ.(r.mc.lib_name)=libstatus{libisloaded(r.mc.lib_name)+1};
            end
            displ.isStreaming=r.isStreaming;
            disp(displ);
        end

        %%
        % Open radar / Close radar
        %
        function r = open(r)
            % Open radar connection
            % r = OPEN(r)
            if ~r.isOpen
                r.mc = ModuleConnector.ModuleConnector(r.COM);
                r.radarInstance = r.mc.get_x2();
                r.isOpen = 1;
            end
        end

        function r = close(r)
            % Close radar connection
            % r = CLOSE(r)
            if r.isOpen
                r.stop();
                r.radarInstance = [];
                r.mc = [];
                r.isOpen = 0;
            end
        end

        %%
        % Start radar / Stop radar
        %
        function r = start(r)
            % Opens radar, sets FPS, sets manual mode, subscribes to raw frames normalized. 
            % r = START(r)
            if ~r.isOpen
                r.open();
            end
            
            r.radarInstance.set_fps(r.FPS);
            r.radarInstance.subscribe_to_raw_normalized;
            r.radarInstance.set_sensor_mode_manual;
            r.isStreaming = 1;
        end
        
        function r = stop(r)
            % Unsubscribes to raw frames normalized, sets idle mode. 
            % r = STOP(r)
            if r.isOpen && r.isStreaming
                r.radarInstance.set_sensor_mode_idle();
                r.clear('raw_normalized');
                r.radarInstance.unsubscribe('raw_normalized');
                r.isStreaming = 0;
            end
        end

        function cleared_frames = clear(r,name)
            % Clear the packets buffered for the subscription <name>
            % number_of_cleared_frames = CLEAR(r,name)
            if nargin==1, name = 'raw_normalized'; end
            cleared_frames = r.bufferSize(name);
            r.radarInstance.clear(name);
        end

       function bufferSize = bufferSize(r,name)
          % Return the number of packets buffered for the subscription <name>
          % buffer_size = BUFFERSIZE(r,name)
          if nargin==1, name = 'raw_normalized'; end
           bufferSize = r.radarInstance.get_number_of_packets(name);
        end

        function [frame,counter] = GetFrameNormalized(r)
            % Get radar frame in streaming mode
            % [frame,counter] = GETFRAMENORMALIZED(r)
            if r.isOpen && r.isStreaming
                [frame,counter] = r.radarInstance.get_raw_normalized_frame('raw_normalized');
            else
                warning('radar is not streaming or it is closed');
                frame = 0;
            end
        end

        function result = ping(r)
            % result = PING(r)
            if r.isOpen
                result = r.radarInstance.ping;
            end
        end
        
        
        function frame = GetSingleFrameNormalized(r)
            % Get one radar frame
            % frame = GETSINGLEFRAMENORMALIZED(r)
            if r.isOpen
                frame = r.radarInstance.capture_single_normalized_frame();
            else
                warning('radar is not streaming or it is closed');
                frame = 0;
            end
        end

        function ok = RunTimingMeas(r)
            ok = 0;
            if r.isOpen
                r.radarInstance.run_timing_measurement();
                ok = 1;
            else
                warning('radar is not streaming or it is closed');
            end
        end
        

        function varia = GetVariable(r,var)
            % Get radar variable
            %   value = GETVARIABLE(r,variable)
            %   value = GETVARIABLE(r,NVA_X2_Variable.property)
            %   value = GETVARIABLE(r,{double,char})
            if r.isOpen
                vartype = var{2};
                var = var{1};
                switch vartype
                    case 'REGFIELD'
                        [varia,status] = r.radarInstance.get_x2_register(var);
                        assert(status==0, 'nva_get_x2_register failed');
                    case 'INT'
                        [varia,status] = r.radarInstance.get_x2_int(var);
                        assert(status==0, 'nva_get_x2_int failed');
                    case 'FLOAT'
                        [varia,status] = r.radarInstance.get_x2_float(var);
                        assert(status==0, 'nva_get_x2_float failed');
                    case 'UNKNOWN'
                        warning('Unknown variable');
                end
            else
                warning('radar is closed');
                varia = nan;
            end
        end
        
 
        function status = SetVariable(r,var,val)
            % Set radar variable
            %   status = SETVARIABLE(r,variable,value)
            %   status = SETVARIABLE(r,NVA_X2_Variable.property,value)
            %   status = SETVARIABLE(r,NVA_X2_API,{double,char},value)
            if r.isOpen                
                vartype = var{2};
                var = var{1};
                switch vartype
                    case 'REGFIELD'
                        status = r.radarInstance.set_x2_register(var,val);
                        %assert(status==0, 'nva_set_x2_register failed');
                    case 'INT'
                        status = r.radarInstance.set_x2_int(var,val);
                        %assert(status==0, 'nva_set_x2_int failed');
                    case 'FLOAT'
                        status = r.radarInstance.set_x2_float(var,val);
                        %assert(status==0, 'nva_set_x2_float failed');
                    case 'UNKNOWN'
                        warning('Unknown variable');
                end
            else
                warning('radar is closed');
                status = 1;
            end
        end
        
        %
        % Get all variables currently set in firmware
        % Syntax:
        %   <containers.Map> = BasicRadarClass.GetAllVariables()
        %
        function map = GetAllVariables(r)
            % Get radar variable Map
            % map = GETALLVARIABLES(r)
            if r.isOpen
                map = containers.Map();
                prp = properties(r.var);
                
                for k=1:length(prp)
                    try
                        map(prp{k})=r.GetVariable(r.var.(prp{k}));
                    catch ME
                        disp(ME);
                    end
                end
            end
        end
        
    end
       
end
