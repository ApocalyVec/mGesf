import sys
import os
from setuptools import setup

def read_version():
    try:
        with open('../build.properties') as f:
            for line in f:
                if "version=" in line:
                    break
    except IOError:
        print("Error opening version file. If this is from a zip package you "
                "should use `python setup.py install`")
        raise SystemExit(1)
    version = line.split('=')[1].strip()

    if version == '':
        import sys
        print("ERROR: Version string not found, something is wrong")
        sys.exit(1)
    return version

if not 'install' in sys.argv:
    # Generate version number from the project build.properties file
    version = read_version()
    s="\nversion = '%s'\n"""%version

    with open("pymoduleconnector/version.py", 'w') as f:
        f.write(s)

sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)),
    'pymoduleconnector'))
from version import version as __version__

setup(name='pymoduleconnector',
      version= __version__,
      description='A Python wrapper for XeThru ModuleConnector',
      long_description="",
      url='http://community.xethru.com',
      author='Novelda AS',
      author_email='support@xethru.com',
      #TODO: license
      #license='MIT',
      packages=[
          'pymoduleconnector', 'pymoduleconnector.moduleconnectorwrapper',
          'pymoduleconnector.examples', 'pymoduleconnector.extras',
          'pymoduleconnector.doc'
      ],
      install_requires=[
      ],
      extras_require={
          'auto': ['pyserial']
      },
      entry_points={
          'console_scripts': ['xt_x2m200resp=pymoduleconnector.examples.x2m200_respiration_example:main'],
      },
      include_package_data=True,
      zip_safe=False,
      )
