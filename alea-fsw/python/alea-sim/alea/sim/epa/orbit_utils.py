import numpy as np

r_e = 6371 #mean earth radius [km]
mu_earth = 398600.440 #[km^3/s^2]

def calculate_sso_elements(altitude_km):
    """
    Approximately calculate orbital elements for a sun sych orbit around earth.
    
    Returns semi major axis and inclination.
    
    Refs
        https://en.wikipedia.org/wiki/Sun-synchronous_orbit
        Vallado, David. Fundamentals of Astrodynamics and Applications, 4th ed. Hawthorne, CA: Microcosm Press, 2013.
    """
    
    #approximate sso relation for almost circular orbits
    a = r_e + altitude_km
    incl = np.acos(-1.0*(a/12352)**(7/2))
    
    #TODO construct more orbital elements
    
    return a, incl

#TODO unit test these algos
def rv2coe(r_eci: np.ndarray, v_eci:np.ndarray) -> tuple[float]:
    """
    convert inertial position and velocity to keplerian orbit elements
    based on Algorithm 9, section 2.5 of Vallado, David A. - Fundamentals of Astrodynamics and Applications

    Parameters
    ----------
    r_eci : array, position in ECI frame [m]
    v_eci : array, velocity in ECI frame [m/s]

    Returns
    -------
    a : float, semi-major axis [m]
    e : float, eccentricity
    argp : float, arg of perigee [rad]
    incl : float, inclination [rad]
    ma : float, mean anomaly [rad]
    n : float, mean motion [rad/s]
    raan : float, right ascension of the ascending node [rad]
    ea : float, eccentric anomaly [?]
    T : time of perigee passage [s]
    """
    TOLERANCE = 1E-10
    h_vec = np.cross(r_eci, v_eci)
    n_vec = np.cross(np.array([0,0,1]), h_vec)
    n = np.linalg.norm(n_vec) #mean motion [rad/s]
    h = np.linalg.norm(h_vec)
    r = np.linalg.norm(r_eci)
    v = np.linalg.norm(v_eci)
    
    e_vec = ((v**2 - mu_earth/r)*r_eci - (np.dot(r_eci, v_eci)*v_eci))/mu_earth
    e = np.linalg.norm(e_vec) #eccentricity
    
    ksi = v**2 / 2 - mu_earth/r
    
    if np.isclose(e, 1.0, 1e-15):
        a = -mu_earth / (2.0 * ksi) #semi major axis [m]
        p = a*(1-e**2) #semiparameter [m]
    else: #slingshot :)
        p = h**2 / mu_earth
        a = np.inf

        
    incl = np.arccos(h_vec[2]/h) #inclindation [rad]

    raan = np.arccos(n_vec[0]/n) #raan, right ascension of the ascending node [rad]
    if n_vec[1] < 0: raan = 2*np.pi - raan

    argp = np.arccos(np.dot(e_vec, r_eci)/(n*e)) #argument of perigee [rad]
    if e_vec[2] < 0: argp = 2*np.pi - argp
    
    ta = np.arccos(np.dot(e_vec, r_eci)/(e*r)) # true anomaly [rad]
    if np.dot(r_eci, v_eci) < 0: ta = 2*np.pi - ta
    
    #eccentric anomaly, eq 2-9, section 2.2
    ea = np.arctan2(np.sqrt(1-e**2)*np.sin(ta), e+np.cos(ta))
    
    ma = ea - e*np.sin(ea) #mean anomaly [rad]
    
    T = -1.0*ma/n
    
    return (a, e, argp, incl, ma, n, raan, ea, T)

#TODO unit test this algo
def coe2rv(a, e, incl, argp, raan, ea) -> tuple[np.ndarray]:
    """
    convert keplerian orbit elements to eci position and velocity
    based on Algorithm 10, section 2.5 of Vallado, David A. - Fundamentals of Astrodynamics and Applications

    Parameters
    ----------
    a : float, semi-major axis [m]
    e : float, eccentricity
    argp : float, arg of perigee [rad]print(se
    incl : float, inclination [rad]
    raan : float, right ascension of the ascending node [rad]
    ea : float, eccentric anomaly
    T : time of perigee passage [s]

    Returns
    -------
    r_eci : array, position in ECI frame [m]
    v_eci : array, velocity in ECI frame [m/s]
    """
    # #1
    # n = np.sqrt(mu_earth/(a**3))
    #2
    #3
    nu = 2*np.arctan(np.sqrt((1+e)/(1-e)) * np.tan(ea/2))
    #4
    r = a*(1 - e*np.cos(ea))
    #5
    h = np.sqrt(mu_earth*a * (1 - e**2))
    #6
    X = r*(np.cos(raan)*np.cos(argp+nu) - np.sin(raan)*np.sin(argp+nu)*np.cos(incl))
    Y = r*(np.sin(raan)*np.cos(argp+nu) + np.cos(raan)*np.sin(argp+nu)*np.cos(incl))
    Z = r*(np.sin(incl)*np.sin(argp+nu))

    #7
    p = a*(1-e**2)

    V_X = (X*h*e/(r*p))*np.sin(nu) - (h/r)*(np.cos(raan)*np.sin(argp+nu) + \
    np.sin(raan)*np.cos(argp+nu)*np.cos(incl))
    V_Y = (Y*h*e/(r*p))*np.sin(nu) - (h/r)*(np.sin(raan)*np.sin(argp+nu) - \
    np.cos(raan)*np.cos(argp+nu)*np.cos(incl))
    V_Z = (Z*h*e/(r*p))*np.sin(nu) + (h/r)*(np.cos(argp+nu)*np.sin(incl))

    return np.array([X,Y,Z]), np.array([V_X,V_Y,V_Z])