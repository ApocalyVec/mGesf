classdef DataRecorderInterface
    %DataRecorderInterface is a layer one MATLAB wrapper for the ModuleConnector
    % DataRecorder interface.
    %
    % Example
    %   Lib = ModuleConnector.Library
    %   
    %   mc = ModuleConector.ModuleConnector(COMPORT);
    %   
    
    properties
        lib_name 	% Used to identify the C/C++ library
    end
    
    properties (Constant)
        % Recording data types
        DataType_BasebandApDataType = uint32(2^0); % = 1 << 0
        DataType_BasebandIqDataType = uint32(2^1); % = 1 << 1,
        DataType_SleepDataType = uint32(2^2); % = 1 << 2,
        DataType_RespirationDataType = uint32(2^3); % = 1 << 3,
        DataType_PerformanceStatusType = uint32(2^4); % = 1 << 4,
        DataType_StringDataType = uint32(2^5); % = 1 << 5,
        DataType_PulseDopplerFloatDataType = uint32(2^6); % = 1 << 6,
        DataType_PulseDopplerByteDataType = uint32(2^7); % = 1 << 7,
        DataType_NoiseMapFloatDataType = uint32(2^8); % = 1 << 8,
        DataType_NoiseMapByteDataType = uint32(2^9); % = 1 << 9,
        DataType_FloatDataType = uint32(2^10); % = 1 << 10,
        DataType_ByteDataType = uint32(2^11); % = 1 << 11,
        DataType_PresenceSingleDataType = uint32(2^12); % = 1 << 12,
        DataType_PresenceMovingListDataType = uint32(2^13); % = 1 << 13,
        
        DataType_InvalidDataType = uint32(0);
        DataType_AllDataTypes = uint32(2^32); % = 0xffffffff,
    end
    
    properties (Constant)
                
    end
    
    methods
        %% Constructor        
        function dri = DataRecorderInterface(lib_name)
            dri.lib_name = lib_name;
        end
        
        %%===================================================================================
        %   DataRecorder interface API 
        %===================================================================================
        
        function recordingOptions_instance = create_recording_options(this)
            recordingOptions_instance = calllib(this.lib_name,'nva_create_recording_options');
            assert(~recordingOptions_instance.isNull, 'nva_create_recording_options failed, check the logs');
        end
        
        function recordingSplitSize_instance = create_recording_split_size(this)
            recordingSplitSize_instance = calllib(this.lib_name,'nva_create_recording_split_size');
            assert(~recordingSplitSize_instance.isNull, 'nva_create_recording_split_size failed, check the logs');
        end
        %
        %% Recording split size.
        function set_duration(this, recordingSplitSize_instance, duration)
            calllib(this.lib_name, 'set_duration', recordingSplitSize_instance, int32(duration));
        end
        %
        function set_byte_count(this, recordingSplitSize_instance, count)
            calllib(this.lib_name, 'set_byte_count', recordingSplitSize_instance, int64(count));
        end
        %
        function set_fixed_daily_hour(this, recordingSplitSize_instance, hour)
            calllib(this.lib_name, 'set_fixed_daily_hour', recordingSplitSize_instance, int32(hour));
        end
        %
        %% Recording options.
        function set_session_id(this, recordingOptions_instance, id, lengthOfId)
            calllib(this.lib_name, 'set_session_id', recordingOptions_instance, id, uint32(lengthOfId));
        end
        %
        function status = get_session_id(this, recordingOptions_instance, resultPtr, lengthPtr, maxLength)
            [status,~,result] = calllib(this.lib_name, 'get_session_id', recordingOptions_instance, resultPtr, lengthPtr, uint32(maxLength));
            assert(status==0,'get_session_id:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
            resultPtr.Value = result;
        end
        %
        function set_file_split_size(this, recordingOptions_instance, recordingSplitSize_instance)
            calllib(this.lib_name, 'set_file_split_size', recordingOptions_instance, recordingSplitSize_instance);
        end
        %
        function set_directory_split_size(this, recordingOptions_instance, recordingSplitSize_instance)
            calllib(this.lib_name, 'set_directory_split_size', recordingOptions_instance, recordingSplitSize_instance);
        end
        %
        function set_data_rate_limit(this, recordingOptions_instance, limit)
            calllib(this.lib_name, 'set_data_rate_limit', recordingOptions_instance, int32(limit));
        end
        %
        function set_user_header(this, recordingOptions_instance, header, lengthOfHeader)
            calllib(this.lib_name, 'set_user_header', recordingOptions_instance, header, uint32(lengthOfHeader));
        end
        %
        %% Data recorder.
        function status = start_recording( this, dataRecorder_instance, xethruDataType, directory, lengthOfDir, recordingOptions_instance)
            status = calllib(this.lib_name, 'nva_start_recording', dataRecorder_instance,...
                uint32(xethruDataType), directory, uint32(lengthOfDir), recordingOptions_instance);
            assert(status==0,'start_recording:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function stop_recording( this, dataRecorder_instance, xethruDataType)
            calllib(this.lib_name, 'nva_stop_recording', dataRecorder_instance, uint32(xethruDataType));
        end
        %
        function status = process( this, dataRecorder_instance, xethruDataType, bytes, lengthOfBytes)
            status = calllib(this.lib_name, 'nva_process', dataRecorder_instance,...
                uint32(xethruDataType), bytes, uint32(lengthOfBytes));
            assert(status==0,'process:libraryStatusFailed',strcat('Call returns status=',num2str(status)));
        end
        %
        function set_basename_for_data_type(this,dataRecorder_instance, data_type, name, lengthOfName)
            calllib(this.lib_name, 'nva_set_basename_for_data_type', dataRecorder_instance, uint32(data_type), name, uint32(lengthOfName));
        end
        %
        function status = get_basename_for_data_type(this, dataRecorder_instance, data_type, dstPtr, lengthPtr, maxLength)
            [status,~,dst] = calllib(this.lib_name, 'nva_get_basename_for_data_type', dataRecorder_instance, uint32(data_type), dstPtr, lengthPtr, uint32(maxLength));
            assert(status==0,'get_basename_for_data_type:libraryStatusFailed',strcat('Call returns status=',num2str(status)));            
            dstPtr.Value = dst(1:lengthPtr.Value);
        end       
        %
        function clear_basename_for_data_types(this, dataRecorder_instance, data_type)
            calllib(this.lib_name, 'nva_clear_basename_for_data_types', dataRecorder_instance, uint32(data_type));
        end
        %
        %%===================================================================================
        %   DataReader interface API 
        %===================================================================================
        
        function status = data_reader_open(this, dataReader_instance, meta_filename, length_of_meta_filename, depth)
            status = calllib(this.lib_name, 'data_reader_open', dataReader_instance, meta_filename, uint32(length_of_meta_filename), int32(depth));
        end
        %
        function data_reader_close(this, dataReader_instance)
            calllib(this.lib_name, 'data_reader_close', dataReader_instance);
        end
        %
        function ret = data_reader_is_open(this, dataReader_instance)
            ret = calllib(this.lib_name, 'data_reader_is_open', dataReader_instance);
        end
        %
        function ret = data_reader_at_end(this, dataReader_instance)
            ret = calllib(this.lib_name, 'data_reader_at_end', dataReader_instance);
        end
        %
        function status = data_reader_read_record(this, dataReader_instance, dataPtr, maxSize, sizePtr, dataTypePtr, epochPtr, isUserHeaderPtr)
            status = calllib(this.lib_name, 'data_reader_read_record', dataReader_instance, dataPtr, sizePtr, uint32(maxSize), dataTypePtr, epochPtr, isUserHeaderPtr);
        end
        %
        function status = data_reader_peek_record(this, dataReader_instance, dataPtr, maxSize, sizePtr, dataTypePtr, epochPtr, isUserHeaderPtr)
            status = calllib(this.lib_name, 'data_reader_peek_record', dataReader_instance, dataPtr, sizePtr, uint32(maxSize), dataTypePtr, epochPtr, isUserHeaderPtr);
        end
        %
        function status = data_reader_seek_ms(this, dataReader_instance, position)
            status = calllib(this.lib_name, 'data_reader_seek_ms', dataReader_instance, int64(position));
        end
        %
        function status = data_reader_seek_byte(this, dataReader_instance, position)
            status = calllib(this.lib_name, 'data_reader_seek_byte', dataReader_instance, int64(position));
        end
        %
        function status = data_reader_set_filter(this, dataReader_instance, data_type)
            status = calllib(this.lib_name, 'data_reader_set_filter', dataReader_instance, uint32(data_type));
        end
        %
        function status = data_reader_get_filter(this, dataReader_instance, dataTypesPtr)
            status = calllib(this.lib_name, 'data_reader_get_filter', dataReader_instance, dataTypesPtr);
        end
        %
        function status = data_reader_get_start_epoch(this, dataReader_instance, startEpochPtr)
            status = calllib(this.lib_name, 'data_reader_get_start_epoch', dataReader_instance, startEpochPtr);
        end
        %
        function status = data_reader_get_duration(this, dataReader_instance, durPtr)
            status = calllib(this.lib_name, 'data_reader_get_duration', dataReader_instance, durPtr);
        end
        %
        function status = data_reader_get_size(this, dataReader_instance, sizePtr)
            status = calllib(this.lib_name, 'data_reader_get_size', dataReader_instance, sizePtr);
        end
        %
        function status = data_reader_get_data_types(this, dataReader_instance, dataTypesPtr)
            status = calllib(this.lib_name, 'data_reader_get_data_types', dataReader_instance, dataTypesPtr);
        end
        %
        function status = data_reader_get_max_record_size(this, dataReader_instance, maxRecSizePtr)
            status = calllib(this.lib_name, 'data_reader_get_max_record_size', dataReader_instance, maxRecSizePtr);
        end
        %
        function status = data_reader_get_session_id(this, dataReader_instance, sessionIdPtr, sizePtr, maxLength)
            status = calllib(this.lib_name, 'data_reader_get_session_id', dataReader_instance, sessionIdPtr, sizePtr, uint32(maxLength));
        end
        %
        %%===================================================================================
        %   DataPlayer interface API 
        %===================================================================================
        
        function status = data_player_play(this, dataPlayer_instance)
            status = calllib(this.lib_name, 'data_player_play', dataPlayer_instance);
        end
        %
        function status = data_player_pause(this, dataPlayer_instance)
            status = calllib(this.lib_name, 'data_player_pause', dataPlayer_instance);
        end
        %
        function status = data_player_stop(this, dataPlayer_instance)
            status = calllib(this.lib_name, 'data_player_stop', dataPlayer_instance);
        end
        %
        function status = data_player_set_state(this, dataPlayer_instance, state)
            status = calllib(this.lib_name, 'data_player_set_state', dataPlayer_instance, uint32(state));
        end
        %
        function status = data_player_get_state(this, dataPlayer_instance, statePtr)
            status = calllib(this.lib_name, 'data_player_get_state', dataPlayer_instance, statePtr);
        end
        %
        function status = data_player_set_filter(this, dataPlayer_instance, data_types)
            status = calllib(this.lib_name, 'data_player_set_filter', dataPlayer_instance, uint32(data_types));
        end
        %
        function status = data_player_get_filter(this, dataPlayer_instance, data_typesPtr)
            status = calllib(this.lib_name, 'data_player_get_filter', dataPlayer_instance, data_typesPtr);
        end
        %
        function status = data_player_set_position(this, dataPlayer_instance, position)
            status = calllib(this.lib_name, 'data_player_set_position', dataPlayer_instance, int64(position));
        end
        %
        function status = data_player_get_position(this, dataPlayer_instance, positionPtr)
            status = calllib(this.lib_name, 'data_player_get_position', dataPlayer_instance, positionPtr);
        end
        %
        function status = data_player_set_playback_rate(this, dataPlayer_instance, rate)
            status = calllib(this.lib_name, 'data_player_set_playback_rate', dataPlayer_instance, single(rate));
        end
        %
        function status = data_player_get_playback_rate(this, dataPlayer_instance, ratePtr)
            status = calllib(this.lib_name, 'data_player_get_playback_rate', dataPlayer_instance, ratePtr);
        end
        %
        function status = data_player_set_loop_mode_enabled(this, dataPlayer_instance, enabled)
            status = calllib(this.lib_name, 'data_player_set_loop_mode_enabled', dataPlayer_instance, uint32(enabled));
        end
        %
        function status = data_player_get_loop_mode_enabled(this, dataPlayer_instance, enabledPtr)
            status = calllib(this.lib_name, 'data_player_get_loop_mode_enabled', dataPlayer_instance, enabledPtr);
        end
        %
        function status = data_player_get_duration(this, dataPlayer_instance, durationPtr)
            status = calllib(this.lib_name, 'data_player_get_duration', dataPlayer_instance, durationPtr);
        end
        %
        function status = data_player_get_meta_filename(this, dataPlayer_instance, meta_filenamePtr, lengthPtr, maxLength)
            [status,~,metafilename] = calllib(this.lib_name, 'data_player_get_meta_filename', dataPlayer_instance, meta_filenamePtr, lengthPtr, uint32(maxLength));
            meta_filenamePtr.Value = metafilename(1:lengthPtr.Value);
        end
        
    end
    
end