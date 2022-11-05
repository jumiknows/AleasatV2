# HIL (Hardware-in-the-Loop) Tests

Hardware-in-the-Loop tests validate the firmware while running on actual hardware.

Documentation can be found on the [wiki](https://wiki.aleasat.space/en/space-segment/cdh/hil-tests)

## Basic Setup and Run

From the `hil-tests` directory:
```console
pipenv install
pipenv run ./run.sh --port <serial port>
```
