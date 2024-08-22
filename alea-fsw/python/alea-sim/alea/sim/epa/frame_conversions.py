"""
Reference frame conversions

Author: Yousif El-Wishahy 
    
ALEASAT 2024

References -
    - Landis Markley and John Crassidis. Fundamentals of Spacecraft Attitude Determination and Control. 
    Jan. 2014. ISBN: ISBN: 978-1-4939-0802-8. DOI: 10.1007/978-1-
    4939-0802-8.
    - Vallado, David. Fundamentals of Astrodynamics and Applications, 4th ed. Hawthorne, CA: Microcosm Press, 2013.
    
Documentation https://wiki.aleasat.space/en/space-segment/

There are 5 main reference frames of interest for ALEASAT:
    * ECI - earth-centered-inertial
        The Earth Centered Inertial Frame is an inertial reference frame with the axis origin
        placed at the center of Earth. The Z axis is aligned with the axis of rotation of Earth. The
        X axis follows the vernal equinox. The Y axis completes the orthogonal coordinate system.
        This frame is realized as the ICRS frame "J2000" coordinates https://en.wikipedia.org/wiki/International_Celestial_Reference_System_and_its_realizations
    * ECEF - earth-centered-earth-fixed
        Same origin and Z axis as inertial frame, the key difference is that the X and Y axes of the ECEF frame
        rotate with the earth.
        This frame is realized as the ITRF frame https://en.wikipedia.org/wiki/International_Terrestrial_Reference_System_and_Frame 
    * NED - north-east-down
        IGRF or WMM magnetic field model reference vectors are expressed in this frame. The origin is fixed to the
        spacecraft body COM and the X axis points true north, Z points nadir, Y completes the orthognal axes.
    * ORBIT - Orbital reference frame
        Origin fixed to satellite body CoM, X points nadir, Z tracks orbit velocity vector, Y completes the
        orthogonal coordinate system.
    * BODY - ALEASAT body frame/
        Origin fixed to spacecraft body CoM and axes rotate with spacecraft.
        Refer to https://wiki.aleasat.space/en/space-segment/system-level-design for the axes definition.
        The X axis points from the camera payload, the Z axis points from the atenna deployer face,
        the Y axis completes the right-handed orthogonal coordinates (or points from solar panel 4).     
"""

import numpy as np
from alea.sim.math_lib.math import skew

OMEGA_EARTH =  7.2921158553E-5
SECONDS_TO_DEGREES = 1/240

def eci_to_ecef_rot_mat(theta_gmst):
    # rotation angle between ECI and ECEF due to elapsed angles [radians] from the EPOCH of ECI frame
    # theta_gmst is Greenwich Mean Sidereal Time (GMST) angle
    #
    # returns rotation matrix R such that r_ecef = R @ r_eci
    th = theta_gmst

    R = np.array([[np.cos(th), np.sin(th), 0], 
                    [-np.sin(th), np.cos(th), 0], 
                    [0, 0, 1]])
    return R

def ned_to_ecef_rot_mat(lon, lat):
    return ecef_to_ned_rot_mat(lon, lat).T
    
def ecef_to_ned_rot_mat(lon, lat):
    """
    ecef to ned dcm matrix from longitude and geodetic latitude [radians]
    
    longitude and latitude referenced to the reference ellipsoid wgs84
    https://www.mathworks.com/help/aeroblks/directioncosinematrixeceftoned.html
    """
    # R = np.array([[-np.sin(lon) * np.cos(lat), -np.sin(lat), -np.cos(lon) * np.cos(lat)], 
    #                 [-np.sin(lon) * np.sin(lat), np.cos(lat), -np.cos(lon) * np.sin(lat)], 
    #                 [np.cos(lon), 0, -np.sin(lat)]])
    
    l = lon
    u = lat
    R = np.array([[-np.sin(u) * np.cos(l), -np.sin(u)*np.sin(l), np.cos(u)], 
                    [-np.sin(l), np.cos(l), 0], 
                    [-np.cos(u)*np.cos(l), -np.cos(u)*np.sin(l), -np.sin(u)]])
    return R

def eci_to_orbit_rot_mat(raan, incl, argp, mean_anomaly):
    """
    Rotation matrix from ECI frame to ORBIT frame.
    
    Depends on the following orbital elements:
        - RAAN - Right ascension of the ascending node
        - Inclination
        - Argument of perigee
        - Mean anomaly

    References:
        * DDJS_AOCS_AcubeSat
        * James Tsui. “Fundamentals of Global Positioning System Receivers: A Software
        Approach.” In: Oct. 2001, pp. i–xvii. ISBN: 9780471200543. DOI: 10.1002/0471200549.
        fmatter_indsub.
    """
    o = raan
    w = argp+mean_anomaly
    i = incl
    
    R = np.array(
        [
        [np.cos(o)*np.sin(w) - np.sin(o)*np.cos(i)*np.sin(w), np.sin(o)*np.cos(w)+np.cos(o)*np.cos(i)*np.sin(w), np.sin(i)*np.sin(w)],
        [-np.cos(o)*np.sin(w) - np.sin(o)*np.cos(i)*np.cos(w), -np.sin(o)*np.sin(w)+np.cos(o)*np.cos(i)*np.cos(w), np.sin(i)*np.cos(w)],
        [np.sin(o)*np.sin(i), -np.cos(o)*np.sin(i), np.cos(i)],
        ])
    return R

