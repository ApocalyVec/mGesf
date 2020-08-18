% Latest examples is located at https://github.com/xethru/XeThru_ModuleConnector_Examples or https://dev.azure.com/xethru/XeThruApps/_git/XeThru_ModuleConnector_Examples.
% 
% Target module: 
% X4M200
% X4M300
% 
% Introduction: 
% This example illustrates how to set up the x4 based sensors for streaming
% pulse-Doppler matrices and how to assemble and plot them out.
%
% prerequisite:
% 1. this example should be placed in ModuleConnector\matlab\examples
% folder.
% 2. disp_module_info.m and disp_sensor_settings.m in the same folder.
%
% How to run:
% Change User configurations section according to your needs. 

%add paths
add_ModuleConnector_path();
% if running on a 32-bit Windows system, instead run:
% add_ModuleConnector_path('win32');

clc
clear

%% User configurations:
device_name         = 'COM13';           % Device com-port
%options.interface   = 'x4m300';         % Interface to use. x4m300 or x4m200
options.datatype    = 'pulsedoppler';   % Data to get. pulsedoppler or noisemap
options.format      = 'float';          % Data format to get. byte or float
options.dopplers    = 'both';           % PD instance to get. fast, slow or both
options.num_messages= 0;                % How many matrices to read (0 = infinite)
detection_zone_start = 0.4;
detection_zone_end = 5;

% Load the library
options.interface = disp_module_info(device_name);
Lib = ModuleConnector.Library;

% Create ModuleConnector object
clear mc; mc = ModuleConnector.ModuleConnector(device_name,0);
pause(1)

% Get interface and load a profile
if strcmp( options.interface, 'X4M200' )
    app = mc.get_x4m200();
    app.set_sensor_mode('stop');
    
    % Adult respiration (respiration2) profile.
    ProfileID_Respiration2 = hex2dec('064e57ad');
    ret = app.load_profile(ProfileID_Respiration2);
    
elseif strcmp( options.interface, 'X4M300' )
    app = mc.get_x4m300();
    app.set_sensor_mode('stop');
    
    % Adult presence (presence2) profile.
    ProfileID_Presence2 = hex2dec('014d4ab8');
    ret = app.load_profile(ProfileID_Presence2);
else
    error('Interface not recognized.\n')
end

if ret
    error('Failed to load profile. Check ModuleConnector logs.\n');
end

% Flush all buffers
while app.peek_message_pulsedoppler_byte()
    app.read_message_pulsedoppler_byte();
end
while app.peek_message_pulsedoppler_float()
    app.read_message_pulsedoppler_float();
end
while app.peek_message_noisemap_byte()
    app.read_message_noisemap_byte();
end
while app.peek_message_noisemap_float()
    app.read_message_noisemap_float();
end

% Turn on outputs
if strcmp( options.dopplers, 'both' )
    output_control = 3;
elseif strcmp( options.dopplers, 'fast' )
    output_control = 2;
elseif strcmp( options.dopplers, 'slow' )
    output_control = 1;
else
    error('Pulse-Doppler instance not recognized.\n')
end

if strcmp( options.datatype, 'pulsedoppler' ) && strcmp( options.format, 'byte' )
    ret = app.set_output_control(hex2dec('00000011'),output_control);
elseif strcmp( options.datatype, 'pulsedoppler' ) && strcmp( options.format, 'float' )
    ret = app.set_output_control(hex2dec('00000010'),output_control);
elseif strcmp( options.datatype, 'noisemap' ) && strcmp( options.format, 'byte' )
    ret = app.set_output_control(hex2dec('00000013'),output_control);
elseif strcmp( options.datatype, 'noisemap' ) && strcmp( options.format, 'float' )
    ret = app.set_output_control(hex2dec('00000012'),output_control);
else
    error('Datatype/format not recognized.\n')
end

if ret
    error('Failed to set output control. Check ModuleConnector logs.\n');
end

app.set_detection_zone(detection_zone_start , detection_zone_end);
% Start sensor
ret = app.set_sensor_mode('run');

if ret
    error('Failed to set sensor mode run. Check ModuleConnector logs.\n');
end

% Set up state machinery for matrix assembly
matrix          = [];
is_negative     = true;
prev_range_idx  = -1;
ok              = true;
instance        = [];
n_printed       = 0;
max_val         = 0;

