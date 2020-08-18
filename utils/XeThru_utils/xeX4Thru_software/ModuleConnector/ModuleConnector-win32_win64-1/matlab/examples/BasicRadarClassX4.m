classdef BasicRadarClassX4 < handle
    % This class serves as an example of how interfacing with the module
    % through ModuleConnector can be made easier by aggregating some
    % function calls in a class.
    %
    % The BasicRadarClassX4 class uses the XEP interface to configure the
    % module and start/stop streaming.
    %
    % To see an example of this class used, see BasicRadarClassX4_example.m
    %
    %   Copyright 2016 Novelda AS
    
    properties (SetAccess = protected)
        debugLvl = 0;
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
    
    properties (SetAccess = private,Hidden)
        dataType = 'bb' % streaming type: rf or bb.
        default_FrameOffset_X4M03 = 0.18; % FrameAreaOffset for X4M03 module.
        default_FrameOffset_X4M02 = 0.18; % FrameAreaOffset for X4M02 module.
    end
    
    methods

        %% Constructors and Destructors
        %
        function r = BasicRadarClassX4(COM,FPS,dataType)
            if nargin > 1
                r.FPS = FPS;
            end
            if nargin > 2
                r.dataType = dataType;
            end                
            r.COM = COM;
        end
        
        function delete(obj)
            %% RadarLib destructor
            disp('Closing radar')
            obj.close();
        end
    end
    
    methods

        function disp(r)
            libstatus = {'Not Loaded', 'Loaded'};
            displ.(r.mc.lib_name)=libstatus(libisloaded(r.mc.lib_name));
            displ.isOpen=r.isOpen;
            displ.isStreaming=r.isStreaming;
            disp(displ);
        end

        %%
        % Open radar / Close radar
        %
        function r = open(r)
            % r = open(r): Open radar connection
            if ~r.isOpen
                r.mc = ModuleConnector.ModuleConnector(r.COM,r.debugLvl);
                r.radarInstance = r.mc.get_xep();
                r.isOpen = 1;
            end
        end

        function r = close(r)
            % r = close(r): Close radar connection
            if r.isOpen
                r.stop();
                r.radarInstance = [];
                r.mc = [];
                r.isOpen = 0;
            end
        end

        %%
        % Initialize radar
        %
        function r = init(r)
            % X4driver init.
            r.radarInstance.x4driver_init();
            switch r.dataType
                case 'rf'
                    r.radarInstance.x4driver_set_downconversion(0);
                case 'bb'
                    r.radarInstance.x4driver_set_downconversion(1);
            end
            
            % Init GPIO pins.
            r.initGPIO();
            
            % Set default FrameAreaOffset
            r.radarInstance.x4driver_set_frame_area_offset(r.default_FrameOffset_X4M03);
            
            % Set TX power
            r.radarInstance.x4driver_set_tx_power(2);

        end
        % Initialize GPIO pins
        %
        function r = initGPIO(r)
            % Initialize GPIO pins.
            
            % Enable pin 11 (enumerated 1).
            % Set direction INPUT (0).
            % Set feature Passive (2).
            r.radarInstance.set_iopin_control(1,1,2);
            % Enable pin 12 (enumerated 2).
            % Set direction OUTPUT (1).
            % Set feature Passive (2).
            r.radarInstance.set_iopin_control(2,0,2);
            
        end
        
        
        %%
        % Set/get GPIO pins
        %
        function setGPIO(r, gpioNum, gpioState)
            assert(gpioNum==1 || gpioNum==2,'GPIO No. not supported');
            assert(gpioState==0 || gpioState==1,'GPIO value must be either 0 (low) or 1 (high)');
            r.radarInstance.set_iopin_value(gpioNum, gpioState);
        end
        
        function val = getGPIO(r, gpioNum)
            assert(gpioNum==1 || gpioNum==2,'GPIO No. not supported');
            val = r.radarInstance.get_iopin_value(gpioNum);
        end
        
        
        %%
        % Start radar / Stop radar
        %
        function r = start(r)
            % r = start(r): opens radar, sets FPS, sets manual mode, subscribes to raw frames normalized. 
            if ~r.isOpen
                r.open();
            end
            
            r.radarInstance.x4driver_set_fps(r.FPS);
            r.isStreaming = 1;
        end
        
        function r = stop(r)
            % r = stop(r): stops streaming by setting FPS = 0. 
            if r.isOpen && r.isStreaming
                r.radarInstance.x4driver_set_fps(0);
                r.isStreaming = 0;
            end
        end

        function cleared_frames = clear(r,name)
            if nargin==1, name = r.dataType; end
            cleared_frames = r.bufferSize(name);
            r.radarInstance.clear(name);
        end

       function bufferSize = bufferSize(r)
           bufferSize = r.radarInstance.peek_message_data_float();
        end


        function [frame,counter] = GetFrameNormalized(r)
            %% Get radar frame in streaming mode
            if r.isOpen && r.isStreaming
                [~,~,counter,frame] = r.radarInstance.read_message_data_float;
            else
                warning('radar is not streaming or it is closed');
                frame = 0;
                counter = 0;
            end
        end
        
    end
    
end
