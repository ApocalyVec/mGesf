classdef XEP
    %XEP provides a set of parameterized low level X4 commands
    % and responses. It is used by developers who want to do low level
    % access to the X4 device. Using this interface requires good knowledge
    % on the X4 device.
    %
    % See also MODULECONNECTOR, X2, X2M200,
    % TRANSPORT, X4M300
    
    properties (SetAccess = private)
        x2_instance      % libpointer to C/C++ library instance
        radarInterface; % Layer one wrapper class
    end
    
    properties (SetAccess = private,Hidden)
        dataFloatPtr    = libpointer('singlePtr',ones(1536,1)); % frame storage: singlePtr
        dataFloatLen = uint32(6400);
    end
    
    properties (SetAccess = private,Hidden)
        dataStringPtr    = libpointer('string',blanks(1536)); % frame storage: singlePtr
        dataStringLen = uint32(6400);
    end
    
    properties (SetAccess = private,Hidden)
        SI_maxLength = 100;
    end
    
    properties (SetAccess = private,Hidden)
        FAPI_maxLength = uint32(1e5);
        FAPI_maxItems = uint32(100);
        
        % File API buffer pointers        
        FAPI_filesPtr = libpointer('uint32Ptr',0);
        FAPI_lengthPtr = libpointer('uint32Ptr',0);
        FAPI_itemsPtr = libpointer('uint32Ptr',0);
        FAPI_fileTypeItemsPtr = libpointer('int32Ptr',zeros(100,1));
        FAPI_fileIdItemsPtr = libpointer('int32Ptr',zeros(100,1));
        FAPI_dataPtr = libpointer('uint8Ptr',zeros(1e5,1));

        pin_setup = libpointer('uint32Ptr',0);
        pin_feature = libpointer('uint32Ptr',0);
    end
    
    properties (Constant)
        % FileType enumerators
         XTFILE_TYPE_CRASHDUMP      = hex2dec('04554D50');
         XTFILE_TYPE_PARFILE        = hex2dec('037DD98C');
         XTFILE_TYPE_NOISEMAP_SLOW  = hex2dec('05F09BCA');
         XTFILE_TYPE_NOISEMAP_FAST  = hex2dec('05F09BCB');
         XTFILE_TYPE_USER_DEFINED   = hex2dec('80000000');
    end
    
    methods
        
        function [system_info,status] = get_system_info(this,info_code)
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
            if ischar(info_code)
                switch info_code
                    case 'XTS_SSIC_ITEMNUMBER'
                        info_code = uint8(0);
                    case 'XTS_SSIC_ORDERCODE'
                        info_code = uint8(1);
                    case 'XTS_SSIC_FIRMWAREID'
                        info_code = uint8(2);
                    case 'XTS_SSIC_VERSION'
                        info_code = uint8(3);
                    case 'XTS_SSIC_BUILD'
                        info_code = uint8(4);
                    case 'XTS_SSIC_SERIALNUMBER'
                        info_code = uint8(6);
                    case 'XTID_SSIC_VERSIONLIST'
                        info_code = uint8(7);
                    otherwise
                        error(['Unknown sensor info code ',info_code,...
                            '. Available modes are [XTS_SSIC_ITEMNUMBER,XTS_SSIC_ORDERCODE,XTS_SSIC_FIRMWAREID,XTS_SSIC_VERSION,XTS_SSIC_SERIALNUMBER,XTS_SSIC_BUILD,XTID_SSIC_VERSIONLIST]']);
                end
            elseif isnumeric(info_code)
                assert((info_code<8 && info_code~=5), 'No match for info code');
            else
                error('Unknown info code');
            end
            system_infoPtr = libpointer('string',blanks(this.SI_maxLength));
            status = this.radarInterface.get_system_info(this.x2_instance,info_code,system_infoPtr,100);
            system_info = strtrim(system_infoPtr.Value); clear system_infoPtr
        end
        
        function pongvalue = ping( this )
            %PING Send a ping message to the module. Responds with a pong value to indicate
            %   if the module is ready for further interaction or not.
            %   pongvalue = PING( this )
            %
            %   pongvalue   Indicates module readyness:
            %       - Ready: XTS_DEF_PONGVAL_READY (0xaaeeaeea)
            %       - Not ready: XTS_DEF_PONGVAL_NOTREADY (0xaeeaeeaa)
            pongvalue = this.radarInterface.ping( this.x2_instance );
        end
        
        function status = module_reset(this)
            %MODULE_RESET Resets and restart the module.
            status = this.radarInterface.module_reset( this.x2_instance );
        end
        
        function status = x4driver_set_fps( this, fps)
            %X4DRIVER_SET_FPS Sets frame rate for frame streaming.
            %   status = X4DRIVER_SET_FPS( this, fps)
            status = this.radarInterface.xep_x4driver_set_fps(this.x2_instance, fps );
        end
        
        function [fps,status] = x4driver_get_fps( this )
            %X4DRIVER_GET_FPS Gets configured fps.
            %   [fps,status] = X4DRIVER_GET_FPS( this )
            fpsPtr = libpointer('singlePtr',0);
            status = this.radarInterface.xep_x4driver_get_fps(this.x2_instance,fpsPtr );
            fps = double(fpsPtr.Value); clear fpsPtr
        end
        
        function status = x4driver_set_iterations( this, iterations)
            %X4DRIVER_SET_ITERATIONS Sets Iterations.
            %   status = X4DRIVER_SET_ITERATIONS( this, iterations)
            status = this.radarInterface.xep_x4driver_set_iterations(this.x2_instance, iterations );
        end
        
        function [iterations,status] = x4driver_get_iterations( this )
        %X4DRIVER_GET_ITERATIONS Sets Iterations.
        % [iterations,status]  = X4DRIVER_GET_ITERATIONS( this )
            itPtr = libpointer('uint32Ptr',0);
 		    status = this.radarInterface.xep_x4driver_get_iterations(this.x2_instance, itPtr );
            iterations = double(itPtr.Value); clear itPtr
         end
        
        function status = x4driver_set_pulsesperstep( this, pulsesperstep)
            %X4DRIVER_SET_PULSESPERSTEP Sets pulses per step.
            %   status = X4DRIVER_SET_PULSESPERSTEP( this, pulsesperstep)
            status = this.radarInterface.xep_x4driver_set_pulsesperstep(this.x2_instance, pulsesperstep );
        end
        function [pulsesperstep,status] = x4driver_get_pulsesperstep( this )
        %X4DRIVER_GET_PULSESPERSTEP Gets pulses per step.
        % [pulsesperstep,status] = X4DRIVER_GET_PULSESPERSTEP( this )
            ppsPtr = libpointer('uint32Ptr',0);
 		    status = this.radarInterface.xep_x4driver_get_pulsesperstep(this.x2_instance, ppsPtr );
            pulsesperstep = double(ppsPtr.Value); clear ppsPtr
         end
        
        function status = x4driver_set_downconversion( this, downconversion)
            %X4DRIVER_SET_DOWNCONVERSION Sets downconversion.
            %   status = X4DRIVER_SET_DOWNCONVERSION( this, downconversion)
            %   downconversion  0 = no downconversion, i.e. rf data.
            %                   1 = downconversion, i.e. bb data.
            status = this.radarInterface.xep_x4driver_set_downconversion(this.x2_instance, downconversion );
        end
        function [downconversion, status] = x4driver_get_downconversion( this)
            %X4DRIVER_GET_DOWNCONVERSION Gets downconversion.
            %   [downconversion, status]  = X4DRIVER_GET_DOWNCONVERSION( this)
            %   downconversion  0 = no downconversion, i.e. rf data.
            %                   1 = downconversion, i.e. bb data.
            itPtr = libpointer('uint8Ptr',0);
 		    status = this.radarInterface.xep_x4driver_get_downconversion(this.x2_instance, itPtr );
            downconversion = double(itPtr.Value); clear itPtr           
        end
        function [framebincount, status] = x4driver_get_framebincount( this )
        %X4DRIVER_GET_FRAMEBINCOUNT Gets number of bins.
        % [framebincount, status] = X4DRIVER_GET_FRAMEBINCOUNT( this )
            fbcPtr = libpointer('uint32Ptr',0);
 		    status = this.radarInterface.xep_x4driver_get_framebincount(this.x2_instance, fbcPtr );
            framebincount = double(fbcPtr.Value); clear fbcPtr
         end
               
        function status = x4driver_set_frame_area( this, start, stop)
            %X4DRIVER_SET_FRAME_AREA Set frame area zone assuming air as transmitter medium. Start and end in meter.
            %   status = X4DRIVER_SET_FRAME_AREA( this, start, stop)
            %
            %   See also X4DRIVER_GET_FRAME_AREA
            status = this.radarInterface.xep_x4driver_set_frame_area(this.x2_instance, start, stop );
        end
        
        function status = x4driver_init( this )
            %X4DRIVER_INIT Will make sure that enable is set, 8051 SRAM is programmed, ldos are enabled, and that the external oscillator has been enabled.
            %   status = X4DRIVER_INIT( this )
            status = this.radarInterface.xep_x4driver_init(this.x2_instance );
        end
        
        function status = x4driver_set_tx_center_frequency( this, frequency)
            %X4DRIVER_SET_TX_CENTER_FREQUENCY  Sets the frequency band used by the radar.
            %   status = X4DRIVER_SET_TX_CENTER_FREQUENCY( this, frequency)
            %   frequency_band = 3 : 6.0 to 8.5 Ghz range (default mode)
            %   frequency_band = 4 : 7.2 to 10.2 GHz range
            status = this.radarInterface.xep_x4driver_set_tx_center_frequency(this.x2_instance, frequency );
        end
        function [frequency, status] = x4driver_get_tx_center_frequency( this)
            %X4DRIVER_GET_TX_CENTER_FREQUENCY  Gets the frequency band used by the radar.
            %   [frequency, status]  = X4DRIVER_GET_TX_CENTER_FREQUENCY( this, frequency)
            %   frequency_band = 3 : 6.0 to 8.5 Ghz range (default mode)
            %   frequency_band = 4 : 7.2 to 10.2 GHz range
            itPtr = libpointer('uint8Ptr',0);
 		    status = this.radarInterface.xep_x4driver_get_tx_center_frequency(this.x2_instance, itPtr );
            frequency = double(itPtr.Value); clear itPtr           
        end
 
        function status = x4driver_set_dac_min( this, dac_min)
            %X4DRIVER_SET_DAC_MIN Sets dac min.
            %   status = X4DRIVER_SET_DAC_MIN( this, dac_min)
            status = this.radarInterface.xep_x4driver_set_dac_min(this.x2_instance, dac_min );
        end
        
        function [dac_min, status] = x4driver_get_dac_min( this )
        %X4DRIVER_GET_DAC_MIN Gets dac min.
        % [dac_min, status] = X4DRIVER_GET_DAC_MIN( this )
            itPtr = libpointer('uint32Ptr',0);
 		    status = this.radarInterface.xep_x4driver_get_dac_min(this.x2_instance, itPtr );
            dac_min = double(itPtr.Value); clear itPtr
         end
        
        function status = x4driver_set_dac_max( this, dac_max)
            %X4DRIVER_SET_DAC_MAX Sets dac max.
            %   status = X4DRIVER_SET_DAC_MAX( this, dac_max)
            status = this.radarInterface.xep_x4driver_set_dac_max(this.x2_instance, dac_max );
        end
        
        function [dac_max, status] = x4driver_get_dac_max( this )
        %X4DRIVER_GET_DAC_MAX Gets dac max.
        % [dac_max, status] = X4DRIVER_GET_DAC_MAX( this )
            itPtr = libpointer('uint32Ptr',0);
 		    status = this.radarInterface.xep_x4driver_get_dac_max(this.x2_instance, itPtr );
            dac_max = double(itPtr.Value); clear itPtr
         end
        
        function status = x4driver_set_frame_area_offset( this, offset)
            %X4DRIVER_SET_FRAME_AREA_OFFSET Offset to adjust frame area reference depending on module product.
            %   status = X4DRIVER_SET_FRAME_AREA_OFFSET( this, offset)
            %
            %   See also X4DRIVER_GET_FRAME_AREA_OFFSET
            status = this.radarInterface.xep_x4driver_set_frame_area_offset(this.x2_instance, offset );
        end
        
        function status = x4driver_set_enable( this, enable)
            %X4DRIVER_SET_ENABLE Set enable for X4 enable pin.
            %   status = X4DRIVER_SET_ENABLE( this, enable)
            status = this.radarInterface.xep_x4driver_set_enable(this.x2_instance, enable );
        end
        
        function status = x4driver_set_tx_power( this, tx_power)
            %X4DRIVER_SET_TX_POWER Set the radar transmitter power.
            %   status = X4DRIVER_SET_TX_POWER( this, tx_power)
            %   tx_power    0 = transmitter off. See datasheet for valid values.
            status = this.radarInterface.xep_x4driver_set_tx_power(this.x2_instance, tx_power );
        end
        function [tx_power, status] = x4driver_get_tx_power( this)
            %X4DRIVER_GET_TX_POWER Get the radar transmitter power.
            %   [tx_power, status] = X4DRIVER_GET_TX_POWER( this)
            %   tx_power    0 = transmitter off. See datasheet for valid values.
            tx_powerPtr = libpointer('uint8Ptr',0);
            status = this.radarInterface.xep_x4driver_get_tx_power(this.x2_instance,tx_powerPtr);
            tx_power = double(tx_powerPtr.Value); clear tx_powerPtr
        end
        
        function [frame_start, frame_stop, status] = x4driver_get_frame_area( this )
            %X4DRIVER_GET_FRAME_AREA Get frame area zone.
            %   [frame_start, frame_stop, status] = X4DRIVER_GET_FRAME_AREA( this )
            %	frame_start 	start of frame in meters
            % 	frame_stop 		end of frame in meters
            % 	status 			call status; 0 if success, error code otherwise
            %
            %   See also X4DRIVER_SET_FRAME_AREA
            startPtr = libpointer('singlePtr',0);
            stopPtr = libpointer('singlePtr',0);
            status = this.radarInterface.xep_x4driver_get_frame_area(this.x2_instance, startPtr, stopPtr);
            frame_start = double(startPtr.Value);
            frame_stop = double(stopPtr.Value);
            clear startPtr stopPtr
        end
        
        function [frame_area_offset, status] = x4driver_get_frame_area_offset( this )
            %X4DRIVER_GET_FRAME_AREA_OFFSET Get frame area offset
            %   [frame_area_offset, status] = X4DRIVER_GET_FRAME_AREA_OFFSET( this )
            % 	frame_area_offset 	distance to the start of the frame in meters
            % 	status 				call status; 0 if success, error code otherwise
            offsetPtr = libpointer('singlePtr',0);
            status = this.radarInterface.xep_x4driver_get_frame_area_offset(this.x2_instance, offsetPtr);
            frame_area_offset = double(offsetPtr.Value);
            clear offsetPtr
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
            status = this.radarInterface.xep_set_iopin_control(this.x2_instance, pinId, pinSetup, pinFeature);
        end

        function [pin_setup, pin_feature,status] = get_iopin_control(this, pinId)
            %GET_IOPIN_CONTROL Returns GPIO pin configuration.
            % [pin_setup, pin_feature, status] = GET_IOPIN_CONTROL( this, pinId )
            status = this.radarInterface.xep_get_output_control(this.x2_instance, output_feature, this.pin_setup, this.pin_feature);
            pin_setup = this.pin_setup.Value;        
            pin_feature = this.pin_feature.Value;        
        end        
        
        function status = set_iopin_value( this , pinId, pinValue)
            %SET_IOPIN_VALUE If IO pin control is used to set pin_id as output, the pin level or value will be set to pin_value.
            %   status = SET_IOPIN_VALUE( this , pinId, pinValue)
            %
            % See also SET_IOPIN_CONTROL
            status = this.radarInterface.xep_set_iopin_value(this.x2_instance, pinId, pinValue);
        end
        
        function [pinValue, status] = get_iopin_value( this , pinId)
            %GET_IOPIN_VALUE Returns value of io pin specified by pinId
            % [status, pinValue] = GET_IOPIN_VALUE( this , pinId)
            % 	pinValue 	the value of pin PINID
            % 	status		call status; 0 if success, error code otherwise
            pinValuePtr = libpointer('uint32Ptr',0);
            status = this.radarInterface.xep_get_iopin_value(this.x2_instance, pinId, pinValuePtr);
            pinValue = double(pinValuePtr.Value);
            clear pinValuePtr
        end
        
        function queuesize = peek_message_data_string( this )
            %PEEK_MESSAGE_DATA_STRING Returns queue size of data string messages
            % queuesize = PEEK_MESSAGE_DATA_STRING( this )
            queuesize = double(this.radarInterface.xep_peek_message_data_string(this.x2_instance));
        end
        
        function [contentID, length, info, data, status] = read_message_data_string( this )
            %READ_MESSAGE_DATA_STRING Reads one data string message
            % [contentID, length, info, data, status] = READ_MESSAGE_DATA_STRING( this )
            
            contentIdPtr = libpointer('uint32Ptr',0);
            infoPtr = libpointer('uint32Ptr',0);
            lengthPtr = libpointer('uint32Ptr',0);
            
            status = this.radarInterface.xep_read_message_data_string(this.x2_instance, contentIdPtr, infoPtr, this.dataStringPtr, lengthPtr, this.dataStringLen);
            
            contentID = double(contentIdPtr.Value);
            length = double(lengthPtr.Value);
            info = double(infoPtr.Value);
            data = double(this.dataStringPtr.Value(1:length));
            
            clear contentIdPtr infoPtr lengthPtr
        end
        
        function queuesize = peek_message_data_float( this )
            %PEEK_MESSAGE_DATA_FLOAT Returns queue size of data float messages
            %   queuesize = PEEK_MESSAGE_DATA_FLOAT( this )
            queuesize = double(this.radarInterface.xep_peek_message_data_float(this.x2_instance));
        end
        
        function [contentID, length, info, data, status] = read_message_data_float( this )
            %READ_MESSAGE_DATA_FLOAT Reads one data float message
            %   [contentID, length, info, data, status]  = READ_MESSAGE_DATA_FLOAT( this )
            
            contentIdPtr = libpointer('uint32Ptr',0);
            infoPtr = libpointer('uint32Ptr',0);
            lengthPtr = libpointer('uint32Ptr',0);
            
            status = this.radarInterface.xep_read_message_data_float(this.x2_instance, contentIdPtr, infoPtr, this.dataFloatPtr, lengthPtr, this.dataFloatLen);
            
            contentID = double(contentIdPtr.Value);
            length = double(lengthPtr.Value);
            info = double(infoPtr.Value);
            data = double(this.dataFloatPtr.Value(1:length));
            
            clear contentIdPtr infoPtr lengthPtr
            
        end
        
