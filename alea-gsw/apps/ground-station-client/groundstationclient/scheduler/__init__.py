from __future__ import absolute_import, division, print_function

import logging

from apscheduler.executors.pool import ThreadPoolExecutor
from apscheduler.jobstores.sqlalchemy import SQLAlchemyJobStore
from apscheduler.schedulers.background import BackgroundScheduler
from pytz import utc

from groundstationclient.db import engine
from groundstationclient.settings import SCHEDULER_LOG_LEVEL

JOBSTORES = {'default': SQLAlchemyJobStore(engine=engine)}

EXECUTORS = {
    'default': ThreadPoolExecutor(20),
}

JOB_DEFAULTS = {'coalesce': True, 'max_instances': 1, 'misfire_grace_time': 30}

SCHEDULER = BackgroundScheduler(jobstores=JOBSTORES,
                                executors=EXECUTORS,
                                job_defaults=JOB_DEFAULTS,
                                timezone=utc)

logging.getLogger('apscheduler').setLevel(SCHEDULER_LOG_LEVEL)
