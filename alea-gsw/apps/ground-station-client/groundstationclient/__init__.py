"""Ground Station Client module initialization"""

from __future__ import absolute_import, division, print_function

import logging
import logging.config
import sys

import pkg_resources
import sentry_sdk

from groundstationclient import settings
from groundstationclient.locator import locator
from groundstationclient.scheduler.tasks import status_listener
from groundstationclient.server.server import app  # noqa: F401

# import time

logging.basicConfig(format=settings.LOG_FORMAT,
                    level=getattr(logging, settings.LOG_LEVEL),
                    style='{')
LOGGER = logging.getLogger(__name__)

if settings.SENTRY_ENABLED:
    # https://github.com/python-poetry/poetry/issues/273
    version = pkg_resources.get_distribution('ground-station-client').version
    sentry_sdk.init(settings.SENTRY_DSN, release='ground-station-client@{}'.format(version))

if not settings.validate(LOGGER):
    LOGGER.error('Settings are invalid, exiting...')
    sys.exit(-1)

LOGGER.info('Starting status listener thread...')
gps_locator = locator.Locator()
gps_locator.update_location()
status_listener()
