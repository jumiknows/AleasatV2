from obc_test_internals.obc_test import main

from pingTest import PingTest
from systemTest import SystemTest
from flashTest import FlashTest
from filesystemTest import FilesystemTest
from epsTest import EpsTest
# from gpsTest import GpsTest

if __name__ == "__main__":
    main([
        PingTest,
        SystemTest,
        FlashTest,
        FilesystemTest,
        EpsTest,
        # GpsTest,
    ])
