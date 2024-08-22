"""
datetime stuff
I hate leap seconds so pyehem handles that which uses libastro (c) 
which has a leapsecond table that is likely out of date from 2024 but will do for now.
TODO confirm leap second table 

pyephem.date date docs - https://rhodesmill.org/pyephem/date.html
Note "PyEphem dates are encoded as the “Dublin Julian Day”, which is the number of days 
(including any fraction) that have passed since the last day of 1899, at noon."
and represented as floats when printed.
"""
import datetime
import time
from skyfield.api import load
import skyfield
from math import pi

SECONDS_PER_HOUR = 3600
SECONDS_PER_DAY = 86400
DEGREES_PER_SECOND = 1/240

def toYearFraction(date):
    def sinceEpoch(date): # returns seconds since epoch
        return time.mktime(date.timetuple())
    s = sinceEpoch

    year = date.year
    startOfThisYear = datetime.datetime(year=year, month=1, day=1)
    startOfNextYear = datetime.datetime(year=year+1, month=1, day=1)

    yearElapsed = s(date) - s(startOfThisYear)
    yearDuration = s(startOfNextYear) - s(startOfThisYear)
    fraction = yearElapsed/yearDuration

    #TODO what is the error on this?
    return date.year + fraction

def datetime_J2000() -> datetime.datetime:
    """j2000 epoch as a python datetime object with adjusted leap seconds"""
    ts = load.timescale()
    j2000 = ts.J2000
    dt, leap_second = j2000.utc_datetime_and_leap_second()
    return dt + datetime.timedelta(seconds=int(leap_second))

def seconds_since_J2000(date: datetime.datetime) -> float:
    """date must have timezone attribute"""
    s = (date-datetime_J2000()).total_seconds()
    return s

def skyfield_time_to_gmst_radians(t: skyfield.timelib.Time):
    """
    Get Greenwich Mean Sidereal Time (GMST) angle as radian
    Takes skyfield skyfield.timelib.Time which contains a ref to GMST hour value
    """
    
    gmst_s = t.gmst*SECONDS_PER_HOUR
    
    # if gmst_s > SECONDS_PER_DAY:
    #    gmst_s-=floor(gmst_s/SECONDS_PER_DAY)*SECONDS_PER_DAY
       
    gmst_rads = gmst_s * DEGREES_PER_SECOND * pi/180
    return gmst_rads
        
    
    
    
    
    
    