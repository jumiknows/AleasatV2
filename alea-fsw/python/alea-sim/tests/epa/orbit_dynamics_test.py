import unittest

import numpy as np

from sgp4.api import Satrec

import skyfield.timelib
import skyfield.api

from alea.sim.kernel.kernel import AleasimKernel
from alea.sim.epa.orbit_dynamics import OrbitDynamicsModel, OrbitDynamicsConfig

from alea.sim.utils import skyfield_utils
from alea.sim.utils.jpl_horizons import JPLHorizons

class OrbitDynamicsTest(unittest.TestCase):
    TIMESTEP = 10
    N_TIMESTEPS = 540

    ONE_HR_JD  = 1 / 24
    ONE_MIN_JD = ONE_HR_JD / 60
    ONE_SEC_JD = ONE_MIN_JD / 60
    ONE_MS_JD  = ONE_SEC_JD / 1000

    # TLE is from CANX-2 (https://celestrak.org/NORAD/elements/gp.php?CATNR=32790&FORMAT=tle)
    # Julian Date: 24244 = 31-Aug-2024
    # which has the following orbital parameters:
    #
    #     | Period (minutes) | Inclination (deg) | Apogee Height (km) | Perigee Height (km) | Eccentricity |
    #     | ---------------- | ----------------- | ------------------ | ------------------- | ------------ |
    #     |      96.23       |       97.77       |        588         |         568         |   0.0014074  |
    #
    TLE = [
        "1 32790U 08021H   24244.87606713  .00010443  00000+0  92429-3 0  9996",
        "2 32790  97.7736 208.3416 0014074  84.6385 275.6441 14.96391346886609",
    ]

    @staticmethod
    def tle_to_orbit_elements(tle: list[str]) -> dict[str, float]:
        satrec = Satrec.twoline2rv(*tle)
        return {
            "ecc"          : satrec.ecco,     # ecco: eccentricity
            "argp"         : satrec.argpo,    # argpo: argument of perigee (radians)
            "incl"         : satrec.inclo,    # inclo: inclination (radians)
            "mean_anomaly" : satrec.mo,       # mo: mean anomaly (radians)
            "mean_motion"  : satrec.no_kozai, # no_kozai: mean motion (radians/minute)
            "raan"         : satrec.nodeo,    # nodeo: right ascension of ascending node (radians)
            "bstar"        : satrec.bstar,    # bstar: drag coefficient (/earth radii)
        }

    @classmethod
    def setUpClass(cls):
        cls.ORBIT_ELEMENTS = cls.tle_to_orbit_elements(cls.TLE)

        # Create skyfield Time object for TLE epoch
        ts = skyfield.api.load.timescale()
        satrec = Satrec.twoline2rv(*cls.TLE)
        two_digit_year = satrec.epochyr
        if two_digit_year < 57:
            year = two_digit_year + 2000
        else:
            year = two_digit_year + 1900

        cls.start_time = ts.utc(year, 1, satrec.epochdays)

        # Generate the same time-series that the simulation will use
        time_generator = skyfield_utils.SkyfieldTimeGen(
                            start_time  = cls.start_time,
                            timestep    = cls.TIMESTEP,
                            n_timesteps = cls.N_TIMESTEPS,
                            timescale   = ts,
                        )
        tlist: list[float] = list(map(lambda t : t.tt, next(iter(time_generator))))

        # Fetch reference data from JPL Horizons
        jpl_horizons = JPLHorizons()
        cls.sat_states_jpl = jpl_horizons.get_vector_table(cls.TLE, tlist, JPLHorizons.Command.TLE, JPLHorizons.Center.EARTH,
                                                           vec_corr=JPLHorizons.VectorCorrection.GEOMETRIC, units=JPLHorizons.Units.KM_S,
                                                           table_type=JPLHorizons.TableType.STATE_VEC)
        cls.sun_vectors_jpl = jpl_horizons.get_vector_table(cls.TLE, tlist, JPLHorizons.Command.SUN, JPLHorizons.Center.TLE,
                                                            vec_corr=JPLHorizons.VectorCorrection.APPARENT, units=JPLHorizons.Units.AU_D,
                                                            table_type=JPLHorizons.TableType.POSITION)

    def _run_sat_state_test(self, kernel: AleasimKernel, odyn: OrbitDynamicsModel):
        max_position_diff = 0
        max_velocity_diff = 0

        self.assertEqual(len(self.sat_states_jpl), self.N_TIMESTEPS)

        for time_jpl, sat_state_jpl in self.sat_states_jpl:
            kernel.step()

            self.assertAlmostEqual(odyn.skyfield_time.tt, time_jpl, delta=self.ONE_MS_JD)

            # NOTE: Subtracting the vectors and checking the norm bounds the spherical error which
            #       is a bit more meaningful than the cubic error you would get from an element-wise
            #       comparison

            position_sim = odyn.position_eci
            position_jpl = sat_state_jpl[0:3] * 1000 # convert km to m
            position_diff = np.linalg.norm(position_sim - position_jpl)
            self.assertLessEqual(position_diff, 50) # 50 metres of position difference tolerated
                                                    # This is likely due to Horizons using WGS 72
                                                    # instead of WGS 84
            max_position_diff = max(max_position_diff, position_diff)

            velocity_sim = odyn.velocity_eci
            velocity_jpl = sat_state_jpl[3:6] * 1000 # convert km to m
            velocity_diff = np.linalg.norm(velocity_sim - velocity_jpl)
            self.assertLessEqual(velocity_diff, 0.1) # 0.1 m/s of velocity difference tolerated
            max_velocity_diff = max(max_velocity_diff, velocity_diff)

        print(f"Max Position Diff: {max_position_diff:.3f} m")
        print(f"Max Velocity Diff: {max_velocity_diff:.6f} m/s")

    def test_sat_state_tle(self):
        cfg = OrbitDynamicsConfig(tle=self.TLE)
        kernel = AleasimKernel(dt=self.TIMESTEP, date=self.start_time)
        odyn = OrbitDynamicsModel(kernel, cfg = cfg)
        kernel.add_model(odyn)
        self._run_sat_state_test(kernel, odyn)

    def test_sat_state_tle_precompute(self):
        cfg = OrbitDynamicsConfig(tle=self.TLE, use_precompute=True, precompute_batch_size=100)
        kernel = AleasimKernel(dt=self.TIMESTEP, date=self.start_time)
        odyn = OrbitDynamicsModel(kernel, cfg = cfg)
        kernel.add_model(odyn)
        self._run_sat_state_test(kernel, odyn)

    def test_sat_state_sgp4_init_data(self):
        cfg = OrbitDynamicsConfig(parameter_type=OrbitDynamicsConfig.ParameterType.ORBIT_ELEMENTS, orbit_elements=self.ORBIT_ELEMENTS)
        kernel = AleasimKernel(dt=self.TIMESTEP, date=self.start_time)
        odyn = OrbitDynamicsModel(kernel, cfg = cfg)
        kernel.add_model(odyn)
        self._run_sat_state_test(kernel, odyn)

    def _run_sun_vector_test(self, kernel: AleasimKernel, odyn: OrbitDynamicsModel):
        max_sun_vector_diff = 0

        for time_jpl, sun_vector_jpl in self.sun_vectors_jpl:
            kernel.step()

            self.assertAlmostEqual(odyn.skyfield_time.tt, time_jpl, delta=self.ONE_MS_JD)

            sun_vector_sim = odyn.sun_vector
            sun_vector_diff = np.linalg.norm(sun_vector_sim - sun_vector_jpl)
            self.assertLessEqual(sun_vector_diff, 1E-8) # 1E-8 au ~= 1.5 km of difference tolerated
            max_sun_vector_diff = max(max_sun_vector_diff, sun_vector_diff)

        print(f"Max Sun Vector Diff: {max_sun_vector_diff:.10f} au")

    def test_sun_vector(self):
        cfg = OrbitDynamicsConfig(tle=self.TLE)
        kernel = AleasimKernel(dt=self.TIMESTEP, date=self.start_time)
        odyn = OrbitDynamicsModel(kernel, cfg = cfg)
        kernel.add_model(odyn)
        self._run_sun_vector_test(kernel, odyn)

    def test_sun_vector_precompute(self):
        cfg = OrbitDynamicsConfig(tle=self.TLE, use_precompute=True, precompute_batch_size=100)
        kernel = AleasimKernel(dt=self.TIMESTEP, date=self.start_time)
        odyn = OrbitDynamicsModel(kernel, cfg = cfg)
        kernel.add_model(odyn)
        self._run_sun_vector_test(kernel, odyn)
