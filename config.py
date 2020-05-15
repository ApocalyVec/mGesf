WINDOW_HEIGHT = 720
WINDOW_WIDTH = 1280

# ============== labels ==============
# button labels

interrupt_btn_label = "Interrupt"
test_btn_label = "Start/end test"
count_sample_btn_label = "Count Samples"
train_btn_label = "Train"
radar_connection_btn_label = "Connect"
record_btn_label = 'Start Recording'
help_btn_label = "Help"
send_config_btn_label = 'Send Config'
sensor_btn_label = 'Start Sensor'
detection_load_btn_label = 'Load Data&Model'
detection_start_Stop_btn_label = 'Start/Stop Detection'

# main window
main_window_control_tab_label = "CONTROL"
main_window_radar_tab_label = "RADAR"
main_window_leap_tab_label = "LEAP"
main_window_uwb_tab_label = "UWB"
main_window_gesture_tab_label = "Gesture"

# control tab
control_tab_user_port_label = 'User Port (Enhanced): '
control_tab_data_port_label = 'Data Port (Standard): '
control_tab_output_path_label = 'Output path:'
control_tab_config_path_label = 'Config path:'

# gesture tab
gesture_index_pen_label = "Index Pen"
gesture_thuMouth_label = "ThuMouth"
gesture_desktop_fingertip_label = "Desktop at Fingertip"

# operation tab
operation_recording_label = "Recording"
operation_training_label = "Training"
operation_detection_label = "Detection"

operation_training_data_path_label = "Training Data Path"
operation_model_path_label = "Model Path"

operation_interval_label = "Interval lasts (sec)"
operation_repeats_label = "Repeats (times)"
operation_classes_label = "Classes"
operation_subject_name_label = "Subject Name"
operation_model_dir_label = "Model Directory"
operation_training_constructor_path_label = "Constructor Path"

# ============== default inputs ==============

# control tab
control_tab_d_port_default = "COM14"
control_tab_u_port_default = "COM3"

# operation tab
operation_classes_default = "A B C D E"
operation_subject_name_default = "god"
operation_training_data_dir_default = "data/"
operation_model_path_default = "../model/idp_model.h5"
operation_model_dir_default = "../model/"
operation_constructor_path_default = "../learn/idp_classifier.py"

# ============== messages ==============
config_set_message = "Config file set"
config_invalid_message = "Invalid config path"
datapath_set_message = "data path set"
datapath_invalid_message = "Invalid data path"

control_tab_start_sensor_message = "Sensor running"
control_tab_stop_sensor_message = "Sensor stopped"
control_tab_UDport_disconnected_message = "Disconnected to ports"
control_tab_UDport_connected_message = "Connected to ports"

control_tab_config_file_path_default = 'default: mGesf/profiles/aop/30fps_azi_rd.cfg'
data_path_default = 'default: ../data'

radar_box_checked = "Checked: Radar recording box"
radar_box_unchecked = "Unchecked: Radar recording box"
leap_box_checked = "Checked: Leap recording box"
leap_box_unchecked = "Checked: Leap recording box"
UWB_box_checked = "Checked: UWB recording box"
UWB_box_unchecked = "Checked: UWB recording box"

color_pink = '#fff5f6'
color_white = '#ffffff'
color_gray = '#f0f0f0'
color_bright = '#E8E9EB'

button_color = color_white
subcontainer_color = color_white
container_color = color_bright

button_style_classic = "background-color: " + button_color + "; border-style: outset; border-width: 2px; " \
                                                             "border-radius: 10px; " \
                                                             "border-color: gray; font: 12px; min-width: 10em; " \
                                                             "padding: 6px; "

# ============== values ==============

rd_shape = (8, 16)
ra_shape = (8, 64)

# operation tab
# # recording tab
recording_interval_range = 30
recording_repeat_range = 10

# ============== links ==============
# recording tab
help_link = "https://docs.google.com/document/d/1KvSAkytYb9Xw-CEyHy4yt82QQG81bF1LhDo5BtfKlxM/edit?usp=sharing"
