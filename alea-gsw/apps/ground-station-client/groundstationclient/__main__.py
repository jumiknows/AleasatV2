"""Ground Station Client module main function"""

from __future__ import absolute_import, division, print_function

import sys

# import groundstationclient
from groundstationclient.server.server import serve

if __name__ == '__main__':
    sys.exit(serve())
