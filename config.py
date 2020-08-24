WINDOW_HEIGHT = 720
WINDOW_WIDTH = 720

# ============== labels ==============
# button labels

interrupt_btn_label = "Interrupt"
test_btn_start_label = "Start testing"
test_btn_end_label = "End testing"
count_sample_btn_label = "Count Samples"
train_btn_label = "Train"
connection_btn_label = "Connect"
record_btn_start_label = 'Start Recording'
record_btn_end_label = 'End Recording'

help_btn_label = "Help"
send_config_btn_label = 'Send Config'
sensor_btn_label = 'Start Sensor'
detection_load_btn_label = 'Load Data-Model'
detection_start_btn_label = 'Start Detection'
detection_end_btn_label = 'Stop Detection'
prob_view_btn_label = 'Prob. View'

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

# uwb control tab
uwb_anchor_port = 'Anchor port: '
uwb_tag_port = 'Tag port: '
uwb_tag_default_port = 'COM30'
uwb_anchor_default_port = 'COM32'

# xeThruX4 control tab
init_xeThruX4_data_dict = {'frame': None,
                           'baseband_frame': None,
                           'clutter_removal_frame': None,
                           'clutter_removal_baseband_frame': None,
                           'ir_spectrogram': None}
xethrux4_default_com_port = 'COM13'
xethrux4_default_fps = 25

# gesture tab
gesture_index_pen_label = "Index Pen"
gesture_thuMouse_label = "ThuMouse"
gesture_desktop_fingertip_label = "Desktop at Fingertip"
init_buffer = {'mmw': {'timestamps': [], 'range_doppler': [], 'range_azi': [], 'detected_points': []},
               'xethrux4': {'timestamps': [], 'ir': [], 'ir_baseband': [], 'ir_cr': [], 'ir_basedband_cr': []}}

# operation tab
unit_size = WINDOW_WIDTH / 6
counter_size_factor = 1.5
ist_text_size_factor = 3
base_size = 512
instruction_block_size = (unit_size * (counter_size_factor + ist_text_size_factor), base_size)
counter_block_size = (unit_size * counter_size_factor, base_size)  # a quarter of the instruction block's width
ist_text_block_size = (unit_size * ist_text_size_factor, base_size)
dtc_text_block_size = (base_size, base_size)
prob_view_window_size = (1280, 720)
interaction_window_size = (800, 800)

operation_recording_label = "Recording"
operation_training_label = "Training"
operation_detection_label = "Detection"

trainingDataPath_label = "Training Data Path"
operation_model_path_label = "Model Path"

operation_interval_label = "Interval lasts (sec) [NOT IMPLEMENTED]"
operation_repeats_label = "Repeats (times)"
operation_classes_label = "Classes"
operation_subject_name_label = "Subject Name"
operation_model_dir_label = "Model Directory"
operation_training_constructor_path_label = "Constructor Path"

# ============== default inputs ==============

# control tab
control_tab_d_port_default = "COM10"
control_tab_u_port_default = "COM11"

# operation tab
# indexPen_classes_default = "A B C D E"
indexPen_classes_default = 'A B C D E F G H I J K L M N O P Q R S T U V W X Y Z Spc Bspc Ent Act'

indexPen_subjectName_default = "god"
indexPen_repeatTime_default = 10
indexPen_trainingDataDir_default = "data"
indexPen_modelPath_default = "models/idp/idp_29_2020-05-04_03-24-10.425555.h5"
indexPen_modelDir_default = "../model/"
indexPen_constructorPath_default = "../learn/idp_classifier.py"

# thumouse
thuMouse_subjectName_default = "someone"
thuMouse_TrainingDataDir_default = "../data"
thuMouse_constructorPath_default = "../learn/Thu_classifier.py"
thuMouse_modelDir_default = "../model/"
thuMouse_modelPath_default = "../model/idp_model.h5"
thuMouse_repeatTimes_default = 4
# ============== messages ==============
config_set_message = "Config file set"
config_invalid_message = "Invalid config path"
datapath_set_message = "data path set"
datapath_invalid_message = "Invalid data path"

control_tab_start_sensor_message = "Sensor running"
control_tab_stop_sensor_message = "Sensor stopped"
control_tab_UDport_disconnected_message = "Disconnected to ports"
control_tab_UDport_connected_message = "Connected to ports"

# indexPen instruction area text
countdown_animation_text = ["Steady...", "..Ready,", "GO!"]
instruction_next_text = "... next: "
instruction_end_text = "NO NEXT"

control_tab_config_file_path_default = 'mGesf/profiles/aop/EV2/30fps_rdHeatmap_aziHeatmap.cfg'
data_path_default = '../data'

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
recording_interval = 4
repeat_times = 10

# ============== links ==============
# recording tab
help_link = "https://docs.google.com/document/d/1KvSAkytYb9Xw-CEyHy4yt82QQG81bF1LhDo5BtfKlxM/edit?usp=sharing"
