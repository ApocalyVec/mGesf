classdef NotSupported
    %UNTITLED6 Summary of this class goes here
    %   Detailed explanation goes here
    
    properties (SetAccess = private)
        x4_instance
        radarInterface;
    end
    
    properties (SetAccess = private,Hidden)  
        framePtr    = libpointer('singlePtr'); % frame storage: singlePtr
        frameCtrPtr = libpointer('uint32Ptr'); % frame counter: uint32Ptr
        frameLen = uint64(6400);
    end
    
    properties (SetAccess = private,Hidden)
        decimframePtr    = libpointer('singlePtr'); % frame storage: singlePtr
        decimframeCtrPtr = libpointer('uint32Ptr'); % frame counter: uint32Ptr
        decimframeLen = uint64(1280);
    end
    
    properties (SetAccess = private,Hidden)
%         infoPtr    = libpointer('uint32Ptr'); % info: uint32Ptr
%         contentIdPtr = libpointer('uint32Ptr'); % contentID: uint32Ptr
    end
    
    properties (SetAccess = private)
    	frame_bb = ones(188*2,1);
        frame_rf = ones(1536,1);
    end

    methods
        
        %% Only for use in TordFW
        function int = get_x4_io_pin_value( this, pin)
            int = this.radarInterface.get_x4_io_pin_value(this.x4_instance,pin);
        end
        %
        function int = set_x4_io_pin_value( this, pin, value)
            int = this.radarInterface.set_x4_io_pin_value(this.x4_instance,pin,value);
        end
        %
        function int = set_x4_io_pin_mode( this, pin, mode)
            int = this.radarInterface.set_x4_io_pin_mode(this.x4_instance, pin, mode);
        end
        %
        function int = set_x4_io_pin_dir( this, pin, direction)
            int = this.radarInterface.set_x4_io_pin_dir(this.x4_instance, pin, direction);
        end
        %
        function int = set_x4_io_pin_enable( this, pin)
            int = this.radarInterface.set_x4_io_pin_enable(this.x4_instance, pin);
        end
        %
        function int = set_x4_io_pin_disable( this, pin)
            int = this.radarInterface.set_x4_io_pin_disable(this.x4_instance, pin);
        end
        %%
        
        %
        % TODO:
        function [result,status] = read_x4_spi( this, address)
            [result,status] = this.radarInterface.read_x4_spi(this.x4_instance, address);
        end
        %
        function int = write_x4_spi( this, address, value)
            int = this.radarInterface.write_x4_spi(this.x4_instance, address, value);
        end
        %
        function int = set_x4_fps( this, fps)
            int = this.radarInterface.set_x4_fps(this.x4_instance, fps);
        end
        %
        function int = subscribe_to_x4_decim( this, name)
            int = this.radarInterface.subscribe_to_x4_decim(this.x4_instance, name);
        end
        %
        % TODO:
        function [result,status] = get_x4_decim_frame( this, name)
            %int nva_get_x4_decim_frame(X2 * instance,const char * name,uint32_t * frame_counter,double * frame_data,unsigned int max_length);
%             [status,~,result] = calllib(this.lib_name,'nva_get_x4_decim_frame', this.x4_instance, name, this.decimframeCtrPtr, double * frame_data,unsigned int max_length);
        end
        %
        function int = subscribe_to_data_float( this, name)
            int = this.radarInterface.subscribe_to_data_float(this.x4_instance, name);
        end
        %
        % TODO:
        function int = set_parameter_file( this )
            %int nva_set_parameter_file(X2 * instance,const char * filename,const char * data);
            %int = calllib(this.lib_name,'nva_set_parameter_file', X2_interface,
        end
        %
        % TODO:
        function int = get_parameter_file( this )
            %int nva_get_profile_parameter_file(X2 * instance,const char * filename);
            %int = calllib(this.lib_name,'nva_get_parameter_file', X2_interface,
        end
        %
        % TODO
        function int = load_profile( this, profile_id)
            %int nva_load_profile(X2 * instance,const uint32_t profile_id);
            int = calllib(this.lib_name,'nva_load_profile', X2_interface, uint32(profile_id));
        end
        %
        function int = subscribe_to_trace( this, name)
            int = this.radarInterface.subscribe_to_trace(this.x4_instance, name );
        end
        %
        % TODO:
        function int = get_trace( this, name)
            %int nva_get_trace(X2 * instance, const char * name, char * trace, unsigned int max_length);
            %int = calllib(this.lib_name,'nva_get_trace', X2_interface,
        end
        %
        function int = subscribe_to_data_byte( this, name)
            int = this.radarInterface.subscribe_to_data_byte(this.x4_instance, name );
        end
    end
    
    %% Class Methods
    %
    methods
        
        %% Constructor
        function x4 = NotSupported(mc)
            % Constructor
            x4.radarInterface = mc.radarInterface;
            x4.x4_instance = calllib(mc.lib_name,'nva_get_x2',mc.mcInstance);
            assert(~x4.x4_instance.isNull, 'create xethru failed check the logs');            
            
%             x4.set_sensor_mode_idle();
%             x4.set_debug_level(5);
            
            x4.framePtr.Value = x4.frame_rf;
            x4.frameCtrPtr.Value = 0;
            x4.decimframePtr.Value = x4.frame_bb;
            x4.decimframeCtrPtr.Value = 0;
                        
        end
        
        %% Destructor
        function delete( this )
            % Destructor
            clear this.framePtr this.frameCtrPtr this.decimframePtr this.decimframeCtrPtr
            calllib(this.radarInterface.lib_name,'nva_destroy_X2_interface',this.x4_instance);
            clear('this.radarInterface')
            clear('this.x4_instance')
        end
    end
    
end
