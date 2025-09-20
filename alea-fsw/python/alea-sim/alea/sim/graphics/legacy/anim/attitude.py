from typing import List, Tuple, Iterable

import numpy as np

from mpl_toolkits import mplot3d
from mpl_toolkits.mplot3d import art3d
import matplotlib.pyplot as plt
from matplotlib import animation

from ..sim.spacecraft.cubesat import Cubesat
from ..math.math_ahrs import AHRSQuaternion as Quaternion
from . import graphics

class ADCSAttitudeAnimation:
    CANVAS_SIZE = 2

    def __init__(self, draw_unit_sphere: bool = True, draw_cube: bool = True, draw_q_est: bool = False, draw_acc: bool = False, draw_mag: bool = False, draw_sun: bool = False):
        self._coord_axes = graphics.CoordinateAxes(ADCSAttitudeAnimation.CANVAS_SIZE, ["red", "green", "blue"], alpha=0.1, include_neg=True)

        self._unit_sphere: graphics.Sphere = None
        if draw_unit_sphere:
            self._unit_sphere = graphics.Sphere(1.0, "lightgray")

        self._sim_axes = graphics.CoordinateAxes(1.0, ["red", "green", "blue"], alpha=0.5, marker_symbol="o", labels=["sim x", "sim y", "sim z"], trace_path=True, max_trace_points=101)

        self._cube: graphics.Cube = None
        if draw_cube:
            self._cube = graphics.Cube(0.25)

        self._acc: graphics.Vector = None
        if draw_acc:
            self._acc = graphics.Vector(np.array([0.0, 0.0, 0.0]), "black", alpha=0.25, label="gravity", normalize=True, trace_path=True, max_trace_points=101)

        self._mag: graphics.Vector = None
        if draw_mag:
            self._mag = graphics.Vector(np.array([0.0, 0.0, 0.0]), "magenta", alpha=0.25, label="mag field", normalize=True, trace_path=True, max_trace_points=101)

        self._sun: graphics.Vector = None
        if draw_sun:
            self._sun = graphics.Vector(np.array([0.0, 0.0, 0.0]), "yellow", alpha=0.25, label="sun vector", normalize=True, trace_path=True, max_trace_points=101)

        self._q_est_axes: graphics.CoordinateAxes = None
        if draw_q_est:
            self._q_est_axes = graphics.CoordinateAxes(1.0, ["orange", "lime", "cyan"], alpha=1.0, marker_symbol="o", labels=["Est. Q x", "Est. Q y", "Est. Q z"], trace_path=True, max_trace_points=101)

        self._error_line: plt.Line2D = None
        self._error_data: Tuple[List[float], List[float]] = ([], [])

        self._gyro_x_line: plt.Line2D = None
        self._gyro_y_line: plt.Line2D = None
        self._gyro_z_line: plt.Line2D = None
        self._gyro_data: Tuple[List[float], List[float], List[float], List[float]] = ([], [], [], [])

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

        error_ax: plt.Axes = fig.add_subplot(grid_spec[0, 1])
        self._plot_error(error_ax)

        gyros_ax: plt.Axes = fig.add_subplot(grid_spec[1, 1], sharex=error_ax)
        self._plot_gyros(gyros_ax)

        self._fig = fig

    def _plot_model(self, ax: mplot3d.Axes3D):
        ax.set_aspect('equal')
        ax.set_axis_off()
        ax.view_init(azim=45)

        ax_lim = ADCSAttitudeAnimation.CANVAS_SIZE - 1.0
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

        if self._acc is not None:
            self._acc.plot(ax)

        if self._mag is not None:
            self._mag.plot(ax)
        
        if self._sun is not None:
            self._sun.plot(ax)

        if self._q_est_axes is not None:
            self._q_est_axes.plot(ax)

        ax.legend(loc="upper left")

    def _plot_error(self, ax: plt.Axes):
        ax.set_title("Quaternion Estimate Angle Error")
        ax.set_xlabel("Time (ms)")
        ax.set_ylabel("Angle (deg)")

        ax.set_xlim((0, self._sat.sim_data.size))
        ax.set_ylim((-180, 180))

        ax.grid(True)

        self._error_line, = ax.plot(0, 0)

    def _plot_gyros(self, ax: plt.Axes):
        ax.set_title("IMU Gyroscope Measurement")
        ax.set_xlabel("Time (ms)")
        ax.set_ylabel("Angular Velocity [rad/s]")

        # x limit controlled by error plot
        ax.set_ylim((-10, 10))

        ax.grid(True)

        self._gyro_x_line, = ax.plot(0, 0, color="red", label=r'$\omega_x$')
        self._gyro_y_line, = ax.plot(0, 0, color="green", label=r'$\omega_y$')
        self._gyro_z_line, = ax.plot(0, 0, color="blue", label=r'$\omega_z$')

        ax.legend(loc="upper right")

    def animate(self, sat:Cubesat, estimator,
                auto_start: bool = False, start_delay: float = 0,
                blit: bool = True, print_time: bool = False):

        self._sat = sat
        self.plot()

        # Collect list of artists for blitting
        artists = []
        artists.append(self._sim_timestamp)
        artists.extend(self._sim_axes.artists)
        if self._cube       is not None: artists.extend(self._cube.artists)
        if self._acc        is not None: artists.extend(self._acc.artists)
        if self._mag        is not None: artists.extend(self._mag.artists)
        if self._q_est_axes   is not None: artists.extend(self._q_est_axes.artists)
        if self._error_line is not None: artists.append(self._error_line)
        artists.extend([self._gyro_x_line, self._gyro_y_line, self._gyro_z_line])
        
        self.dt = sat.sim_data.time_step
        self.t_s = 0
        size = sat.sim_data.size

        def frame_generator() -> Iterable[float]:
            count = 0
            while count <= size:
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
            sim_orientation = sat.sim_data.get_attitude_quaternion(t=self.t_s)
            sat.update_state(sim_orientation)

            # Collect measurements
            gyr, acc, mag, sun = sat.measure(self.t_s)
            
            obs = [acc.value, mag.value, sun.value]
            ref = [sat.sim_data.get_accel_vector(self.t_s), sat.sim_data.get_mag_vector_ref(self.t_s), sat.sim_data.get_sun_vector_ref(self.t_s)]
            
            #estimate state
            est_orientation:Quaternion = Quaternion(estimator(np.array(obs), np.array(ref)))
            sat.update_estimate(est_orientation)

            # Update gyro plot
            if (len(self._gyro_data[0]) == 0) or advance:
                self._gyro_data[0].append(self.t_s)
                self._gyro_data[1].append(gyr.value[0])
                self._gyro_data[2].append(gyr.value[1])
                self._gyro_data[3].append(gyr.value[2])

                self._gyro_x_line.set_data(self._gyro_data[0], self._gyro_data[1])
                self._gyro_y_line.set_data(self._gyro_data[0], self._gyro_data[2])
                self._gyro_z_line.set_data(self._gyro_data[0], self._gyro_data[3])

            # Update plots
            self._sim_axes.update(sim_orientation)
            if self._cube is not None: self._cube.update(sim_orientation)
            if self._acc  is not None: self._acc.update(ref[0])
            if self._mag  is not None: self._mag.update(ref[1])
            if self._sun is not None: self._sun.update(ref[2])

            if self._q_est_axes is not None:
                self._q_est_axes.update(est_orientation)

                error_angle_deg = sat.get_state_error_deg()
                if (len(self._error_data[0]) == 0) or advance:
                    self._error_data[0].append(self.t_s)
                    self._error_data[1].append(error_angle_deg)

                    self._error_line.set_data(self._error_data[0], self._error_data[1])

            self._drawn = True

            return artists

        self._anim = animation.FuncAnimation(self._fig, anim_frame, frames=frame_generator(), interval=int(self.dt*1000), blit=blit, repeat=False, save_count=sat.sim_data.size)

        if auto_start:
            self.resume()
        else:
            self.pause()

        self._fig.canvas.mpl_connect('key_press_event', self.toggle_pause)

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
