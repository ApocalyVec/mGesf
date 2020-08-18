classdef X4M200
    %X4M200 is the API for the X4M200 Respiration Sensor based on
    % the Xethru X4 SOC. It contains the high level API that is used to
    % control the module.
    %
    % Example
    %   mc = ModuleConnector.ModuleConnector('COM3')
    %   X4M200 = mc.get_X4M200();
    %
	% See also MODULECONNECTOR, X4M300, X2M200, X2, RAWINTERFACE, XEP
	%
	% 
    
    properties (SetAccess = private)
        X4M200_instance;    % Libpointer to C/C++ library instance
        radarInterface;     % Layer one wrapper class
    end
    
    % Buffers
    properties (SetAccess = private,Hidden) % baseband_ap / baseband_iq  buffers        
        AP_framePtr = libpointer('singlePtr',zeros(1280,1)); % AP data
        AP_max_length = 1280;
        pAP_frame_counter     = libpointer('uint32Ptr',0);
        pAP_num_bins          = libpointer('uint32Ptr',0);
        pAP_bin_length        = libpointer('singlePtr',0);
        pAP_sample_frequency  = libpointer('singlePtr',0);
        pAP_carrier_frequency = libpointer('singlePtr',0);
        pAP_range_offset      = libpointer('singlePtr',0);
        pAP_amplitude         = libpointer('singlePtr',zeros(1280,1));
        pAP_phase             = libpointer('singlePtr',zeros(1280,1));
        
        IQ_framePtr = libpointer('singlePtr',zeros(1280,1)); % IQ data
        IQ_max_length = 1280;
        pIQ_frame_counter     = libpointer('uint32Ptr',0);
        pIQ_num_bins          = libpointer('uint32Ptr',0);
        pIQ_bin_length        = libpointer('singlePtr',0);
        pIQ_sample_frequency  = libpointer('singlePtr',0);
        pIQ_carrier_frequency = libpointer('singlePtr',0);
        pIQ_range_offset      = libpointer('singlePtr',0);
        pIQ_i_data            = libpointer('singlePtr',zeros(1280,1));
        pIQ_q_data            = libpointer('singlePtr',zeros(1280,1));
    end

    properties (SetAccess = private,Hidden) % sleep message buffers
        S_frame_counter      = libpointer('uint32Ptr',0);
        S_sensor_state       = libpointer('uint32Ptr',0);
        S_RPM                = libpointer('singlePtr',0);
        S_distance           = libpointer('singlePtr',0);
        S_signal_quality     = libpointer('uint32Ptr',0);
        S_movement_fast      = libpointer('singlePtr',0);
        S_movement_slow      = libpointer('singlePtr',0);
    end
    
    properties (SetAccess = private,Hidden) % vital signs message buffers
        VS_frame_counter        = libpointer('uint32Ptr',0);
        VS_state                = libpointer('uint32Ptr',0);
        VS_respRate             = libpointer('singlePtr',0);
        VS_respDist             = libpointer('singlePtr',0);
        VS_respConf             = libpointer('singlePtr',0);
        VS_heartRate            = libpointer('singlePtr',0);
        VS_heartDist            = libpointer('singlePtr',0);
        VS_heartConf            = libpointer('singlePtr',0);
        VS_norm_movement_slow   = libpointer('singlePtr',0);
        VS_norm_movement_fast   = libpointer('singlePtr',0);
        VS_norm_movement_start  = libpointer('singlePtr',0);
        VS_norm_movement_stop   = libpointer('singlePtr',0);
    end
    
    properties (SetAccess = private,Hidden) % respiration legacy buffers
        rL_frame_counter      = libpointer('uint32Ptr',0);
        rL_sensor_state       = libpointer('uint32Ptr',0);
        rL_RPM                = libpointer('uint32Ptr',0);
        rL_distance           = libpointer('singlePtr',0);
        rL_movement           = libpointer('singlePtr',0);
        rL_signal_quality     = libpointer('uint32Ptr',0);
    end
    
    properties (SetAccess = private,Hidden) % respiration movinglist buffers
        rM_maxLength = 200;
        rM_frame_counter         = libpointer('uint32Ptr',0);
        rM_movementIntervalCount = libpointer('uint32Ptr',0);
        rM_movementSlowItem      = libpointer('singlePtr',zeros(200,1));
        rM_movementFastItem      = libpointer('singlePtr',zeros(200,1));
    end
    
    properties (SetAccess = private,Hidden) % respiration detectionlist buffers
        rD_maxLength = 200;
        rD_frame_counter              = libpointer('uint32Ptr',0);
        rD_detectionCount             = libpointer('uint32Ptr',0);
        rD_detectionDistance          = libpointer('singlePtr',zeros(200,1));
        rD_detectionRadarCrossSection = libpointer('singlePtr',zeros(200,1));
        rD_detectionVelocity          = libpointer('singlePtr',zeros(200,1));
    end
    
    properties (SetAccess = private,Hidden) % respiration normalizedmovementlist buffers
        rMPL_maxLength = 200;
        rMPL_frame_counter              = libpointer('uint32Ptr',0);
        rMPL_start                      = libpointer('singlePtr',0);
        rMPL_bin_length                 = libpointer('singlePtr',0);
        rMPL_count                      = libpointer('uint32Ptr',0);
        rMPL_normalizedMovementSlowItem = libpointer('singlePtr',zeros(200,1));
        rMPL_normalizedMovementFastItem = libpointer('singlePtr',zeros(200,1));
    end
    
    properties (SetAccess = private,Hidden) % system info buffers
        SI_max_length = 100;
        sensitivity = libpointer('uint32Ptr',0);
        led_mode = libpointer('uint8Ptr',0);
        output_ctrl = libpointer('uint32Ptr',0); 
        pin_setup = libpointer('uint32Ptr',0);
        pin_feature = libpointer('uint32Ptr',0);
    end
    
    properties (SetAccess = private,Hidden) % noisemap control buffer
        NMCptr = libpointer('uint32Ptr',0);
    end

    properties (SetAccess = private,Hidden) % pulse-Doppler buffers
        pD_maxLength             = 2048;
        pD_frame_counter         = libpointer('uint32Ptr',0);
        pD_matrix_counter        = libpointer('uint32Ptr',0);
        pD_range_idx             = libpointer('uint32Ptr',0);
        pD_range_bins            = libpointer('uint32Ptr',0);
        pD_frequency_count       = libpointer('uint32Ptr',0);
        pD_pulsedoppler_instance = libpointer('uint32Ptr',0);

        pD_byte_step_start       = libpointer('singlePtr',0);
        pD_byte_step_size        = libpointer('singlePtr',0);

        pD_fps                   = libpointer('singlePtr',0);
        pD_fps_decimated         = libpointer('singlePtr',0);
        pD_frequency_start       = libpointer('singlePtr',0);
        pD_frequency_step        = libpointer('singlePtr',0);
        pD_range                 = libpointer('singlePtr',0);
        pD_float_data            = libpointer('singlePtr',zeros(2048, 1));

        pD_byte_data             = libpointer('uint8Ptr', zeros(2048, 1, 'uint8'));
    end

    methods
        
        function [system_info, status] = get_system_info(this, info_code)
            %GET_SYSTEM_INFO Returns a string containing system information given an infocode
            %    [system_info,status] = GET_SYSTEM_INFO(this,info_code)
            %    Info codes:
            %    XTID_SSIC_ITEMNUMBER    = 0x00, Returns the internal Novelda PCBA Item Number,
            %                                    including revision. This is programmed in Flash during manufacturing.
            %    XTID_SSIC_ORDERCODE     = 0x01, Returns the PCBA / PCBA stack order code.
            %    XTID_SSIC_FIRMWAREID    = 0x02, Returns the installed Firmware ID. As viewed from the "highest"
            %                                    level of the software, "X4M300".
            %    XTID_SSIC_VERSION       = 0x03, Returns the installed Firmware Version. As viewed from the "highest"
            %                                    level of the software.
            %    XTID_SSIC_BUILD         = 0x04, Returns information of the SW Build installed on the device
            %    XTID_SSIC_SERIALNUMBER  = 0x06, Returns the PCBA serial number
            %    XTID_SSIC_VERSIONLIST   = 0x07, Returns ID and version of all components. Calls all components and compound
            %                                    a string. E.g. "X4M300:1.0.0.3;XEP:2.3.4.5;X4C51:1.0.0.0;DSP:1.1.1.1"
            system_infoPtr = libpointer('string',blanks(this.SI_max_length));
            
            status = this.radarInterface.get_system_info( this.X4M200_instance, info_code, system_infoPtr, this.SI_max_length);
            
            system_info = system_infoPtr.Value; clear system_infoPtr;
        end
        
        function status = module_reset(this)
            %MODULE_RESET Resets and restart the module.
            status = this.radarInterface.module_reset(this.X4M200_instance);
        end

        function status = reset_to_factory_preset(this)
            %RESET_TO_FACTORY_PRESET Resets the module to factory preset settings.
            status = this.radarInterface.reset_to_factory_preset(this.X4M200_instance);
        end

        function status = set_debug_level(this, debug_level)
            %SET_DEBUG_LEVEL Set the debug level of the profile.
            status = this.radarInterface.set_debug_level(this.X4M200_instance, debug_level);
        end
	
        function status = set_sensor_mode( this, mode )
            %SET_SENSOR_MODE Control the sensor module mode of execution.
            % When the module is configured to run an application, such as X4M200,
            % this method is used to control the execution.
            %
            % status = SET_SENSOR_MODE( this, mode )
            %   Run  - [0x01] Run profile application
            %   Idle - [0x11] Halts profile application, can be Run again.
            %   Stop - [0x13] Stop profile application
            %   XEP  - [0x12] Interact directly with XDriver, i.e. allow streaming directly to host
            % @return
            %	0 = Succes, other indicates some error.
            switch mode
                case {'Run','run'}
                    mode = hex2dec('1');
                case {'Idle','idle'}
                    mode = hex2dec('11');
                case {'Stop','stop'}
                    mode = hex2dec('13');
                case {'XEP','xep'}
                    mode = hex2dec('12');
            end
            status = this.radarInterface.set_sensor_mode( this.X4M200_instance, mode );
         end
        
        function status = load_profile( this, profile_id)
            %LOAD_PROFILE Loads the a preset profile.
            %
            % If another profile is loaded, the other profile is unloaded before the new profile is loaded. The profile does not start, the module remains idle.
            % status = LOAD_PROFILE( this, profile_id)
            %   profile_id = X : respiration profile
            status = this.radarInterface.load_profile( this.X4M200_instance, profile_id);
        end

        function status = set_output_control(this, output_feature, output_ctrl)
            %SET_OUTPUT_CONTROL Control module profile output.
            %
            % Enable and disable data messages. Several calls can be made, one for each available output message the profile provides.
            %
            % output_feature:
            %     Respiration (0x2375fe26)
            %     Sleep (0x2375a16c)
            %     RespirationMovingList (0x610a3b00)
            %     RespirationDetectionList (0x610a3b02)
            %     BasebandIQ (0x0000000c)
            %     BasebandAP (0x0000000d)
            %     PulseDopplerFloat (0x00000010)
            %     PulseDopplerByte (0x00000011)
            %     NoisemapFloat (0x00000012)
            %     NoisemapByte (0x00000013)
            %
            % output_control:
            %     0 = disable
            %     1 = enable. For pulse-Doppler/noisemap, enable slow
            %     2 = for pulse-Doppler/noisemap, enable fast
            %     3 = for pulse-Doppler/noisemap, enable both fast and slow
            status = this.radarInterface.set_output_control( this.X4M200_instance, output_feature, output_ctrl);
        end
        
        function [output_ctrl,status] = get_output_control(this, output_feature)
            %GET_OUTPUT_CONTROL Returns the output control of the specified output feature.
            % [output_ctrl,status] = GET_OUTPUT_CONTROL( this, output_feature )
            status = this.radarInterface.get_output_control(this.X4M200_instance, output_feature, this.output_ctrl);
            output_ctrl = this.output_ctrl.Value;        
        end

        function status = set_debug_output_control(this, output_feature, output_ctrl)
            %SET_OUTPUT_CONTROL Control module profile output.
            %
            % Enable and disable data messages. Several calls can be made, one for each available output message the profile provides.
            %
            % output_feature:
            %     Respiration (0x2375fe26)
            %     Sleep (0x2375a16c)
            %     RespirationMovingList (0x610a3b00)
            %     RespirationDetectionList (0x610a3b02)
            %     BasebandIQ (0x0000000c)
            %     BasebandAP (0x0000000d)
            %     PulseDopplerFloat (0x00000010)
            %     PulseDopplerByte (0x00000011)
            %     NoisemapFloat (0x00000012)
            %     NoisemapByte (0x00000013)
            %
            % output_control:
            %     0 = disable
            %     1 = enable. For pulse-Doppler/noisemap, enable slow
            %     2 = for pulse-Doppler/noisemap, enable fast
            %     3 = for pulse-Doppler/noisemap, enable both fast and slow
            status = this.radarInterface.set_debug_output_control( this.X4M200_instance, output_feature, output_ctrl);
        end
        
        function [output_ctrl,status] = get_debug_output_control(this, output_feature)
            %GET_OUTPUT_CONTROL Returns the output control of the specified output feature.
            % [output_ctrl,status] = GET_DEBUG_OUTPUT_CONTROL( this, output_feature )
            status = this.radarInterface.get_debug_output_control(this.X4M200_instance, output_feature, this.output_ctrl);
            output_ctrl = this.output_ctrl.Value;        
        end        
        
        function status = set_detection_zone( this, start, stop )
            %SET_DETECTION_ZONE Set the desired detection zone.
            % The sensor will detect movements within this area.
            % status = SET_DETECTION_ZONE( this, start, stop )
            % 	start   Start of detection zone in meters.
            % 	stop    End of detection zone in meters.
            %   status  Successful call returns 0, otherwise error code.
            status = this.radarInterface.set_detection_zone(this.X4M200_instance,start,stop);
        end
        
        function [start,stop,status] = get_detection_zone( this )
            %GET_DETECTION_ZONE Returns the actual range window.
            % The sensor will detect movements within this area.
            % [start,stop,status] = GET_DETECTION_ZONE( this )
            % 	start   Start of detection zone in meters.
            % 	stop    End of detection zone in meters.
            %   status  Successful call returns 0, otherwise error code.
            startPtr = libpointer('singlePtr',0);
            stopPtr  = libpointer('singlePtr',0);
            status = this.radarInterface.get_detection_zone(this.X4M200_instance,startPtr,stopPtr);
            start= double(startPtr.Value);
            stop = double(stopPtr.Value);
            clear startPtr stopPtr;
        end      

        function [start,stop,step,status] = get_detection_zone_limits( this )
            %GET_DETECTION_ZONE_LIMITS Returns the potential settings of detection zone from the module.
            % The sensor can detect movements within this area.
            % [start,stop,step,status] = GET_DETECTION_ZONE_LIMITS( this )
            % 	start   Start of detection zone in meters.
            % 	stop    End of detection zone in meters.
            %   step
            %   status  Successful call returns 0, otherwise error code.
            startPtr = libpointer('singlePtr',0);
            stopPtr  = libpointer('singlePtr',0);
            stepPtr  = libpointer('singlePtr',0);
            status = this.radarInterface.get_detection_zone_limits(this.X4M200_instance,startPtr,stopPtr,stepPtr);
            start= double(startPtr.Value);
            stop = double(stopPtr.Value);
            step = double(stepPtr.Value);
            clear startPtr stopPtr stepPtr;
        end
        
        function status = store_noisemap(this)
            %STORE_NOISEMAP Send command to module to store the current noisemap to module flash.
            % status = STORE_NOISEMAP( this )
            %
            % Fails if a store already is active, for example during the first
            % initialize with XTID_NOISEMAP_CONTROL_INIT_ON_RESET disabled.
            status = this.radarInterface.store_noisemap(this.X4M200_instance);
        end
        
        function status = set_noisemap_control(this, nmapCtrl)
            %SET_NOISEMAP_CONTROL Configure the use of noisemap.
            % status = SET_NOISEMAP_CONTROL( this, nmapCtrl )
            %   nmapCtrl A bitfield of the various features.
            %       XTID_NOISEMAP_CONTROL_ENABLE -> Bit 0, turn on use of noisemap.
            %       XTID_NOISEMAP_CONTROL_ADAPTIVE -> Bit 1, adapt noisemap over time.
            %       XTID_NOISEMAP_CONTROL_INIT_ON_RESET -> Bit 2, don't store but initialize the noisemap every time.
            status = this.radarInterface.set_noisemap_control(this.X4M200_instance, nmapCtrl);
        end
        
        function [nmapCtrl,status] = get_noisemap_control(this)
            %GET_NOISEMAP_CONTROL Configure the use of noisemap.
            % [nmapCtrl,status] = GET_NOISEMAP_CONTROL( this )
            status = this.radarInterface.get_noisemap_control(this.X4M200_instance, this.NMCptr);
            nmapCtrl = this.NMCptr.Value;
        end
        
        function status = set_parameter_file( this, filename, data)
            %SET_PARAMETER_FILE Set parameter file for DSP and other parameters.
            % status = SET_PARAMETER_FILE( this, filename, data )
            %   filename File name of parameter file.
            %   data Character buffer with parameter file. The underlying
            %   function will replace typecast \n characters as newline and
            %   add null termination of string.
            status = this.radarInterface.set_parameter_file(this.X4M200_instance,filename,data);
        end
        
        function status = set_sensitivity( this, new_sensitivity )
            %SET_SENSITIVITY Set module sensitivity.
            % status = SET_SENSITIVITY( this, new_sensitivity )
            %   new_sensitivity Sensitivity level from 0 (low) to 9 (high).
            %   status  Successful call returns 0, otherwise error code.
            status = this.radarInterface.set_sensitivity(this.X4M200_instance,new_sensitivity);
        end

        function [sensitivity,status] = get_sensitivity(this)
            %GET_SENSITIVITY Get module sensitivity.
            % [sensitivity,status] = GET_SENSITIVITY( this )
            status = this.radarInterface.get_sensitivity(this.X4M200_instance, this.sensitivity);
            sensitivity = this.sensitivity.Value;
        end 
        
        function status = set_iopin_control( this , pinId, pinSetup, pinFeature)
            %SET_IOPIN_CONTROL Enable or disable GPIO feature.
            %   status = SET_IOPIN_CONTROL( this , pinId, pinSetup, pinFeature)
            %
            %   pin_id  = 0 : all pins
            %   pin_id ~= 0 : designated pin
            %   pin_feature = 0 : disabled - pin tri-stated / input (TBD)
            %   pin_feature = 1 : default
            %   pin_feature > 1 : designated feature
            %
            %   Pin setup:
            %       0 = input
            %       1 = output
            %   Pin feature:
            %       0 = Disable all iopin features
            %       1 = Configure according to datasheet default
            %       2 = Passive - Set and get iopin level from host
            status = this.radarInterface.xep_set_iopin_control(this.X4M200_instance, pinId, pinSetup, pinFeature);
        end

        function [pin_setup, pin_feature,status] = get_iopin_control(this, pinId)
            %GET_IOPIN_CONTROL Returns GPIO pin configuration.
            % [pin_setup, pin_feature, status] = GET_IOPIN_CONTROL( this, pinId )
            status = this.radarInterface.xep_get_iopin_control(this.X4M200_instance, pinId, this.pin_setup, this.pin_feature);
            pin_setup = this.pin_setup.Value;        
            pin_feature = this.pin_feature.Value;        
        end        
        
        function status = set_iopin_value( this , pinId, pinValue)
            %SET_IOPIN_VALUE If IO pin control is used to set pin_id as output, the pin level or value will be set to pin_value.
            %   status = SET_IOPIN_VALUE( this , pinId, pinValue)
            %
            % See also SET_IOPIN_CONTROL
            status = this.radarInterface.xep_set_iopin_value(this.X4M200_instance, pinId, pinValue);
        end
        
        function [pinValue, status] = get_iopin_value( this , pinId)
            %GET_IOPIN_VALUE Returns value of io pin specified by pinId
            % [status, pinValue] = GET_IOPIN_VALUE( this , pinId)
            % 	pinValue 	the value of pin PINID
            % 	status		call status; 0 if success, error code otherwise
            pinValuePtr = libpointer('uint32Ptr',0);
            status = this.radarInterface.xep_get_iopin_value(this.X4M200_instance, pinId, pinValuePtr);
            pinValue = double(pinValuePtr.Value);
            clear pinValuePtr
        end

        function status = set_tx_center_frequency( this, frequency_band )
            %SET_TX_CENTER_FREQUENCY Set TX center frequency.
            % status = SET_TX_CENTER_FREQUENCY( this, frequency_band )
            %   frequency_band  3 for low band (EU), 4 for high band (KCC).
            %   status  Successful call returns 0, otherwise error code.
            status = this.radarInterface.set_tx_center_frequency(this.X4M300_instance,frequency_band);
        end
        
        function [frequency_band,status] = get_tx_center_frequency( this )
            %GET_TX_CENTER_FREQUENCY Get TX center frequency.
            % [frequency_band,status] = GET_TX_CENTER_FREQUENCY( this)
            %   frequency_band  TX center frequency 3 for low band (EU), 4 for high band (KCC).
            %   status  Successful call returns 0, otherwise error code.
            itPtr = libpointer('uint32Ptr',0);
            status = this.radarInterface.get_tx_center_frequency(this.X4M300_instance,itPtr);
            frequency_band = double(itPtr.Value); clear itPtr
        end
                
        function status = set_led_control( this, mode, intensity )
            %SET_LED_CONTROL Configures the module LED mode.
            % status = SET_LED_CONTROL( this, mode, intensity )
            %   mode        0:Off, 1:Simple, 2:Full(default).
            %   intensity   Intensity: Future use, 0=low, 100=high
            status = this.radarInterface.set_led_control(this.X4M200_instance, mode, intensity);
        end

        function [mode,status] = get_led_control(this)
            %GET_LED_CONTROL Get led control mode.
            % [mode,status] = GET_LED_CONTROL( this )
            status = this.radarInterface.get_led_control(this.X4M200_instance, this.led_mode);
            mode = this.led_mode.Value;
        end         
        
        function int = peek_message_baseband_iq( this )
            %PEEK_MESSAGE_BASEBAND_IQ  Return the number of buffered baseband iq messages.
            % int = PEEK_MESSAGE_BASEBAND_IQ( this )
            int = double(this.radarInterface.peek_message_baseband_iq(this.X4M200_instance));
        end
        
        function int = peek_message_baseband_ap( this )
            %PEEK_MESSAGE_BASEBAND_AP  Return the number of buffered baseband ap messages.
            % int = PEEK_MESSAGE_BASEBAND_AP( this )
            int = double(this.radarInterface.peek_message_baseband_ap(this.X4M200_instance));
        end
        
        function int = peek_message_respiration_legacy( this )
            %PEEK_MESSAGE_RESPIRATION_LEGACY Return the number of buffered respiration legacy messages.
            % int = PEEK_MESSAGE_RESPIRATION_LEGACY( this )
            int = double(this.radarInterface.peek_message_respiration_legacy(this.X4M200_instance));
        end
        
        function int = peek_message_respiration_sleep( this )
            %PEEK_MESSAGE_RESPIRATION_SLEEP Return the number of buffered sleep messages.
            % int = PEEK_MESSAGE_RESPIRATION_SLEEP( this )
            int = double(this.radarInterface.peek_message_respiration_sleep(this.X4M200_instance));
        end
        
        function int = peek_message_respiration_movinglist( this )
            %PEEK_MESSAGE_RESPIRATION_MOVINGLIST Return the number of buffered respiration movinglist messages.
            % int = PEEK_MESSAGE_RESPIRATION_MOVINGLIST( this )
            int = double(this.radarInterface.peek_message_respiration_movinglist(this.X4M200_instance));
        end
        
        function int = peek_message_respiration_detectionlist( this )
            %PEEK_MESSAGE_RESPIRATION_DETECTIONLIST Return the number of buffered respiration detectionlist messages.
            % int = PEEK_MESSAGE_RESPIRATION_DETECTIONLIST( this )
            int = double(this.radarInterface.peek_message_respiration_detectionlist(this.X4M200_instance));
        end
        
        function int = peek_message_respiration_normalizedmovementlist( this )
            %PEEK_MESSAGE_RESPIRATION_NORMALIZEDMOVEMENTLIST Return the number of buffered respiration normalizedmovementlist messages.
            % int = PEEK_MESSAGE_RESPIRATION_NORMALIZEDMOVEMENTLIST( this )
            int = double(this.radarInterface.peek_message_respiration_normalizedmovementlist(this.X4M200_instance));
        end
        
        function int = peek_message_vital_signs( this )
            %PEEK_MESSAGE_VITAL_SIGNS Return the number of buffered vital signs messages.
            % int = PEEK_MESSAGE_VITAL_SIGNS( this )
            int = double(this.radarInterface.peek_message_vital_signs(this.X4M200_instance));
        end

        function [rd,status] = read_message_respiration_legacy( this )
            %READ_MESSAGE_RESPIRATION_LEGACY Get one buffered respiration_legacy data message. Order is FIFO.
            % [rd,status] = READ_MESSAGE_RESPIRATION_LEGACY( this )
            %   rd                   a data struct containing the respiration data.
            %   rd.counter           message sequence number
            %   rd.respiration_state respiration state
            %   rd.RPM               respiration rate
            %   rd.distance          distance to closest target
            %   rd.movement          movement of object, e.g. breathing pattern
            %   rd.signal_quality    relative number from 0 to 10 where 10 indicates highest signal quality.
            status = this.radarInterface.read_message_respiration_legacy(this.X4M200_instance, this.rL_frame_counter, this.rL_sensor_state, this.rL_RPM, this.rL_distance, this.rL_movement, this.rL_signal_quality);
            rd.counter               = double(this.rL_frame_counter.Value);
            rd.respiration_state     = double(this.rL_sensor_state.Value);
            rd.RPM                   = double(this.rL_RPM.Value);
            rd.distance              = double(this.rL_distance.Value);
            rd.movement              = double(this.rL_movement.Value);
            rd.signal_quality        = double(this.rL_signal_quality.Value);
        end

        function [rd,status] = read_message_respiration_sleep( this )
            %READ_MESSAGE_RESPIRATION_SLEEP Get one buffered sleep data message. Order is FIFO.
            % [rd,status] = READ_MESSAGE_RESPIRATION_SLEEP( this )
            %   rd                   a data struct containing the sleep data.
            %   rd.counter           message sequence number
            %   rd.respiration_state respiration state
            %   rd.RPM               respiration rate
            %   rd.distance          distance to closest target
            %   rd.signal_quality    relative number from 0 to 10 where 10 indicates highest signal quality.
            %   rd.movement_fast     metric from 0 to 100 indicating degree of fast movement in detection zone.
            %   rd.movement_slow     metric from 0 to 100 indicating degree of slow movement in detection zone.
            status = this.radarInterface.read_message_respiration_sleep(this.X4M200_instance, this.S_frame_counter, this.S_sensor_state, this.S_RPM, this.S_distance, this.S_signal_quality, this.S_movement_fast, this.S_movement_slow);
            rd.counter               = double(this.S_frame_counter.Value);
            rd.respiration_state     = double(this.S_sensor_state.Value);
            rd.RPM                   = double(this.S_RPM.Value);
            rd.distance              = double(this.S_distance.Value);
            rd.signal_quality        = double(this.S_signal_quality.Value);
            rd.movement_fast         = double(this.S_movement_fast.Value);
            rd.movement_slow         = double(this.S_movement_slow.Value);
        end
         
        function [rd,status] = read_message_respiration_movinglist( this )
            %READ_MESSAGE_RESPIRATION_MOVINGLIST Get one buffered respiration movinglist message. Order is FIFO.
            % [rd,status] = READ_MESSAGE_RESPIRATION_MOVINGLIST( this )
            %   rd                        a data struct containing the respiration movinglist data.
            %   rd.counter                message sequence number
            %   rd.movementIntervalCount  number of movement intervals
            %   rd.movementSlowItem       movement slow per interval
            %   rd.movementFastItem       movement fast per interval
            status = this.radarInterface.read_message_respiration_movinglist(this.X4M200_instance, this.rM_frame_counter, this.rM_movementIntervalCount, this.rM_movementSlowItem, this.rM_movementFastItem, this.rM_maxLength);
            rd.counter               = double(this.rM_frame_counter.Value);
            rd.movementIntervalCount = double(this.rM_movementIntervalCount.Value);
            rd.movementSlowItem      = double(this.rM_movementSlowItem.Value);
            rd.movementFastItem      = double(this.rM_movementFastItem.Value);
            
            rd.movementSlowItem      = rd.movementSlowItem(1:rd.movementIntervalCount);
            rd.movementFastItem      = rd.movementFastItem(1:rd.movementIntervalCount);
        end
        
        function [rd,status] = read_message_respiration_detectionlist( this )
            %READ_MESSAGE_RESPIRATION_DETECTIONLIST Get one buffered respiration detectionlist message. Order is FIFO.
            % [rd,status] = READ_MESSAGE_RESPIRATION_DETECTIONLIST( this )
            %   rd                              a data struct containing the respiration detectionlist data.
            %   rd.counter                      message sequence number
            %   rd.detectionCount               number of detections
            %   rd.detectionDistance            distance to detections
            %   rd.detectionRadarCrossSection   RCS of detections
            %   rd.detectionVelocity            velocity of detection
            status = this.radarInterface.read_message_respiration_detectionlist(this.X4M200_instance, this.rD_frame_counter, this.rD_detectionCount, this.rD_detectionDistance, this.rD_detectionRadarCrossSection, this.rD_detectionVelocity, this.rD_maxLength);
            rd.counter                    = double(this.rD_frame_counter.Value);
            rd.detectionCount             = double(this.rD_detectionCount.Value);
            rd.detectionDistance          = double(this.rD_detectionDistance.Value);
            rd.detectionRadarCrossSection = double(this.rD_detectionRadarCrossSection.Value); 
            rd.detectionVelocity          = double(this.rD_detectionVelocity.Value);
            
            rd.detectionDistance          = rd.detectionDistance(1:rd.detectionCount);
            rd.detectionRadarCrossSection = rd.detectionRadarCrossSection(1:rd.detectionCount); 
            rd.detectionVelocity          = rd.detectionVelocity(1:rd.detectionCount);
        end
        
        function [rd,status] = read_message_respiration_normalizedmovementlist( this )
            %READ_MESSAGE_RESPIRATION_NORMALIZEDMOVEMENTLIST Get one buffered respiration normalizedmovementlist message. Order is FIFO.
            % [rd,status] = READ_MESSAGE_RESPIRATION_NORMALIZEDMOVEMENTLIST( this )
            %   rd                                a data struct containing the respiration normalizedmovementlist data.
            %   rd.counter                        message sequence number
            %   rd.start                          start distance
            %   rd.bin_length                     length of each range bin
            %   rd.count                          number of range bins
            %   rd.normMovementSlowItem           normalized movement slow vector per range bin
            %   rd.normMovementFastItem           normalized movement fast vector per range bin
            status = this.radarInterface.read_message_respiration_normalizedmovementlist(this.X4M200_instance,this.rMPL_frame_counter,this.rMPL_start,this.rMPL_bin_length,this.rMPL_count,this.rMPL_normalizedMovementSlowItem,this.rMPL_normalizedMovementFastItem,this.rMPL_maxLength);
            rd.counter                  = double(this.rMPL_frame_counter.Value);
            rd.start                    = double(this.rMPL_start.Value);
            rd.bin_length               = double(this.rMPL_bin_length.Value);
            rd.count                    = double(this.rMPL_count.Value);
            rd.normalizedMovementSlowItem = double(this.rMPL_normalizedMovementSlowItem.Value);
            rd.normalizedMovementFastItem = double(this.rMPL_normalizedMovementFastItem.Value);
            
            rd.normalizedMovementFastItem       = rd.normalizedMovementFastItem(1:rd.count); 
            rd.normalizedMovementFastItem       = rd.normalizedMovementFastItem(1:rd.count);
        end
        
        function [rd,status] = read_message_vital_signs( this )
            %READ_MESSAGE_VITAL_SIGNS Get one buffered vital signs message. Order is FIFO.
            % [rd,status] = READ_MESSAGE_VITAL_SIGNS( this )
            %   rd                              a data struct containing the vital signs data.
            %   rd.counter                      message sequence number
            %   rd.sensor_state                 sensor state
            %   rd.respRate                     respiration rate
            %   rd.respDist                     respiration distance
            %   rd.respConf                     respiration confidence
            %   rd.heartRate                    heart rate
            %   rd.heartDist                    heart distance
            %   rd.heartConf                    heart rate confidence
            %   rd.normalizedMovementSlow       normalized movement slow
            %   rd.normalizedMovementFast       normalized movement fast
            %   rd.normalizedMovementStart      normalized movement start distance
            %   rd.normalizedMovementStop       normalized movement stop distance
            status = this.radarInterface.read_message_vital_signs(this.X4M200_instance, this.VS_frame_counter, this.VS_state, this.VS_respRate, this.VS_respDist, this.VS_respConf, this.VS_heartRate, this.VS_heartDist, this.VS_heartConf, this.VS_norm_movement_slow, this.VS_norm_movement_fast, this.VS_norm_movement_start, this.VS_norm_movement_stop);
            rd.counter              = double(this.VS_frame_counter.Value);
            rd.sensor_state         = double(this.VS_frame_counter.Value);
            rd.respRate             = double(this.VS_respRate.Value);
            rd.respDist             = double(this.VS_respDist.Value);
            rd.respConf             = double(this.VS_respConf.Value);
            rd.heartRate            = double(this.VS_heartRate.Value);
            rd.heartDist            = double(this.VS_heartDist.Value);
            rd.heartConf            = double(this.VS_heartConf.Value);
            rd.normalizedMovementSlow  = double(this.VS_norm_movement_slow.Value);
            rd.normalizedMovementFast  = double(this.VS_norm_movement_fast.Value);
            rd.normalizedMovementStart = double(this.VS_norm_movement_start.Value);
            rd.normalizedMovementStop  = double(this.VS_norm_movement_stop.Value);
        end
        
        function [iq_data,header,status] = read_message_baseband_iq( this )
            % Get one buffered baseband IQ data message. Order is FIFO.
            % [iq_data,header,status] = READ_MESSAGE_BASEBAND_IQ( this, name )
            %   iq_data   complex vector containing i and q baseband data
            %   header    a data struct containing the following header data:
            %   header.frame_counter       message sequence number
            %   header.num_bins            number of bins in the i or q data message
            %	header.bin_length
            %	header.sample_frequency    receiver sampling frequency
            %	header.carrier_frequency   transmitter center frequency
            %	header.range_offset        range to the beginning of the frame
            %   status
            status = this.radarInterface.read_message_baseband_iq(this.X4M200_instance, this.pIQ_frame_counter, this.pIQ_num_bins, this.pIQ_bin_length, this.pIQ_sample_frequency, this.pIQ_carrier_frequency, this.pIQ_range_offset, this.pIQ_i_data, this.pIQ_q_data,this.IQ_max_length);
            header.frame_counter     = double(this.pIQ_frame_counter.Value);
            header.num_bins          = double(this.pIQ_num_bins.Value);
            header.bin_length        = double(this.pIQ_bin_length.Value);
            header.sample_frequency  = double(this.pIQ_sample_frequency.Value);
            header.carrier_frequency = double(this.pIQ_carrier_frequency.Value);
            header.range_offset      = double(this.pIQ_range_offset.Value);
            i_data    = double(this.pIQ_amplitude.Value(1:header.num_bins));
            q_data    = double(this.pIQ_phase.Value(1:header.num_bins));
            iq_data   = complex(i_data,q_data);
        end
        
        function [amplitude,phase,header,status] = read_message_baseband_ap( this )
            % Get one buffered baseband AP data message. Order is FIFO.
            % [amplitude,phase,header,status] = READ_MESSAGE_BASEBAND_AP( this, name )
            %   amplitude  vector containing the signal envelope
            %   phase      vector containing the signal phase
            %   header                      a data struct containing the following header data:
            %   header.frame_counter        message sequence number
            %   header.num_bins             number of bins in the i or q data message
            %   header.bin_length
            %   header.sample_frequency     receiver sampling frequency
            %   header.carrier_frequency    transmitter center frequency
            %   header.range_offset         range to the beginning of the frame
            %   status
            status = this.radarInterface.read_message_baseband_ap(this.X4M200_instance, this.pAP_frame_counter, this.pAP_num_bins, this.pAP_bin_length, this.pAP_sample_frequency, this.pAP_carrier_frequency, this.pAP_range_offset, this.pAP_amplitude, this.pAP_phase,this.AP_max_length);
            header.frame_counter     = double(this.pAP_frame_counter.Value);
            header.num_bins          = double(this.pAP_num_bins.Value);
            header.bin_length        = double(this.pAP_bin_length.Value);
            header.sample_frequency  = double(this.pAP_sample_frequency.Value);
            header.carrier_frequency = double(this.pAP_carrier_frequency.Value);
            header.range_offset      = double(this.pAP_range_offset.Value);
            amplitude = double(this.pAP_amplitude.Value(1:header.num_bins));
            phase     = double(this.pAP_phase.Value(1:header.num_bins));
        end

        function int = peek_message_pulsedoppler_float( this )
            %PEEK_MESSAGE_PULSEDOPPLER_FLOAT  Return the number of buffered pulse-Doppler float messages
            % int = PEEK_MESSAGE_PULSEDOPPLER_FLOAT( this )
            int = double(this.radarInterface.peek_message_pulsedoppler_float(this.X4M200_instance));
        end

        function [data,header,status] = read_message_pulsedoppler_float( this )
            % Get one buffered pulse-Doppler float data message. Order is FIFO.
            % [data,header,status] = READ_MESSAGE_PULSEDOPPLER_FLOAT( this )
            %   data                          vector containing frequency
            %   header                        a data struct containing the following header data:
            %   header.frame_counter          Frame counter generated from chip data rate
            %   header.matrix_counter         Incremental matrix counter.
            %   header.range_idx              Range bin index of current Doppler vector
            %   header.range_bins             Number of total range bins in the pulse-Doppler output matrix
            %   header.frequency_count        Number of points in frequency axis
            %   header.pulsedoppler_instance  Selected pulse-Doppler type from [0..N-1] where N is number of PDs.
            %   header.fps                    Output chip framerate (frames per second)
            %   header.fps_decimated          Input FPS of this pulse-Doppler instance
            %   header.frequency_start        Frequency of first value
            %   header.frequency_step         Difference between each frequency bin
            %   header.range                  Absolute range of current frequency array
            %   status                        Status of execution
            status = this.radarInterface.read_message_pulsedoppler_float( ...
                this.X4M200_instance, this.pD_frame_counter, ...
                this.pD_matrix_counter, this.pD_range_idx, ...
                this.pD_range_bins, this.pD_frequency_count, ...
                this.pD_pulsedoppler_instance, ...
                this.pD_fps, this.pD_fps_decimated, this.pD_frequency_start, ...
                this.pD_frequency_step, this.pD_range, this.pD_float_data, ...
                this.pD_maxLength);

            header.frame_counter         = double(this.pD_frame_counter.Value);
            header.matrix_counter        = double(this.pD_matrix_counter.Value);
            header.range_idx             = double(this.pD_range_idx.Value);
            header.range_bins            = double(this.pD_range_bins.Value);
            header.frequency_count       = double(this.pD_frequency_count.Value);
            header.pulsedoppler_instance = double(this.pD_pulsedoppler_instance.Value);
            header.fps                   = double(this.pD_fps.Value);
            header.fps_decimated         = double(this.pD_fps_decimated.Value);
            header.frequency_start       = double(this.pD_frequency_start.Value);
            header.frequency_step        = double(this.pD_frequency_step.Value);
            header.range                 = double(this.pD_range.Value);
            data                         = double(this.pD_float_data.Value(1:header.frequency_count));
        end

        function int = peek_message_pulsedoppler_byte( this )
            %PEEK_MESSAGE_PULSEDOPPLER_BYTE  Return the number of buffered pulse-Doppler byte messages
            % int = PEEK_MESSAGE_PULSEDOPPLER_BYTE( this )
            int = double(this.radarInterface.peek_message_pulsedoppler_byte(this.X4M200_instance));
        end

        function [data,header,status] = read_message_pulsedoppler_byte( this )
            % Get one buffered pulse-Doppler byte data message. Order is FIFO.
            % [data,header,status] = READ_MESSAGE_PULSEDOPPLER_BYTE( this )
            %   data                          vector containing frequency
            %   header                        a data struct containing the following header data:
            %   header.frame_counter          Frame counter generated from chip data rate
            %   header.matrix_counter         Incremental matrix counter.
            %   header.range_idx              Range bin index of current Doppler vector
            %   header.range_bins             Number of total range bins in the pulse-Doppler output matrix
            %   header.frequency_count        Number of points in frequency axis
            %   header.pulsedoppler_instance  Selected pulse-Doppler type from [0..N-1] where N is number of PDs.
            %   header.byte_step_start        Start of dB-compression range
            %   header.byte_step_size         Size of one step in dB
            %   header.fps                    Output chip framerate (frames per second)
            %   header.fps_decimated          Input FPS of this pulse-Doppler instance
            %   header.frequency_start        Frequency of first value
            %   header.frequency_step         Difference between each frequency bin
            %   header.range                  Absolute range of current frequency array
            %   status                        Status of execution
            status = this.radarInterface.read_message_pulsedoppler_byte( ...
                this.X4M200_instance, this.pD_frame_counter, ...
                this.pD_matrix_counter, this.pD_range_idx, ...
                this.pD_range_bins, this.pD_frequency_count, ...
                this.pD_pulsedoppler_instance, this.pD_byte_step_start, ...
                this.pD_byte_step_size, this.pD_fps, ...
                this.pD_fps_decimated, this.pD_frequency_start, ...
                this.pD_frequency_step, this.pD_range, this.pD_byte_data, ...
                this.pD_maxLength);

            header.frame_counter         = double(this.pD_frame_counter.Value);
            header.matrix_counter        = double(this.pD_matrix_counter.Value);
            header.range_idx             = double(this.pD_range_idx.Value);
            header.range_bins            = double(this.pD_range_bins.Value);
            header.frequency_count       = double(this.pD_frequency_count.Value);
            header.pulsedoppler_instance = double(this.pD_pulsedoppler_instance.Value);
            header.byte_step_start       = double(this.pD_byte_step_start.Value);
            header.byte_step_size        = double(this.pD_byte_step_size.Value);
            header.fps                   = double(this.pD_fps.Value);
            header.fps_decimated         = double(this.pD_fps_decimated.Value);
            header.frequency_start       = double(this.pD_frequency_start.Value);
            header.frequency_step        = double(this.pD_frequency_step.Value);
            header.range                 = double(this.pD_range.Value);
            data                         = double(this.pD_byte_data.Value(1:header.frequency_count));
        end

        function int = peek_message_noisemap_float( this )
            %PEEK_MESSAGE_NOISEMAP_FLOAT  Return the number of buffered noisemap float messages
            % int = PEEK_MESSAGE_NOISEMAP_FLOAT( this )
            int = double(this.radarInterface.peek_message_noisemap_float(this.X4M200_instance));
        end

        function [data,header,status] = read_message_noisemap_float( this )
            % Get one buffered noisemap float data message. Order is FIFO.
            % [data,header,status] = READ_MESSAGE_NOISEMAP_FLOAT( this )
            %   data                          vector containing frequency noise level
            %   header                        a data struct containing the following header data:
            %   header.frame_counter          Frame counter generated from chip data rate
            %   header.matrix_counter         Incremental matrix counter.
            %   header.range_idx              Range bin index of current Doppler vector
            %   header.range_bins             Number of total range bins in the pulse-Doppler output matrix
            %   header.frequency_count        Number of points in frequency axis
            %   header.pulsedoppler_instance  Selected pulse-Doppler type from [0..N-1] where N is number of PDs.
            %   header.fps                    Output chip framerate (frames per second)
            %   header.fps_decimated          Input FPS of this pulse-Doppler instance
            %   header.frequency_start        Frequency of first value
            %   header.frequency_step         Difference between each frequency bin
            %   header.range                  Absolute range of current frequency array
            %   status                        Status of execution
            status = this.radarInterface.read_message_noisemap_float( ...
                this.X4M200_instance, this.pD_frame_counter, ...
                this.pD_matrix_counter, this.pD_range_idx, ...
                this.pD_range_bins, this.pD_frequency_count, ...
                this.pD_pulsedoppler_instance, ...
                this.pD_fps, this.pD_fps_decimated, this.pD_frequency_start, ...
                this.pD_frequency_step, this.pD_range, this.pD_float_data, ...
                this.pD_maxLength);

            header.frame_counter         = double(this.pD_frame_counter.Value);
            header.matrix_counter        = double(this.pD_matrix_counter.Value);
            header.range_idx             = double(this.pD_range_idx.Value);
            header.range_bins            = double(this.pD_range_bins.Value);
            header.frequency_count       = double(this.pD_frequency_count.Value);
            header.pulsedoppler_instance = double(this.pD_pulsedoppler_instance.Value);
            header.fps                   = double(this.pD_fps.Value);
            header.fps_decimated         = double(this.pD_fps_decimated.Value);
            header.frequency_start       = double(this.pD_frequency_start.Value);
            header.frequency_step        = double(this.pD_frequency_step.Value);
            header.range                 = double(this.pD_range.Value);
            data                         = double(this.pD_float_data.Value(1:header.frequency_count));
        end

        function int = peek_message_noisemap_byte( this )
            %PEEK_MESSAGE_NOISEMAP_BYTE  Return the number of buffered noisemap byte messages
            % int = PEEK_MESSAGE_NOISEMAP_BYTE( this )
            int = double(this.radarInterface.peek_message_noisemap_byte(this.X4M200_instance));
        end

        function [data,header,status] = read_message_noisemap_byte( this )
            % Get one buffered noisemap byte data message. Order is FIFO.
            % [data,header,status] = READ_MESSAGE_NOISEMAP_BYTE( this )
            %   data                          vector containing frequency noise level
            %   header                        a data struct containing the following header data:
            %   header.frame_counter          Frame counter generated from chip data rate
            %   header.matrix_counter         Incremental matrix counter.
            %   header.range_idx              Range bin index of current Doppler vector
            %   header.range_bins             Number of total range bins in the pulse-Doppler output matrix
            %   header.frequency_count        Number of points in frequency axis
            %   header.pulsedoppler_instance  Selected pulse-Doppler type from [0..N-1] where N is number of PDs.
            %   header.byte_step_start        Start of dB-compression range
            %   header.byte_step_size         Size of one step in dB
            %   header.fps                    Output chip framerate (frames per second)
            %   header.fps_decimated          Input FPS of this pulse-Doppler instance
            %   header.frequency_start        Frequency of first value
            %   header.frequency_step         Difference between each frequency bin
            %   header.range                  Absolute range of current frequency array
            %   status                        Status of execution
            status = this.radarInterface.read_message_noisemap_byte( ...
                this.X4M200_instance, this.pD_frame_counter, ...
                this.pD_matrix_counter, this.pD_range_idx, ...
                this.pD_range_bins, this.pD_frequency_count, ...
                this.pD_pulsedoppler_instance, this.pD_byte_step_start, ...
                this.pD_byte_step_size, this.pD_fps, ...
                this.pD_fps_decimated, this.pD_frequency_start, ...
                this.pD_frequency_step, this.pD_range, this.pD_byte_data, ...
                this.pD_maxLength);

            header.frame_counter         = double(this.pD_frame_counter.Value);
            header.matrix_counter        = double(this.pD_matrix_counter.Value);
            header.range_idx             = double(this.pD_range_idx.Value);
            header.range_bins            = double(this.pD_range_bins.Value);
            header.frequency_count       = double(this.pD_frequency_count.Value);
            header.pulsedoppler_instance = double(this.pD_pulsedoppler_instance.Value);
            header.byte_step_start       = double(this.pD_byte_step_start.Value);
            header.byte_step_size        = double(this.pD_byte_step_size.Value);
            header.fps                   = double(this.pD_fps.Value);
            header.fps_decimated         = double(this.pD_fps_decimated.Value);
            header.frequency_start       = double(this.pD_frequency_start.Value);
            header.frequency_step        = double(this.pD_frequency_step.Value);
            header.range                 = double(this.pD_range.Value);
            data                         = double(this.pD_byte_data.Value(1:header.frequency_count));
        end

        function clear( this, name )
            %CLEAR Clears the in-buffer of the named subscription.
            %
            % @param[in]
            %	name Subscription identification
            this.radarInterface.clear( this.X4M200_instance, name );
        end     
        
    end
    
    methods
        % Constructor
        function x4m200 = X4M200( mc )
            % Constructor
            x4m200.radarInterface = mc.radarInterface;
            x4m200.X4M200_instance = calllib(mc.lib_name,'nva_get_x2',mc.mcInstance);
            assert(~x4m200.X4M200_instance.isNull, 'create xethru failed check the logs');            
        end
        
        % Destructor
        function delete( this )
            % Destructor
            clear this.AP_framePtr this.IQ_framePtr;
            clear this.NMCptr;
            clear this.S_frame_counter this.S_sensor_state this.S_RPM this.S_distance this.S_signal_quality this.S_movement_fast this.S_movement_slow
            clear this.rL_frame_counter this.rL_sensor_state this.rL_RPM this.rL_distance this.rL_movement this.rL_signal_quality
            clear this.rM_frame_counter this.rM_movementIntervalCount this.rM_movementSlowItem this.rM_movementFastItem
            clear this.rD_frame_counter this.rD_detectionCount this.rD_detectionDistance this.rD_detectionRadarCrossSection this.rD_detectionVelocity
            clear this.pAP_frame_counter this.pAP_num_bins this.pAP_bin_length this.pAP_sample_frequency this.pAP_carrier_frequency this.pAP_range_offset this.pAP_amplitude this.pAP_phase
            clear this.pIQ_frame_counter this.pIQ_num_bins this.pIQ_bin_length this.pIQ_sample_frequency this.pIQ_carrier_frequency this.pIQ_range_offset this.pIQ_i_data this.pIQ_q_data
            clear this.pD_maxLength, this.pD_frame_counter, this.pD_matrix_counter, this.pD_range_idx, this.pD_range_bins, this.pD_frequency_count, this.pD_pulsedoppler_instance, this.pD_byte_step_start, this.pD_byte_step_size, this.pD_fps, this.pD_fps_decimated, this.pD_frequency_start, this.pD_frequency_step, this.pD_range, this.pD_float_data, this.pD_byte_data

            calllib(this.radarInterface.lib_name,'nva_destroy_X2_interface',this.X4M200_instance);
            clear('this.radarInterface')
            clear('this.X4M200_instance')
        end
    end
end
