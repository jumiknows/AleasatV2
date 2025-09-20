#TODO fix this, does not currently work
from typing import List, Tuple, Iterable

import numpy as np

from mpl_toolkits import mplot3d
from mpl_toolkits.mplot3d import art3d
import matplotlib.pyplot as plt
from matplotlib import animation

from alea.sim.math_lib import Quaternion
from alea.sim.kernel.kernel import AleasimKernel
from alea.sim.spacecraft.spacecraft import Spacecraft
from . import graphics

class AttitudeAnimation:
    CANVAS_SIZE = 2

    def __init__(self, draw_unit_sphere: bool = True, draw_cube: bool = True):
        self._coord_axes = graphics.CoordinateAxes(AttitudeAnimation.CANVAS_SIZE, ["red", "green", "blue"], alpha=0.1, include_neg=True)

        self._unit_sphere: graphics.Sphere = None
        if draw_unit_sphere:
            self._unit_sphere = graphics.Sphere(1.0, "lightgray")

        self._sim_axes = graphics.CoordinateAxes(1.0, ["red", "green", "blue"], alpha=0.5, marker_symbol="o", labels=["sim x", "sim y", "sim z"], trace_path=True, max_trace_points=101)

        self._cube: graphics.Cube = None
        if draw_cube:
            self._cube = graphics.Cube(0.25)

        self._fig: plt.Figure = None
        self._anim: animation.FuncAnimation = None
        self._started = False
        self._paused = True
        self._drawn = False

    def plot(self):
        fig = plt.figure(figsize=(16, 8), dpi=100)
        fig.suptitle("Attitude Simulation")

        grid_spec = fig.add_gridspec(2, 2, left=0.0, right=0.95, top=0.9, bottom=0.1, hspace=0.3)

        model_ax: mplot3d.Axes3D = fig.add_subplot(grid_spec[:, 0], projection="3d")
        self._plot_model(model_ax)

        self._fig = fig

    def _plot_model(self, ax: mplot3d.Axes3D):
        ax.set_aspect('equal')
        ax.set_axis_off()
        ax.view_init(azim=45)

        ax_lim = AttitudeAnimation.CANVAS_SIZE - 1.0
        ax.axes.set_xlim3d(left=-ax_lim, right=ax_lim)
        ax.axes.set_ylim3d(bottom=-ax_lim, top=ax_lim)
        ax.axes.set_zlim3d(bottom=-ax_lim, top=ax_lim)

        self._sim_timestamp: plt.Text = ax.text2D(0.80, 0.95, "t = 0 ms", transform=ax.transAxes)

        self._coord_axes.plot(ax)

        if self._unit_sphere is not None:
            self._unit_sphere.plot(ax)

        self._sim_axes.plot(ax)

        if self._cube is not None:
            self._cube.plot(ax)

        ax.legend(loc="upper left")

    def animate(self, kernel: AleasimKernel,
                auto_start: bool = False, start_delay: float = 0,
                blit: bool = True, print_time: bool = False):

        self.plot()

        # Collect list of artists for blitting
        artists = []
        artists.append(self._sim_timestamp)
        artists.extend(self._sim_axes.artists)
        if self._cube       is not None: artists.extend(self._cube.artists)
        
        self.dt = kernel.timestep
        self.t_s = kernel.time

        def frame_generator() -> Iterable[float]:
            count = 0
            while count <= 0:
                if self._paused:
                    yield False
                else:
                    if auto_start:
                        if (count * self.dt) >= start_delay:
                            yield True
                        else:
                            count += 1
                            yield False
                    else:
                        yield True

        def anim_frame(advance: bool) -> List[art3d.artist.Artist]:
            # It's not possible to pause the animation before showing it, so instead we pause
            # it immediately after the first frame is drawn
            if not auto_start:
                if not self._started:
                    self.pause()

            # If we're keeping the animation stationary and we've drawn at least one frame, return immediately
            if (not advance) and self._drawn:
                return artists

            if print_time:
                print(f"Simulation Time: {self.t_s} s / {self.dt * self._sat.sim_data.size} ms", end="\r")
            self._sim_timestamp.set_text(f"t = {self.t_s} s")

            if advance:
                self.t_s += self.dt
                self.t_s = round(self.t_s, 3)
            #true state
            attdyn: AttitudeDynamicsModel = kernel.get_model('attitude dynamics')
            sim_orientation = attdyn.quaternion
            attdyn.update()

            # Update plots
            self._sim_axes.update(sim_orientation)
            if self._cube is not None: self._cube.update(sim_orientation)

            self._drawn = True

            return artists

        self._anim = animation.FuncAnimation(self._fig, anim_frame, frames=None, interval=int(self.dt*1000), blit=blit, repeat=False)

        if auto_start:
            self.resume()
        else:
            self.pause()

        # self._fig.canvas.mpl_connect('key_press_event', self.toggle_pause)

    def toggle_pause(self, event):
        if event.key == " ":
            if self._paused:
                self.resume()
            else:
                self.pause()

    def pause(self):
        if self._anim is not None:
            self._anim.pause()
            self._paused = True

    def resume(self):
        if self._anim is not None:
            self._anim.resume()
            self._paused = False
            self._started = True

    def save(self, dt: float, out_path: str):
        writer = animation.FFMpegWriter(fps=int(1 / dt))
        self._anim.save(out_path, writer)

    def show(self):
        plt.show()
