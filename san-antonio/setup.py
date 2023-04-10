from setuptools import setup

setup(name='sanantonio',
      version='0.1',
      description='San Antonio',
      url='https://gitlab.com/alea-2020/command-data-handling/obc2-firmware',
      author='ALEASAT',
      author_email='administrator@aleasat.space',
      packages=['sanantonio'],
      install_requires=[
          'pyserial',
          'pyqt5',
      ],
      zip_safe=False)
