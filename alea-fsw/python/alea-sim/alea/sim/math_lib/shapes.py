import numpy as np

from . import coord_utils

class Spheroid:
    def __init__(self, a: float = 1.0, f: float = 0.0):
        """Initialize a Spheroid.

        Args:
            a (optional): Semi-major axis. Defaults to 1.0.
            f (optional): Flattening = (a - b) / a. Defaults to 0.0.
        """
        self._a = a
        self._f = f

        self._b = self._a * (1 - self._f)
        self._r = ((2 * self._a) + self._b) / 3

        self._artist = None

    @property
    def semi_major_axis(self) -> float:
        return self._a

    @property
    def semi_minor_axis(self) -> float:
        return self._b

    @property
    def mean_radius(self) -> float:
        return self._r

    def radius(self, theta_deg: float | np.ndarray = 90) -> float | np.ndarray:
        """Calculate the radius of the spheroid (in the x-y plane) at a given polar angle
        (theta).

        If an array of polar angles is given, an array of radii will be returned.

        Args:
            theta_deg (optional): Polar angle in degrees. Defaults to 90.

        Returns:
            The radius (radii) at the given polar angle(s).
        """
        return self._radius(np.deg2rad(theta_deg))

    def _radius(self, theta_rad: float | np.ndarray) -> float | np.ndarray:
        return self._a * np.sin(theta_rad)

    def cartesian_surface_grid(self, phi_step_deg: float = 10, theta_step_deg: float = 10) -> tuple[np.ndarray, np.ndarray, np.ndarray]:
        """Generates a 2D grid of cartesian coordinates over the surface of this Spheroid.
        The grid will be evenly spaced in the spherical coordinate domain (azimuthal angle, polar angle).

        Args:
            phi_step_deg (optional): Step size for azimuthal angle in degrees. Defaults to 10.
            theta_step_deg (optional): Step size for polar angle in degrees. Defaults to 10.

        Returns:
            A tuple of (x, y, z) coordinates where each of x, y, and z is a 2D array over the grid points.
        """
        phi, theta = coord_utils.spherical_mesh(phi_step_deg, theta_step_deg)
        x, y, z = coord_utils.spherical_to_cartesian(self._a, phi, theta, r_z=self._b)
        return (x, y, z)

    def cartesian_surface_arc_az(self, theta_deg: float = 90, phi_start_deg: float = 0,
                                 phi_stop_deg: float = 360, num_points: float = 36) -> tuple[np.ndarray, np.ndarray, np.ndarray]:
        """Generates a 1D array of cartesian coordinates for the arc of points on the surface of
        the spheroid at the given polar angle (theta) between the given start and stop azimuthal
        (phi) angles.

        The array of points will be evenly spaced in the azimuthal coordinate (phi) domain.

        Args:
            theta_deg (optional):
                Fixed polar angle in degrees. Defaults to 90.
            phi_start_deg (optional):
                The starting azimuthal angle (inclusive) in degrees. Defaults to 0.
            phi_stop_deg (optional):
                The ending azimuthal angle (inclusive) in degrees. Defaults to 360.
            num_points (optional):
                The number of points to generate. Defaults to 36.

        Returns:
            A tuple of (x, y, z) coordinates where each of x, y, and z is a 1D array over the
            designated surface arc.
        """

        phi_rad = np.linspace(np.deg2rad(phi_start_deg), np.deg2rad(phi_stop_deg), num_points)
        return self._cartesian_surface_point(phi_rad, np.deg2rad(theta_deg))

    def cartesian_surface_arc_polar(self, phi_deg: float = 0, theta_start_deg: float = 0,
                                    theta_stop_deg: float = 180, num_points: float = 36) -> tuple[np.ndarray, np.ndarray, np.ndarray]:
        """Generates a 1D array of cartesian coordinates for the arc of points on the surface of
        the spheroid at the given azimuthal angle (phi) between the given start and stop polar
        (theta) angles.

        The array of points will be evenly spaced in the polar coordinate (phi) domain.

        Args:
            phi_deg (optional):
                Fixed azimuthal angle in degrees. Defaults to 0.
            theta_start_deg (optional):
                The starting polar angle (inclusive) in degrees. Defaults to 0.
            theta_stop_deg (optional):
                The ending polar angle (inclusive) in degrees. Defaults to 180.
            num_points (optional):
                The number of points to generate. Defaults to 18.

        Returns:
            A tuple of (x, y, z) coordinates where each of x, y, and z is a 1D array over the
            designated surface arc.
        """

        theta_rad = np.linspace(np.deg2rad(theta_start_deg), np.deg2rad(theta_stop_deg), num_points)
        return self._cartesian_surface_point(np.deg2rad(phi_deg), theta_rad)

    def cartesian_surface_point(self, phi_deg: float | np.ndarray, theta_deg: float | np.ndarray) -> tuple[float, float] | tuple[np.ndarray, np.ndarray]:
        return self._cartesian_surface_point(np.deg2rad(phi_deg), np.deg2rad(theta_deg))

    def _cartesian_surface_point(self, phi_rad: float | np.ndarray, theta_rad: float | np.ndarray) -> tuple[float, float] | tuple[np.ndarray, np.ndarray]:
        r = self._radius(theta_rad)

        x, y = coord_utils.polar_to_cartesian(r, phi_rad)
        z = self._b * np.cos(theta_rad)

        return (x, y, z)

class Sphere(Spheroid):
    def __init__(self, r: float = 1.0):
        super().__init__(a = r, f = 0)

class WGS84Earth(Spheroid):
    # References:
    #   [1] https://www.unoosa.org/pdf/icg/2012/template/WGS_84.pdf
    #   [2] https://en.wikipedia.org/wiki/World_Geodetic_System

    # These are the parameters defined by WGS84
    SEMI_MAJOR_AXIS_M = 6378137.0   # [m]
    INV_FLATTENING  = 298.257223563 # 1/f

    def __init__(self, scale: float = 1.0):
        super().__init__(self.SEMI_MAJOR_AXIS_M * scale, 1.0/self.INV_FLATTENING)
