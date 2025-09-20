"""Ground Station Client settings file"""

from __future__ import absolute_import, division, print_function

import os
from distutils.util import strtobool  # pylint: disable=E0401,E0611
from os import environ

from dotenv import load_dotenv

# from validators.url import url


def _cast_or_none(func, value):
    try:
        return func(value)
    except (ValueError, TypeError):
        return None


try:
    load_dotenv(dotenv_path='.env', verbose=True)
except EnvironmentError:
    # XXX: Workaround for readthedocs  # pylint: disable=fixme
    pass

# Ground station information
# GROUND_STATION_CLIENT_API_TOKEN = environ.get('GROUND_STATION_CLIENT_API_TOKEN', None)
GROUND_STATION_CLIENT_PRE_OBSERVATION_SCRIPT = environ.get(
    'GROUND_STATION_CLIENT_PRE_OBSERVATION_SCRIPT', None)
GROUND_STATION_CLIENT_POST_OBSERVATION_SCRIPT = environ.get(
    'GROUND_STATION_CLIENT_POST_OBSERVATION_SCRIPT', None)
GROUND_STATION_CLIENT_STATION_ID = _cast_or_none(
    int, environ.get('GROUND_STATION_CLIENT_STATION_ID', None))
GROUND_STATION_CLIENT_STATION_LAT = _cast_or_none(
    float, environ.get('GROUND_STATION_CLIENT_STATION_LAT', None))
GROUND_STATION_CLIENT_STATION_LON = _cast_or_none(
    float, environ.get('GROUND_STATION_CLIENT_STATION_LON', None))
GROUND_STATION_CLIENT_STATION_ELEV = _cast_or_none(
    int, environ.get('GROUND_STATION_CLIENT_STATION_ELEV', None))
GROUND_STATION_CLIENT_GPSD_CLIENT_ENABLED = bool(
    strtobool(environ.get('GROUND_STATION_CLIENT_GPSD_CLIENT_ENABLED', 'False')))
GROUND_STATION_CLIENT_GPSD_HOST = environ.get('GROUND_STATION_CLIENT_GPSD_HOST', '127.0.0.1')
GROUND_STATION_CLIENT_GPSD_PORT = _cast_or_none(
    int, environ.get('GROUND_STATION_CLIENT_GPSD_PORT', 2947))
GROUND_STATION_CLIENT_GPSD_TIMEOUT = _cast_or_none(
    int, environ.get('GROUND_STATION_CLIENT_GPSD_TIMEOUT', 30))

# Output paths
GROUND_STATION_CLIENT_APP_PATH = environ.get('GROUND_STATION_CLIENT_APP_PATH', '/tmp/.satnogs')
GROUND_STATION_CLIENT_OUTPUT_PATH = environ.get('GROUND_STATION_CLIENT_OUTPUT_PATH',
                                                '/tmp/.satnogs/data')
GROUND_STATION_CLIENT_COMPLETE_OUTPUT_PATH = environ.get(
    'GROUND_STATION_CLIENT_COMPLETE_OUTPUT_PATH', '')
GROUND_STATION_CLIENT_INCOMPLETE_OUTPUT_PATH = environ.get(
    'GROUND_STATION_CLIENT_INCOMPLETE_OUTPUT_PATH', '/tmp/.satnogs/data/incomplete')
GROUND_STATION_CLIENT_ARTIFACTS_OUTPUT_PATH = environ.get(
    'GROUND_STATION_CLIENT_ARTIFACTS_OUTPUT_PATH', '/tmp/.satnogs/artifacts')

for p in (
        GROUND_STATION_CLIENT_APP_PATH,
        GROUND_STATION_CLIENT_OUTPUT_PATH,
        GROUND_STATION_CLIENT_COMPLETE_OUTPUT_PATH,
        GROUND_STATION_CLIENT_INCOMPLETE_OUTPUT_PATH,
):
    if p and not os.path.exists(p):
        os.mkdir(p)

GROUND_STATION_CLIENT_REMOVE_RAW_FILES = bool(
    strtobool(environ.get('GROUND_STATION_CLIENT_REMOVE_RAW_FILES', 'True')))
GROUND_STATION_CLIENT_KEEP_ARTIFACTS = bool(
    strtobool(environ.get('GROUND_STATION_CLIENT_KEEP_ARTIFACTS', 'False')))

GROUND_STATION_CLIENT_VERIFY_SSL = bool(
    strtobool(environ.get('GROUND_STATION_CLIENT_VERIFY_SSL', 'True')))

