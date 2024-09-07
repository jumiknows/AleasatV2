from alea.sim.kernel.kernel import AleasimKernel
from alea.sim.epa.orbit_dynamics import OrbitDynamicsModel

from alea.sim.graphics.scene.orbit_scene import OrbitScene2D, OrbitScene3D, AltitudeScene
from alea.sim.graphics.anim.sim_animation import SimAnimation

class OrbitWindow:
    def __init__(self, sim_kernel: AleasimKernel, duration: float, **kwargs):
        self._anim = SimAnimation(sim_kernel, "Orbit Simulation", (2, 2), **kwargs)

        odyn = sim_kernel.get_model(OrbitDynamicsModel)

        orbit_scene_3d = OrbitScene3D.create(self.anim.figure, self.anim.grid_spec[:, 0], odyn=odyn, orbit_trail_minutes=90)
        self._anim.add_scene(orbit_scene_3d)

        orbit_scene_2d = OrbitScene2D.create(self.anim.figure, self.anim.grid_spec[0, 1], odyn=odyn, orbit_trail_minutes=90, use_blue_marble=True)
        self._anim.add_scene(orbit_scene_2d)

        altitude_scene = AltitudeScene.create(self.anim.figure, self.anim.grid_spec[1, 1], model=odyn, xlim=[0, duration], ylim=[500, 700])
        self._anim.add_scene(altitude_scene)

    @property
    def anim(self) -> SimAnimation:
        return self._anim

if __name__ == "__main__":
    import logging
    import argparse
    from pathlib import Path

    parser = argparse.ArgumentParser()
    parser.add_argument("dt", type=float, action="store", help="Simulation Timestep (in seconds)")
    parser.add_argument("duration", type=float, action="store", help="Simulation Duration (in minutes)")
    parser.add_argument("--output", "-o", type=str, action="store", help="Output file to save animation")

    args = parser.parse_args()

    dt: float = args.dt
    duration: float = args.duration * 60 # min -> sec
    output: str = args.output

    logging.basicConfig(level=logging.INFO)

    sim_kernel = AleasimKernel(dt=dt)
    odyn = OrbitDynamicsModel(sim_kernel)
    sim_kernel.add_model(odyn)

    window = OrbitWindow(sim_kernel, duration, figsize=(16, 8), dpi=100)

    if output is None:
        window.anim.animate(duration=duration, auto_start=False, blit=True, print_time=False)
        window.anim.show()
    else:
        output = Path(output)
        window.anim.animate(duration=duration, auto_start=True, start_delay=0, blit=False, print_time=True)
        window.anim.save(1/30, output)
