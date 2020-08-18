classdef DataRecorder
    % The DataRecorder class allows recording of xethru data types.
    %
    % The DataRecorder class is a high level recorder class. It can be used to record all data types
    % sent by a xethru device over serial port or similar. All low-level I/O is handled by the recorder
    % itself with no setup required. Data is stored on disk as specified by the XeThru File Formats document
    % and can be easily read back using the DataReader class.
    
    properties
        lib_name = 'libModuleConnector';
    end
    
    properties (SetAccess = private)
    	dataRecorder_instance;  % Libpointer to C/C++ library instance
        dataRecorderInterface   % Layer one wrapper class for recording API
         
    	recordingOptions_instance   % Libpointer to C/C++ library instance
        
        recordingDirectorySplitSize_instance     % Libpointer to C/C++ library instance
        recordingFileSplitSize_instance     % Libpointer to C/C++ library instance
    end
    
    methods
        
        %% Methods for data recording
        function status = start_recording(this,data_type,output_dir)
            %START_RECORDING Starts recording the specified data type(s).
            %
            % Recorded files are output to the directory specified.
            % This command does not enable data output on the module.
            status = this.dataRecorderInterface.start_recording(this.dataRecorder_instance, data_type, output_dir, length(output_dir), this.recordingOptions_instance);
        end
        
        function stop_recording(this, data_type)
            %STOP_RECORDING Stops recording the specified data type(s).
            % This command does not disable data output on the module.
            this.dataRecorderInterface.stop_recording(this.dataRecorder_instance, data_type);
        end
        
        function status = process(this, data_type, bytes)
            %PROCESS Process the specified data for a given data type.
            %
            % This function should not be used when recording data from a physical xethru device. It is only relevant
            % for recording data generated elsewhere. Most users will not need this functionality.
            status = this.dataRecorderInterface.process(this.dataRecorder_instance, data_type, bytes, length(bytes));
        end
        
        function set_basename_for_data_type(this, data_type, name)
            %SET_BASENAME_FOR_DATA_TYPE Sets the basename for the specified data type.
            %
            % Most users will not need this functionality.
            this.dataRecorderInterface.set_basename_for_data_type(this.dataRecorder_instance, data_type, name, length(name));
        end
        
        function [dst,status] = get_basename_for_data_type(this, data_type)
            %GET_BASENAME_FOR_DATA_TYPE Gets the basename for the specified data type.
            %
            % Most users will not need this functionality.
            maxLength = 100;
            dstPtr = libpointer('string',blanks(maxLength));
            lengthPtr = libpointer('uint32Ptr',0);
            status = this.dataRecorderInterface.get_basename_for_data_type(this.dataRecorder_instance, data_type, dstPtr, lengthPtr, maxLength);
            dst = dstPtr.Value;
        end
        
        function clear_basename_for_data_types(this, data_type)
            %CLEAR_BASENAME_FOR_DATA_TYPES Resets the basename(s) to default value(s) for the specified data type(s).
            %
            % Most users will not need this functionality.
            this.dataRecorderInterface.clear_basename_for_data_types(this.dataRecorder_instance, data_type);
        end
        
        %% Methods for setting file/directory split intervals.        
        function set_file_split_duration(this, duration)
            %SET_FILE_SPLIT_DURATION Sets the preferred file split size specified in seconds.            
            this.dataRecorderInterface.set_duration(this.recordingFileSplitSize_instance, duration);
            this.dataRecorderInterface.set_file_split_size(this.recordingOptions_instance,this.recordingFileSplitSize_instance);
        end
        
        function set_directory_split_duration(this, duration)
            %SET_DIRECTORY_SPLIT_DURATION Sets the preferred directory split size specified in seconds.
            this.dataRecorderInterface.set_duration(this.recordingDirectorySplitSize_instance, duration);
            this.dataRecorderInterface.set_directory_split_size(this.recordingOptions_instance,this.recordingDirectorySplitSize_instance);
        end
        
        function set_file_split_byte_count(this, count)
            %SET_FILE_SPLIT_BYTE_COUNT Sets the preferred file split size specified as number of bytes.
            this.dataRecorderInterface.set_byte_count(this.recordingFileSplitSize_instance, count);
            this.dataRecorderInterface.set_file_split_size(this.recordingOptions_instance,this.recordingFileSplitSize_instance);
        end
        
        function set_directory_split_byte_count(this, count)
            %SET_DIRECTORY_SPLIT_BYTE_COUNT Sets the preferred directory split size specified as number of bytes.
            this.dataRecorderInterface.set_byte_count(this.recordingDirectorySplitSize_instance, count);
            this.dataRecorderInterface.set_directory_split_size(this.recordingOptions_instance,this.recordingDirectorySplitSize_instance);
        end
        
        function set_file_split_fixed_daily_hour(this, hour)
            %SET_FILE_SPLIT_FIXED_DAILY_HOUR Sets the preferred file split size to a fixed daily hour.
            %
            % Range is 24 hour clock (0-23).
            % For example, if splitting every day at 2pm is desired, the parameter to this function
            % would be 14. By default, no fixed daily hour is set.
            this.dataRecorderInterface.set_fixed_daily_hour(this.recordingFileSplitSize_instance, hour);
            this.dataRecorderInterface.set_file_split_size(this.recordingOptions_instance,this.recordingFileSplitSize_instance);
        end
        
        function set_directory_split_fixed_daily_hour(this, hour)
            %SET_DIRECTORY_SPLIT_FIXED_DAILY_HOUR Sets the preferred directory split size to a fixed daily hour.
            %
            % Range is 24 hour clock (0-23).
            % For example, if splitting every day at 2pm is desired, the parameter to this function
            % would be 14. By default, no fixed daily hour is set.
            this.dataRecorderInterface.set_fixed_daily_hour(this.recordingDirectorySplitSize_instance, hour);
            this.dataRecorderInterface.set_directory_split_size(this.recordingOptions_instance,this.recordingDirectorySplitSize_instance);
        end
        
        %% Methods for setting recording options.        
        function set_session_id(this, id)
            %SET_SESSION_ID Sets the session id as specified.
            %
            % Overrides the default constructed value which
            % is an universally unique identifier (UUID).
            this.dataRecorderInterface.set_session_id(this.recordingOptions_instance, id, length(id));
            this.dataRecorderInterface.set_directory_split_size(this.recordingOptions_instance,this.recordingDirectorySplitSize_instance);
        end
                
        function [id, status] = get_session_id(this)
            %GET_SESSION_ID By default, this function returns an universally unique identifier (UUID) if no custom id is set.
            lengthPtr = libpointer('uint32Ptr',0);
            maxLength = 100;
            resultPtr = libpointer('string',blanks(maxLength));
            status = this.dataRecorderInterface.get_session_id(this.recordingOptions_instance, resultPtr, lengthPtr, maxLength);
            id = resultPtr.Value;
            length = lengthPtr.Value;
            id = id(1:length);
            clear lengthPtr resultPtr;
        end
        
        function set_data_rate_limit(this, limit)
            %SET_DATA_RATE_LIMIT Sets the data rate (ms) the recorder will read data from the module.
            %
            % A value of 1000 ms means data is read every second. Data in between is discarded.
            % By default, this parameter is -1 (no data rate limit).
            this.dataRecorderInterface.set_data_rate_limit(this.recordingOptions_instance, limit);
        end
        
        function set_user_header(this, header)
            %SET_USER_HEADER Sets a custom header applied to the beginning of the recorded file.
            %
            % By default, this parameter is Bytes() (no custom header).
            this.dataRecorderInterface.set_user_header(this.recordingOptions_instance, header, length(header));
        end
        

    end
    
    methods
        
        %% Constructor
        function dr = DataRecorder(lib_name,mc)
            % Constructor
            
            if nargin > 0, dr.lib_name = lib_name; end
            
            % Get DataRecorderInterface layer one wrapper class.
            dr.dataRecorderInterface = ModuleConnector.DataRecorderInterface(dr.lib_name);
            
            % Create DataRecorder instance.
            % Check mode of operation: standalone or via MC.
            if nargin < 2
                % Call create method without MC argument.
                dr.dataRecorder_instance = calllib(dr.lib_name,'nva_create_data_recorder');
            else
                dr.lib_name = mc.lib_name;
                % Else, call get method with MC argument to record from module.
                dr.dataRecorder_instance = calllib(dr.lib_name,'nva_get_data_recorder',mc.mcInstance);
            end
            assert(~dr.dataRecorder_instance.isNull, 'nva_get_data_recorder failed, check the logs');
            
            % Create RecordingOptions instance.
            dr.recordingOptions_instance = dr.dataRecorderInterface.create_recording_options();
            % Create RecordingSplitSize instance for directory splitting.
            dr.recordingDirectorySplitSize_instance = dr.dataRecorderInterface.create_recording_split_size();
            % Create RecordingSplitSize instance for file splitting.            
            dr.recordingFileSplitSize_instance = dr.dataRecorderInterface.create_recording_split_size();
            
        end
            
        %% Destructor
        function delete( this )
            % Destructor
            
            % Destroy recording options.
            calllib(this.lib_name, 'nva_destroy_recording_options',this.recordingOptions_instance);
            clear('this.recordingOptions_instance');
            % Destroy recording split size.
            calllib(this.lib_name, 'nva_destroy_recording_split_size',this.recordingDirectorySplitSize_instance);
            calllib(this.lib_name, 'nva_destroy_recording_split_size',this.recordingFileSplitSize_instance);
            clear('this.recordingDirectorySplitSize_instance');
            clear('this.recordingFileSplitSize_instance');
            % Destroy data recorder.
            calllib(this.lib_name, 'nva_destroy_data_recorder',this.dataRecorder_instance);
            clear('this.dataRecorder_instance');
        end
        
    end
    
end

