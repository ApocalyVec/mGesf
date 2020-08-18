classdef X2M200
    %X2M200 is the Xethru respritaion & sleep sensor based on the
    % Xethru X2 SOC.
    %
    % % EXAMPLE
    %   Lib = ModuleConnector.Library;
    %
    %   mc = ModuleConnector.ModuleConnector(COMPORT);
    %   x2m200 = mc.get_x2m200();
    %
    %   x2m200.set_sensor_mode_idle();
    %   x2m200.load_sleep_profile();
    %
    %   x2m200.subscribe_to_baseband_ap('baseband_ap');
    %   x2m200.enable_baseband_ap();
    %
    %   x2m200.set_sensor_mode_run();
    %
    %   [amplitude,phase] = x2m200.get_baseband_ap_data('baseband_ap');
    %
    %   x2m200.subscribe_to_baseband_iq('baseband_iq');
    %   x2m200.enable_baseband_iq();
    %
    %   iq_data = x2m200.get_baseband_iq_data('baseband_iq');
    %
    %   x2m200.set_sensor_mode_idle();
    %
    %   x2m200.unsubscribe('baseband_ap');
    %   x2m200.unsubscribe('baseband_iq');
    %
    %   figure;
    %   subplot(2,2,1);plot(amplitude);      xlabel('Bin#');ylabel('Amplitude')
    %   subplot(2,2,2);plot(phase);          xlabel('Bin#');ylabel('Phase')
    %   subplot(2,2,3);plot(real(iq_data));  xlabel('Bin#');ylabel('Amplitude (I-data)')
    %   subplot(2,2,4);plot(imag(iq_data));  xlabel('Bin#');ylabel('Amplitude (Q-data)')
    %
    %   clear x2m200 mc
    %   Lib.unloadlib();
    %
	% See also MODULECONNECTOR, X2
	%
    % <https://www.xethru.com/community/resources/x2m200-respiration-module-preliminary-datasheet.9>
	% 
    
    properties (SetAccess = private)
        x2m200_instance;    % Libpointer to C/C++ library instance
        radarInterface;     % Layer one wrapper class
    end
    
    % Buffers
    properties (SetAccess = private,Hidden) % baseband_ap / baseband_iq  buffers
        framePtr    = libpointer('uint8Ptr');
        
        AP_framePtr = libpointer('singlePtr',zeros(1280,1)); % AP data
        AP_max_length = 1280;
        pAP_frame_counter     = libpointer('uint32Ptr',0);
        pAP_num_bins          = libpointer('uint32Ptr',0);
        pAP_bin_length        = libpointer('singlePtr',0);
        pAP_sample_frequency  = libpointer('singlePtr',0);
        pAP_carrier_frequency = libpointer('singlePtr',0);
        pAP_range_offset      = libpointer('singlePtr',0);
        pAP_amplitude         = libpointer('singlePtr',ones(1280,1));
        pAP_phase             = libpointer('singlePtr',ones(1280,1));
        
        IQ_framePtr = libpointer('singlePtr',zeros(1280,1)); % IQ data
        IQ_max_length = 1280;
        pIQ_frame_counter     = libpointer('uint32Ptr',0);
        pIQ_num_bins          = libpointer('uint32Ptr',0);
        pIQ_bin_length        = libpointer('singlePtr',0);
        pIQ_sample_frequency  = libpointer('singlePtr',0);
        pIQ_carrier_frequency = libpointer('singlePtr',0);
        pIQ_range_offset      = libpointer('singlePtr',0);
        pIQ_i_data            = libpointer('singlePtr',ones(1280,1));
        pIQ_q_data            = libpointer('singlePtr',ones(1280,1));
    end

    properties (SetAccess = private,Hidden) % resp_status buffers
        pR_frame_counter      = libpointer('uint32Ptr',0);
        pR_sensor_state       = libpointer('uint32Ptr',0);
        pR_respiration_rate   = libpointer('uint32Ptr',0);
        pR_distance           = libpointer('singlePtr',0);
        pR_movement           = libpointer('singlePtr',0);
        pR_signal_quality     = libpointer('uint32Ptr',0);
    end
    
    properties (SetAccess = private,Hidden) % sleep_status buffers
        pS_frame_counter      = libpointer('uint32Ptr',0);
        pS_sensor_state       = libpointer('uint32Ptr',0);
        pS_respiration_rate   = libpointer('singlePtr',0);
        pS_distance           = libpointer('singlePtr',0);
        pS_signal_quality     = libpointer('uint32Ptr',0);
        pS_movement_slow      = libpointer('singlePtr',0);
        pS_movement_fast      = libpointer('singlePtr',0);
    end
    
    
    methods
	
		function item_number = get_item_number( this )
		%GET_ITEM_NUMBER Returns the internal Novelda PCBA Item Number, including revision.
		%   item_number = GET_ITEM_NUMBER( this )
        % 
        % See also GET_ORDER_CODE, GET_FIRMWARE_ID, GET_FIRMWARE_VERSION, 
        % GET_SERIAL_NUMBER, GET_BUILD_INFO, GET_APP_ID_LIST
            item_number = this.radarInterface.get_item_number(this.x2m200_instance,100);
        end
        
        function order_code = get_order_code( this )
		%GET_ORDER_CODE Returns the PCBA Order Code.
		%   order_code = GET_ORDER_CODE( this )
        % 
        % See also GET_ITEM_NUMBER, GET_FIRMWARE_ID, GET_FIRMWARE_VERSION, 
        % GET_SERIAL_NUMBER, GET_BUILD_INFO, GET_APP_ID_LIST
            order_code = this.radarInterface.get_order_code(this.x2m200_instance,100);
        end
        
        function firmware_id = get_firmware_id( this )
		%GET_FIRMWARE_ID Returns the installed Firmware ID.
		%   firmware_id = GET_FIRMWARE_ID( this )
        % 
        % See also GET_ITEM_NUMBER, GET_ORDER_CODE, GET_FIRMWARE_VERSION, 
        % GET_SERIAL_NUMBER, GET_BUILD_INFO, GET_APP_ID_LIST
            firmware_id = this.radarInterface.get_firmware_id(this.x2m200_instance,100);
        end
        
		function firmware_version = get_firmware_version( this )
		%GET_FIRMWARE_VERSION Returns the installed Firmware Version.
		%   firmware_version = GET_FIRMWARE_VERSION( this )
        % 
        % See also GET_ITEM_NUMBER, GET_ORDER_CODE, GET_FIRMWARE_ID, 
        % GET_SERIAL_NUMBER, GET_BUILD_INFO, GET_APP_ID_LIST
            firmware_version = this.radarInterface.get_firmware_version(this.x2m200_instance,100);
        end
		
		function serial_number = get_serial_number( this )
		%GET_SERIAL_NUMBER Returns the PCB serial number.
		%   serial_number = GET_SERIAL_NUMBER( this )
        % 
        % See also GET_ITEM_NUMBER, GET_ORDER_CODE, GET_FIRMWARE_ID,  
        % GET_FIRMWARE_VERSION, GET_BUILD_INFO, GET_APP_ID_LIST
			serial_number = this.radarInterface.get_serial_number(this.x2m200_instance,100);
        end
        
		function build_info = get_build_info( this )
		%GET_BUILD_INFO Returns information of the SW Build installed on the device.
		%   build_info = GET_BUILD_INFO( this )
        % 
        % See also GET_ITEM_NUMBER, GET_ORDER_CODE, GET_FIRMWARE_ID,  
        % GET_FIRMWARE_VERSION, GET_SERIAL_NUMBER, GET_APP_ID_LIST
			build_info = this.radarInterface.get_build_info(this.x2m200_instance,100);
        end
        
		function app_id_list = get_app_id_list( this )
		%GET_APP_ID_LIST Get a list of supported profiles.
		%   app_id_list = GET_APP_ID_LIST( this )
        % 
        % See also GET_ITEM_NUMBER, GET_ORDER_CODE, GET_FIRMWARE_ID,  
        % GET_FIRMWARE_VERSION, GET_SERIAL_NUMBER, GET_BUILD_INFO
			app_id_list = this.radarInterface.get_app_id_list(this.x2m200_instance,100);
        end
        
		function status = reset( this )
		%RESET Resets the module to it's last recovered state.
		%   status = RESET( this )
			status = this.radarInterface.reset(this.x2m200_instance);
        end
        
        function status = start_bootloader( this )
		%START_BOOTLOADER Enters the bootloader for FW upgrades.
		%   status = ENTER_BOOTLOADER( this )
            status = this.radarInterface.start_bootloader(this.x2m200_instance);
        end
        
        function status = set_sensor_mode_run( this )
        %SET_SENSOR_MODE_RUN Run sensor application 
        %   status = SET_SENSOR_MODE_RUN( this )
        %   In run mode the sensor will send a stream of data back to the 
        %   host. The data in the data strem depends on the current configuration.
        %
        % See also SET_SENSOR_MODE_IDLE
            status = this.radarInterface.set_sensor_mode_run(this.x2m200_instance);
        end
        
        function status = set_sensor_mode_idle( this )
        %SET_SENSOR_MODE_IDLE Set the sensor in idle mode. 
        %   status = SET_SENSOR_MODE_IDLE( this )
        %   The sensor will stop sending data. The sensor can be put back 
        %   in run mode with SET_SENSOR_MODE_RUN.
        %
        % See also SET_SENSOR_MODE_RUN
            status = this.radarInterface.set_sensor_mode_idle(this.x2m200_instance);
        end
        
        function status = load_sleep_profile( this )
        %LOAD_SLEEP_PROFILE Load the sleep profile.
        %   status = LOAD_SLEEP_PROFILE( this )
        %
        %   A profile is a preset configuration that makes the module behave in a
        %   certain way. The module will not start sending data until a
        %   set_sensor_mode_run command is executed.
        %
        %   There are six states in the Sleep Profile:
        %   No Movement:        No presence detected
        %   Movement:           Presence, but no identifiable breathing movement
        %   Movement Tracking:  Presence and possible breathing movement detected
        %   Breathing:          Valid breathing movement detected
        %   Initializing:       The sensor initializes after the Sleep Profile is chosen
        %   Unknown:            The sensor is in an unknown state and requires 
        %                       that the Profile and User Settings are loaded.
        %
        % See also LOAD_RESPIRATION_PROFILE
        % 
            status = this.radarInterface.load_sleep_profile(this.x2m200_instance);
        end
        
        function status = load_respiration_profile( this )
        %LOAD_RESPIRATION_PROFILE Load the respiration profile.
        %   status = LOAD_RESPIRATION_PROFILE( this )
        %
        %   A profile is a preset configuration that makes the module behave in a
        %   certain way. The module will not start sending data until a
        %   set_sensor_mode_run command is executed.
        %
        %   There are six states in the Respiration Profile:
        %       No Movement:        No presence detected
        %       Movement:           Presence, but no identifiable breathing movement
        %       Movement Tracking:  Presence and possible breathing movement detected
        %       Breathing:          Valid breathing movement detected
        %       Initializing:       The sensor initializes after the Respiration Profile is chosen
        %       Unknown:            The sensor is in an unknown state and requires that the Profile and User Settings are loaded.
        % 
        % See also LOAD_RESPIRATION_PROFILE
        %
            status = this.radarInterface.load_respiration_profile(this.x2m200_instance);
        end
        
        function status = enable_baseband( this )
        %ENABLE_BASEBAND Enable baseband output.
        %   status = ENABLE_BASEBAND( this )
            status = this.radarInterface.enable_baseband(this.x2m200_instance);
        end
        
        function status = enable_baseband_ap( this )
        %ENABLE_BASEBAND_AP Enable amplitude/phase baseband output.
        %   status = ENABLE_BASEBAND_AP( this )
        %
        %   ENABLE_BASEBAND_AP will disable ENABLE_BASEBAND_IQ
        %   See also ENABLE_BASEBAND_IQ SUBSCRIBE_TO_BASEBAND_AP
            status = this.radarInterface.enable_baseband_ap(this.x2m200_instance);
        end
        
        function status = enable_baseband_iq( this )
        %ENABLE_BASEBAND_IQ Enable I/Q baseband output.
        %   status = ENABLE_BASEBAND_IQ( this )
        % 
        %   ENABLE_BASEBAND_IQ will disable ENABLE_BASEBAND_AP
        %   See also ENABLE_BASEBAND_AP SUBSCRIBE_TO_BASEBAND_IQ
            status = this.radarInterface.enable_baseband_iq(this.x2m200_instance);
        end
        
        function status = set_detection_zone( this, start, stop )
        %SET_DETECTION_ZONE Set the desired detection zone.
        %   status = SET_DETECTION_ZONE( this, start, stop )
        %
     	%   The sensor will detect movements within this area START to STOP.
     	%       start   Start of detection zone in meters.
     	%       stop    End of detection zone in meters.
     	%       status  Successful call returns 0, otherwise error code.
            status = this.radarInterface.set_detection_zone(this.x2m200_instance,start,stop);
        end
        
        function status = set_sensitivity( this, new_sensitivity )
        % SET_SENSITIVITY Set module sensitivity.
        % status = SET_SENSITIVITY( this, new_sensitivity )
     	%   new_sensitivity Sensitivity level from 0 (low) to 9 (high).
     	%   status  Successful call returns 0, otherwise error code.
            status = this.radarInterface.set_sensitivity(this.x2m200_instance,new_sensitivity);
        end
        
        function status = set_led_control( this, mode, intensity )
        %SET_LED_CONTROL Configures the module LED mode.
        %   status = SET_LED_CONTROL( this, mode, intensity )
        %
        %       mode        0:Off, 1:Simple, 2:Full(default).
        %       intensity   Not in use, can be 0
            status = this.radarInterface.set_led_control(this.x2m200_instance, mode, intensity);
        end
        
        function int = subscribe_to_resp_status( this, name )
        % SUBSCRIBE_TO_RESP_STATUS Subscribe to respiration status data messages.
        % status = SUBSCRIBE_TO_RESP_STATUS( this, name )
        %   name: a name that identifies the subscription
            if nargin==1, name = 'resp_status'; end
            int = this.radarInterface.subscribe_to_resp_status(this.x2m200_instance, name);
        end
        
        function status = subscribe_to_sleep_status( this, name )
        % SUBSCRIBE_TO_SLEEP_STATUS Subscribe to sleep status data messages.
        % status = SUBSCRIBE_TO_SLEEP_STATUS( this, name )
        %   name: a name that identifies the subscription
            if nargin==1, name = 'sleep_status'; end
            status = this.radarInterface.subscribe_to_sleep_status(this.x2m200_instance, name);
        end
        
        function status = subscribe_to_baseband_ap( this, name )
        %SUBSCRIBE_TO_BASEBAND_AP Start buffering baseband_ap messages.
        %   status = SUBSCRIBE_TO_BASEBAND_AP( this, name )
        %
        % See also ENABLE_BASEBAND_AP, GET_BASEBAND_AP_DATA
            if nargin==1, name = 'baseband_ap'; end
            status = this.radarInterface.subscribe_to_baseband_ap(this.x2m200_instance, name);
        end
        
        function status = subscribe_to_baseband_iq( this, name )
        %SUBSCRIBE_TO_BASEBAND_IQ Start buffering baseband_iq messages.
        %   status = SUBSCRIBE_TO_BASEBAND_IQ( this, name )
        %
        % See also ENABLE_BASEBAND_IQ, GET_BASEBAND_IQ_DATA
            if nargin==1, name = 'baseband_iq'; end
            status = this.radarInterface.subscribe_to_baseband_iq(this.x2m200_instance, name);
        end
        
        function status = disable_resp_output( this )
        %DISABLE_RESP_OUTPUT disables respiration output
		%   status = DISABLE_RESP_OUTPUT( this )
        %
        % See also SUBSCRIBE_TO_RESP_STATUS, ENABLE_RESP_OUTPUT,
        % GET_RESPIRATION_DATA
            status = this.radarInterface.disable_resp_output(this.x2m200_instance);
        end
        
        function status = enable_resp_output( this )
        %ENABLE_RESP_OUTPUT enables respiration output. A subscription must be defined before this call can be made.
		%   status = ENABLE_RESP_OUTPUT( this )
        %
        % See also SUBSCRIBE_TO_RESP_STATUS, GET_RESPIRATION_DATA, 
        % DISABLE_RESP_OUTPUT
            status = this.radarInterface.enable_resp_output(this.x2m200_instance);
        end
        
        function int = get_number_of_packets( this, name )
        %GET_NUMBER_OF_PACKETS Returns the number of packets buffered
        %   with the subscription comparator <name>
        %   int = GET_NUMBER_OF_PACKETS( this, name )
            int = double(this.radarInterface.get_number_of_packets(this.x2m200_instance,name)); 
        end
        
        function [rd,status] = get_respiration_data( this, name )
        %GET_RESPIRATION_DATA Get one buffered respiration data message. Order is FIFO.
        %   [rd,status] = GET_RESPIRATION_DATA( this, name )
        %
        %   name  Name of the respiration data subscription of interest
     	%   rd    A data struct containing the respiration data.
        %   rd.frame_counter    Message sequence number 
        %   rd.sensor_state    
        %   rd.respiration_rate
        %   rd.distance
        %   rd.movement
        %   rd.signal_quality   A relative number from 0 to 10 where 10 
        %                       indicates highest signal quality.
        %
        % See also SUBSCRIBE_TO_RESP_STATUS, ENABLE_RESP_OUTPUT,
        % DISABLE_RESP_OUTPUT GET_SLEEP_DATA
            if nargin==1, name = 'resp_status'; end           
            status = this.radarInterface.get_respiration_data(this.x2m200_instance,name, this.pR_frame_counter, this.pR_sensor_state, this.pR_respiration_rate, this.pR_distance, this.pR_movement, this.pR_signal_quality);
            rd.frame_counter    = double(this.pR_frame_counter.Value);
            rd.sensor_state     = double(this.pR_sensor_state.Value);
            rd.respiration_rate = double(this.pR_respiration_rate.Value);
            rd.distance         = double(this.pR_distance.Value);
            rd.movement         = double(this.pR_movement.Value);
            rd.signal_quality   = double(this.pR_signal_quality.Value);
        end
