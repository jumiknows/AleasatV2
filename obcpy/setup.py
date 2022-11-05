from setuptools import setup

setup(name='obcpy',
      version='0.1',
      description='OBC Python API',
      url='https://gitlab.com/alea-2020/command-data-handling/obc2-firmware',
      author='ALEASAT',
      author_email='administrator@aleasat.space',
      packages=['obcpy'],
      install_requires=[
          'pyserial',
      ],
      zip_safe=False)
