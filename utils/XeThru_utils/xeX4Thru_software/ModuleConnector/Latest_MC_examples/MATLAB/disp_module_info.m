% Latest examples is located at https://github.com/xethru/XeThru_ModuleConnector_Examples or https://dev.azure.com/xethru/XeThruApps/_git/XeThru_ModuleConnector_Examples.
%
% display manufacturing information of XeThru modules
% 
% Input: device serial port like "COM8"
% 
% Target module: X4M2xx, X4M3xx, X4M0x
%
% Example:
% ModuleConnector.Library;
% disp_module_info(device_name);

function OrderCode = disp_module_info(device_name)
    XTID_SSIC_FIRMWAREID = 2;
    XTID_SSIC_VERSION = 3;
    XTID_SSIC_BUILD = 4;
    XTID_SSIC_VERSIONLIST = 7;
    XTID_SSIC_ORDERCODE = 1;
    XTID_SSIC_ITEMNUMBER = 0;
    XTID_SSIC_SERIALNUMBER = 6;
    % Load the library
    LIB = ModuleConnector.Library;
    %ModuleConnector.Library;
    % Stop running application and set module in manual mode.
    mc = ModuleConnector.ModuleConnector(device_name,0);
    % Assume an X4M300/X4M200 module and try to enter XEP mode
    app = mc.get_x4m200(); 
    % we have to go to manual mode
    app.set_sensor_mode('stop');
    app.set_sensor_mode('xep');
    % Stop running application and set module in manual mode.
    xep = mc.get_xep();
    pong = xep.ping();
    disp("");
    disp("********** XeThru Module Information **********");
    disp("");
    disp(['Received pong= ', dec2hex(pong), ' connection build!']);
    disp(['FirmWareID = ', xep.get_system_info(XTID_SSIC_FIRMWAREID)]);
    disp(['Version = ', xep.get_system_info(XTID_SSIC_VERSION)]);
    disp(['Build = ', xep.get_system_info(XTID_SSIC_BUILD)]);
    disp(['VersionList = ', xep.get_system_info(XTID_SSIC_VERSIONLIST)]);

    % Following three item only supported by XeThru Sensor, e.g.X4M200, X4M300. X4M03 does not these information and will feedback error message when read.
    OrderCode = xep.get_system_info(XTID_SSIC_ORDERCODE);
    disp(['OrderCode = ', OrderCode]);
    disp(['ItemNumber = ', xep.get_system_info(XTID_SSIC_ITEMNUMBER)]);
    disp(['SerialNumber = ', xep.get_system_info(XTID_SSIC_SERIALNUMBER)]);
    %LIB.unloadlib(); 
end 