% Assemble and plot whole range-Doppler matrices from the individual
% packets. Close window to stop.
fh=figure(1);
while ishandle(fh)
    if strcmp(options.datatype, 'pulsedoppler') && strcmp(options.format, 'byte')
        [data,header,status] = app.read_message_pulsedoppler_byte;
    elseif strcmp(options.datatype, 'pulsedoppler') && strcmp(options.format, 'float')
        [data,header,status] = app.read_message_pulsedoppler_float;
    elseif strcmp(options.datatype, 'noisemap') && strcmp(options.format, 'byte')
        [data,header,status] = app.read_message_noisemap_byte;
    elseif strcmp(options.datatype, 'noisemap') && strcmp(options.format, 'float')
        [data,header,status] = app.read_message_noisemap_float;
    end
    
    if status
        error('Failed to read message. Check ModuleConnector logs.\n')
    end
    
    if isempty(instance)
        instance = header.pulsedoppler_instance;
        
        range_first = header.range;
        frequency_first_RPM = 60*header.frequency_start;
    end
    
    if header.range_idx == 0 && is_negative && ~isempty(matrix)
        % We now have a complete range-Doppler matrix.
        
        % Plot
        range_vec = linspace(range_first, range_last, size(matrix,1));
        frequency_vec = linspace(frequency_first_RPM, frequency_last_RPM, size(matrix,2));
        matrix_dB = 10*log10(matrix);
        if max(max(matrix_dB)) > max_val
            max_val = max(max(matrix_dB));
        end
        
        if instance == 0
            subplot 121;
            mesh(frequency_vec, range_vec, matrix_dB); view(160,20);
            xlim([frequency_first_RPM frequency_last_RPM]);
            ylim([range_first range_last]);
            zlim([-30 max_val]);
            title('Slow Doppler');xlabel('Doppler frequency [RPM]');ylabel('Range [m]');
        elseif instance == 1
            subplot 122;
            mesh(frequency_vec, range_vec, matrix_dB); view(160,20);
            xlim([frequency_first_RPM frequency_last_RPM]);
            ylim([range_first range_last]);
            zlim([-30 max_val]);
            title('Fast Doppler');xlabel('Doppler frequency [RPM]');ylabel('Range [m]');
        else
            warning('Matrix with unknown pd instance.')
        end
        drawnow
    
        % Reset
        n_printed = n_printed + 1;
        if (options.num_messages ~= 0) && (n_printed >= options.num_messages)
            break
        end
        matrix = [];
        prev_range_idx = -1;
        instance = header.pulsedoppler_instance;
        range_first = header.range;
        frequency_first_RPM = 60*header.frequency_start;
    end
    
    if is_negative && (header.range_idx == prev_range_idx + 1)
        ok = true;
        prev_range_idx = header.range_idx;
    elseif ~is_negative && (header.range_idx == prev_range_idx)
        % Do nothing
    else
        if ok
            warning('Bad range_idx %d in instance %d, prev_range_idx %d, resetting and waiting for zero.\n', header.range_idx, instance, prev_range_idx)
            matrix = [];
            is_negative = true;
            prev_range_idx = -1;
            ok = false;
        end
        continue
    end
    
    if strcmp( options.format, 'byte' )
        % Convert byte data to float.
        offset = header.byte_step_start;
        step = header.byte_step_size;
        data = 10.^((data*step + offset)/10);
    else
        % Do nothing
    end

    % A noisemap misses a piece in the middle, so fill it with zeroes
    % Has no effect if nothing is missing.
    if is_negative
        frequency_end = header.frequency_start + header.frequency_step * header.frequency_count;
        if frequency_end < 0.000001
            data = [data; 0*ones(round(-frequency_end / header.frequency_step), 1)];
        end
    else
        if header.frequency_start > 0.000001
            data = [0*ones(round(header.frequency_start / header.frequency_step), 1); data];
        end
    end
    
    if is_negative
        matrix(header.range_idx+1,1:size(data,1)) = data;
    else
        matrix(header.range_idx+1,size(data,1)+1:2*size(data,1)) = data;
        
        range_last = header.range;
        frequency_last_RPM = 60*(header.frequency_start + header.frequency_step*header.frequency_count);
    end
    
    is_negative = ~is_negative;
end

% Stop sensor
app.set_sensor_mode('stop');


% Clean up.
clear;








