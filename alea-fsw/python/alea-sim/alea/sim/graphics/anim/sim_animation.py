from typing import Iterable
from pathlib import Path

from mpl_toolkits.mplot3d import art3d
import matplotlib.pyplot as plt
from matplotlib import animation

from alea.sim.kernel.kernel import AleasimKernel
from alea.sim.graphics.scene.base_scene import BaseScene

class SimAnimation:
    def __init__(self, sim_kernel: AleasimKernel, title: str, layout: tuple[int, int] = (1,1), **kwargs):
        self._sim_kernel = sim_kernel
        self._title = title
        self._layout = layout

        self._kwargs = kwargs

        self._anim: animation.FuncAnimation = None

        self._started = False
        self._paused = True
        self._drawn = False
        self._done = False

        self._scenes: list[BaseScene] = []

        self._fig = plt.figure(**self._kwargs)
        self._fig.suptitle(self._title)

        self._grid_spec = self._fig.add_gridspec(self._layout[0], self._layout[1],
                                                 left=0.0, right=0.95, top=0.9, bottom=0.1, hspace=0.3)

    @property
    def sim_kernel(self) -> AleasimKernel:
        return self._sim_kernel

    @property
    def figure(self) -> plt.Figure:
        return self._fig

    @property
    def grid_spec(self) -> plt.GridSpec:
        return self._grid_spec

    def add_scene(self, scene: BaseScene):
        self._scenes.append(scene)

    def plot(self) -> list[art3d.artist.Artist]:
        artists: list[art3d.artist.Artist] = []
        for scene in self._scenes:
            artists.extend(scene.plot())
        return artists

    def update(self) -> list[art3d.artist.Artist]:
        artists: list[art3d.artist.Artist] = []
        for scene in self._scenes:
            artists.extend(scene.update())
        return artists

    def animate(self, duration: float, auto_start: bool = False, start_delay: float = 0, blit: bool = True,
                print_time: bool = False):

        def init():
            return self.plot()

        def frame_generator() -> Iterable[bool]:
            start_delay_counter = 0
            while self.sim_kernel.time <= duration:
                if self._paused:
                    yield False
                else:
                    if auto_start:
                        if (start_delay_counter * self.sim_kernel.timestep) >= start_delay:
                            yield True
                        else:
                            start_delay_counter += 1
                            yield False
                    else:
                        yield True
            self._done = True

        def anim_frame(advance: bool) -> list[art3d.artist.Artist]:
            # It's not possible to pause the animation before showing it, so instead we pause
            # it immediately after the first frame is drawn
            if not auto_start:
                if not self._started:
                    self.pause()

            # If we're keeping the animation stationary and we've drawn at least one frame, return immediately
            if (not advance):# and self._drawn:
                # Nothing changed, so return an empty list
                return []

            if print_time:
                print(f"Simulation Time: {self.sim_kernel.time} s / {duration} s")

            if advance:
                self.sim_kernel.step()

            artists = self.update()

            self._drawn = True

            return artists

        self._anim = animation.FuncAnimation(self.figure, anim_frame, frames=frame_generator(), init_func=init, interval=1, blit=blit, repeat=False)

        if auto_start:
            self.resume()
        else:
            self.pause()

        self.figure.canvas.mpl_connect('key_press_event', self.toggle_pause)

    def toggle_pause(self, event):
        if not self._done:
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

    def save(self, dt: float, out_path: Path):
        writers = {
            ".gif": "pillow",
            ".html": "html",
            ".mp4": "ffmpeg",
        }
        writer = writers[out_path.suffix]
        self._anim.save(str(out_path.resolve()), writer, fps=int(1/dt))

    def show(self):
        plt.show()
