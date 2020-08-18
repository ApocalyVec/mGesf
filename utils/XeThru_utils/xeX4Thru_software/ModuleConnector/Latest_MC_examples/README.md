# ModuleConnector-Examples
This repo contains latest CPP/PYTHON/MATLAB examples for ModuleConnector.

There is one application note intruducing how to use Module Connector:
https://github.com/novelda/Legacy-Documentation/blob/master/Application-Notes/XTAN-06_XeThruSensorsIntroduction_rev_a.pdf

Please make sure your sensor are running latest firmware. It can be updated from the latest XeThru Explorer:
https://github.com/novelda/XeThru-Explorer

# Example Folder Structure

* [/CPP](./CPP)
* [/MATLAB](./MATLAB)
* [/PYTHON](./PYTHON)
* [README.md](./README.md)
* [LICENSE.md](./LICENSE.md)

# Example Content

## CPP examples

1. Profile messages for the normal user
* [xt_modules_print_record_playback_radar_raw_data_message.cpp](./CPP/xt_modules_print_record_playback_radar_raw_data_message.cpp)
* [x4m200_print_record_RESP_SLEEP_message.cpp](./CPP/x4m200_print_record_RESP_SLEEP_message.cpp)
* [x4m300_print_record_PRESENCE_message.cpp](./CPP/x4m300_print_record_PRESENCE_message.cpp)


## MATLAB examples

1. Profile messages for the normal user
* [xt_modules_print_product_info.m](./MATLAB/xt_modules_print_product_info.m)
* [x4m200_plot_record_RESP_SLEEP_message.m](./MATLAB/x4m200_plot_record_RESP_SLEEP_message.m)
* [x4m300_plot_record_PRESENCE_message.m](./MATLAB/x4m300_plot_record_PRESENCE_message.m)


2. Radar DSP data message for the advanced user
* [xt_modules_plot_record_playback_radar_raw_data_message.m](./MATLAB/xt_modules_plot_record_playback_radar_raw_data_message.m)
* [xt_sensors_plot_record_playback_baseband_IQ_AP_message.m](./MATLAB/xt_sensors_plot_record_playback_baseband_IQ_AP_message.m)
* [xt_sensors_plot_record_movinglist_message.m](./MATLAB/xt_sensors_plot_record_movinglist_message.m)
* [xt_modules_plot_algrithom_parameter_file.m](./MATLAB/xt_modules_plot_algrithom_parameter_file.m)
* [xt_sensors_plot_PulseDoppler_Noisemap_message.m](./MATLAB/xt_sensors_plot_PulseDoppler_Noisemap_message.m)

3. Assistant functions
* [add_ModuleConnector_path.m ](./MATLAB/add_ModuleConnector_path.m)
* [disp_module_info.m ](./MATLAB/disp_module_info.m)
* [disp_sensor_settings.m ](./MATLAB/disp_sensor_settings.m)
* [disp_x4_settings.m ](./MATLAB/disp_x4_settings.m)

4. Read recording data directly without using MC
* [read_baseband_IQ_dat_recording_file.m ](./MATLAB/read_baseband_IQ_dat_recording_file.m)

## Python examples

1. Profile messages for the normal user
* [x4m200_print_record_playback_RESP_SLEEP_message.py](./PYTHON/x4m200_print_record_playback_RESP_SLEEP_message.py)
* [x4m300_print_record_playback_PRESENCE_message.py](./PYTHON/x4m300_print_record_playback_PRESENCE_message.py)


2. Radar DSP data message for the advanced user
* [xt_modules_plot_record_playback_radar_raw_data_message_2D.py](./PYTHON/xt_modules_plot_record_playback_radar_raw_data_message_2D.py)  
* [xt_modules_plot_record_playback_radar_raw_data_message_3D.py](./PYTHON/xt_modules_plot_record_playback_radar_raw_data_message_3D.py)          
* [xt_sensors_plot_record_playback_baseband_IQ_AP_message.py](./PYTHON/xt_sensors_plot_record_playback_baseband_IQ_AP_message.py)
* [xt_sensors_print_record_PulseDoppler_Noisemap_message.py](./PYTHON/xt_sensors_print_record_PulseDoppler_Noisemap_message.py)

3. Other X4 based XeThru module examples
* [xt_modules_print_info.py](./PYTHON/xt_modules_print_info.py)
* [xt_modules_record_playback_messages.py](./PYTHON/xt_modules_record_playback_messages.py)  
* [xt_modules_firmware_update.py](./PYTHON/xt_modules_firmware_update.py) 
* [xt_modules_access_registers.py](./PYTHON/xt_modules_access_registers.py)
* [xep_distance_demo_host.py](./PYTHON/xep_distance_demo_host.py)
* [xep_sample_direct_path.py](./PYTHON/xep_sample_direct_path.py)
* [xt_modules_set_certification_mode.py](./PYTHON/xt_modules_set_certification_mode.py)
* [x4m2x0_plot_sleep_csv_recording_file.py](./PYTHON/x4m2x0_plot_sleep_csv_recording_file.py)
* [x4m2x0_plot_vitalsigns_csv_recording_file.py](./PYTHON/x4m2x0_plot_vitalsigns_csv_recording_file.py)

