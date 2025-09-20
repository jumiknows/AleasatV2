import numpy as np

def polar_to_cartesian(r: float | np.ndarray, phi: float | np.ndarray, r_y: float | np.ndarray = None) -> tuple[float, float] | tuple[np.ndarray, np.ndarray]:
    """Convert polar coordinates to cartesian coordinates.

    This function works with both single coordinates or n-dimensional arrays of coordinates.
    All calculations are done element-wise on arrays of coordinates.

    Args:
        r:
            Radius. If r_y is supplied this radius is used only for the x coordinate.
        phi:
            Azimuthal angle (in radians)
        r_y:
            Radius for calculating the y coordinate. Useful for ellipse calculations.

    Returns:
        A tuple of (x, y) coordinates
    """
    r_x = r
    if r_y is None:
        r_y = r

    x = r_x * np.cos(phi)
    y = r_y * np.sin(phi)

    return (x, y)

def spherical_mesh(phi_step_deg: float = 10, theta_step_deg: float = 10) -> tuple[np.ndarray, np.ndarray]:
    """Generate a 2D mesh grid of spherical coordinates (phi, theta) where phi
    is the azimuthal angle (from 0 to 360) and theta is the polar angle (from 0 to 180).

    Args:
        phi_step_deg (optional): Step size for azimuthal angle in degrees. Defaults to 10.
        theta_step_deg (optional): Step size for polar angle in degrees. Defaults to 10.

    Returns:
        A tuple where the first element is a mesh grid (2D array) of phi (azimuthal angle) values
        and the second element is a mesh grid (2D array) of theta (polar angle) values.
        NOTE: All angle values in the arrays are in radians so they can be directly converted
        to cartesian coordinates if desired.
    """
    num_phi   = int(360 /   phi_step_deg) + 1 # + 1 for endpoint
    num_theta = int(180 / theta_step_deg) + 1 # + 1 for endpoint

    phi_spec   = slice(0, 2*np.pi, (  num_phi * 1j)) # Making the step complex (* 1j) means we will include the endpoint
    theta_spec = slice(0,   np.pi, (num_theta * 1j)) # Making the step complex (* 1j) means we will include the endpoint

    phi, theta = np.mgrid[phi_spec, theta_spec]
    return (phi, theta)

def spherical_to_cartesian(r: float | np.ndarray, phi: float | np.ndarray, theta: float | np.ndarray,
                           r_y: float | np.ndarray = None, r_z: float | np.ndarray = None) -> tuple[float, float, float] | tuple[np.ndarray, np.ndarray, np.ndarray]:
    """Convert spherical coordinates to cartesian coordinates.

    This function works with both single coordinates or n-dimensional arrays of coordinates.
    All calculations are done element-wise on arrays of coordinates.

    Args:
        r:
            Radius. If r_y or r_z is supplied this radius is used for calculating the coordinate
            that wasn't supplied.
        phi:
            Azimuthal angle (in radians)
        theta:
            Polar angle (in radians)
        r_y:
            Radius for calculating the y coordinate. Useful for spheroid calculations.
        r_z:
            Radius for calculating the z coordinate. Useful for spheroid calculations.

    Returns:
        A tuple of (x, y, z) coordinates
    """
    r_x = r
    if r_y is None:
        r_y = r
    if r_z is None:
        r_z = r

    sin_theta = np.sin(theta)

    x, y = polar_to_cartesian(r_x * sin_theta, phi, r_y=(r_y * sin_theta))
    z = r_z * np.cos(theta)

    return (x, y, z)
