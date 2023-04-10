# obcpy

`obcpy` is a Python package for communicating with the ALEASAT On-Board Computer (OBC).

## Package Structure

The top-level package is called `obcpy`. This package contains multiple sub-packages and sub-modules. The `obcpy` package does not import or define any names itself (since users may only require the functionality of certain subpackages). Therefore, users must explicitly import the sub-packages and/or sub-modules that are required for a given script / application.

### Sub-Packages

**Main Sub-Package**

`obcpy.obc`
* Defines the `OBC` class which is the primary entrypoint for communicating with the OBC.

**Support Sub-Packages**

`obcpy.data_fmt`
* Classes to handle the encoding and decoding of data fields to/from a binary format

`obcpy.cmd_sys`
* Classes to represent OBC commands and responses and the data format specifications for those commands / responses

`obcpy.protocol`
* General-purpose library for representing and implementing a multi-layer, multi-threaded, bi-directional communication protocol.

`obcpy.device`
* Contains general-purpose drivers for hardware device interfaces (e.g. `obcpy.device.serial` contains a general-purpose serial port driver)

`obcpy.utils`
* Miscellaneous utility functions / classes
* Does not define / import any names directly. Users must import the specific sub-module they would like to use
