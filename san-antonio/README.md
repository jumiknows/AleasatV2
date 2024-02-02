# San Antonio

San Antonio is a GUI for interacting with the ALEASAT OBC firmware.

Documentation can be found on the [wiki](https://wiki.aleasat.space/en/space-segment/cdh/san-antonio)

## Basic Setup and Run

From the `san-antonio` directory:
```console
pipenv install
pipenv run python main.py
```

## Build (general)

To build a standalone executable of San Antonio for current OS 
- If building from Linux, creates a linux binary, windows a windows binary, etc.

```console
cd san-antonio
pipenv install
pipenv run python build.py
```

Alternatively, run the Vscode task:

***CTRL + SHIFT + B***

Select `San Antonio - Build Executable`


The binary is output at /san-antonio/dist/SanAntonio


