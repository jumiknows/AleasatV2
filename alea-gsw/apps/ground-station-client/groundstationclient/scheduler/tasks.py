from __future__ import absolute_import, division, print_function

# import base64
# import json
import logging
# import os
import threading
from datetime import datetime, timedelta

import pytz
from apscheduler.jobstores.base import JobLookupError
# import requests
from dateutil import parser

from groundstationclient.db import ContactProfile, Session
# from groundstationclient import settings
# from groundstationclient.locator import locator
from groundstationclient.observer.observer import Observer
from groundstationclient.scheduler import SCHEDULER

# try:
#     from urllib.parse import urljoin
# except ImportError:
#     from urlparse import urljoin

LOGGER = logging.getLogger(__name__)
OBSERVER_LOCK = threading.Lock()


def spawn_observer(**kwargs):
    obj = kwargs.pop('contact')
    # we aren't providing a tle0 but its just a name that isn't used so we can leave it blank
    tle = {
        'tle0': obj['spacecraft']['tle0'],
        'tle1': obj['spacecraft']['tle1'],
        'tle2': obj['spacecraft']['tle2'],
    }
    end = parser.parse(obj['reservationEndTime'])

    observer = Observer()

    # # Get the baudrate. In case of CW baudrate equals the WPM
    # baud = 0
    # if 'baud' in obj:
    #     baud = obj['baud']

    session = Session()
    contact_profile = None
    with Session() as session:
        contact_profile = (session.query(ContactProfile).get(
            obj['contactProfile']['id']).to_dict())

    setup_kwargs = {
        'observation_id': obj['id'],
        'tle': tle,
        'observation_end': end,
        'frequency': contact_profile['centerFrequencyMHz'],
        # new ones
        'bw': contact_profile['bandwidthMHz'],
        'downlink_destination_ips': contact_profile['downlinkDestinationIPs'],
        'uplink_origin_ip': contact_profile['uplinkOriginIP'],
        'save_iq_files': contact_profile['saveIQFiles'],
        'tx_start_time': obj['txStartTime'],
        'tx_end_time': obj['txEndTime'],
        'rx_start_time': obj['rxStartTime'],
        'rx_end_time': obj['rxEndTime'],
        'start_azimuth_degrees': obj['startAzimuthDegrees'],
        'end_azimuth_degrees': obj['endAzimuthDegrees'],
        # removed
        # 'mode': obj['mode'],
        # 'baud': baud
    }

    LOGGER.debug('Observer args: %s', setup_kwargs)

    if observer.setup(**setup_kwargs):
        LOGGER.debug('Spawning observer worker.')
        timeout_timedelta = end - datetime.now(pytz.utc)
        if timeout_timedelta.total_seconds() <= 0:
            timeout_timedelta = timedelta()
        if not OBSERVER_LOCK.acquire(timeout=timeout_timedelta.total_seconds()):
            LOGGER.error('Observer job lock acquiring timed out.')
            return
        try:
            observer.observe()
        except Exception as ex:  # pylint: disable=broad-except
            LOGGER.error(
                'Observation aborted due to an unhandled exception %s',
                ex,
                exc_info=True,
            )
        finally:
            OBSERVER_LOCK.release()
    else:
        raise RuntimeError('Error in observer setup.')


# def keep_or_remove_file(filename):
#     # If set, move uploaded file to `GROUND_STATION_CLIENT_COMPLETE_OUTPUT_PATH`,
#     # otherwise delete it
#     if settings.GROUND_STATION_CLIENT_COMPLETE_OUTPUT_PATH:
#         os.rename(os.path.join(settings.GROUND_STATION_CLIENT_OUTPUT_PATH, filename),
#                   os.path.join(settings.GROUND_STATION_CLIENT_COMPLETE_OUTPUT_PATH, filename))
#     else:
#         os.remove(os.path.join(settings.GROUND_STATION_CLIENT_OUTPUT_PATH, filename))

# def post_data():
#     """PUT observation data back to Network API."""
#     LOGGER.debug('Post data started')

#     for fil in next(os.walk(settings.GROUND_STATION_CLIENT_OUTPUT_PATH))[2]:
#         file_path = os.path.join(*[settings.GROUND_STATION_CLIENT_OUTPUT_PATH, fil])
#         if (fil.startswith('receiving_satnogs') or fil.startswith('receiving_waterfall')
#                 or fil.startswith('receiving_data') or not os.stat(file_path).st_size):
#             continue
#         if fil.startswith('satnogs'):
#             # This file is an audio file
#             if not settings.GROUND_STATION_CLIENT_UPLOAD_AUDIO_FILES:
#                 keep_or_remove_file(fil)
#                 continue
#             observation = {'payload': open(file_path, 'rb')}
#         elif fil.startswith('waterfall'):
#             # This file is a waterfall image
#             if not settings.GROUND_STATION_CLIENT_UPLOAD_WATERFALL_FILES:
#                 keep_or_remove_file(fil)
#                 continue
#             observation = {'waterfall': open(file_path, 'rb')}
#         elif fil.startswith('data'):
#             # This file is a data frame
#             # Check if it is serialized using json OR raw data
#             try:
#                 with open(file_path, 'r', encoding='utf-8') as json_string:
#                     data = json.load(json_string)
#                 if not isinstance(data, dict) or 'pdu' not in data.keys():
#                     # File can accidentally be parsed as json, but is missing the expected fields.
#                     # Thus we assume it is a raw data frame
#                     raise ValueError

