## Changelog {#changelog}

1.6.2 (2018-10-16)

	Added flush_on_write option in RecordingOptions
	Added RecordingErrorCallback in DataRecorder.
	Improved Unicode filenames support.
	Support for various number formats and settings for getting radar data.
	Fixes to matlab documentation.
	Fix datatype usage in matlab.

1.5.3 (2018-06-22)

	Implemented missing get calls in x4driver API.
	Implemented get calls for sensitivity and led_control.
	Added get/set tx_center_frequency.
	Added C++ TCP/IP example.
	Added matlab to osx distribution.
	Updated RPi target to Raspbian based on Debian stretch.
	Added vitalsigns.
	Added normalizedmovementlist.
	RESPIRATION_4/5 support with smaller buffers for file i/o.
	Added iopin API to X4M200/X4M300 interface.
	Changed example names, minor cleanups.
	Added flush_on_write to RecordingOptions.
	Added DataRecorder::get_recording_directory.
	Updated noisemap_control documentation.
	Performance improvements.

1.4.3 (2017.11.28)

	Fixed build for Raspberry Pi 1/Zero.
	Many unaligned accesses fixed for Raspberry Pi.
	X4M300 MovingList bugfix in Matlab API.
	More reliable fetching of parameter file when recording.

1.4.2 (2017.09.15)

	Examples cleanup
	Added runtime dependencies to python modules on windows

1.4.1 (2017.09.13)

	Added updated examples
	Fixed bug in matlab ModuleConnector constructor
	Fixed python DataPlayer support
	Fixed unaligned memory access while recording respiration messages on raspberry pi

1.4.0 (2017.09.11)

	Renamed library to libModuleConnector for all platforms
	Added python3 support
	Added 32bit python support on windows
	Added TCP/IP support
	Added pulse-Doppler and noisemap outputs
	Added method to read/write multiple bytes at once to X4 SPI register
	Updated examples
	Added pymoduleconnector.ids python module
	Fixed race condition in debug output

1.2.2 (2017.06.23)

	Added set_noisemap_control to X4M300 and X4M200 interfaces
	Added set_parameter_file to X4M300 and X4M200 interfaces
	Added get_profileid to X4M300 and X4M200 interfaces
	Added support for X4M200 interface
	Added support for storing the sensor's parameter file to disk when recording
	Added support for storing the sensor's profile id in recording meta header
	Fixed library issues on Raspberry Pi
	Fixed bug in DataPlayer preventing backwards seek_ms from working correctly
	Fixed bug in DataPlayer causing problems with pulse doppler float packets

1.1.8 (2017.05.23)

	Added support for playback of recorded data via DataPlayer interface
	DataRecorder stores parameter file from module (currently no support in FW)
	DataReader improved performance
	DataReader fixed bug related to filtering of data
	Added function to disable baseband output from X2M200
	Improved performance when accessing const Byte references from Python
	Added module file system API to XEP interface.
	Added set_baudrate() to X4M300 interface.
	Added reset_to_factory_preset() to X4M300 interface.
	Renamed methods from peak_*  to peek_*

1.1.5 (2017.03.08)

	Add support for X4M300 and XEP interfaces.

1.0.0 (2017.02.01)

	This is the initial release of ModuleConnector which support communication with X2M200 and data recording.


