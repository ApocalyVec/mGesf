import subprocess
from pydub import AudioSegment
from pydub.playback import play
import threading
import ffmpeg

dih_sound = AudioSegment.from_wav('resource/sound/dih.wav')

dah_sound = AudioSegment.from_wav('resource/sound/dah.wav')


def dih_sub():
    subprocess.call(['afplay', 'resource/sound/dih.wav'])


def dah_sub():
    subprocess.call(['afplay', 'resource/sound/dah.wav'])


def dih():
    t = threading.Thread(target=play, args=(dih_sound,))
    t.start()


def dah():
    t = threading.Thread(target=play, args=(dah_sound,))
    t.start()