# GROUND_STATION_CLIENT_NETWORK_API_URL = environ.get('GROUND_STATION_CLIENT_NETWORK_API_URL',
#                                       'https://network.satnogs.org/api/')
# GROUND_STATION_CLIENT_NETWORK_API_QUERY_INTERVAL = _cast_or_none(int,
#                                                    environ.get(
#                                                        'GROUND_STATION_CLIENT_NETWORK_API_QUERY_INTERVAL',
#                                                        60))  # In seconds
# GROUND_STATION_CLIENT_NETWORK_API_POST_INTERVAL = _cast_or_none(int,
#                                                   environ.get('GROUND_STATION_CLIENT_NETWORK_API_POST_INTERVAL',
#                                                               180))  # In seconds
GROUND_STATION_CLIENT_NETWORK_API_TIMEOUT = 1800  # In seconds
GNURADIO_UDP_PORT = 16886
GNURADIO_IP = '127.0.0.1'

GROUND_STATION_CLIENT_ROT_ENABLED = bool(
    strtobool(environ.get('GROUND_STATION_CLIENT_ROT_ENABLED', 'True')))
GROUND_STATION_CLIENT_ROT_MODEL = environ.get('GROUND_STATION_CLIENT_ROT_MODEL', 'ROT_MODEL_DUMMY')
GROUND_STATION_CLIENT_ROT_BAUD = int(environ.get('GROUND_STATION_CLIENT_ROT_BAUD', 19200))
GROUND_STATION_CLIENT_ROT_PORT = environ.get('GROUND_STATION_CLIENT_ROT_PORT', '/dev/ttyUSB0')
GROUND_STATION_CLIENT_RIG_IP = environ.get('GROUND_STATION_CLIENT_RIG_IP', '127.0.0.1')
GROUND_STATION_CLIENT_RIG_PORT = int(environ.get('GROUND_STATION_CLIENT_RIG_PORT', 4532))
GROUND_STATION_CLIENT_ROT_THRESHOLD = int(environ.get('GROUND_STATION_CLIENT_ROT_THRESHOLD', 4))
GROUND_STATION_CLIENT_ROT_FLIP = bool(
    strtobool(environ.get('GROUND_STATION_CLIENT_ROT_FLIP', 'False')))
GROUND_STATION_CLIENT_ROT_FLIP_ANGLE = int(environ.get('GROUND_STATION_CLIENT_ROT_FLIP_ANGLE', 75))

# Common script parameters
GROUND_STATION_CLIENT_SOAPY_RX_DEVICE = environ.get('GROUND_STATION_CLIENT_SOAPY_RX_DEVICE', None)
GROUND_STATION_CLIENT_RX_SAMP_RATE = environ.get('GROUND_STATION_CLIENT_RX_SAMP_RATE', None)
# GROUND_STATION_CLIENT_RX_BANDWIDTH = environ.get('GROUND_STATION_CLIENT_RX_BANDWIDTH', None)
GROUND_STATION_CLIENT_DOPPLER_CORR_PER_SEC = environ.get(
    'GROUND_STATION_CLIENT_DOPPLER_CORR_PER_SEC', None)
GROUND_STATION_CLIENT_LO_OFFSET = environ.get('GROUND_STATION_CLIENT_LO_OFFSET', None)
GROUND_STATION_CLIENT_PPM_ERROR = environ.get('GROUND_STATION_CLIENT_PPM_ERROR', None)
GROUND_STATION_CLIENT_GAIN_MODE = environ.get('GROUND_STATION_CLIENT_GAIN_MODE', 'Overall')
GROUND_STATION_CLIENT_RF_GAIN = environ.get('GROUND_STATION_CLIENT_RF_GAIN', None)
GROUND_STATION_CLIENT_ANTENNA = environ.get('GROUND_STATION_CLIENT_ANTENNA', None)
GROUND_STATION_CLIENT_DEV_ARGS = environ.get('GROUND_STATION_CLIENT_DEV_ARGS', None)
GROUND_STATION_CLIENT_STREAM_ARGS = environ.get('GROUND_STATION_CLIENT_STREAM_ARGS', None)
GROUND_STATION_CLIENT_TUNE_ARGS = environ.get('GROUND_STATION_CLIENT_TUNE_ARGS', None)
GROUND_STATION_CLIENT_OTHER_SETTINGS = environ.get('GROUND_STATION_CLIENT_OTHER_SETTINGS', None)
GROUND_STATION_CLIENT_DC_REMOVAL = environ.get('GROUND_STATION_CLIENT_DC_REMOVAL', None)
GROUND_STATION_CLIENT_BB_FREQ = environ.get('GROUND_STATION_CLIENT_BB_FREQ', None)

# ENABLE_IQ_DUMP = bool(strtobool(environ.get('ENABLE_IQ_DUMP', 'False')))
IQ_DUMP_FILENAME = environ.get('IQ_DUMP_FILENAME', None)
# DISABLE_DECODED_DATA = bool(strtobool(environ.get('DISABLE_DECODED_DATA', 'False')))
UDP_DUMP_HOST = environ.get('UDP_DUMP_HOST', None)
UDP_DUMP_PORT = int(environ.get('UDP_DUMP_PORT', 57356))

