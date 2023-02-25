# MicroGesture SensorFusion (mGesf)

Check out our [video](https://drive.google.com/file/d/1wNtAK8W8OSPjI1Kx1LN0ByB2U8i-aJUJ/view?usp=sharing) 

This repo extends the work in this two papers: [ThuMouse: A Micro-gesture Cursor Input through mmWave Radar-based Interaction
](https://ieeexplore.ieee.org/document/9043082) and [IndexPen: Two-Finger Text Input with Millimeter-Wave Radar](https://dl.acm.org/doi/abs/10.1145/3534601?casa_token=WrCaOdiA2TEAAAAA:pVsCqW-sZAjfqxEPoi66yL3PIVf_JuCfGWPDklqONgGUjUDahJmrZ0SC4VudWOvGb1vjIRglK2wiZA). Cite us in your work if you find the code we shared help your project.


This application supports a variety of sensors, including:
* Texas Instrument mmWave Sensors: IWR6843ISK, IWR6843AoP
* LeapMotion (coming soon)
* DecaWave UWB sensors (coming soon)


To run this application, first make sure you have the GUI backend PyQT5 installed. Depending on your O.S., use the following commands:

ubuntu: `sudo apt-get install python3-pyqt5`
MacOs: `brew install pyqt`

Then install the package in your python interpreter: 
`pip install pyqt5`

first run this command to install the prerequisites.
```bash
python setup.py install
```

Now run this app with this command
```bash
python main.py
```

Make sure you have the audio library installed:
Mac:
```bash
brew install ffmpeg
```
Ubuntu:
```bash
sudo apt-get install ffmpeg
```
For Windows, it's a bit more involved as the downloaded library needs to be added to the environment variables.
This is a good [tuorial](http://blog.gregzaal.com/how-to-install-ffmpeg-on-windows/).

If you are getting 'permission denied' errors (likely to occur on Windows), install the simpleaudio package by running:
```bash
pip install simpleaudio
```
