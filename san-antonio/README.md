# San Antonio

San Antonio is a serial terminal and test interface for the ALEASAT OBC firmware.

## Development Environment

This project uses [Pipenv](https://github.com/pypa/pipenv) to manage dependencies

1. Install Pipenv if you haven't already

    ```
    pip install --user pipenv
    ```
2. Activate the Pipenv environment (from the `san_antonio` directory if using Python 3.7, or `python_3_8` folder if Python 3.8)

    ```
    pipenv shell
    ```
3. Install dependencies

    ```
    pipenv install
    ```

**A couple notes about Pipenv**

* Dependencies are stored in the `Pipfile` (instead of a typical `requirements.txt`)
* The exact packages installed are stored in Pipfile.lock (similar to the results of `pip freeze`)
* Instead of keeping the virtual environment in your working directly, Pipenv keeps all virtual environments in a central location for the user
  * Run `which python` (in a bash shell) after activating the Pipenv environment to see where it's stored on your system
* Pipenv tutorial and more info: https://realpython.com/pipenv-guide/

## Running

After activating the Pipenv environment

```
python src/main.py
```

## Installing Additional Packages

To install additional python packages run
```
pipenv install <package name>
```

**NOTE:** Use `pipenv` not `pip` to install packages or else they won't be tracked in the `Pipfile`

## Modifying the GUI

1. Launch Qt Designer from `<virtual env path>/Lib/site-packages/qt5_applications/Qt/bin/designer.exe`
    * See the notes about Pipenv above to find out where your virtual env path is
2. Click "Open..." and select the `san_antonio.ui` file
1. Make your changes and save the file
2. Run the following command from the `src` folder
    ```
    pyuic5 san_antonio.ui -o san_antonio.py
    ```

## Changing Settings

If you want the settings change to work edit the constants in main.py or use the settings menu.

**NOTE:** The labels on the menu won't change but they will toggle the settings.
