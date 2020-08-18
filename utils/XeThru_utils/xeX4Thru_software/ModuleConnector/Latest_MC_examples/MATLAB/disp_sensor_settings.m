% Latest examples is located at https://github.com/xethru/XeThru_ModuleConnector_Examples or https://dev.azure.com/xethru/XeThruApps/_git/XeThru_ModuleConnector_Examples.
%
% display configuration information of XeThru sensors expect development kits. 
% 
% Prerequisite:
% This example should be placed in ModuleConnector\matlab\examples
% folder.
%
% Target module: X4M2xx, X4M3xx
% 
% Example:
% ModuleConnector.Library;
% mc = ModuleConnector.ModuleConnector(device_name,0);
% X4M200 = mc.get_x4m200;
% disp_sesnor_settings(X4M200);

function disp_sensor_settings(xethru_sensor)
    % Check values (to confirm we have the values we want);:
    disp("");
    disp("********** Current sensor settings **********");
    disp("");
    disp(['profile id: ', num2str(dec2hex(xethru_sensor.get_profileid()))]);
    disp(['sensitivity: ' , num2str(xethru_sensor.get_sensitivity())]);
    disp(['led control: ' , num2str(xethru_sensor.get_led_control())]);
    disp(['noise map control: ' , num2str(xethru_sensor.get_noisemap_control())]);
    [start, stop] = xethru_sensor.get_detection_zone();
    disp(['Detection zone: ' , num2str(start),  ' to ' , num2str(stop)]);
    clear;
end