import subprocess

import threading

import ffmpeg

"""
change audio library depending on the os
"""
dih_path = 'mGesf/resource/sound/dih.wav'
dah_path = 'mGesf/resource/sound/dah.wav'
from sys import platform
if platform == "linux" or platform == "linux2" or platform == "darwin":
    from pydub import AudioSegment
    from pydub.playback import play
    dih_sound = AudioSegment.from_wav(dih_path)
    dah_sound = AudioSegment.from_wav(dah_path)
    def dih():
        t = threading.Thread(target=play, args=(dih_sound,))
        t.start()
    def dah():
        t = threading.Thread(target=play, args=(dah_sound,))
        t.start()

elif platform == "win32":
    # Windows...
    import winsound

    def dih():
        t = threading.Thread(target=winsound.Beep, args=(900, 500))
        t.start()
        # winsound.PlaySound(dih_path, winsound.SND_FILENAME | winsound.SND_NOWAIT | winsound.SND_ASYNC)

    def dah():
        t = threading.Thread(target=winsound.Beep, args=(1500, 500))
        t.start()
        # winsound.PlaySound(dah(), winsound.SND_FILENAME | winsound.SND_NOWAIT | winsound.SND_ASYNC)

# def dih_sub():
#     subprocess.call(['afplay', 'mGesf/resource/sound/dih.wav'])
#
#
# def dah_sub():
#     subprocess.call(['afplay', 'mGesf/resource/sound/dah.wav'])


