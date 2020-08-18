classdef RadarInterface
    % A matlab wrapper for the XeThru Module Communication Library written in C.
    %
    % See also MODULECONNECTOR, X2M200, X2,

    properties
        device_name % Used to identify device / port
        log_level 	% Regulate the amount of logging 
        lib_name 	% Used to identify the C/C++ library
    end

    methods

        function ri = RadarInterface(device_name,log_level,lib_name)
            ri.device_name = device_name;
            ri.log_level = log_level;
            ri.lib_name = lib_name;
        end

        %% Module Connector
        %
        %function set_default_timeout(new_default_timeout)
        %   disp('empty void'); 
        %end
        %
        function sha = git_sha(this)
            sha = calllib(this.lib_name,'nva_git_sha');
        end

    %% =================================================================================
    %  RadarRaw interface specific 
    %===================================================================================
        function status = send_command(this, x2_instance, command)
            status = calllib(this.lib_name,'nva_subscribe',x2_instance,command);
        end
        %
        function status = send_command_single(this, x2_instance, command,comparator)
           status = calllib(this.lib_name,'nva_subscribe',x2_instance,command,comparator);
        end
        %
        %       %function Bytes = send_command_multi(command,comparator)
        %       %         disp('empty std');
        %       %end
        %
        function status = subscribe(this, x2_instance, name, comparator)
            status = calllib(this.lib_name,'nva_subscribe',x2_instance,name);
        end
        %
        function status = unsubscribe(this, x2_instance, name)
            status = calllib(this.lib_name,'nva_unsubscribe',x2_instance,name);
        end
        %
        %function vector_Bytes = get_all_packets(name)
        %   disp('Empty Bytes');
        %end
        %
        function status = get_packet(this, x2_instance, name, packetPtr, maxLen)
            [status,~,name,packet] = calllib(this.lib_name,'nva_get_packet', x2_instance,name,packetPtr,maxLen);
        end
        %
        function number_of_packets = get_number_of_packets(this, x2_instance, name)
            [status,~,~,number_of_packets] = calllib(this.lib_name,'nva_get_number_of_packets', x2_instance,name,libpointer('uint32Ptr',0));
            assert(status==0, 'nva_get_number_of_packets failed');
        end
        %
        function status = clear(this, x2_instance, name)
            status = calllib(this.lib_name,'nva_clear',x2_instance,name);
        end
          
    %% =================================================================================
    %  X2M200 interface specific */
    %===================================================================================
    
        function item_number = get_item_number(this, x2_instance, max_length)
            [status,~,item_number]=calllib(this.lib_name,'nva_get_item_number',x2_instance,libpointer('string',blanks(max_length)),max_length);
            assert(status==0,'item_number:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function [order_code,status] = get_order_code(this, x2_instance, max_length)
            [status,~,order_code]=calllib(this.lib_name,'nva_get_order_code',x2_instance,libpointer('string',blanks(max_length)),max_length);
            assert(status==0,'get_order_code:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function firmware_id = get_firmware_id(this, x2_instance, max_length)
            [status,~,firmware_id]=calllib(this.lib_name,'nva_get_firmware_id',x2_instance,libpointer('string',blanks(max_length)),max_length);
            assert(status==0,'get_firmware_id:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function firmware_version = get_firmware_version(this, x2_instance, max_length)
            [status,~,firmware_version]=calllib(this.lib_name,'nva_get_firmware_version',x2_instance,libpointer('string',blanks(max_length)),max_length);
            assert(status==0,'get_firmware_version:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function serial = get_serial_number(this, x2_instance, max_length)
            [status,~,serial]=calllib(this.lib_name,'nva_get_serial_number',x2_instance,libpointer('string',blanks(max_length)),max_length);
            assert(status==0,'get_serial_number:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function build_info = get_build_info(this, x2_instance, max_length)
            [status,~,build_info]=calllib(this.lib_name,'nva_get_build_info',x2_instance,libpointer('string',blanks(max_length)),max_length);
            assert(status==0,'get_build_info:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function app_id_list = get_app_id_list(this, x2_instance, max_length)
            [status,~,app_id_list]=calllib(this.lib_name,'nva_get_app_id_list',x2_instance,libpointer('string',blanks(max_length)),max_length);
            assert(status==0,'get_app_id_list:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = reset(this,x2_instance)
            status = calllib(this.lib_name,'nva_reset',x2_instance);
            assert(status==0,'reset:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = start_bootloader(this,x2_instance)
            status = calllib(this.lib_name,'nva_start_bootloader',x2_instance);
            assert(status==0,'start_bootloader:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = set_sensor_mode_run(this,x2_instance)
            status = calllib(this.lib_name,'nva_set_sensor_mode_run',x2_instance);
            assert(status==0,'set_sensor_mode_run:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = set_sensor_mode_idle(this,x2_instance)
            status = calllib(this.lib_name,'nva_set_sensor_mode_idle',x2_instance);
            assert(status==0,'set_sensor_mode_idle:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = load_sleep_profile(this,x2_instance)
            status = calllib(this.lib_name,'nva_load_sleep_profile',x2_instance);
            assert(status==0,'load_sleep_profile:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = load_respiration_profile(this,x2_instance)
            status = calllib(this.lib_name,'nva_load_respiration_profile',x2_instance);
            assert(status==0,'load_respiration_profile:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = enable_baseband(this,x2_instance, state )
            status = calllib(this.lib_name,'nva_enable_baseband',x2_instance,state);
            assert(status==0,'enable_baseband:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = enable_baseband_ap(this,x2_instance)
            status = calllib(this.lib_name,'nva_enable_baseband_ap',x2_instance);
            assert(status==0,'enable_baseband_ap:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = enable_baseband_iq(this,x2_instance)
            status = calllib(this.lib_name,'nva_enable_baseband_iq',x2_instance);
            assert(status==0,'enable_baseband_iq:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = set_detection_zone(this,x2_instance, start, stop)
            status = calllib(this.lib_name,'nva_set_detection_zone',x2_instance,single(start),single(stop));
            assert(status==0,'set_detection_zone:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = set_sensitivity(this,x2_instance, new_sensitivity)
            status = calllib(this.lib_name,'nva_set_sensitivity',x2_instance,uint32(new_sensitivity));
            assert(status==0,'set_sensitivity:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = set_led_control(this,x2_instance, mode, intensity)
            status = calllib(this.lib_name,'nva_set_led_control',x2_instance, uint8(mode), uint8(intensity));
            assert(status==0,'set_led_control:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = subscribe_to_resp_status(this,x2_instance, name)
            status = calllib(this.lib_name,'nva_subscribe_to_resp_status',x2_instance, name);
            assert(status==0,'subscribe_to_resp_status:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = subscribe_to_sleep_status(this,x2_instance, name)
            status = calllib(this.lib_name,'nva_subscribe_to_sleep_status',x2_instance, name);
            assert(status==0,'subscribe_to_sleep_status:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = subscribe_to_baseband_ap(this,x2_instance, name)
            status = calllib(this.lib_name,'nva_subscribe_to_baseband_ap',x2_instance, name);
            assert(status==0,'subscribe_to_baseband_ap:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = subscribe_to_baseband_iq(this,x2_instance, name)
            status = calllib(this.lib_name,'nva_subscribe_to_baseband_iq',x2_instance,name);
            assert(status==0,'subscribe_to_baseband_iq:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = disable_resp_output(this,x2_instance)
            status = calllib(this.lib_name,'nva_disable_resp_output',x2_instance);
            assert(status==0,'disable_resp_output:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = enable_resp_output(this,x2_instance)
            status = calllib(this.lib_name,'nva_enable_resp_output',x2_instance);
            assert(status==0,'enable_resp_output:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = get_respiration_data(this, x2_instance, name, frame_counterPtr, sensor_statePtr, respiration_ratePtr, distancePtr, movementPtr, signal_qualityPtr)
            status = calllib(this.lib_name,'nva_get_respiration_data',x2_instance,name, frame_counterPtr, sensor_statePtr, respiration_ratePtr, distancePtr, movementPtr, signal_qualityPtr);
            assert(status==0,'get_respiration_data:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = get_sleep_data(this, x2_instance, name, frame_counterPtr, sensor_statePtr, respiration_ratePtr, distancePtr, signal_qualityPtr,movement_slowPtr, movement_fastPtr)
             status = calllib(this.lib_name,'nva_get_sleep_data',x2_instance,name, frame_counterPtr, sensor_statePtr,respiration_ratePtr, distancePtr,signal_qualityPtr,movement_slowPtr, movement_fastPtr);
             assert(status==0,'get_sleep_data:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        % 
        function status = get_baseband_iq_data(this, x2_instance, name,frame_counterPtr, num_binsPtr, bin_lengthPtr, sample_frequencyPtr,carrier_frequencyPtr, range_offsetPtr, i_dataPtr, q_dataPtr, max_length)
           status = calllib(this.lib_name,'nva_get_baseband_iq_data',x2_instance,name,frame_counterPtr, num_binsPtr, bin_lengthPtr, sample_frequencyPtr,carrier_frequencyPtr, range_offsetPtr, i_dataPtr, q_dataPtr, max_length);
           assert(status==0,'get_baseband_iq_data:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %     
        function status = get_baseband_ap_data(this, x2_instance, name,frame_counterPtr, num_binsPtr, bin_lengthPtr, sample_frequencyPtr, carrier_frequencyPtr, range_offsetPtr, amplitudePtr, phasePtr, max_length)
            status = calllib(this.lib_name,'nva_get_baseband_ap_data', x2_instance, name,frame_counterPtr, num_binsPtr, bin_lengthPtr, sample_frequencyPtr, carrier_frequencyPtr, range_offsetPtr, amplitudePtr, phasePtr, max_length);
            assert(status==0,'get_baseband_ap_data:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end    

    %% =================================================================================
    %  X2 interface specific
    %===================================================================================
    
        function ok = set_x2_float(this,x2_instance,id, value)
            ok = calllib(this.lib_name,'nva_set_X2_float',x2_instance,uint32(id),single(value));
        end
        %
        function [float_var,status] = get_x2_float(this,x2_instance,id)
            [status,~,float_var] = calllib(this.lib_name,'nva_get_x2_float',x2_instance, uint32(id),libpointer('singlePtr',0));
            if status, error('get_x2_int return error'); end
        end
        %
        function ok = set_x2_int(this,x2_instance,id, value)
            ok = calllib(this.lib_name,'nva_set_X2_int',x2_instance,uint32(id),uint32(value));
        end
        %
        function [uint32_var,status] = get_x2_int(this,x2_instance,id)
            [status,~,uint32_var] = calllib(this.lib_name,'nva_get_x2_int',x2_instance, uint32(id),libpointer('uint32Ptr',0));
            if status, error('get_x2_int return error'); end
        end
        %
        function ok = set_x2_register(this,x2_instance,id, value)
            ok = calllib(this.lib_name,'nva_set_X2_register',x2_instance, uint32(id), uint32(value));
        end
        %
        function [uint32_var,status] = get_x2_register(this,x2_instance,register_id)
            [status,~,uint32_var] = calllib(this.lib_name,'nva_get_x2_register',x2_instance, uint32(register_id), libpointer('uint32Ptr',0));
            if status, error('get_x2_register return error'); end
        end
        %
        function int = set_debug_level( this, x2_instance, new_dbg_level )
            int = calllib(this.lib_name,'nva_set_debug_level', x2_instance, uint8(new_dbg_level));
        end
        %
        function pong = ping( this, x2_instance )
            [status,~,pong] = calllib(this.lib_name, 'nva_ping', x2_instance, libpointer('uint32Ptr',0));
            assert(status==0,['ping --',num2str(status)]);
        end
        %
        function int = run_timing_measurement( this, x2_instance )
            int = calllib(this.lib_name,'nva_run_timing_measurement', x2_instance);
        end
        %
        function int = set_fps( this, x2_instance, fps )
            int = calllib(this.lib_name,'nva_set_fps', x2_instance, fps );
        end
        %
        function int = set_sensor_mode( this, x2_instance, code )
            if ischar(code)
                switch code
                    case {'Run','run','XTS_SM_RUN'}
                        code = uint8(1);
                    case 'XTS_SM_NORMAL'
                        code = uint8(10);
                    case {'XEP','xep','XTS_SM_MANUAL'}
                        code = uint8(12);
                    case {'Idle','idle','XTS_SM_IDLE'}
                        code = uint8(11);
                    case {'Stop','stop'} 
                        code = uint8(13);
                    otherwise
                        error(['Unknown sensor mode ',code,'. Available modes are [XTS_SM_RUN,XTS_SM_NORMAL,XTS_SM_MANUAL,XTS_SM_IDLE]']);
                end
            end
            int = calllib(this.lib_name,'nva_set_sensor_mode', x2_instance,uint8(code));
        end
        %
        function int = set_sensor_mode_manual(this,x2_instance)
            int = calllib(this.lib_name,'nva_set_sensor_mode_manual',x2_instance);%,set_sensor_mode_idle(X2 * instance);
        end
        %
        function int = set_sensor_mode_and_submode( this, unsigned_char_mode, unsigned_char_submode )
		% Legacy command. Do not use.
		    int = this.radarInterface.set_sensor_mode_and_submode( this.x2_instance, unsigned_char_mode, unsigned_char_submode );
        end
        %
        function int = subscribe_to_raw_normalized( this, x2_instance, name )
            int = calllib(this.lib_name,'nva_subscribe_to_raw_normalized', x2_instance, name);
        end
        %
        function status = capture_single_normalized_frame( this, x2_instance, frameCtrPtr, framePtr, frameLenPtr, frameLen )
            status = calllib(this.lib_name,'nva_capture_single_normalized_frame', ...
            x2_instance,frameCtrPtr,framePtr,frameLenPtr, frameLen);
            assert(status==0,['GetSingleFrameNormalized returns ',num2str(status)]);
        end
        %
        function status = get_raw_normalized_frame( this, x2_instance, name, frameCtrPtr, framePtr, frameLenPtr,maxFrameLen )
            status = calllib(this.lib_name,'nva_get_raw_normalized_frame', ...
            x2_instance, name, frameCtrPtr, framePtr, frameLenPtr, maxFrameLen);
        end

    %===================================================================================
    %   XEP interface API 
    %===================================================================================

        function int = xep_peek_message_data_float( this, x2_instance )
            int = calllib(this.lib_name,'nva_peek_message_data_float', x2_instance);
        end
        %
        function status = xep_read_message_data_float( this, x2_instance, contentIdPtr, infoPtr, framePtr, lengthPtr, maxLength)
            status = calllib(this.lib_name,'nva_read_message_data_float', x2_instance, contentIdPtr, ...
                infoPtr, framePtr, lengthPtr, uint32(maxLength));
            assert(status==0,'xep_read_message_data_float:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = xep_x4driver_set_fps( this, x2_instance, fps)
            status = calllib(this.lib_name,'xep_x4driver_set_fps', x2_instance, single(fps));
            assert(status==0,'xep_x4driver_set_fps:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = xep_x4driver_get_fps( this, x2_instance, fpsPtr )
            status = calllib(this.lib_name,'xep_x4driver_get_fps', x2_instance, fpsPtr);
            assert(status==0,'xep_x4driver_get_fps:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = xep_x4driver_set_iterations( this, x2_instance, iterations)
            status = calllib(this.lib_name,'xep_x4driver_set_iterations', x2_instance, uint32(iterations));
            assert(status==0,'xep_x4driver_set_iterations:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = xep_x4driver_get_iterations( this, x2_instance, iterPtr )
            status = calllib(this.lib_name,'xep_x4driver_get_iterations', x2_instance, iterPtr);
            assert(status==0,'xep_x4driver_get_iterations:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %        
        function status = xep_x4driver_set_pulsesperstep( this, xep_instance, pulsesperstep)
            status = calllib(this.lib_name,'xep_x4driver_set_pulsesperstep', xep_instance,uint32(pulsesperstep));
            assert(status==0,'xep_x4driver_set_pulsesperstep:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = xep_x4driver_get_pulsesperstep( this, x2_instance, ppsPtr )
            status = calllib(this.lib_name,'xep_x4driver_get_pulsesperstep', x2_instance, ppsPtr);
            assert(status==0,'xep_x4driver_get_pulsesperstep:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end        %
        function status = xep_x4driver_set_downconversion( this, xep_instance, downconversion)
            status = calllib(this.lib_name,'xep_x4driver_set_downconversion', xep_instance, uint8(downconversion));
            assert(status==0,'xep_x4driver_set_downconversion:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = xep_x4driver_get_downconversion( this, xep_instance, downconversionPtr)
            status = calllib(this.lib_name,'xep_x4driver_get_downconversion', xep_instance, downconversionPtr);
            assert(status==0,'xep_x4driver_get_downconversion:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = xep_x4driver_set_frame_area( this, xep_instance, start, stop)
            status = calllib(this.lib_name,'xep_x4driver_set_frame_area', xep_instance, single(start), single(stop));
            assert(status==0,'xep_x4driver_set_frame_area:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = xep_x4driver_init( this, xep_instance)
            status = calllib(this.lib_name,'xep_x4driver_init', xep_instance);
            assert(status==0,'xep_x4driver_init:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = xep_x4driver_set_tx_center_frequency( this, xep_instance, frequency_band)
            status = calllib(this.lib_name,'xep_x4driver_set_tx_center_frequency', xep_instance, uint8(frequency_band));
            assert(status==0,'xep_x4driver_set_tx_center_frequency:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = xep_x4driver_get_tx_center_frequency( this, xep_instance, freqPtr)
            status = calllib(this.lib_name,'xep_x4driver_get_tx_center_frequency', xep_instance,freqPtr);
            assert(status==0,'xep_x4driver_get_tx_center_frequency:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = xep_x4driver_set_dac_min( this, xep_instance, dac_min)
            status = calllib(this.lib_name,'xep_x4driver_set_dac_min', xep_instance, uint32(dac_min));
            assert(status==0,'xep_x4driver_set_dac_min:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = xep_x4driver_get_dac_min( this, x2_instance, dac_minPtr )
            status = calllib(this.lib_name,'xep_x4driver_get_dac_min', x2_instance, dac_minPtr);
            assert(status==0,'xep_x4driver_get_dac_min:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = xep_x4driver_set_dac_max( this, xep_instance, dac_max)
            status = calllib(this.lib_name,'xep_x4driver_set_dac_max', xep_instance, uint32(dac_max));
            assert(status==0,'xep_x4driver_set_dac_max:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = xep_x4driver_get_dac_max( this, x2_instance, dac_maxPtr )
            status = calllib(this.lib_name,'xep_x4driver_get_dac_max', x2_instance, dac_maxPtr);
            assert(status==0,'xep_x4driver_get_dac_max:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = xep_x4driver_get_framebincount( this, xep_instance, fbincntPtr)
            status = calllib(this.lib_name,'xep_x4driver_get_framebincount', xep_instance, fbincntPtr);
            assert(status==0,'xep_x4driver_get_framebincount:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = xep_x4driver_set_frame_area_offset( this, xep_instance, offset)
            status = calllib(this.lib_name,'xep_x4driver_set_frame_area_offset', xep_instance, single(offset));
            assert(status==0,'xep_x4driver_set_frame_area_offset:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = xep_x4driver_set_enable( this, xep_instance, enable)
            status = calllib(this.lib_name,'xep_x4driver_set_enable', xep_instance, uint8(enable));
            assert(status==0,'xep_x4driver_set_enable:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = xep_x4driver_set_tx_power( this, xep_instance, tx_power)
            status = calllib(this.lib_name,'xep_x4driver_set_tx_power', xep_instance, uint8(tx_power));
            assert(status==0,'xep_x4driver_set_tx_power:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = xep_x4driver_get_tx_power( this, xep_instance,tx_powerPtr)
            status = calllib(this.lib_name,'xep_x4driver_get_tx_power', xep_instance,tx_powerPtr);
            assert(status==0,'xep_x4driver_set_tx_power:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = xep_x4driver_get_frame_area( this, xep_instance, startPtr, stopPtr)
            status = calllib(this.lib_name,'xep_x4driver_get_frame_area', xep_instance, startPtr, stopPtr);
            assert(status==0,'xep_x4driver_get_frame_area:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = xep_x4driver_get_frame_area_offset( this, xep_instance, offsetPtr)
            status = calllib(this.lib_name,'xep_x4driver_get_frame_area_offset', xep_instance, offsetPtr);
            assert(status==0,'xep_x4driver_get_frame_area_offset:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = xep_set_iopin_control( this, xep_instance, pinId, pinSetup, pinFeature)
            status = calllib(this.lib_name,'xep_set_iopin_control', xep_instance, uint32(pinId), uint32(pinSetup), uint32(pinFeature));
            assert(status==0,'xep_set_iopin_control:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = xep_get_iopin_control(this, xep_instance, pin_id, pin_setupPtr, pin_featurePtr)
            % Return setting
            status = calllib(this.lib_name,'xep_get_iopin_control',xep_instance, uint32(pin_id), pin_setupPtr, pin_featurePtr);
            assert(status==0,'xep_get_iopin_control:libraryStatusFailed',strcat('Call returns status=',num2str(status)));            
        end  
        %      
        function status = xep_set_iopin_value( this, xep_instance, pinId, pinValue)
            status = calllib(this.lib_name,'xep_set_iopin_value', xep_instance, uint32(pinId), uint32(pinValue));
            assert(status==0,'xep_set_iopin_value:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = xep_get_iopin_value( this, xep_instance, pinId, pinValuePtr)
            status = calllib(this.lib_name,'xep_get_iopin_value', xep_instance, uint32(pinId), pinValuePtr);
            assert(status==0,'xep_get_iopin_value:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        % FIX: Verify return value of XEP_PEEK_MESSAGE_DATA_STRING
		% FIX: Verify return value of XEP_PEEK_MESSAGE_DATA_FLOAT
         function status = xep_peek_message_data_string( this, xep_instance )
            status = calllib(this.lib_name,'xep_peek_message_data_string', xep_instance);
        end
        %
        function status = xep_read_message_data_string( this, xep_instance, content_idPtr, infoPtr, dataPtr, lengthPtr, max_length )
            [status,~,~,data] = calllib(this.lib_name,'xep_read_message_data_string', xep_instance, content_idPtr, infoPtr, dataPtr, lengthPtr, uint32(max_length));
            assert(status==0,'xep_read_message_data_string:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
            dataPtr.Value = data;
        end
        %
        function status = xep_x4driver_set_spi_register( this, xep_instance, address, value )
            status = calllib(this.lib_name,'xep_x4driver_set_spi_register',xep_instance,uint8(address), uint8(value) );
            assert(status==0,'xep_x4driver_set_spi_register:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = xep_x4driver_get_spi_register( this, xep_instance,address,valuePtr )
            status = calllib(this.lib_name,'xep_x4driver_get_spi_register',xep_instance,uint8(address), valuePtr  );
            assert(status==0,'xep_x4driver_get_spi_register:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = xep_x4driver_set_pif_register( this, xep_instance,address,value )
            status = calllib(this.lib_name,'xep_x4driver_set_pif_register',xep_instance,uint8(address),uint8(value) );
            assert(status==0,'xep_x4driver_set_pif_register:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = xep_x4driver_get_pif_register( this, xep_instance,address,valuePtr )
            status = calllib(this.lib_name,'xep_x4driver_get_pif_register',xep_instance,uint8(address),valuePtr  );
            assert(status==0,'xep_x4driver_get_pif_register:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = xep_x4driver_set_xif_register( this, xep_instance,address,value )
            status = calllib(this.lib_name,'xep_x4driver_set_xif_register', xep_instance,uint8(address),uint8(value) );
            assert(status==0,'xep_x4driver_set_xif_register:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = xep_x4driver_get_xif_register( this, xep_instance,address,valuePtr )
            status = calllib(this.lib_name,'xep_x4driver_get_xif_register',xep_instance,uint8(address),valuePtr );
            assert(status==0,'xep_x4driver_get_xif_register:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = xep_x4driver_set_prf_div( this, xep_instance,prf_div )
            status = calllib(this.lib_name,'xep_x4driver_set_prf_div',xep_instance,uint8(prf_div));
            assert(status==0,'xep_x4driver_set_prf_div:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = xep_x4driver_get_prf_div( this, xep_instance,prf_divPtr )
            status = calllib(this.lib_name,'xep_x4driver_get_prf_div',xep_instance,prf_divPtr);
            assert(status==0,'xep_x4driver_get_prf_div:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        
        %% =================================================================================
        %   XEP File API 
        %===================================================================================
        
        function status = search_for_file_by_type(this, xep_instance, type, filesPtr, lengthPtr, maxLength)
            status = calllib(this.lib_name,'nva_search_for_file_by_type',xep_instance,uint32(type),filesPtr,lengthPtr,uint32(maxLength));
            assert(status==0,'search_for_file_by_type:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = find_all_files(this, xep_instance, fileTypeItemsPtr, fileIdItemsPtr, itemsPtr, maxItems)
            status = calllib(this.lib_name,'nva_find_all_files',xep_instance,fileTypeItemsPtr,fileIdItemsPtr,itemsPtr,uint32(maxItems));
            assert(status==0,'find_all_files:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = create_file(this, xep_instance, fileType, fileID, length)
            status = calllib(this.lib_name,'nva_create_file',xep_instance,uint32(fileType),uint32(fileID),uint32(length));
            assert(status==0,'create_file:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = open_file(this, xep_instance, fileType, fileID)
            status = calllib(this.lib_name,'nva_open_file',xep_instance,uint32(fileType),uint32(fileID));
            assert(status==0,'open_file:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = set_file_data(this, xep_instance, fileType, fileID, offset, dataBuffer, length)
            status = calllib(this.lib_name,'nva_set_file_data',xep_instance,uint32(fileType),uint32(fileID),uint32(offset),uint8(dataBuffer),uint32(length));
            assert(status==0,'set_file_data:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = close_file(this, xep_instance, fileType, fileID, commit)
            status = calllib(this.lib_name,'nva_close_file',xep_instance,uint32(fileType),uint32(fileID),logical(commit));
            assert(status==0,'close_file:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = get_file_length(this, xep_instance, fileType, fileID, lengthPtr)
            status = calllib(this.lib_name,'nva_get_file_length',xep_instance,uint32(fileType),uint32(fileID),lengthPtr);
            assert(status==0,'get_file_length:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = delete_file(this, xep_instance, fileType, fileID)
            status = calllib(this.lib_name,'nva_delete_file',xep_instance,uint32(fileType),uint32(fileID));
            assert(status==0,'delete_file:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = get_file_data(this, xep_instance, fileType, fileID, offset, length, dataBufferPtr)
            status = calllib(this.lib_name,'nva_get_file_data',xep_instance,uint32(fileType),uint32(fileID),uint32(offset),uint32(length),dataBufferPtr);
            assert(status==0,'get_file_data:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = format_filesystem(this, xep_instance, key)
            status = calllib(this.lib_name,'nva_format_filesystem',xep_instance,uint32(key));
            assert(status==0,'format_filesystem:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = set_file(this, xep_instance, fileType, fileID, maxLength, dataBuffer)
            status = calllib(this.lib_name,'nva_set_file',xep_instance,uint32(fileType),uint32(fileID),uint32(maxLength),dataBuffer);
            assert(status==0,'set_file:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function status = get_file(this, xep_instance, fileType, fileID, maxLength, dataBufferPtr)
            status = calllib(this.lib_name,'nva_get_file',xep_instance,uint32(fileType),uint32(fileID),uint32(maxLength),dataBufferPtr);
            assert(status==0,'get_file:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        
        %% =================================================================================
        %   NotSupported API 
        %===================================================================================
        
        function [result,status] = get_x4_io_pin_value( this, x2_instance, pin)
            %int nva_get_x4_io_pin_value( X2 * instance,unsigned char pin,unsigned char * result );
            [status,~,result] = calllib(this.lib_name,'nva_get_x4_io_pin_value', x2_instance, uint8(pin));
        end
        %
        function int = set_x4_io_pin_value( this, x2_instance, pin, value)
            %int nva_set_x4_io_pin_value( X2 * instance,unsigned char pin,unsigned char value);
            int = calllib(this.lib_name,'nva_set_x4_io_pin_value', x2_instance, uint8(pin), uint8(value));
        end
        %
        function int = set_x4_io_pin_mode( this, x2_instance, pin, mode)
            %int nva_set_x4_io_pin_mode(X2 * instance,const unsigned char pin,const unsigned char mode);
            int = calllib(this.lib_name,'nva_set_x4_io_pin_mode', x2_instance, uint8(pin), uint8(mode));
        end
        %
        function int = set_x4_io_pin_dir( this, x2_instance, pin, direction)
            %int nva_set_x4_io_pin_dir(X2 * instance,const unsigned char pin,const unsigned char direction);
            int = calllib(this.lib_name,'nva_set_x4_io_pin_dir', x2_instance, uint8(pin), uint8(direction));
        end
        %
        function int = set_x4_io_pin_enable( this, x2_instance, pin)
            %int nva_set_x4_io_pin_enable(X2 * instance,const unsigned char pin);
            int = calllib(this.lib_name,'nva_set_x4_io_pin_enable', x2_instance, uint8(pin));
        end
        %
        function int = set_x4_io_pin_disable( this, x2_instance, pin)
            %int nva_set_x4_io_pin_disable(X2 * instance,const unsigned char pin);
            int = calllib(this.lib_name,'nva_set_x4_io_pin_disable', x2_instance, uint8(pin));
        end
        %
        function [result,status] = read_x4_spi( this, x2_instance, address)
            %int nva_read_x4_spi(X2 * instance,const unsigned char address,unsigned char * result);
            [status,~,result] = calllib(this.lib_name,'nva_read_x4_spi', x2_instance, uint8(address), libpointer('uint8Ptr',0));
        end
        %
        function int = write_x4_spi( this, x2_instance, address, value)
            %int nva_write_x4_spi(X2 * instance,unsigned char address,const unsigned char value);
            int = calllib(this.lib_name,'nva_write_x4_spi', x2_instance, uint8(address), uint8(value));
        end
        %
        function int = set_x4_fps( this, x2_instance, fps)
            %int nva_set_x4_fps(X2 * instance,const uint32_t fps);
            int = calllib(this.lib_name,'nva_set_x4_fps', x2_instance, uint32(fps));
        end
        %
        function int = subscribe_to_x4_decim( this, x2_instance, name)
            %int nva_subscribe_to_x4_decim(X2 * instance,const char * name);
            int = calllib(this.lib_name,'nva_subscribe_to_x4_decim', x2_instance, name);
        end
        %
        % TODO:
        function [result,status] = get_x4_decim_frame( this, x2_instance, name)
            result = [];status=1;
            %int nva_get_x4_decim_frame(X2 * instance,const char * name,uint32_t * frame_counter,double * frame_data,unsigned int max_length);
            %[status,~,result] = calllib(this.lib_name,'nva_get_x4_decim_frame', x2_instance,const char * name,uint32_t * frame_counter,double * frame_data,unsigned int max_length);
        end
        %
        function int = subscribe_to_data_float( this, x2_instance, name)
            %int nva_subscribe_to_data_float(X2 * instance,const char * name);
            int = calllib(this.lib_name,'nva_subscribe_to_data_float', x2_instance, name);
        end        %
        %
        function int = subscribe_to_trace( this, x2_instance, name)
            %int nva_subscribe_to_trace(X2 * instance, const char * name);
            int = calllib(this.lib_name,'nva_subscribe_to_trace', x2_instance, name);
        end
        %
        function int = get_trace( this, x2_instance, name, tracePtr, max_length)
            %int nva_get_trace(X2 * instance, const char * name, char * trace, unsigned int max_length);
            int = calllib(this.lib_name,'nva_get_trace', x2_instance, name, tracePtr, max_length);
        end
        %
        function int = subscribe_to_data_byte( this, x2_instance, name)
            %int nva_subscribe_to_data_byte(X2 * instance, const char * name);
            int = calllib(this.lib_name,'nva_subscribe_to_data_byte', x2_instance, name);
        end
        
    %% =================================================================================
    %  X4M_Common Interface specific */
    %===================================================================================

        %
        % Defined in X4MX00 interfaces
%         function status = reset_to_factory_preset(this, xep_instance)
%              status = 1;
% %            status=calllib(this.lib_name,'nva_reset_to_factory_preset',xep_instance);
%         end
        
        % % Defined in X2M200 interface
        %   function status = start_bootloader(this, xep_instance)
        %       status=calllib(this.lib_name,'nva_start_bootloader',xep_instance);
        %   end
        
        % TODO: 
        function status = system_run_test( this, xep_instance,test_code,data_lengthPtr,dataPtr)
            status = 1;
%            status=calllib(this.lib_name,'nva_system_run_test',xep_instance,uint8(test_code),data_lengthPtr,dataPtr);
        end
         
    %% =================================================================================
    %    X4M300 interface / X4M200 interface API 
    %===================================================================================

        % Defined by X2M200; 1.1.0 / 1.0.0
        %     function status = set_debug_level( this, X4M_instance, level)
        %     % Set the debug level of the profile.
        %     status = calllib(this.lib_name,'nva_set_debug_level', X4M_instance, uint8(level));
        %     end
        
        % Defined by X2M200; 1.1.0 / 1.0.0        
        %     % 1.0.0 / 1.0.0
        %     function status = set_sensor_mode( this, X4M_instance, mode, param)
        %     % Modes:
        %     %     Run - Run profile application
        %     %     Idle - Halts profile application, can be Run again.
        %     %     Stop - Stop profile application
        %     %     XEP - Interact directly with XDriver, i.e. allow streaming directly to host
        %     %
        %     % Param: Not used, ignored, can be 0.
        %         int set_sensor_mode(uint8_t mode, uint8_t param)
        %     end
        % 1.0.0 / 1.0.0
        function status = get_system_info( this, xep_instance,info_code,system_infoPtr,max_length)
            [status,~,info] = calllib(this.lib_name,'nva_get_system_info',xep_instance,uint8(info_code),system_infoPtr,uint32(max_length));
            system_infoPtr.Value = info;
        end
        % 1.0.0 / 1.0.0
        function status = module_reset(this, xep_instance)
        %   Resets the module.
            status=calllib(this.lib_name,'nva_module_reset',xep_instance);
        end

        % 1.0.0 / 1.0.0
        function status = load_profile( this, X4M_instance, profile_id)
        %   Set the debug level of the profile.
            status = calllib(this.lib_name,'nva_load_profile', X4M_instance, uint32(profile_id));
        end
        % 1.1.0 / 1.0.0
        function status = get_profileid( this, X4M_instance, profile_idPtr)
        %   Loads the presence profile. If another profile is loaded, the other profile is unloaded before the presence profile is loaded. The profile does not start, the module remains idle.
        %   profile = X : presence profile 
            status = calllib(this.lib_name,'nva_get_profileid', X4M_instance, profile_idPtr);
        end
        % 1.1.0 / 1.0.0
        function status = reset_to_factory_preset( this, X4M_instance )
        %   This command resets all factory presets. The module is in the same state as delivered from factory
            status = calllib(this.lib_name,'nva_reset_to_factory_preset', X4M_instance);
        end
        % Defined by X2M200; 1.0.0 / 1.0.0
        %   function status = set_sensitivity( this, X4M_instance, sensitivity)
        %   % Sets the overall sensitivity
        %   % sensitivity : 0 to 9, 0 = low, 9 = high
        %       status = calllib(this.lib_name,'nva_set_sensitivity',X4M_instance,uint32(sensitivity));
        %   end
        
        % 1.1.0 / 1.0.0
        function status = get_sensitivity( this, X4M_instance, sensitivityPtr)
        %   Returns success status. Pointer to resulting sensitivity level. See set_sensitivity.
            status = calllib(this.lib_name,'nva_get_sensitivity',X4M_instance,sensitivityPtr);
            %int get_sensitivity(uint32_t * sensitivity)
        end
        
        % Defined by X2M200; 1.0.0 / 1.0.0
        %   function status = set_detection_zone(this, X4M_instance,start,stop )
        %   % Sets the current detection zone. Rules: See datasheet.
        %   % The actual detection zone is determined by radar settings. Use the get_detection_zone command to get the actual values 
        %   status = calllib(this.lib_name,'nva_set_detection_zone',X4M_instance,single(start),single(stop));
        %   end
        
        % 1.0.0 / 1.0.0
        function status = get_detection_zone(this, X4M_instance,startPtr,stopPtr )
            status = calllib(this.lib_name,'nva_get_detection_zone', X4M_instance,startPtr,stopPtr);
        end
        % 1.0.0 / 1.0.0
        function status = get_detection_zone_limits(this, X4M_instance,startPtr,stopPtr,stepPtr )
            status = calllib(this.lib_name,'nva_get_detection_zone_limits', X4M_instance,startPtr,stopPtr,stepPtr);
        end
        %
        function status = store_noisemap(this, X4M_instance)
            status = calllib(this.lib_name,'nva_store_noisemap', X4M_instance);
        end
        %
        function status = set_noisemap_control(this, X4M_instance, nmapCtrl)
            status = calllib(this.lib_name,'nva_set_noisemap_control', X4M_instance, uint32(nmapCtrl));
        end
        %
        function status = get_noisemap_control(this, X4M_instance, nmapCtrlPtr)
            status = calllib(this.lib_name,'nva_get_noisemap_control', X4M_instance, nmapCtrlPtr);
        end
        %
        function status = set_parameter_file(this, X4M_instance,filename,dataBuffer)
            % Set parameter file for DSP and other parametres (sends a complete file, or subset)
            % Use specific filename for user settings, e.g. "USER". This can contain all datasheet user settings values.
            % Replace \n with MATLAB newline character (char(10)).
            dataBuffer = strrep(dataBuffer,'\n',char(10));
            % Replace \r with MATLAB newline character (char(13)).
            dataBuffer = strrep(dataBuffer,'\r',char(13));
            % Replace \r with MATLAB newline character (char(0)).
            dataBuffer = strrep(dataBuffer,'\0',char(0));
            % Append null termination.
            dataBuffer(end+1) = char(0);
            status = calllib(this.lib_name,'nva_set_parameter_file', X4M_instance,filename,dataBuffer);
        end
        %
        % Not to be used, handled by file API instead.
        %	  function status = get_parameter_file(filename_length,filenamePtr,data_lengthPtr,dataPtr)
        %    	  % Get parameter file (returns the complete file)
        %   	  status = calllib(this.lib_name,'nva_set_parameter_file', X4M_instance,filename_length,filenamePtr,data_lengthPtr,dataPtr);
        %   	  % int get_parameter_file(uint32_t filename_length, const char * filename, uint32_t * data_length, char * data)
        %     end
        %
        % Defined by X2M200; 1.0.0 / 1.0.0
        %     function status = set_led_control( this, X4M_instance, mode, intensity )
        %     %This command configures the LED mode.  The setting survives a power cycle.
        %     %     mode = 0 : OFF
        %     %     mode = 1 : simple
        %     %     mode = 2 : full (default)
        %     % Intensity: Future use, 0=low, 100=high
        %       status = calllib(this.lib_name,'nva_set_led_control',X4M_instance, uint8(mode), uint8(intensity));
        %       % int set_led_control(uint8_t mode, uint8_t intensity)
        %     end
        
        % 1.1.0 / 1.0.0
        function status = get_led_control( this, X4M_instance, modePtr )
        %   Returns the current LED mode
            status = calllib(this.lib_name,'nva_get_led_control',X4M_instance, modePtr);
            % int get_led_control(uint8_t * mode)
        end
        % 1.1.0 / 1.0.0
        function status = set_tx_center_frequency( this, X4M_instance, frequency_band)
            %Sets the frequency band used by the radar
            % frequency_band = 3 : X4M300 operates in the 6.0 to 8.5 Ghz range (default mode)
            % frequency_band = 4 : X4M300 operates in the 7.2 to 10.2 GHz range
            status = calllib(this.lib_name,'nva_set_tx_center_frequency',X4M_instance, frequency_band);
            %int set_tx_center_frequency(uint8_t tx_frequency)
        end
        % 1.1.0 / 1.0.0
        function status = get_tx_center_frequency( this, X4M_instance, frequencyPtr)
            %Returns the current frequency band
            status = calllib(this.lib_name,'nva_get_tx_center_frequency',X4M_instance, frequencyPtr);
            %int get_tx_center_frequency(uint8_t * tx_frequency)
        end
        % 1.1.0 / 1.0.0 
        function status = set_output_control( this, X4M_instance, output_feature, output_ctrl )
            status = calllib(this.lib_name,'nva_set_output_control',X4M_instance, output_feature, output_ctrl );
            %int set_output_control(uint32_t output_feature, uint32_t output_control)
        end
        % 1.1.0 / 1.0.0 
        function status = get_output_control( this, X4M_instance, output_feature, output_ctrlPtr )
            status = calllib(this.lib_name,'nva_get_output_control',X4M_instance, output_feature, output_ctrlPtr );
            %int get_output_control(uint32_t output_feature, uint32_t * output_control)
        end
        % 1.1.0 / 1.0.0 
        function status = set_debug_output_control( this, X4M_instance, output_feature, output_ctrl )
            status = calllib(this.lib_name,'nva_set_debug_output_control',X4M_instance, output_feature, output_ctrl );
            %int set_debug_output_control(uint32_t output_feature, uint32_t output_control)
        end
        % 1.1.0 / 1.0.0 
        function status = get_debug_output_control( this, X4M_instance, output_feature, output_ctrlPtr )
            status = calllib(this.lib_name,'nva_get_debug_output_control',X4M_instance, output_feature, output_ctrlPtr );
            %int get_debug_output_control(uint32_t output_feature, uint32_t * output_control)
        end        
        % 1.1.0 / 1.0.0 
        function status = set_iopin_control( this, X4M_instance, pin_id, pin_setup, pin_feature)
            %Enable or disable GPIO feature.
            %
            %     pin_id = 0 : all pins
            %     pin_id != 0 : designated pin
            %     pin_feature = 0 : disabled - pin tri-stated / input (TBD)
            %     pin_feature = 1 : default
            %     pin_feature > 1 : designated feature
            %
            % Pin setup:
            %     0 = tri-state
            %     1 = input
            %     2 = output
            % Pin feature:
            %     0 = Disable all iopin features
            %     1 = Configure according to datasheet default
            %     2 = Passive - Set and get iopin level from host
            status = calllib(this.lib_name,'xep_set_iopin_control',X4M_instance, uint32(pin_id), uint32(pin_setup), uint32(pin_feature));
            %int set_iopin_control(uint32_t pin_id, uint32_t pin_setup, uint32_t pin_feature)
        end        
        
        % 1.1.0 / 1.0.0 
        function status = get_iopin_control(this, X4M_instance,pin_id,pin_setupPtr,pin_featurePtr)
            % Return setting
            status = calllib(this.lib_name,'xep_get_iopin_control',X4M_instance,uint32(pin_id), pin_setupPtr, pin_featurePtr);
            %int get_iopin_control(uint32_t pin_id, uint32_t * pin_setup, uint32_t * pin_feature)
        end
        % 1.1.0 / 1.0.0 
        function status = set_iopin_value(this, X4M_instance,pin_id,pin_value) 
            % If IO pin control is used to set pin_id as output, the pin level or value will be set to pin_value.
            status = calllib(this.lib_name,'nva_set_iopin_value',X4M_instance,uint32(pin_id), uint32(pin_value));
            %int set_iopin_value(uint32_t pin_id, uint32_t pin_value)
        end
        % 1.1.0 / 1.0.0 
        function status = get_iopin_value(this, X4M_instance,pin_id,pin_valuePtr) 
            % Read IO pin level or value.
            status = calllib(this.lib_name,'nva_get_iopin_value',X4M_instance,uint32(pin_id), pin_valuePtr);
            %int get_iopin_value(uint32_t pin_id, uint32_t * pin_value)
        end
        % 1.0.0 / 1.0.0 
        function int = peek_message_baseband_ap(this, X4M_instance)
            % Return number of messages available.
            int = calllib(this.lib_name,'nva_peek_message_baseband_ap',X4M_instance);
            %int peek_message_baseband_ap()
        end
        % 1.0.0 / 1.0.0
        % FIX: Verify max_length argument
        function status = read_message_baseband_ap(this, X4M_instance,frame_counterPtr, num_binsPtr, bin_lengthPtr, sample_frequencyPtr, carrier_frequencyPtr, range_offsetPtr, amplitudePtr, phasePtr, max_length)
            status = calllib(this.lib_name,'nva_read_message_baseband_ap',X4M_instance,frame_counterPtr, num_binsPtr, bin_lengthPtr, sample_frequencyPtr, carrier_frequencyPtr, range_offsetPtr, amplitudePtr, phasePtr, uint32(max_length));
            %int read_message_baseband_ap(uint32_t * counter, uint32_t * numOfBins, float * binLength, float * samplingFrequency, float * carrierFrequency, float * rangeOffset, float * dataAmplitude, float * dataPhase)
        end
        % 1.0.0 / 1.0.0
        function int = peek_message_baseband_iq(this, X4M_instance)
            % Return number of messages available.
            int = calllib(this.lib_name,'nva_peek_message_baseband_iq',X4M_instance);
            %int peek_message_baseband_iq()
        end
        % 1.0.0 / 1.0.0
        % FIX: Verify max_length argument
        function status = read_message_baseband_iq(this, X4M_instance,frame_counterPtr, num_binsPtr, bin_lengthPtr, sample_frequencyPtr,carrier_frequencyPtr, range_offsetPtr, i_dataPtr, q_dataPtr, max_length)
            %int = this.get_baseband_iq_data(this, x2_instance, name,frame_counterPtr, num_binsPtr, bin_lengthPtr, sample_frequencyPtr,carrier_frequencyPtr, range_offsetPtr, i_dataPtr, q_dataPtr, max_length);
            status = calllib(this.lib_name,'nva_read_message_baseband_iq',X4M_instance,frame_counterPtr, num_binsPtr, bin_lengthPtr, sample_frequencyPtr,carrier_frequencyPtr, range_offsetPtr, i_dataPtr, q_dataPtr, max_length);
            % int read_message_baseband_iq(uint32_t * counter, uint32_t * numOfBins, float * binLength, float * samplingFrequency, float * carrierFrequency, float * rangeOffset, float * signalI, float* signalQ)
        end
        %     % TODO: TBD  1.1.0 / 1.0.0
        %     function int = read_message_pulsedoppler(this, X4M_instance)
        %         int read_message_pulsedoppler(TBD)
        %     end
        % 1.1.0 / 1.1.0
        function int = peek_message_iopin(this, X4M_instance)
            % Return number of messages available.
            int = calllib(this.lib_name,'nva_peek_message_iopin',X4M_instance);
            %int peek_message_iopin()
        end
        
        %   % 1.1.0 / 1.1.0 
        %   % FIX: Should there be any pointer arguments here?
        %   function int = read_message_iopin(this, X4_interface)
        %        %int read_message_iopin(uint8_t pin_id, uint32_t pin_feature, uint32_t value)
        %   end
        
        
        %===================================================================================
        %   X4M200 interface API 
        %===================================================================================

        % 1.0.0
        function int = peek_message_respiration_legacy(this, X4M200_instance)
            int = calllib(this.lib_name,'nva_peek_message_respiration_legacy',X4M200_instance);
        end
        % 1.0.0
        function int = read_message_respiration_legacy(this, X4M200_instance, counterPtr, sensor_statePtr, respiration_ratePtr, distancePtr, movementPtr, signal_qualityPtr)
            int = calllib(this.lib_name,'nva_read_message_respiration_legacy',X4M200_instance, counterPtr, sensor_statePtr, respiration_ratePtr, distancePtr, movementPtr, signal_qualityPtr);
        end
        % 1.0.0
        function int = peek_message_respiration_sleep(this, X4M200_instance)
            int = calllib(this.lib_name,'nva_peek_message_respiration_sleep',X4M200_instance);
        end
        % 1.0.0
        function status = read_message_respiration_sleep(this, X4M200_instance, frame_counterPtr, sensor_statePtr, respiration_ratePtr, distancePtr, signal_qualityPtr,movement_slowPtr, movement_fastPtr)
            status = calllib(this.lib_name,'nva_read_message_respiration_sleep', X4M200_instance,frame_counterPtr, sensor_statePtr, respiration_ratePtr, distancePtr, signal_qualityPtr,movement_slowPtr, movement_fastPtr);
        end
        % 1.0.0 
        function int = peek_message_respiration_movinglist(this, X4M200_instance)
            int = calllib(this.lib_name,'nva_peek_message_respiration_movinglist', X4M200_instance);
        end
        % 1.0.0
        function status = read_message_respiration_movinglist(this, X4M200_instance, counterPtr, movementIntervalCountPtr, movementSlowItemPtr, movementFastItemPtr, maxLength)
            status = calllib(this.lib_name,'nva_read_message_respiration_movinglist', X4M200_instance, counterPtr, movementIntervalCountPtr, movementSlowItemPtr, movementFastItemPtr, uint32(maxLength));
        end
        % 1.0.0 
        function int = peek_message_respiration_detectionlist(this, X4M200_instance)
            int = calllib(this.lib_name,'nva_peek_message_respiration_detectionlist', X4M200_instance);
        end
        % 1.0.0
        function status = read_message_respiration_detectionlist(this, X4M200_instance, counterPtr, detectionCntPtr, detectionDistPtr, detectionRCSPtr, detectionVelocityPtr, maxLength)
            status = calllib(this.lib_name,'nva_read_message_respiration_detectionlist', X4M200_instance, counterPtr, detectionCntPtr, detectionDistPtr, detectionRCSPtr, detectionVelocityPtr, uint32(maxLength));
        end
        % 1.0.0
        function int = peek_message_respiration_normalizedmovementlist(this, X4M200_instance)
            int = calllib(this.lib_name,'nva_peek_message_respiration_normalizedmovementlist', X4M200_instance);
        end
        % 1.0.0
        function status = read_message_respiration_normalizedmovementlist(this, X4M200_instance, ctrPtr, startPtr, binLengthPtr, countPtr, normMoveSlowPtr, normMoveFastPtr, maxLength)
            status = calllib(this.lib_name,'nva_read_message_respiration_normalizedmovementlist', X4M200_instance, ctrPtr, startPtr, binLengthPtr, countPtr, normMoveSlowPtr, normMoveFastPtr, uint32(maxLength));
        end
        % 1.0.0
        function status = peek_message_vital_signs(this, X4M200_instance)
            status = calllib(this.lib_name,'nva_peek_message_vital_signs', X4M200_instance);
        end
        % 1.0.0
        function status = read_message_vital_signs(this, X4M200_instance, counterPtr, statePtr, respRatePtr, respDistPtr, respConfPtr, heartRatePtr, heartDistPtr, heartConfPtr, normMoveSlowPtr, normMoveFastPtr, normMoveStartPtr, normMoveStopPtr)
            status = calllib(this.lib_name,'nva_read_message_vital_signs', X4M200_instance, counterPtr, statePtr, respRatePtr, respDistPtr, respConfPtr, heartRatePtr, heartDistPtr, heartConfPtr, normMoveSlowPtr, normMoveFastPtr, normMoveStartPtr, normMoveStopPtr);
        end
        
        %===================================================================================
        %   X4M300 interface API 
        %===================================================================================
        
        % x.0.0
%         function status = subscribe_to_presence_single(this, X4M300_instance)
%             status = calllib(this.lib_name,'nva_subscribe_to_presence_single', X4M300_instance);
%         end
        % 1.0.0
        function int = peek_message_presence_single(this, X4M300_instance)
            int = calllib(this.lib_name,'nva_peek_message_presence_single', X4M300_instance);
        end
        % 1.0.0
        function status = read_message_presence_single(this, X4M300_instance, frame_counterPtr, sensor_statePtr, distancePtr, directionPtr, signal_qualityPtr)
            status = calllib(this.lib_name,'nva_read_message_presence_single', X4M300_instance, frame_counterPtr, sensor_statePtr, distancePtr, directionPtr, signal_qualityPtr);
        end
        % x.0.0
%         function status = subscribe_to_presence_movinglist(this, X4M300_instance)
%             status = calllib(this.lib_name,'nva_subscribe_to_presence_movinglist', X4M300_instance);
%         end        
        % 1.0.0 
        function int = peek_message_presence_movinglist(this, X4M300_instance)
            % Return number of messages available.
            int = calllib(this.lib_name,'nva_peek_message_presence_movinglist', X4M300_instance);
        end
        % 1.0.0
        function status = read_message_presence_movinglist(this, X4M300_instance, counterPtr, sensor_statePtr, movementIntervalCountPtr, detectionCountPtr, movementSlowItemPtr, movementFastItemPtr, detectionDistancePtr, detectionRadarCrossSectionPtr, detectionVelocityPtr, maxLength)
            status = calllib(this.lib_name,'nva_read_message_presence_movinglist', X4M300_instance, counterPtr, sensor_statePtr, movementIntervalCountPtr, detectionCountPtr, movementSlowItemPtr, movementFastItemPtr, detectionDistancePtr, detectionRadarCrossSectionPtr, detectionVelocityPtr, uint32(maxLength));
        end
        % TODO: TBD
        function int = peek_message_presence_staticlist(this, X4M300_instance)
            int = calllib(this.lib_name,'nva_peek_message_presence_staticlist', X4M300_instance);
            %int peek_message_presence_staticlist()
        end
        % TODO: TBD
        function status = read_message_presence_staticlist(this, X4M300_instance,counterPtr,presenceStatePtr,staticCountPtr,staticDistancePtr,staticRadarCrossSectionPtr)
            status = calllib(this.lib_name,'nva_read_message_presence_staticlist', X4M300_instance,counterPtr,presenceStatePtr,staticCountPtr,staticDistancePtr,staticRadarCrossSectionPtr);
            %int read_message_presence_staticlist(uint32_t * counter, uint32_t * presenceState, uint32_t * staticCount, float * staticDistance, float * staticRadarCrossSection)
        end


        function int = peek_message_pulsedoppler_float( this, inst )
            int = calllib(this.lib_name,'nva_peek_message_pulsedoppler_float', inst);
        end

        function status = read_message_pulsedoppler_float(this, inst, frameCounterPtr, matrixCounterPtr, rangeIdxPtr, rangeBinsPtr, frequencyCountPtr, pulsedopplerInstancePtr, fpsPtr, fpsDecimPtr, frequencyStartPtr, frequencyStepPtr, rangePtr, floatDataPtr, maxLen)
            status = calllib(this.lib_name, ...
                'nva_read_message_pulsedoppler_float_data', inst, ...
                frameCounterPtr, matrixCounterPtr, rangeIdxPtr, ...
                rangeBinsPtr, frequencyCountPtr, pulsedopplerInstancePtr, ...
                fpsPtr, fpsDecimPtr, frequencyStartPtr, frequencyStepPtr, ...
                rangePtr, floatDataPtr, maxLen);
        end

        function int = peek_message_pulsedoppler_byte( this, inst )
            int = calllib(this.lib_name,'nva_peek_message_pulsedoppler_byte', inst);
        end

        function status = read_message_pulsedoppler_byte(this, inst, frameCounterPtr, matrixCounterPtr, rangeIdxPtr, rangeBinsPtr, frequencyCountPtr, pulsedopplerInstancePtr, byteStepStartPtr, byteStepSizePtr, fpsPtr, fpsDecimPtr, frequencyStartPtr, frequencyStepPtr, rangePtr, byteDataPtr, maxLen)
            status = calllib(this.lib_name, ...
                'nva_read_message_pulsedoppler_byte_data', inst, ...
                frameCounterPtr, matrixCounterPtr, rangeIdxPtr, ...
                rangeBinsPtr, frequencyCountPtr, pulsedopplerInstancePtr, ...
                byteStepStartPtr, byteStepSizePtr, fpsPtr, fpsDecimPtr, ...
                frequencyStartPtr, frequencyStepPtr, rangePtr, byteDataPtr, ...
                maxLen);
        end

        function int = peek_message_noisemap_float( this, inst )
            int = calllib(this.lib_name,'nva_peek_message_noisemap_float', inst);
        end

        function status = read_message_noisemap_float(this, inst, frameCounterPtr, matrixCounterPtr, rangeIdxPtr, rangeBinsPtr, frequencyCountPtr, pulsedopplerInstancePtr, fpsPtr, fpsDecimPtr, frequencyStartPtr, frequencyStepPtr, rangePtr, floatDataPtr, maxLen)
            status = calllib(this.lib_name, ...
                'nva_read_message_noisemap_float_data', inst, ...
                frameCounterPtr, matrixCounterPtr, rangeIdxPtr, ...
                rangeBinsPtr, frequencyCountPtr, pulsedopplerInstancePtr, ...
                fpsPtr, fpsDecimPtr, frequencyStartPtr, frequencyStepPtr, ...
                rangePtr, floatDataPtr, maxLen);
        end

        function int = peek_message_noisemap_byte( this, inst )
            int = calllib(this.lib_name,'nva_peek_message_noisemap_byte', inst);
        end

        function status = read_message_noisemap_byte(this, inst, frameCounterPtr, matrixCounterPtr, rangeIdxPtr, rangeBinsPtr, frequencyCountPtr, pulsedopplerInstancePtr, byteStepStartPtr, byteStepSizePtr, fpsPtr, fpsDecimPtr, frequencyStartPtr, frequencyStepPtr, rangePtr, byteDataPtr, maxLen)
            status = calllib(this.lib_name, ...
                'nva_read_message_noisemap_byte_data', inst, ...
                frameCounterPtr, matrixCounterPtr, rangeIdxPtr, ...
                rangeBinsPtr, frequencyCountPtr, pulsedopplerInstancePtr, ...
                byteStepStartPtr, byteStepSizePtr, fpsPtr, fpsDecimPtr, ...
                frequencyStartPtr, frequencyStepPtr, rangePtr, byteDataPtr, ...
                maxLen);
        end
    end
    
end

