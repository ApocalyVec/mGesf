% Latest examples is located at https://github.com/xethru/XeThru_ModuleConnector_Examples or https://dev.azure.com/xethru/XeThruApps/_git/XeThru_ModuleConnector_Examples.
%
% display X4 chip settings of XeThru modules
% 
% Prerequisite:
% This example should be placed in ModuleConnector\matlab\examples
% folder.
%
% Input: device serial port like "COM8"
% 
% Target module: X4M2xx, X4M3xx, X4M0x
%
% Example:
% ModuleConnector.Library;
% mc = ModuleConnector.ModuleConnector(device_name,0);
% xep = mc.get_xep();
% disp_x4_settings(xep);

function disp_x4_settings(xep)
    % Check values (to confirm we have the values we want);:
[downconversion, status] = xep.x4driver_get_downconversion;
[frequency, status] = xep.x4driver_get_tx_center_frequency;
[txpower, status] = xep.x4driver_get_tx_power;
[prf_div,status] = xep.x4driver_get_prf_div;
[x4_iterations,status] = xep.x4driver_get_iterations;
[x4_pps,status] = xep.x4driver_get_pulsesperstep;
[dac_min, status_min] = xep.x4driver_get_dac_min;
[dac_max, status_max] = xep.x4driver_get_dac_max;
% framearea
[frame_area_offset, status] = xep.x4driver_get_frame_area_offset;
[framebincount, status] = xep.x4driver_get_framebincount;
[start, stop, status] = xep.x4driver_get_frame_area;

% disp status
if downconversion==0
    downstr= 'disabled';
else
    downstr = 'enabled';
end

if frequency==3
    freqstr = '6.0 to 8.5 Ghz range, centre=7.29GHz';
elseif frequency==4
    freqstr = '7.2 to 10.2 GHz range, centre=8.4GHz';
end


if txpower == 0
    txpowerstr = 'OFF';
elseif txpower == 1
    txpowerstr = 'Low';
elseif  txpower == 2
    txpowerstr = 'Medium';
elseif  txpower == 3
    txpowerstr = 'High';
end


%Prf
prf_base_freq = 243e6;
prf_freq = prf_base_freq / single(prf_div);
c= 3e8;
Ramb = c/(2*prf_freq);
frameInterval_ideal = 1/prf_freq * x4_iterations * x4_pps * (dac_max-dac_min+1);
disp("");
disp("********** Current X4 chip settings **********");
disp(['Downconversion = ' downstr]);
disp(['Frequency = ' freqstr]);
disp(['Tx_power = ' txpowerstr]);
disp(['Prf-div = ' num2str(prf_div) ' --> PRF_Freq= ' num2str(prf_freq/1e6,5) ...
    ' MHz giving ambigous range Ramb = ' num2str(Ramb) ' m']); 
disp(['Iterations = ' num2str(x4_iterations)]);
disp(['PulsesPerStep = ' num2str(x4_pps)]);
disp(['DacMin = ' num2str(dac_min)]);
disp(['DacMax = ' num2str(dac_max)]);
disp(['FrameInterval_ideal= ' num2str(frameInterval_ideal*1000) ...
    ' ms -> giving max theoretical fps_max=' num2str(1/frameInterval_ideal)]);
disp(['frameAreaOffset = ' num2str(frame_area_offset)]);
disp(['frameAreaStart = ' num2str(start)]);
disp(['frameAreaStop = ' num2str(stop)]);
disp(['frameBinCount = ' num2str(framebincount)]);
end
