# ALEASIM

Aleasim is a spacecraft simulator primarily focused on simulating the ADCS and other ADCS related subsystems of ALEASAT.

## Architecure

Aleasim is designed to be modular such that the implementation of spacecraft models can be seperated from the core implementation of the simular (the aleasim kernel).

The key components of Aleasim are:
- kernel - responsible for startup/shutdown, connecting all the models, integrating states and advancing the simulation time, and providing utility classes
- models - Aleasim is composed of multiple spacecraft and environment models which implement a generic AleasimModel class, and have a tree heirachy with parent and children models
- analyser - responsible for extracting/saving/plotting sim data, generating animations, and conducting statistical analysis of the simulation

**Important Directories**
- `alea/sim` - top level package, installable and build-able via poetry
    - `epa` - stands for "Environment, Position and Attitude" , contains all environment, orbit, attitude models, and reference frame code
    - `algorithms` - contains adcs related algorithms implemented in Python
    - `kernel` - core simulation functionality is contained here, including the kernel, generic models, scheduler, frame maneger etc
    - `legacy` - old code from ADCS simulations developed prior to aleasim, will likely get removed by Aleasim V1.0
    - `math`
    - `spacecraft` - spacecraft hardware models including sensors, actuators and electrical power system (EPS)
- `notebooks` - demonstration code and analysis notebooks
- `test` - unit and integration tests to validate aleasim

### File Tree
```bash
alea/sim/
├── Pipfile
├── Pipfile.lock
├── aleasim
│   ├── __init__.py
│   ├── aleasim.py
│   ├── configuration
│   ├── epa
│   │   ├── attitude_dynamics.py
│   │   ├── disturbances
│   │   │   └── ...
│   │   ├── frame_conversions.py
│   │   ├── earth_magnetic_field.py
│   │   ├── orbit_dynamics.py
│   │   └── ...
│   ├── algorithms
│   │   ├── attitude_determination
│   │   │   └── __init__.py
│   │   ├── control
│   │   │   └── __init__.py
│   │   └── sensor_processing
│   │       └── __init__.py 
│   ├── kernel
│   │   ├── aleasim_model.py
│   │   ├── config_loader.py
│   │   ├── frame_manager.py
│   │   ├── frames.py
│   │   ├── generic
│   │   │   ├── abstract_model.py
│   │   │   ├── attitude_hardware_model.py
│   │   │   ├── dynamic_model.py
│   │   │   └── powered_unit_model.py
│   │   ├── graphics
│   │   │   ├── anim_attitude.py
│   │   │   ├── graphics.py
│   │   │   └── ... 
│   │   ├── kernel.py
│   │   ├── scheduler.py
│   │   ├── time_utils.py
│   │   └── ... 
│   ├── math_lib
│   │   ├── math.py
│   │   ├── math_ahrs.py
│   │   ├── math_scipy.py
│   │   └── ... 
│   ├── spacecraft
│   │   ├── actuators
│   │   │   └── ... 
│   │   ├── eps
│   │   │   └── ... 
│   │   ├── flight_software
│   │   │   └── ... 
│   │   ├── sensors
│   │   │   └── ... 
│   │   ├── spacecraft.py
│   │   └── ...
│   └── version.txt
├── aleasim_readme.md
├── demos
│   └── ... 
├── setup.py
└── test
│   └── ... 
```

## Progress 

### Kernel
- [x] Time and Reference Frame Management
- [x] Scheduler

### Analyser
- [x] Shared memory interface
- [x] Simple grahpics demo

### Models
- [x] Spacecraft - Parent Model
- [x] Spacecraft -Attitude Dynamics
- [x] Spacecraft - Orbital Dynamics
- [x] Magnetic Field Model
- [x] Earth, Sun, Moon Ephimeris Models
- [x] Simple Spacecraft sensor and actuator models
- [x] Pd Control function for slew maneuvers
- [x] Wahba solvers for attitude determination
- [ ] Spacecraft - Magnetometer Model
- [ ] Spacecraft - Coarse Sun Sensor Model
- [ ] Spacecraft - Gyroscope Model
- [ ] Spacecraft - IMU Model
- [ ] FSW - CDH / state machine model
- [ ] FSW - Attitude Estimator Models (EKF, etc)
- [ ] FSW - Controller Models (PID, BDOT, etc)

## Developer Environment

By virtue of Python and pipenv, aleasim's install is very flexible.
A Python version of `3.10.12` was used for the development and testing of aleasim v0.1 , and the execution of this very notebook. But other versions near 3.10 (3.7-3.12) should do just fine. Please correct this document if this turns out to not be the case.

### Installation Instructions

Useful cheat sheets:
* https://about.gitlab.com/images/press/git-cheat-sheet.pdf
* https://gist.github.com/karthiks/1700a56b7bfb79e6e1c345230c2e26b6
* https://gist.github.com/bradtraversy/c70a93d6536ed63786c434707b898d55
* https://www.geeksforgeeks.org/linux-commands-cheat-sheet/

1. Complete CDH environment setup **until you finish the 'cloning obc2-firmware' step**, https://wiki.aleasat.space/en/space-segment/cdh/dev-env.
2. Install Python 3.10 (consider using https://github.com/pyenv/pyenv to manage python versions)
3. [Install Poetry](https://python-poetry.org/docs/#installing-with-the-official-installer)
    * Configure poetry to use in-project virtual environments
        ```
        poetry config virtualenvs.in-project true
        ```
4. Clone this repository (if you haven't yet) - `git clone https://gitlab.com/alea-2020/command-data-handling/obc2-firmware.git`
5. Navigate to alea-sim directory in your terminal - `cd <obc2-firmware directory>/alea-fsw/python/alea-sim`
6. Install alea-sim and all dependencies:
    ```
    poetry install
    ```
7. If you haven't already, install the vscode "Jupyter" and "Python" extensions. The debugger and jupyter notebook viewer are **extremely** helpful for python development, testing and analysis.
8. Now Develop, Test, Iterate, and Launch ALEASAT!

Additional notes on running tests:
* Activate the poetry environment by running `poetry shell`. All python commands should be run with the poetry environment activated
* Run test cases with `python test/<testcase_name>.py` or `python test/main.py`
* For Running Python scripts or notebooks through VsCode, install Python and Jupyter extensions, and configure interpreter to the poetry environment (in the .venv folder)
