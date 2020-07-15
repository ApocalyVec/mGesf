import setuptools

with open("README.md", "r") as fh:
    long_description = fh.read()

requires = ['PyQt5',
            'numpy',
            'pyqtgraph',
            'pyserial',
            'matplotlib',
            'qimage2ndarray',
            'pydub',
            'ffmpeg',
            'pyautogui',
            'pandas',
            'tensorflow'
            ]

setuptools.setup(
    name="mGesf",  # Replace with your own username
    version="0.0.1",
    author="ApocalyVec",
    author_email="s-vector.lee@hotmail.com",
    description="This package is a micro-gesture application that took on a sensor fusion approach/",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/ApocalyVec/mGesf",
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
    ],
    python_requires='>=3.6',
    install_requires=requires)

import os

os.chdir('utils/XeThru_utils/pymoduleconnector')
import subprocess

subprocess.call(['python', 'setup.py', 'install'])

# from setuptools import sandbox
# sandbox.run_setup('/Users/Leo/PycharmProjects/mGesf/utils/XeThru_utils/pymoduleconnector/setup.py', ['clean', 'bdist_wheel'])
