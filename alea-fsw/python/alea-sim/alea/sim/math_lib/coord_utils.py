import numpy as np

def polar_to_cartesian(r: float | np.ndarray, phi: float | np.ndarray) -> tuple[float, float] | tuple[np.ndarray, np.ndarray]:
    """Convert polar coordinates to cartesian coordinates.

    This function works with both single coordinates or n-dimensional arrays of coordinates.
    All calculations are done element-wise on arrays of coordinates.

    Args:
        r:
            Radius
        phi:
            Azimuthal angle (in radians)

    Returns:
        A tuple of (x, y) coordinates
    """
    x = r * np.cos(phi)
    y = r * np.sin(phi)
    return (x, y)

def cartesian_to_polar(x: float | np.ndarray, y: float | np.ndarray) -> tuple[float, float] | tuple[np.ndarray, np.ndarray]:
    """Convert cartesian coordinates to polar coordinates.

    This function works with both single coordinates or n-dimensional arrays of coordinates.
    All calculations are done element-wise on arrays of coordinates.

    Args:
        x:
            The x cartesian coordinate
        y:
            The y cartesian coordinate

    Returns:
        A tuple of (r, phi) coordinates where r is the radius at the azimuthal angle of phi (in radians).
        The azimuthal angle (phi) is returned in the range [-pi, pi].
    """
    r = np.sqrt((x**2) + (y**2))
    phi = np.arctan2(y, x)
    return (r, phi)

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

def spherical_to_cartesian(r: float | np.ndarray, phi: float | np.ndarray, theta: float | np.ndarray) -> tuple[float, float, float] | tuple[np.ndarray, np.ndarray, np.ndarray]:
    """Convert spherical coordinates to cartesian coordinates.

    This function works with both single coordinates or n-dimensional arrays of coordinates.
    All calculations are done element-wise on arrays of coordinates.

    Args:
        r:
            Radius
        phi:
            Azimuthal angle (in radians)
        theta:
            Polar angle (in radians)

    Returns:
        A tuple of (x, y, z) coordinates
    """
    x, y = polar_to_cartesian(r * np.sin(theta), phi)
    z = r * np.cos(theta)
    return (x, y, z)

def cartesian_to_spherical(x: float | np.ndarray, y: float | np.ndarray, z: float | np.ndarray) ->  tuple[float, float] | tuple[np.ndarray, np.ndarray]:
    """Convert cartesian coordinates to spherical coordinates.

    This function works with both single coordinates or n-dimensional arrays of coordinates.
    All calculations are done element-wise on arrays of coordinates.

    Args:
        x:
            The x cartesian coordinate
        y:
            The y cartesian coordinate
        z:
            The z cartesian coordinate

    Returns:
        A tuple of (r, phi, theta) coordinates where r is the radius at the azimuthal angle
        of phi (in radians) and polar angle of theta (in radians).
          - The azimuthal angle (phi) is returned in the range [-pi, pi]. If the polar angle
            is 0 or pi, the azimuthal angle is set to 0.
          - The polar angle (theta) is returned in the range [0, pi].
    """
    r, phi = cartesian_to_polar(x, y)
    theta = np.arctan2(r, z)
    r = np.sqrt((r**2) + (z**2))

    # Enforce zero azimuthal angle when theta is polar
    phi = np.where(theta == 0, 0, phi)
    phi = np.where(theta == np.pi, 0, phi)

    return (r, phi, theta)
