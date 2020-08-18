classdef X2
    %X2_INTERFACE provides a set of parameterized low level X2 commands 
    % and responses. It is used by developers who want to do low level 
    % access to the X2 device. Using this interface requires good knowledge 
    % on the X2 device.
    %
	% % Example
    %   LIB = ModuleConnector.Library;                % Load and link runtime library
    %   mc = ModuleConnector.ModuleConnector('COM3'); % Attach module connector to port COM3
    %   x2 = mc.get_x2();                             % Create an interface to the X2 chip
    %   ChipID = x2.get_x2_register(2);               % Read X2 chip register
    %   clear x2 mc                                   % Clean up
    %   LIB.unloadlib();                              % Unload library
    %
    % See also MODULECONNECTOR, X2M200,
    %
    
    properties (SetAccess = private)
        x2_instance         % Libpointer to C/C++ library instance
        radarInterface;     % Layer one wrapper class
        variables
    end
    
    properties (SetAccess = private,Hidden)  
        framePtr    = libpointer('singlePtr'); % frame storage: singlePtr
        frameCtrPtr = libpointer('uint32Ptr'); % frame counter: uint32Ptr
        frameLenPtr = libpointer('uint32Ptr'); % frame length
        frameLen = 1280;
    end
    
    methods

		function status = set_x2_float( this, id, value )
		% SET_X2_FLOAT Calls Radarlib method NVA_X2_SetFloatVariable().
		% status = SET_X2_FLOAT(this,id,value). Refer to Radarlib support for details.
		%
		% @param[in]  
		%	id    Radarlib variable ID
		%	value Radarlib float variable value
		% @return       
		% 	0 = Succes, other indicates some error.
		% See also NVA_X2_Variable
            if iscell(id), id = id{1}; end
            status = this.radarInterface.set_x2_float(this.x2_instance,id,value);
        end
        
        function [float,status] = get_x2_float( this, id )
		% GET_X2_FLOAT Calls Radarlib method NVA_X2_GetFloatVariable().
		% float = GET_X2_FLOAT(this,id)
        % [float,status] = GET_X2_FLOAT(this,id). Refer to Radarlib support for details.
		%
		% @param[in]  
		%	id    Radarlib variable ID
		% @return      
		%	Radarlib float variable value
        %   Call status. 0 = Succes, other indicates some error.
		% See also NVA_X2_Variable
		%
            if iscell(id), id = id{1}; end
            [float,status] = this.radarInterface.get_x2_float(this.x2_instance,id);
            float = double(float);
        end
        
        function status = set_x2_int( this, id, value )
		% SET_X2_INT Calls Radarlib method NVA_X2_SetIntVariable().
		% status = SET_X2_INT(this,id,value). Refer to Radarlib support for details.
		%
		% @param[in]  
		%	id    Radarlib variable ID
		%	value Radarlib integer variable value
		% @return       
		% 	0 = Succes, other indicates some error.
		% See also NVA_X2_Variable
            if iscell(id), id = id{1}; end
            status = this.radarInterface.set_x2_int(this.x2_instance,id,value);
        end
        
        function [int,status] = get_x2_int( this, id )
		% GET_X2_INT Calls Radarlib method NVA_X2_GetIntVariable().
		% int = GET_X2_INT(this,id). Refer to Radarlib support for details.
		%
		% @param[in]  
		%	id    Radarlib variable ID
		% @return       
		%	Radarlib integer variable value
        %   Call status. 0 = Succes, other indicates some error.
		% See also NVA_X2_Variable
            if iscell(id), id = id{1}; end
            [int,status] = this.radarInterface.get_x2_int( this.x2_instance, id );
        end
        
        function status = set_x2_register( this, id, value )
		% SET_X2_REGISTER Calls Radarlib method NVA_X2_WriteRegisterFieldId().
		% status = SET_X2_REGISTER(this,id,value). Refer to Radarlib support for details.
		%
		% @param[in]  
		%	id    Radarlib variable ID
		%	value Radarlib integer variable value
		% @return       
		% 	0 = Succes, other indicates some error.
		% See also NVA_X2_Variable
            if iscell(id), id = id{1}; end
            status = this.radarInterface.set_x2_register( this.x2_instance, id, value );
        end
        
        function [reg_val,status] = get_x2_register( this, id )
		% GET_X2_REGISTER Calls Radarlib method NVA_X2_ReadRegisterFieldId().
		% Refer to Radarlib support for details.
		%
		% @param[in]  
		%	id    Radarlib variable ID
		% @return       
		%	X2 register variable value
        %   Call status. 0 = Succes, other indicates some error.
		% See also NVA_X2_Variable
            if iscell(id), id = id{1}; end
            [reg_val,status] = this.radarInterface.get_x2_register( this.x2_instance, id );
            reg_val = double(reg_val);
        end

        function string = git_sha( this )
        % GIT_SHA
            string = this.radarInterface.git_sha( this.x2_instance );
        end
        
        function status = set_debug_level( this, new_debug_level )
		% SET_DEBUG_LEVEL Set debug level.
		% status = SET_DEBUG_LEVEL(this,new_debug_level)
		% Sets debug level for the module.
		% Controls the verbosity of the module debug output.
		%
		%	0 = No debug messages
		%	- ...
		%	- 9 = All debug messages
		%
		% @param[in]   level Module debug messages verbosity,
		%	0 = no messages, 9 = all messages
		% @return      
		% 	0 = Succes, other indicates some error.
             status = this.radarInterface.set_debug_level( this.x2_instance, new_debug_level );
         end

       function uint = ping( this )
		% PING Send a ping message to the module. Responds with a pong value to indicate
		% if the module is ready for further interaction or not.
		%
		% @return Pongvalue. Indicates module readyness:
		%	- Ready: XTS_DEF_PONGVAL_READY (0xaaeeaeea)
		%	- Not ready: XTS_DEF_PONGVAL_NOTREADY (0xaeeaeeaa)
            uint = this.radarInterface.ping( this.x2_instance );
        end

        function clear( this, name )
		% CLEAR Clears the in-buffer of the named subscription.
		%
		% @param[in] 
		%	name Subscription identification
            this.radarInterface.clear( this.x2_instance, name );
        end        
        
        function string = get_system_info( this, info_code )
		% GET_SYSTEM_INFO Request system info from the module.
        % string = GET_SYSTEM_INFO( this, info_code )
        %
        % @param[in]  info_code Select which type of information to request:
        %   Novelda item number: XTS_SSIC_ITEMNUMBER         = 0x00
        %	Novelda ordercode: XTS_SSIC_ORDERCODE            = 0x01
        %	Firmware ID, e.g. X2M200: XTS_SSIC_FIRMWAREID    = 0x02
        %	Firmware version: XTS_SSIC_VERSION               = 0x03
        %	Firmware build information: XTS_SSIC_BUILD       = 0x04
        %	Module serial number: XTS_SSIC_SERIALNUMBER      = 0x06
        % @return       
        %   0 = Succes, other indicates some error.
            string = this.radarInterface.get_system_info( this.x2_instance, info_code, 100);
        end

        function int = run_timing_measurement( this )
		% RUN_TIMING_MEASUREMENT Starts a timing measurement on module.
		%
		% Timing measurement runs for approximately 1 minute. It is a process to
		% calibrate the X2 radar chip in order to produce accurate results.
		%
		% During timing measurement execution, the module may be less responsive.
		% Returns when the measurement is done.
		%
		% @return       
		%	0 = Succes, other indicates some error.
            int = this.radarInterface.run_timing_measurement( this.x2_instance );
        end
           
        function int = set_fps( this, fps )
		% SET_FPS Select module sweep control frequency. When in a running mode, the
		% module will generate continous radar sweeps at a fixed time interval.
		%
		% @param[in] 
		%	fps  Frames Per second
		%
		% @return
		%	0 = Succes, other indicates some error.
            int = this.radarInterface.set_fps( this.x2_instance, fps );
         end

        function int = set_sensor_mode( this, unsigned_char_mode )
		% SET_SENSOR_MODE Control the sensor module mode of execution.
		%
		% When the module is configured to run an application, such as X2M200,
		% this method is used to control the execution.
		% It can also be used to configure the module to a MANUAL mode where
		% it sends raw data from the X2 chip directly.
		%
		% @param[in]  mode List of valid modes are:
		%	- Application normal execution: XTS_SM_RUN = 0x01
		%	- Pause application execurion: XTS_SM_IDLE = 0x11
		%	- Disable application execution, stream raw data directly from X2: XTS_SM_MANUAL = 0x12
		% @return       
		%	0 = Succes, other indicates some error.
            int = this.radarInterface.set_sensor_mode( this.x2_instance, unsigned_char_mode );
         end
         
        function int = set_sensor_mode_idle( this )
		% SET_SENSOR_MODE_IDLE Pause application execurion. Equal to set_sensor_mode('XTS_SM_IDLE')
		% @return       
		%	0 = Succes, other indicates some error.
            int = this.radarInterface.set_sensor_mode_idle( this.x2_instance );
         end
		 
        function int = set_sensor_mode_manual( this )
        % SET_SENSOR_MODE_MANUAL Disable application execution. Equal to set_sensor_mode('XTS_SM_MANUAL');
		% @return       
		%	0 = Succes, other indicates some error.
            int = this.radarInterface.set_sensor_mode_manual( this.x2_instance );
        end
                  
        function int = subscribe_to_raw_normalized( this, name )
		% SUBSCRIBE_TO_RAW_NORMALIZED Start a subscription on raw normalized radar data.
		%
		% @param[in]  
		%	name Subscription identification
		% @return       
		%	0 = Succes, other indicates some error.
            if nargin == 1, name = 'raw_normalized'; end
            int = this.radarInterface.subscribe_to_raw_normalized( this.x2_instance, name );
         end
          
        function int = unsubscribe( this, name )
		% UNSUBSCRIBE Unsubsribe from a previously enabled data subscription.
		%
		% @param[in] 
		%	name Subscription identification
		%
            int = this.radarInterface.unsubscribe( this.x2_instance, name);
        end
          
        function [frame,status] = capture_single_normalized_frame( this )
        % CAPTURE_SINGLE_NORMALIZED_FRAME Start a single sweep and capture the radar data.
		%
		% @return       Normalized radar frame subscribe_to_raw_normalized.
		% See also subscribe_to_raw_normalized()
            status = this.radarInterface.capture_single_normalized_frame( this.x2_instance, this.frameCtrPtr, this.framePtr,this.frameLenPtr, this.frameLen );
            frame = double(this.framePtr.Value(1:this.frameLenPtr.Value));
        end
        
        function [frame,counter,status] = get_raw_normalized_frame( this, name )
        % GET_RAW_NORMALIZED_FRAME Start a subscription on raw normalized radar data. 
        % A subscription must be defined before this call can be made.
            if nargin == 1, name = 'raw_normalized'; end
            status = this.radarInterface.get_raw_normalized_frame( this.x2_instance, name,this.frameCtrPtr, this.framePtr, this.frameLenPtr,this.frameLen );
            frame = double(this.framePtr.Value(1:this.frameLenPtr.Value));
            counter = double(get(this.frameCtrPtr,'Value'));
        end
       
        function int = get_number_of_packets( this, name )
        % GET_NUMBER_OF_PACKETS Returns the number of packets buffered
        % with the subscription comparator <name>
        % int = GET_NUMBER_OF_PACKETS( this, name )
            int = double(this.radarInterface.get_number_of_packets(this.x2_instance,name)); 
        end
        
    end

    methods
        
        function x2 = X2(mc)
        % X2 returns an interface object for low level X2 functionality
		% x2 = X2(mc)
		%
		% Example
        %   LIB = ModuleConnector.Library;                % Load and link runtime library
        %   mc = ModuleConnector.ModuleConnector('COM3'); % Attach module connector to port COM3
        %   X2 = mc.get_x2();                   % Create an interface to the X2 chip
        %   ChipID = X2.get_x2_register(2);               % Read X2 chip register
        %   clear X2 mc                                   % Clean up
        %   LIB.unloadlib();                              % Unload library
        %
            x2.radarInterface = mc.radarInterface;
            x2.x2_instance = calllib(mc.lib_name,'nva_get_x2',mc.mcInstance);
            assert(~x2.x2_instance.isNull, 'create xethru failed check the logs');            
            
            x2.set_sensor_mode_idle();
            x2.set_debug_level(5);
            
            x2.variables = ModuleConnector.NVA_X2_Variable;
            
            x2.framePtr.Value = ones(x2.frameLen,1);
            x2.frameCtrPtr.Value = 0;
            x2.frameLenPtr.Value = 0;
        end
        
        function delete( this )
            % Destructor
            clear this.framePtr this.packetPtr this.frameCtrPtr this.framLenPtr
            calllib(this.radarInterface.lib_name,'nva_destroy_X2_interface',this.x2_instance);
            clear('this.radarInterface')
            clear('this.x2_instance')
        end
    end
    
end