#                 # This file is a json-encoded data frame
#                 observation = {'demoddata': (fil, base64.b64decode(data['pdu']))}
#             except ValueError:
#                 # This file is a raw data frame
#                 observation = {'demoddata': open(file_path, 'rb')}
#         else:
#             # This file is of unknown type
#             LOGGER.debug('Ignore file: %s', fil)
#             continue
#         if '_' not in fil:
#             continue
#         observation_id = fil.split('_')[1]
#         upload_observation_data(observation_id, observation, fil)

# def upload_observation_data(observation_id, observation, fil):
#     """Upload observation data to SatNOGS Network API."""
#     base_url = urljoin(settings.GROUND_STATION_CLIENT_NETWORK_API_URL, 'observations/')
#     headers = {'Authorization': 'Token {0}'.format(settings.GROUND_STATION_CLIENT_API_TOKEN)}

#     LOGGER.debug('Trying to PUT observation data for id: %s', observation_id)
#     url = urljoin(base_url, observation_id)
#     if not url.endswith('/'):
#         url += '/'
#     LOGGER.debug('PUT file %s to network API', fil)
#     LOGGER.debug('URL: %s', url)
#     LOGGER.debug('Headers: %s', headers)
#     LOGGER.debug('Observation file: %s', observation)
#     try:
#         response = requests.put(url,
#                                 headers=headers,
#                                 files=observation,
#                                 verify=settings.GROUND_STATION_CLIENT_VERIFY_SSL,
#                                 stream=True,
#                                 timeout=settings.GROUND_STATION_CLIENT_NETWORK_API_TIMEOUT)
#         response.raise_for_status()

#         LOGGER.info('Upload successful.')

#         keep_or_remove_file(fil)
#     except requests.exceptions.Timeout:
#         LOGGER.error('Upload of %s for observation %s failed '
#                      'due to timeout.', fil, observation_id)
#     except requests.exceptions.HTTPError:
#         if response.status_code == 404:
#             LOGGER.error(
#                 "Upload of %s for observation %s failed, %s doesn't exist (404)."
#                 'Probably the observation was deleted.', fil, observation_id, url)

#             # Move file to `GROUND_STATION_CLIENT_INCOMPLETE_OUTPUT_PATH`
#             os.rename(os.path.join(settings.GROUND_STATION_CLIENT_OUTPUT_PATH, fil),
#                       os.path.join(settings.GROUND_STATION_CLIENT_INCOMPLETE_OUTPUT_PATH, fil))
#         if response.status_code == 403 and 'has already been uploaded' in response.text:
#             LOGGER.error('Upload of %s for observation %s is forbidden, %s\n URL: %s', fil,
#                          observation_id, response.text, url)
#             keep_or_remove_file(fil)
#         else:
#             LOGGER.error('Upload of %s for observation %s failed, '
#                          'response status code: %s', fil, observation_id, response.status_code)

# def get_jobs():
#     """Query SatNOGS Network API to GET jobs."""
#     gps_locator = locator.Locator()
#     gps_locator.update_location()

#     url = urljoin(settings.GROUND_STATION_CLIENT_NETWORK_API_URL, 'jobs/')
#     params = {
#         'ground_station': settings.GROUND_STATION_CLIENT_STATION_ID,
#         'lat': settings.GROUND_STATION_CLIENT_STATION_LAT,
#         'lon': settings.GROUND_STATION_CLIENT_STATION_LON,
#         'alt': int(settings.GROUND_STATION_CLIENT_STATION_ELEV)
#     }
#     headers = {'Authorization': 'Token {0}'.format(settings.GROUND_STATION_CLIENT_API_TOKEN)}

#     LOGGER.debug('Fetching jobs from network...')
#     try:
#         response = requests.get(url,
#                                 params=params,
#                                 headers=headers,
#                                 verify=settings.GROUND_STATION_CLIENT_VERIFY_SSL,
#                                 timeout=45)
#         response.raise_for_status()
#     except (requests.exceptions.HTTPError, requests.exceptions.RequestException) as err:
#         LOGGER.error('%s', err)
#         LOGGER.error('Fetching jobs from network failed.')
#         return

