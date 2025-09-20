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

    @property
    def semi_major_axis(self) -> float:
        return self._a

    @property
    def semi_minor_axis(self) -> float:
        return self._b

    @property
    def mean_radius(self) -> float:
        return self._r

    @property
    def flattening(self) -> float:
        return self._f

    def is_on_surface(self, x: float | np.ndarray, y: float | np.ndarray, z: float | np.ndarray, tolerance: float = 1E-8) -> bool | np.ndarray:
        """Verifies that a point given by cartesian coordinates is on the surface of the spheroid
        by checking that the point satisfies the equation of the spheroid within the specified tolerance.

        If arrays of coordinates are given, an array of booleans will be returned.

        Args:
            x: The x cartesian coordinate.
            y: The y cartesian coordinate.
            z: The z cartesian coordinate.
            tolerance (optional): Floating point tolerance. Defaults to 1E-8.

        Returns:
            True if the point is on the surface of the spheroid, otherwise False.
        """
        err = np.abs((x / self._a)**2 + (y / self._a)**2 + (z / self._b)**2 - 1)
        return err < tolerance

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
        x, y, z = self._cartesian_surface_point(phi, theta)
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

    def cartesian_surface_point(self, phi_deg: float | np.ndarray, theta_deg: float | np.ndarray) -> tuple[float, float, float] | tuple[np.ndarray, np.ndarray, np.ndarray]:
        """Calculates the cartesian coordinates of a point on the surface of the spheroid at a given
        azimuthal (phi) and polar (theta) angle.

        If arrays of angles are provided, arrays of cartesian coordinates will be returned.

        Args:
            phi_deg: The azimuthal angle in degrees.
            theta_deg: The polar angle in degrees.

        Returns:
            A tuple of (x, y, z) cartesian coordinates.
        """
        return self._cartesian_surface_point(np.deg2rad(phi_deg), np.deg2rad(theta_deg))

    def _cartesian_surface_point(self, phi_rad: float | np.ndarray, theta_rad: float | np.ndarray) -> tuple[float, float, float] | tuple[np.ndarray, np.ndarray, np.ndarray]:
        """Convert spherical to cartesian coordinates for points on the surface of the spheroid.
        """
        x, y, z = coord_utils.spherical_to_cartesian(self._a, phi_rad, theta_rad)
        z *= (1.0 - self._f) # Distort the z-axis to match spheroid flattening
        return (x, y, z)

    def spherical_surface_point(self, x: float | np.ndarray, y: float | np.ndarray, z: float | np.ndarray) ->  tuple[float, float] | tuple[np.ndarray, np.ndarray]:
        """Calculates the spherical coordinates (azimuthal and polar angle) for a point on the surface
        of the spheroid at a given set of cartesian coordinates.

        This method does not verify that the cartesian coordinates actually represent a point on the
        surface of the ellipsoid to save computation time and be forgiving of NaN values. You can use
        self.is_on_surface() to verify this independently.

        Args:
            x: The x cartesian coordinate.
            y: The y cartesian coordinate.
            z: The z cartesian coordinate.

        Returns:
            A tuple of (phi, theta) where phi is the azimuthal angle and theta is the polar angle,
            both in degrees.
        """
        phi_rad, theta_rad = self._spherical_surface_point(x, y, z)
        phi_deg = np.rad2deg(phi_rad)
        theta_deg = np.rad2deg(theta_rad)
        return (phi_deg, theta_deg)

    def _spherical_surface_point(self, x: float | np.ndarray, y: float | np.ndarray, z: float | np.ndarray) ->  tuple[float, float] | tuple[np.ndarray, np.ndarray]:
        """Convert cartesian to spherical coordinates for points on the surface of the spheroid.
        """
        z /= (1.0 - self._f) # Distort the z-axis to match spheroid flattening
        _, phi_rad, theta_rad = coord_utils.cartesian_to_spherical(x, y, z)
        return (phi_rad, theta_rad)

    def surface_intersection_point(self, origin: np.ndarray, direction: np.ndarray) -> np.ndarray:
        """Calculates the closest intersection point of a vector with the surface of the spheroid.

        This method supports vectorization. Each coordinate in the origin and direction arguments
        can itself be an array of values. In this case, the coordinates of the returned intersection
        point will also be arrays corresponding to the input arrays.

        Args:
            origin:
                The origin of the vector as a 3-element np.ndarray (x, y, z)
            direction:
                The direction vector as a 3-element np.ndarray (x, y, z). This does not have
                to be a unit-vector; however, if it is a unit-vector, the d value that is returned
                will be the straight-line distance between the origin and the intersection point.

        Returns:
            A tuple of two elements (d, w) where d is the parameter such that (origin + d * direction)
            gives the closest intersection point and w is the closest intersection point itself as a
            3-element np.ndarray (x, y, z).
            If an intersection does not exist, d and all three coordinates of w will be set to np.nan
        """
        # Let r = (r_x, r_y, r_z) be a point along the line from the origin to the intersection point.
        #     p = origin
        #     u = direction vector
        #
        # Then we have r(t) = p + d*u and we must find d such that r(t) satisfies the equation of the spheroid:
        #
        #    (x/a)^2 + (y/a)^2 + (z/b)^2 = 1
        #
        #  where a = semi-major axis
        #        b = semi-minor axis
        #
        # r(d) = p + d*u is a set of three equations in d (one for each coordinate) that we can substitute into
        # the equation of the spheroid and solve for d. The result is a quadratic equation for d of the form:
        #
        #    (c_2 * d^2) + (c_1 * d) + c_0 = 0
        #
        # with the standard solutions:
        #
        #    d = (-c_1 +/- sqrt(c_1^2 - 4*c_0*c_2)) / (2*c_2)
        #
        # There are three scenarios for the solutions of d:
        #
        #    1) Two positive solutions: the direction vector points towards the spheroid (from the origin) and we
        #                               get the entry and exit points of the line with the spheroid
        #    2) Two negative solutions: similar to 1) but the direction vector points away from the spheroid
        #    3) Imaginary solutions:    there is no intersection
        #

        p = origin
        u = direction

        a = self.semi_major_axis
        b = self.semi_minor_axis

        a2 = a**2
        b2 = b**2

        # Calculate the coefficients c_0, c_1, c_2 (NOTE: these have all been multiplied by (a^2 * b^2) to avoid divisions):
        c_0 = ((p[0]**2 + p[1]**2) * b2) + ((p[2]**2) * a2) - (a2*b2)
        c_1 = (((2*u[0]*p[0]) + (2*u[1]*p[1])) * b2) + ((2*u[2]*p[2]) * a2)
        c_2 = ((u[0]**2 + u[1]**2) * b2) + ((u[2]**2) * a2)

        # Solve the quadratic equation
        sqrt_arg = c_1**2 - (4*c_2*c_0)
        denom = 2*c_2

        d_0 = -c_1 / denom

        # Ignore sqrt warnings (the result will be set to NaN and that is perfectly fine because it will be handled by np.minimum)
        np.seterr(invalid="ignore")
        d_1 = np.sqrt(sqrt_arg)
        np.seterr(invalid="warn")

        d_1 /= denom

        d_plus  = d_0 + d_1
        d_minus = d_0 - d_1

        # Negative values of d indicate u points away from the spheroid and should not be included, so replace those with NaN
        d_plus = np.where(d_plus < 0, np.nan, d_plus)
        d_minus = np.where(d_minus < 0, np.nan, d_minus)

        # Take only the closest intersection
        d = np.minimum(d_plus, d_minus)

        return d, (p + (d * u))

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

    @staticmethod
    def spherical_to_geographic(phi_deg: float | np.ndarray, theta_deg: float | np.ndarray) -> tuple[float, float] | tuple[np.ndarray, np.ndarray]:
        """Convert spherical coordinates (azimuthal and polar angle) to geographic coordinates (latitude and longitude).

        If arrays of spherical coordinates are provided, arrays of geographic coordinates will be returned.

        Args:
            phi_deg: Azimuthal angle in degrees
            theta_deg: Polar angle in degrees

        Returns:
            A tuple of (latitude, longitude) both in degrees
        """
        latitude = 90 - theta_deg
        longitude = phi_deg # Already in the range -180 to 180
        return (latitude, longitude)

    @staticmethod
    def geographic_to_spherical(latitude: float | np.ndarray, longitude: float | np.ndarray) -> tuple[float, float] | tuple[np.ndarray, np.ndarray]:
        """Convert geographic coordinates (latitude and longitude) to spherical coordinates (azimuthal and polar angle).

        If arrays of geographic coordinates are provided, arrays of spherical coordinates will be returned.

        Args:
            latitude: Latitude in degrees
            longitude: Longitude in degrees

        Returns:
            A tuple of (phi, theta), where phi is the azimuthal angle and theta is the polar angle (both in degrees)
        """
        theta_deg = 90 - latitude
        phi_deg = longitude # Already in the range -180 to 180
        return (phi_deg, theta_deg)