# Upload settings
# GROUND_STATION_CLIENT_UPLOAD_AUDIO_FILES = bool(strtobool(environ.get('GROUND_STATION_CLIENT_UPLOAD_AUDIO_FILES', 'True')))
# GROUND_STATION_CLIENT_UPLOAD_WATERFALL_FILES = bool(
#     strtobool(environ.get('GROUND_STATION_CLIENT_UPLOAD_WATERFALL_FILES', 'True')))

# Artifacts settings
# ARTIFACTS_ENABLED = bool(strtobool(environ.get('GROUND_STATION_CLIENT_ARTIFACTS_ENABLED', 'False')))
# ARTIFACTS_API_URL = environ.get('GROUND_STATION_CLIENT_ARTIFACTS_API_URL', 'https://db.satnogs.org/api/')
# ARTIFACTS_API_POST_INTERVAL = _cast_or_none(int,
#                                             environ.get('GROUND_STATION_CLIENT_ARTIFACTS_API_POST_INTERVAL',
#                                                         180))  # In seconds
# ARTIFACTS_API_TIMEOUT = 1800  # In seconds
# ARTIFACTS_API_TOKEN = environ.get('GROUND_STATION_CLIENT_ARTIFACTS_API_TOKEN', None)

# Waterfall settings
# GROUND_STATION_CLIENT_WATERFALL_AUTORANGE = bool(strtobool(environ.get('GROUND_STATION_CLIENT_WATERFALL_AUTORANGE', 'True')))
# GROUND_STATION_CLIENT_WATERFALL_MIN_VALUE = int(environ.get('GROUND_STATION_CLIENT_WATERFALL_MIN_VALUE', -100))
# GROUND_STATION_CLIENT_WATERFALL_MAX_VALUE = int(environ.get('GROUND_STATION_CLIENT_WATERFALL_MAX_VALUE', -50))

# Logging configuration
LOG_FORMAT = '{levelname:<8s} {name:<31s} {message}'
LOG_LEVEL = environ.get('GROUND_STATION_CLIENT_LOG_LEVEL', 'WARNING')
SCHEDULER_LOG_LEVEL = environ.get('GROUND_STATION_CLIENT_SCHEDULER_LOG_LEVEL', 'WARNING')

# Sentry
SENTRY_DSN = environ.get(
    'SENTRY_DSN',
    'https://glet_dd022b8312e28c32d34df70d0c400ab1@observe.gitlab.com:443/errortracking/api/v1/projects/51807169'
)
SENTRY_ENABLED = bool(strtobool(environ.get('SENTRY_ENABLED', 'False')))

REQUIRED_VARIABLES = [
    # 'GROUND_STATION_CLIENT_API_TOKEN',
    'GROUND_STATION_CLIENT_STATION_ID',
    'GROUND_STATION_CLIENT_SOAPY_RX_DEVICE',
    'GROUND_STATION_CLIENT_RX_SAMP_RATE',
    'GROUND_STATION_CLIENT_ANTENNA',
]


def validate(logger):
    """Validate the provided settings:
    - Check for the existance of all required variables
    - Validate format of the provided value for some required variables

    Since this module has to be loaded before the logger has been initialized,
    this method requires a configured logger to be passed.

    Arguments:
    logger -- the output logger
    """
    settings_valid = True

    # Get all variable in global scobe (this includes all global variables from this module)
    settings = globals()

    for variable_name in REQUIRED_VARIABLES:
        # Check the value of the variable defined in settings
        if not settings[variable_name]:
            logger.error('%s not configured but required', variable_name)
            settings_valid = False

    # try:
    #     url(GROUND_STATION_CLIENT_NETWORK_API_URL)
    # except ValueError:
    #     logger.error('Invalid GROUND_STATION_CLIENT_NETWORK_API_URL: %s', GROUND_STATION_CLIENT_NETWORK_API_URL)
    #     settings_valid = False

    if not (GROUND_STATION_CLIENT_STATION_LAT or GROUND_STATION_CLIENT_GPSD_CLIENT_ENABLED):
        logger.error('GROUND_STATION_CLIENT_STATION_LAT not configured')
        settings_valid = False

    if not (GROUND_STATION_CLIENT_STATION_LON or GROUND_STATION_CLIENT_GPSD_CLIENT_ENABLED):
        logger.error('GROUND_STATION_CLIENT_STATION_LON not configured')
        settings_valid = False

    if not (GROUND_STATION_CLIENT_STATION_ELEV or GROUND_STATION_CLIENT_GPSD_CLIENT_ENABLED):
        logger.error('GROUND_STATION_CLIENT_STATION_ELEV not configured')
        settings_valid = False

    return settings_valid
