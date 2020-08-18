classdef X4M_Common_Interface
    %X4M_COMMON_INTERFACE contains, as the name states, a common interface for all X4 based modules.
    %
    % See also MODULECONNECTOR 
    
    properties
        radarInterface % RADARINTERFACE
        x4m_instance   % libpointer to C/C++ library instance
    end
    
    methods
        
       function [system_info,status] = get_system_info(this,info_code)
       %GET_SYSTEM_INFO Returns a string containing system information given an infocode
       % [system_info,status] = GET_SYSTEM_INFO(this,info_code)
       %Info codes:
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
            system_infoPtr = libpointer('string',blanks(max_length));
            status = this.radarInterface.get_system_info(this.x4m_instance,info_code,system_infoPtr,100);
            system_info = strtrim(system_infoPtr.Value); clear system_infoPtr
       end
       
        function status = module_reset(this )
            %MODULE_RESET Resets and restart the module
            % status = MODULE_RESET(this, XEP_interface)
            status=this.radarInterface.module_reset(this.x4m_instance);
        end
        
        function status = reset_to_factory_preset(this )
            %RESET_TO_FACTORY_PRESET Resets XEP to factory presets (including XDRIVER)
            % status = RESET_TO_FACTORY_PRESET( this )
            status=this.radarInterface.reset_to_factory_preset(this.x4m_instance);
        end
        
        function status = start_bootloader(this )
            %START_BOOTLOADER Enters the bootloader for FW upgrades
            % status = START_BOOTLOADER( this )
            status=this.radarInterface.start_bootloader(this.x4m_instance);
        end
        
        function [test_results,status] = system_run_test( this, test_code, max_length )
        %SYSTEM_RUN_TESTS Runs the different manufacturing tests identified 
        % by testcode. Can return any number of results depending on test_mode. Host must know how to parse test results.
        % status = SYSTEM_RUN_TESTS( this, test_code, max_length )
        %
            data_lengthPtr = libpointer('uint32Ptr',0);
            dataPtr = libpointer('string',blanks(max_length));
            status=calllib(this.lib_name,'nva_system_run_test',XEP_interface,uint8(test_code),data_lengthPtr,dataPtr);
            test_results = strtrim(system_infoPtr.Value); 
            clear dataPtr data_lengthPtr
        end
       
    end
    
    
    methods            
        function x4m = X4M_Common_Interface(mc)
            % X4M_COMMON_INTERFACE Object constructor
            x4m.radarInterface = mc.radarInterface;
            x4m.x4m_instance = calllib(mc.lib_name,'nva_get_xep',mc.mcInstance);
            assert(~x4m.x4m_instance.isNull, 'create xethru failed check the logs');            
        end
        
        function delete( this )
            % DELETE Object destructor
            calllib(this.radarInterface.lib_name,'nva_destroy_X2_interface',this.x4m_instance);
            clear('this.radarInterface')
            clear('this.x4m_instance')
        end
    end
    
end

