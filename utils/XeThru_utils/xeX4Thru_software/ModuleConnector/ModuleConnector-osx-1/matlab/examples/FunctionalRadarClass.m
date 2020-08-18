classdef FunctionalRadarClass < BasicRadarClass
    % FUNCTIONALRADARCLASS offers plot, record and baseband conversion in 
    % addition to the functionality inherited from BASICRADARCLASS.
    %
    % Example usage: 
    %   Lib = ModuleConnector.Library;        % load library
    %   radar = FunctionalRadarClass('COM3'); % Init connection to radar
    %   radar.killstream_num = 200;           % Stream 200 frames (0 is unlimited)
    %   radar.open;                           % Open radar connection 
    %   radar.start;                          % Start streaming
    %   plot(radar)                           % Plot stream
    %   radar.close;                          % Close radar connection
    %
    % RadarClass methods:
    %   plot                -   Plot single frame or frame stream
    %   record              -   Record data to framebuffer
    %   baseband            -   Convert frames to baseband
    %   GetFrameBuffer      -   Get nonzero frames from framebuffer
    %
    % See also BASICRADARCLASS, MODULECONNECTOR
    %
 
    % Application Control
    properties 
        %%
        % Control the number of frames plotted and/or recorded
        %
        % plot and/or record maximum killstream_num frames in stream mode.
        killstream_num = 160;  % 0 is disabled
        % plot and/or record maximum killstream_sec seconds in stream mode.
        killstream_sec = 0; % 0 is disabled
        % enable recording while running plot in streaming mode.
        record_enable = 0;   % 0 is disabled
    end

    % Baseband configuration
    properties 
        %%
        % Convert to BaseBand. The fraction Fc/Sf is more important than
        % the actual values themselves.
        bb_data = 0;         % On/Off   
        Sf = 41e9;           % Sample freq
        Fc = 7.2e9;          % Center freq 
        aa=[0.00211375865,0.00595695619,0.01239431207,0.02171406610,...
            0.03382013836,0.04803996925,0.06302843966,0.07724827056,...
            0.08906610300,0.09684857802,0.09953881630,0.09684857802,...
            0.08906610300,0.07724827056,0.06302843966,0.04803996925,...
            0.03382013836,0.02171406610,0.01239431207,0.00595695619,...
            0.00211375865]; 
    end
        
    % Refer to GetFrameBuffer() for buffer access
    properties (Access = private)
        framebuffer = [];
        framebuffer_cnt = 0;
    end
    
    methods
        
        function r = FunctionalRadarClass(COM)
            % Create a radar object
            % radar = FunctionalRadarClass( COM_port );
            r@BasicRadarClass(COM);
        end

        %%
        % Display and Visualization
        % If the frame is complex (baseband) the abs value is plotted.
        %
        function plot(r,varargin)
            % plot(r,varargin)
            if nargin==1,varargin = {'b'};end

            if r.isStreaming
                
                cleared = r.clear(); % clear the frame buffer
                disp(['# Cleared ',num2str(cleared),' frames.'])
                
                timer = tic;    % compare to killstream_sec  
                counter = 0;    % compare to killstream_num
                l = line();
                while plot_fun(r) 

                    f = double(r.GetFrameNormalized());
                    if size(get(l,'YData')) ~= size(f)
                        l = plot(1:length(f),zeros(size(f)),varargin{:});
                        xlim([1,length(f)]);
                        ylabel('Amplitude');
                        xlabel('Sample #')
                    end
                    
                    if r.bb_data,f=r.baseband(f);end

                    if r.record_enable
                       r.record(f); 
                    end
                    
                    if r.bb_data
                        set(l,'ydata',abs(f));
                    else
                        set(l,'ydata',f);
                    end
                    drawnow
                end
                clear timer counter
            else
                f = double(r.GetSingleFrameNormalized());
                if r.bb_data,f=r.baseband(f);
                    l = plot(abs(f),varargin{:});
                else
                    l = plot(f,varargin{:});
                end
                xlim([1,length(f)]);
                ylabel('Amplitude');
                xlabel('Sample #')
                drawnow
            end
            
            %%
            % This code will run each time a frame is received in streaming
            % mode.
            function cont = plot_fun(r)
                cont = ishandle(l);
                
                if r.killstream_num
                   counter = counter+1;
                   cont = cont & (r.killstream_num>=counter);
                end
                if r.killstream_sec
                   cont = cont & (r.killstream_sec>=toc(timer));
                end
            end
        end % end plot

        
        %%
        % Record data frames
        %
        function record(r,frame)
            % record(r,frame): if nargin==2, frame is appended to recording
            if isempty(r.framebuffer)
                r.framebuffer = zeros(256,2000);
                r.framebuffer_cnt = 0;
            elseif size(r.framebuffer,2) == r.framebuffer_cnt;
                r.framebuffer = [r.framebuffer,zeros(256,2000)];
            end
            
            if nargin == 2
                % if record is called from another function
                r.framebuffer(:,mod(r.framebuffer_cnt,2000)+1)=frame;
                r.framebuffer_cnt=r.framebuffer_cnt+1;
            else
                % if record is the only function
                if r.isStreaming
                    
                    cleared = r.clear(); % clear the frame buffer
                    disp(['# Cleared ',num2str(cleared),' frames.'])
                    
                    timer = tic;    % compare to killstream_sec
                    counter = 0;    % compare to killstream_num
                    r.framebuffer_cnt = 0;
                    while record_fun(r)
                        f = double(r.GetFrameNormalized());
                        if r.bb_data,f=r.baseband(f);end
                        
                        %m = mod(r.framebuffer_cnt,2000)+1;
                        %r.framebuffer(:,m)=f;
                        
                        r.framebuffer_cnt=r.framebuffer_cnt+1;
                        
                        if size(r.framebuffer,2) == r.framebuffer_cnt
                            r.framebuffer = [r.framebuffer,zeros(256,2000)];
                        end
                        r.framebuffer(:,r.framebuffer_cnt)=f;
                    end
                    clear timer counter
                end
            end
            %%
            % This code will run each time a frame is received in streaming
            % mode.
            function cont = record_fun(r)
                cont = 1;
                if r.killstream_num
                    counter = counter+1;
                    cont = cont & (r.killstream_num>=counter);
                end
                if r.killstream_sec
                    cont = cont & (r.killstream_sec>=toc(timer));
                end
            end
        end % end record
        
        
        function bb = baseband(r,frm,~)
            
            len = size(frm,1);
            if mod(len,256),warning('Function requires column input');end
                        
            % no DC
            frm = detrend(frm,'constant');
            % demodulation 
            ydemod = exp(-1j*2*pi*(0:len-1)*r.Fc/r.Sf)';
            bb_ = frm.*(ydemod*ones(1,size(frm,2)));
            % LP-filter single sideband
            bb = filtfilt(r.aa,1,2*bb_);
            
            if nargin > 2 % debug
                t = (0:len-1)/r.Sf;
                f = (0:len-1)/len*r.Sf;
                figure
                subplot(2,1,1)
                plot(f,abs(fft(frm)),'b',...
                     f,abs(fft(bb_)),'g',...
                     f,abs(fft(bb )),'r');
                title('Verify baseband conversion');
                legend('original-DC','down-converted','LP-filtered')
                ylabel('Magnitude');xlabel('Frequency bin (two-sided)')
                xlim([f(1),f(end)]);
                subplot(2,1,2)
                plot(t,frm,'b');hold all;
                plot(t,real(bb),'r',t,imag(bb),'k',t,abs(bb),'g');hold off;
                legend('original-DC','real','imag','abs')
                ylabel('Amplitude');xlabel('time')
                xlim([t(1),t(end)]);
            end
            
        end  % end baseband
        
        
        function buffer = GetFrameBuffer(r)
            buffer = r.framebuffer(:,1:r.framebuffer_cnt);
        end % end GetFrameBuffer
        
    end % end methods
    
end % end class