%         
        function [sd,status] = get_sleep_data( this, name )
        %GET_SLEEP_DATA Get one buffered sleep data message. Order is FIFO.
        %   [sd,status] = GET_SLEEP_DATA( this, name )
        %
        %   name  Name of the sleep data subscription of interest
     	%   sd    A data struct containing the respiration data.
        %   sd.frame_counter       Message sequence number 
        %   sd.sensor_state        
        %   sd.respiration_rate    
        %   sd.distance
        %   sd.movement
        %   sd.signal_quality
        %   sd.movement_slow
        %   sd.movement_fast
        %
        % See also SUBSCRIBE_TO_SLEEP_STATUS, ENABLE_SLEEP_OUTPUT,
        % DISABLE_SLEEP_OUTPUT  GET_RESPIRATION_DATA
            if nargin==1, name = 'sleep_status'; end           
            status = this.radarInterface.get_sleep_data(this.x2m200_instance,name, this.pR_frame_counter, this.pR_sensor_state, this.pR_respiration_rate, this.pR_distance, this.pR_signal_quality, this.pS_movement_slow, this.pS_movement_fast);
            sd.frame_counter    = double(this.pS_frame_counter.Value);
            sd.sensor_state     = double(this.pS_sensor_state.Value);
            sd.respiration_rate = double(this.pS_respiration_rate.Value);
            sd.distance         = double(this.pS_distance.Value);
            sd.signal_quality   = double(this.pS_signal_quality.Value);
            sd.movement_slow    = double(this.pS_movement_slow.Value);
            sd.movement_fast    = double(this.pS_movement_fast.Value);
        end
        
        function [iq_data,header,status] = get_baseband_iq_data( this, name )
        %GET_BASEBAND_IQ_DATA Get one buffered baseband IQ data message. Order is FIFO.
        %   [iq_data,header,status] = GET_BASEBAND_IQ_DATA( this, name )
        %
        %   iq_data   complex vector containing i and q baseband data
        %   header    A data struct containing the following header data:
        %   header.frame_counter       Message sequence number 
        %   header.num_bins            Number of bins in the i or q data message
        %	header.bin_length          
        %	header.sample_frequency    Receiver sampling frequency
        %	header.carrier_frequency   Transmitter center frequency
        %	header.range_offset        Range to the beginning of the frame
        %   status 
        %
        % % EXAMPLE
        %   x2m200.SUBSCRIBE_TO_BASEBAND_IQ('baseband_iq');
        %   x2m200.ENABLE_BASEBAND_IQ();
        %   [amplitude,phase] = x2m200.GET_BASEBAND_IQ_DATA('baseband_iq');
        %
        % See also SUBSCRIBE_TO_BASEBAND_IQ, ENABLE_BASEBAND_IQ, GET_BASEBAND_AP_DATA
            status = this.radarInterface.get_baseband_iq_data(this.x2m200_instance,name, this.pIQ_frame_counter, this.pIQ_num_bins, this.pIQ_bin_length, this.pIQ_sample_frequency, this.pIQ_carrier_frequency, this.pIQ_range_offset, this.pIQ_i_data, this.pIQ_q_data,this.IQ_max_length);
            header.frame_counter     = double(this.pIQ_frame_counter.Value);
            header.num_bins          = double(this.pIQ_num_bins.Value);
            header.bin_length        = double(this.pIQ_bin_length.Value);
            header.sample_frequency  = double(this.pIQ_sample_frequency.Value);
            header.carrier_frequency = double(this.pIQ_carrier_frequency.Value);
            header.range_offset      = double(this.pIQ_range_offset.Value);
            i_data    = double(this.pIQ_i_data.Value(1:header.num_bins));
            q_data    = double(this.pIQ_q_data.Value(1:header.num_bins));
            iq_data   = complex(i_data,q_data);
        end
        
        function [amplitude,phase,header,status] = get_baseband_ap_data( this, name )
        %GET_BASEBAND_AP_DATA Get one buffered baseband AP data message. Order is FIFO.
        %   [amplitude,phase,header,status] = GET_BASEBAND_AP_DATA( this, name )
        %
        %   amplitude  vector containing the signal envelope
        %   phase      vector containing the signal phase
        %   header
        %   header.frame_counter       Message sequence number 
        %   header.num_bins            Number of bins in the i or q data message
        %   header.bin_length
        %   header.sample_frequency    Receiver sampling frequency
        %   header.carrier_frequency   Transmitter center frequency
        %   header.range_offset        Range to the beginning of the frame
        %   status 
        %
        % % EXAMPLE
        %   x2m200.SUBSCRIBE_TO_BASEBAND_AP('baseband_ap');
        %   x2m200.ENABLE_BASEBAND_AP();
        %   [amplitude,phase] = x2m200.GET_BASEBAND_AP_DATA('baseband_ap');
        %
        % See also SUBSCRIBE_TO_BASEBAND_AP, ENABLE_BASEBAND_AP, GET_BASEBAND_IQ_DATA
            status = this.radarInterface.get_baseband_ap_data(this.x2m200_instance,name, this.pAP_frame_counter, this.pAP_num_bins, this.pAP_bin_length, this.pAP_sample_frequency, this.pAP_carrier_frequency, this.pAP_range_offset, this.pAP_amplitude, this.pAP_phase,this.AP_max_length);
            header.frame_counter     = double(this.pAP_frame_counter.Value);
            header.num_bins          = double(this.pAP_num_bins.Value);
            header.bin_length        = double(this.pAP_bin_length.Value);
            header.sample_frequency  = double(this.pAP_sample_frequency.Value);
            header.carrier_frequency = double(this.pAP_carrier_frequency.Value);
            header.range_offset      = double(this.pAP_range_offset.Value);
            amplitude = double(this.pAP_amplitude.Value(1:header.num_bins));
            phase     = double(this.pAP_phase.Value(1:header.num_bins));
        end
        
        function status = unsubscribe( this, name )
        %UNSUBSCRIBE Unsubscribe to a named subscription.
        %   status = UNSUBSCRIBE( this, name )
            status = this.radarInterface.unsubscribe(this.x2m200_instance,name);
        end
        
        function clear( this, name )
		% CLEAR Clears the in-buffer of the named subscription.
		%
		% @param[in] 
		%	name Subscription identification
            this.radarInterface.clear( this.x2m200_instance, name );
        end       
        
        %%
        % GET_PACKET
        function [packet,status] = get_packet(this,name)
             switch name
                 case {'baseband_ap','baseband_iq'}
                     status = this.radarInterface.get_packet(this.x2m200_instance,name,this.framePtr,1280);
                     numbins = double(typecast(this.framePtr.Value(10:13),'uint32'));
                     packet = double(typecast(this.framePtr.Value(30:29+numbins*4*2),'single')); % 30:.. for sleep
                 case {'resp_status','sleep_status'}
                     status = this.radarInterface.get_packet(this.x2m200_instance,name,this.resp_statusPtr,1280);
                     packet = double(typecast(this.resp_statusPtr.Value(30:445),'single'));
                 otherwise
                     error('Available packets names are: baseband_ap,baseband_iq,resp_status,sleep_status');
             end
        end
    end
    
    methods
        % Constructor
        function x2m200 = X2M200( mc )
            % Constructor
            x2m200.radarInterface = mc.radarInterface;
            x2m200.x2m200_instance = calllib(mc.lib_name,'nva_get_x2',mc.mcInstance);
            assert(~x2m200.x2m200_instance.isNull, 'create xethru failed check the logs');            
            
%            X2M200.set_sensor_mode_idle();
            
            x2m200.framePtr.Value = zeros(1280,1,'uint8');           
        end
        
        % Destructor
        function delete( this )
            % Destructor
            clear this.framePtr this.packetPtr this.frameCtrPtr
            clear this.pR_frame_counter this.pR_sensor_state this.pR_respiration_rate this.pR_distance this.pR_movement this.pR_signal_quality 
            clear this.pS_frame_counter this.pS_sensor_state this.pS_respiration_rate this.pS_distance this.pS_signal_quality this.pS_movement_slow this.pS_movement_fast
            clear this.pAP_frame_counter this.pAP_num_bins this.pAP_bin_length this.pAP_sample_frequency this.pAP_carrier_frequency this.pAP_range_offset this.pAP_amplitude this.pAP_phase
            clear this.pIQ_frame_counter this.pIQ_num_bins this.pIQ_bin_length this.pIQ_sample_frequency this.pIQ_carrier_frequency this.pIQ_range_offset this.pIQ_i_data this.pIQ_q_data
            calllib(this.radarInterface.lib_name,'nva_destroy_X2_interface',this.x2m200_instance);
            clear('this.radarInterface')
            clear('this.x2m200_instance')
        end
    end
end
