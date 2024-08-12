from setuptools import setup

setup(name='aleasim',
      version='0.1',
      description='Aleasim is an Python based Spacecraft ADCS Simulation with PIL/HIL test integration',
      url='https://gitlab.com/alea-2020/command-data-handling/obc2-firmware',
      author='ALEASAT ADCS Team',
      author_email='administrator@aleasat.space',
      packages=['aleasim'],
      install_requires=['numpy', 'scipy', 'ahrs', 'skyfield', 'sgp4', 'pyjson5',
                        'pygeomag @ git+https://gitlab.com/alea-2020/adcs/pygeomag.git'
                        ],
      extras_require={},
      zip_safe=False)