#     latest_jobs = {job['id']: job for job in response.json()}
#     existing_jobs = {
#         int(job.id): job
#         for job in SCHEDULER.get_jobs() if job.name == spawn_observer.__name__
#     }

#     dropped_job_ids = set(existing_jobs) - set(latest_jobs)
#     new_job_ids = set(latest_jobs) - set(existing_jobs)

#     LOGGER.debug('Fetched jobs from network, received %d future observations.', len(latest_jobs))

#     for observation_id in dropped_job_ids:
#         LOGGER.info('Drop planned observation %s (reason: deleted in network).', observation_id)
#         existing_jobs[observation_id].remove()

#     for observation_id in new_job_ids:
#         obj = latest_jobs[observation_id]
#         start = parser.parse(obj['start'])
#         job_id = str(obj['id'])
#         kwargs = {'obj': obj}

#         LOGGER.info('Received job for observation %s, starting at %s', job_id, start.isoformat())
#         LOGGER.debug('Observation obj: %s', obj)
#         SCHEDULER.add_job(spawn_observer,
#                           'date',
#                           run_date=start,
#                           id='{0}'.format(job_id),
#                           kwargs=kwargs,
#                           replace_existing=True)


def get_contacts():
    """Get all contacts"""
    contacts = []
    for job in SCHEDULER.get_jobs():
        if job.name == spawn_observer.__name__:
            contact = job.kwargs['contact']
            # get contact profile and add to contact
            contact_profile = None
            with Session() as session:
                contact_profile = session.query(ContactProfile).get(
                    contact['contactProfile']['id'])
            contact['contactProfile'] = contact_profile.to_dict()
            contacts.append(contact)
    return contacts


def get_contact(contact_id):
    """Get a contact by its contact id. Throws JobLookupError if not found"""
    try:
        # return [x for x in SCHEDULER.get_jobs() if x.id == contact_id][0]
        # get the target job, extract contact
        contact = [x for x in SCHEDULER.get_jobs() if x.id == contact_id][0].kwargs['contact']
        # get contact profile and add to contact
        contact_profile = None
        with Session() as session:
            contact_profile = session.query(ContactProfile).get(contact['contactProfile']['id'])
        contact['contactProfile'] = contact_profile.to_dict()
        return contact
    except IndexError:
        raise JobLookupError(contact_id)


def add_contact(contact):
    """Add a job to the scheduler"""
    job_id = str(contact['id'])
    kwargs = {'contact': contact}

    # add or update contact profile
    LOGGER.debug('Adding or updating contact profile for new job: %s', job_id)
    with Session() as session:
        contact_profile_data = contact['contactProfile']
        contact_profile = ContactProfile(
            id=contact_profile_data['id'],
            downlink_destination_ips=contact_profile_data['downlinkDestinationIPs'],
            uplink_origin_ip=contact_profile_data['uplinkOriginIP'],
            center_frequency_mhz=contact_profile_data['centerFrequencyMHz'],
            bandwidth_mhz=contact_profile_data['bandwidthMHz'],
            save_iq_files=contact_profile_data['saveIQFiles'],
        )
        session.merge(contact_profile)
        session.commit()

    LOGGER.debug('Adding new job: %s', job_id)
    LOGGER.debug('Contact obj: %s', contact)
    SCHEDULER.add_job(
        spawn_observer,
        'date',
        run_date=contact['reservationStartTime'],
        id='{0}'.format(job_id),
        kwargs=kwargs,
        replace_existing=True,
    )
    LOGGER.debug('Added new job: %s', job_id)
    return contact


def remove_contact(contact_id):
    """Remove a job from the scheduler. Throws JobLookupError if not found"""
    job_id = str(contact_id)
    LOGGER.debug('Removing job: %s', job_id)
    SCHEDULER.remove_job(job_id)
    LOGGER.debug('Removed job: %s', job_id)


def status_listener():
    LOGGER.debug('Starting scheduler...')
    SCHEDULER.start()

    # SCHEDULER.remove_all_jobs()
    # LOGGER.info('Registering `get_jobs` periodic task (%d sec. interval)',
    #             settings.GROUND_STATION_CLIENT_NETWORK_API_QUERY_INTERVAL)
    # SCHEDULER.add_job(get_jobs,
    #                   'interval',
    #                   seconds=settings.GROUND_STATION_CLIENT_NETWORK_API_QUERY_INTERVAL,
    #                   next_run_time=datetime.now(pytz.utc))
    # LOGGER.info('Registering `post_data` periodic task (%d sec. interval)',
    #             settings.GROUND_STATION_CLIENT_NETWORK_API_POST_INTERVAL)
    # SCHEDULER.add_job(post_data, 'interval', seconds=settings.GROUND_STATION_CLIENT_NETWORK_API_POST_INTERVAL)
