classdef Transport
    %TRANSPORT Generic send commands, subscriptions and data packets.
    %
    % See also MODULECONNECTOR 
    
    properties
        radarInterface % Layer one wrapper class
        raw_instance   % libpointer to C/C++ library instance
        packetPtr    = libpointer('uint8Ptr');  % packet storage: uint8Ptr
        packetMaxLen = 4000; % Maximum lenght of packet byte sequence 
    end
    
    methods
        
        function int = send_command( this, command )
        % SEND_COMMAND sends a sequence of bytes to the module. It does not
        % wait for the reply.
        %
            int = this.radarInterface.send_command(this.raw_instance,command,100); 
        end
        
        function int = send_command_single(this,command,comparator)
        % SEND_COMMAND_SINGLE sends a sequence of bytes to the module. 
        % It waits until a byte-sequence starting with comparator is sent 
        % from the module. It times out after 2 seconds.
        %
            int = this.radarInterface.send_command_single(this.raw_instance,command,comparator);
        end
        
        % SEND_COMMAND_MULTI
        %  function Bytes = send_command_multi(this,command,comparator)
        %      disp('empty std'); 
        %  end
        
		function int = subscribe( this, name, comparator )
		% SUBSCRIBE Subscribes to streaming data that starts with the bytes
		% sequence given by comparator.
        %
            int = this.radarInterface.subscribe(this.raw_instance,name,comparator);
        end

        function int = unsubscribe( this, name )
        % UNSUBSCRIBE Unsubscribe to a named subscription.
        %
            int = this.radarInterface.subscribe(this.raw_instance,name);
        end
        
        %%%
        %% GET_ALL_PACKETS
        %  function vector_Bytes = get_all_packets(name)
        %      disp('Empty Bytes'); 
        %  end
        
%         function packet = get_packet(this,name)
        % GET_PACKET Gets a packet from top of queue from a subscription as a byte array.
%             [status,~,packet] = this.radarInterface.get_packet(this,name,this.packetPtr,this.packetMaxLen); 
%         end
        
        %
        function int = get_number_of_packets( this, name )
        % GET_NUMBER_OF_PACKETS Returns the number of packets buffered
        % with the subscription comparator <name>
        % int = GET_NUMBER_OF_PACKETS( this, name )
            int = double(this.radarInterface.get_number_of_packets(this.raw_instance,name)); 
        end

        %%
        % CLEAR
%         function clear( this, name )                            
%             [status,~,packet] = this.radarInterface.clear(this,name,this.packetPtr,this.packetMaxLen); 
%             calllib(r.lib.radarlib,'nva_clear', r.radarInstance,'raw_normalized');
%         end 
    end
    
    
    methods            
        function raw = Transport(mc)
            % TRANSPORT Object constructor
            raw.radarInterface = mc.radarInterface;
            raw.raw_instance = calllib(mc.lib_name,'nva_get_x2',mc.mcInstance);
            assert(~raw.raw_instance.isNull, 'create xethru failed check the logs');            

            raw.packetPtr.Value = uint8(zeros(4000,1));
        end
        
        function delete( this )
            % DELETE Object destructor
            clear this.packetPtr
            calllib(this.radarInterface.lib_name,'nva_destroy_X2_interface',this.raw_instance);
            clear('this.radarInterface')
            clear('this.raw_instance')
        end
    end
    
end