def eci_to_ecef(vec_eci: np.ndarray, theta_gmst: float) -> np.ndarray:
    """
    Description
    ----------
    Rotate vector from ECI to ECEF frame.
    Do not use this function for velocities and accelerations.

    Parameters
    ----------
    vec_eci : numpy array
    theta_gmst : float [radians]
        Greenwich Mean Sidereal Time (GMST) angle

    Returns
    -------
    vec_ecef : numpy array
    """
    R = eci_to_ecef_rot_mat(theta_gmst)
    return R @ vec_eci

def ecef_to_eci(vec_ecef: np.ndarray, theta_gmst: float) -> np.ndarray:
    """
    Description
    ----------
    Rotate vector from ECEF to ECI frame.
    Do not use this function for velocities and accelerations.
    See  ecef_to_eci_velocity and  ecef_to_eci_acceleration instead

    Parameters
    ----------
    vec_ecef : numpy array
    theta_gmst : float [radians]
        Greenwich Mean Sidereal Time (GMST) angle

    Returns
    -------
    vec_eci : numpy array
    """
    #get the rot mat and inverse (transpose (rotation) == inverse(rotation))
    R = eci_to_ecef_rot_mat(theta_gmst).T
    return R @ vec_ecef

def ned_to_ecef(vec_ned: np.ndarray, longitude: float, latitude: float) -> np.ndarray:
    """
    Description
    ----------
    Rotate vector from NED to ECEF frame.

    Parameters
    ----------
    vec_ned : numpy array
    longitude : float [rad]
    latitude : float [rad]

    Returns
    -------
    vec_ecef : numpy array
    """
    R = ned_to_ecef_rot_mat(longitude, latitude)
    return R @ vec_ned

def ecef_to_ned(vec_ecef: np.ndarray, longitude: float, latitude: float) -> np.ndarray:
    """
    Description
    ----------
    Rotate vector from NED to ECEF frame.

    Parameters
    ----------
    vec_ned : numpy array
    longitude : float [rad]
    latitude : float [rad]

    Returns
    -------
    vec_ecef : numpy array
    """
    R = ecef_to_ned_rot_mat(longitude, latitude)
    return R @ vec_ecef

#special functions for velocity and accelerations
#because ECEF is rotating w.r.t ECI, the velocity and acceleration vectors are NOT the same
#i.e. coriolis, centripital and tangential acceelration terms arise for the acceleration


def ecef_to_eci_velocity(r_ecef:np.ndarray, v_ecef:np.ndarray):
    """
    Convert a cartesian vecloity vector from the ECEF frame to ECI frame.
    ECEF is rotating w.r.t ECI about the Z axis at an angular rate of OMEGA_EARTH
    The resulting velocity in the inertial frame is corrected using what is known as the kinematics equation
    
    Ref.
    Vallado, David. Fundamentals of Astrodynamics and Applications, 4th ed. Hawthorne, CA: Microcosm Press, 2013.
    """
    omega_earth = np.array([0.0, 0.0, OMEGA_EARTH])
    v_eci : np.ndarray = v_ecef + np.cross(omega_earth, r_ecef)
    return v_eci

def eci_to_ecef_velocity(r_ecef:np.ndarray, v_eci:np.ndarray):
    omega_earth = np.array([0.0, 0.0, OMEGA_EARTH])
    v_ecef : np.ndarray = v_eci - np.cross(omega_earth, r_ecef)
    return v_ecef

def ecef_to_eci_acceleration(r_ecef:np.ndarray, v_ecef:np.ndarray, a_ecef: np.ndarray):
    """
    Convert a cartesian acceleration vector from the ECEF frame to ECI frame.
    ECEF is rotating w.r.t ECI about the Z axis at an angular rate of OMEGA_EARTH
    The resulting acceleration is adjusted with centripital and coriolis acceleration terms.
    
    Note - the eq is approximated to tangential acceleration 0 , an assumption is made that OMEGA_EARTH is constant,
    but the earth's angular velocity does change over time by a small rate assumed to be negligibly small.
    https://en.wikipedia.org/wiki/Earth%27s_rotation
    
    Ref.
    Vallado, David. Fundamentals of Astrodynamics and Applications, 4th ed. Hawthorne, CA: Microcosm Press, 2013.
    """
    omega_earth = np.array([0.0, 0.0, OMEGA_EARTH])
    omegax = skew(omega_earth)
    
    #eci accel = ecefaccel +  centripital accel + coriolis accel + tangential(ignored)
    a_eci: np.ndarray = a_ecef + omegax @ ( omegax @ r_ecef) + 2 * (omegax @ v_ecef)
    return a_eci

def eci_to_ecef_acceleration(r_ecef:np.ndarray, v_ecef:np.ndarray, a_eci: np.ndarray):
    omega_earth = np.array([0.0, 0.0, OMEGA_EARTH])
    omegax = skew(omega_earth)
    
    #eci accel = ecefaccel +  centripital accel + coriolis accel + tangential(ignored)
    a_ecef: np.ndarray = a_eci - omegax @ ( omegax @ r_ecef) - 2 * (omegax @ v_ecef)
    return a_ecef

#ref for wgs84 values https://apps.dtic.mil/sti/pdfs/ADA280358.pdf
wgs84_re = 6378137 #earth radius [m]
wgs84_fe = 1/298.257223563 #wgs84 flattening factor
    