%         function status = set_max_message_buffer_count_data_float( this, newsize )
% 		%SET_MAX_MESSAGE_BUFFER_COUNT_DATA_FLOAT
% 		% status = SET_MAX_MESSAGE_BUFFER_COUNT_DATA_FLOAT( this, newsize )
%             status = this.radarInterface.set_max_message_buffer_count_data_float(this.x2_instance, newsize);
%         end
%
%         function status = set_max_message_buffer_count_data_string( this, newsize )
% 		%SET_MAX_MESSAGE_BUFFER_COUNT_DATA_STRING
% 		%	status = SET_MAX_MESSAGE_BUFFER_COUNT_DATA_STRING( this, newsize )
%             status = this.radarInterface.set_max_message_buffer_count_data_string(this.x2_instance, newsize);
%         end
        
        function status = x4driver_set_spi_register( this, address, value )
            %X4DRIVER_SET_SPI_REGISTER Set spi register on radar chip.
            %	status = X4DRIVER_SET_SPI_REGISTER( this, address, value )
            status = this.radarInterface.xep_x4driver_set_spi_register(this.x2_instance, address, value );
        end
        %
        function [value,status] = x4driver_get_spi_register( this,address )
            %X4DRIVER_GET_SPI_REGISTER Get spi register on radar chip.
            %	[value,status] = X4DRIVER_GET_SPI_REGISTER( this,address )
            valuePtr = libpointer('uint8Ptr',0);
            status = this.radarInterface.xep_x4driver_get_spi_register(this.x2_instance, address,valuePtr );
            value = valuePtr.Value; clear valuePtr
        end
        %
        function status = x4driver_set_pif_register( this,address,value )
            %X4DRIVER_SET_PIF_REGISTER Sets PIF register value.
            %	status = X4DRIVER_SET_PIF_REGISTER( this,address,value )
            status = this.radarInterface.xep_x4driver_set_pif_register(this.x2_instance,address,value);
        end
        %
        function [value,status] = x4driver_get_pif_register( this,address )
            %X4DRIVER_GET_PIF_REGISTER Gets PIF register value.
            %	[value,status] = X4DRIVER_GET_PIF_REGISTER( this,address )
            valuePtr = libpointer('uint8Ptr',0);
            status = this.radarInterface.xep_x4driver_get_pif_register(this.x2_instance,address ,valuePtr);
            value = valuePtr.Value; clear valuePtr
        end
        %
        function status = x4driver_set_xif_register( this,address,value )
            %X4DRIVER_SET_XIF_REGISTER Sets XIF register value.
            %	status = X4DRIVER_SET_XIF_REGISTER( this,address,value )
            status = this.radarInterface.xep_x4driver_set_xif_register(this.x2_instance,address,value);
        end
        %
        function [value,status] = x4driver_get_xif_register( this,address )
            %X4DRIVER_GET_XIF_REGISTER Gets XIF register value.
            %	[value,status] = X4DRIVER_GET_XIF_REGISTER( this,address )
            valuePtr = libpointer('uint8Ptr',0);
            status = this.radarInterface.xep_x4driver_get_xif_register(this.x2_instance, address,valuePtr);
            value = valuePtr.Value; clear valuePtr
        end
        %
        function status = x4driver_set_prf_div( this,prf_div )
            %X4DRIVER_SET_PRF_DIV Sets Pulse Repetition Frequency(PRF) divider
            %	status = X4DRIVER_SET_PRF_DIV( this,prf_div )
            status = this.radarInterface.xep_x4driver_set_prf_div(this.x2_instance, prf_div);
        end
        %
        function [prf_div,status] = x4driver_get_prf_div( this )
            %X4DRIVER_GET_PRF_DIV Gets Pulse Repetition Frequency(PRF) divider
            %	[prf_div,status]  = X4DRIVER_GET_PRF_DIV( this )
            prf_divPtr = libpointer('uint8Ptr',0);
            status = this.radarInterface.xep_x4driver_get_prf_div(this.x2_instance, prf_divPtr);
            prf_div = prf_divPtr.Value; clear prf_divPtr
        end
        
        %% File API
        function [fileIDs, status] = search_for_file_by_type( this, fileType )
            %SEARCH_FOR_FILE_BY_TYPE Searches for and returns a list of identifiers for all files of the specified type.
            %   [fileIDs, status] = SEARCH_FOR_FILE_BY_TYPE( this, fileType )
            status = this.radarInterface.search_for_file_by_type(this.x2_instance, fileType, this.FAPI_filesPtr, this.FAPI_lengthPtr, this.FAPI_maxLength);
            fileIDs = this.FAPI_filesPtr.Value;
            length = this.FAPI_lengthPtr.Value;
            fileIDs = fileIDs(1:length);
        end
        %
        function [fileTypeItems, fileIdItems, items, status] = find_all_files( this )
            %FIND_ALL_FILES Finds all files and outputs types and identifier of each file.
            %   [fileTypeItems, fileIdItems, items, status] = FIND_ALL_FILES( this )
            status = this.radarInterface.find_all_files(this.x2_instance, this.FAPI_fileTypeItemsPtr, this.FAPI_fileIdItemsPtr, this.FAPI_itemsPtr, this.FAPI_maxItems);
            fileTypeItems = this.FAPI_fileTypeItemsPtr.Value;
            fileIdItems = this.FAPI_fileIdItemsPtr.Value;
            items = this.FAPI_itemsPtr.Value;
            fileTypeItems = fileTypeItems(1:items);
            fileIdItems = fileIdItems(1:items);
        end
        %
        function status = create_file( this, fileType, fileID, length )
            %CREATE_FILE Creates and opens a new file with given type, identifiers and length.
            %   status = CREATE_FILE( this, fileType, fileID, length )
            status = this.radarInterface.create_file(this.x2_instance, fileType, fileID, length);
        end
        %
        function status = open_file( this, fileType, fileID )
            %OPEN_FILE Opens the file with given type band identifiers.
            %   status = OPEN_FILE( this, fileType, fileID )
            status = this.radarInterface.open_file(this.x2_instance, fileType, fileID);
        end
        %
        function status = set_file_data( this, fileType, fileID, offset, data )
            %SET_FILE_DATA Writes data at offset to the file.
            %   status = SET_FILE_DATA( this, fileType, fileID, offset, data )
            status = this.radarInterface.set_file_data(this.x2_instance, fileType, fileID, offset, data, length(data));
        end
        %
        function status = close_file( this, fileType, fileID, commit )
            %CLOSE_FILE Close the file.
            %   status = CLOSE_FILE( this, fileType, fileID, commit )
            %
            % Commit determines whether to save changes made to the file closing.
            status = this.radarInterface.close_file(this.x2_instance, fileType, fileID, commit);
        end
        %
        function [length, status] = get_file_length( this, fileType, fileID )
            %GET_FILE_LENGTH Gets the length of a file.
            %   [length, status] = GET_FILE_LENGTH( this, fileType, fileID )
            status = this.radarInterface.get_file_length(this.x2_instance, fileType, fileID, this.FAPI_lengthPtr);
            length = this.FAPI_lengthPtr.Value;
        end
        %
        function status = delete_file( this, fileType, fileID )
            %DELETE_FILE Deletes a file.
            %   status = DELETE_FILE( this, fileType, fileID )
            status = this.radarInterface.delete_file(this.x2_instance, fileType, fileID);
        end
        %
        function [fileData, status] = get_file_data( this, fileType, fileID, offset, length)
            %GET_FILE_DATA Reads a subsection of the file at an offset.
            %   [fileData, status = GET_FILE_DATA( this, fileType, fileID, offset, length)
            status = this.radarInterface.get_file_data(this.x2_instance, fileType, fileID, offset, length, this.FAPI_dataPtr);
            fileData = this.FAPI_dataPtr.Value;
            fileData = fileData(1:length);
        end
        %
        function status = format_filesystem( this, key)
            status = this.radarInterface.format_filesystem(this.x2_instance, key);
        end
        %
        function status = set_file( this, fileType, fileID, dataBuffer )
            dataLength = length(dataBuffer);
            status = this.radarInterface.set_file(this.x2_instance, fileType, fileID, dataLength, dataBuffer); 
        end
        %
        function [data, status] = get_file( this, fileType, fileID )
            fileLength = this.get_file_length(fileType, fileID);
            status = this.radarInterface.get_file(this.x2_instance, fileType, fileID, this.FAPI_maxLength, this.FAPI_dataPtr);
            data = this.FAPI_dataPtr.Value;
            data = data(1:fileLength);
        end
    end
    
    %% Class Methods
    %
    methods
        
        %% Constructor
        function x4 = XEP(mc)
            % Constructor
            x4.radarInterface = mc.radarInterface;
            x4.x2_instance = calllib(mc.lib_name,'nva_get_x2',mc.mcInstance);
            assert(~x4.x2_instance.isNull, 'create xethru failed check the logs');
        end
        
        %% Destructor
        function delete( this )
            % Destructor
            clear this.dataSringPtr this.dataFloatPtr
            clear this.FAPI_filesPtr this.FAPI_lengthPtr this.FAPI_fileTypeItemsPtr
            clear this.FAPI_fileIdItemsPtr this.FAPI_itemsPtr this.FAPI_dataPtr
            calllib(this.radarInterface.lib_name,'nva_destroy_X2_interface',this.x2_instance);
            clear('this.radarInterface')
            clear('this.x2_instance')
        end
    end
    
end
