from typing import Iterable
from enum import Enum
from pathlib import Path

import matplotlib.pyplot as plt
from matplotlib import animation
from matplotlib.backend_bases import KeyEvent

from alea.sim.kernel.kernel import AleasimKernel
from alea.sim.graphics.scene.base_scene import BaseScene

class State(Enum):
    AUTO_PAUSED = 0
    START_DELAY = 1
    RESUMING    = 2
    RUNNING     = 3
    PAUSED      = 4
    DONE        = 5

class StateEvent(Enum):
    FRAME  = 0
    TOGGLE_PAUSE_RESUME = 1

class SimAnimation:
    def __init__(self, sim_kernel: AleasimKernel, title: str, layout: tuple[int, int] = (1,1), **kwargs):
        self._sim_kernel = sim_kernel
        self._title = title
        self._layout = layout

        self._kwargs = kwargs

        self._anim: animation.FuncAnimation = None

        self._state = State.AUTO_PAUSED
        self._start_delay = 0
        self._start_delay_counter = 0

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

    def plot(self) -> list[plt.Artist]:
        artists: list[plt.Artist] = []
        for scene in self._scenes:
            artists.extend(scene.plot())
        return artists

    def update(self) -> list[plt.Artist]:
        artists: list[plt.Artist] = []
        for scene in self._scenes:
            artists.extend(scene.update())
        return artists

    def animate(self, duration: float, auto_start: bool = False, start_delay: int = 0, blit: bool = True,
                print_time: bool = False):

        self._start_delay = start_delay
        self._start_delay_counter = 0

        self.plot()

        if auto_start:
            self._state = State.START_DELAY

        def frame_generator() -> Iterable[State]:
            while self.sim_kernel.time <= duration:
                yield True

            self._state = State.DONE

        def anim_frame(_: bool) -> list[plt.Artist]:
            state = self._state

            # By default, assume nothing needs to be re-drawn
            artists = []

            if state == State.RUNNING:

                self.sim_kernel.step()
                if print_time:
                    print(f"Simulation Time: {self.sim_kernel.time} s / {duration} s")
                artists = self.update()

            self._update_state(StateEvent.FRAME)

            return artists

        self._anim = animation.FuncAnimation(self.figure, anim_frame, frames=frame_generator(), init_func=None, interval=1, blit=blit, cache_frame_data=False, repeat=False)

        self.figure.canvas.mpl_connect('key_press_event', self.toggle_pause)

    def toggle_pause(self, event: KeyEvent):
        if self._state != State.DONE:
            if event.key == " ":
                self._update_state(StateEvent.TOGGLE_PAUSE_RESUME)

    def _pause_anim(self):
        if self._anim is not None:
            self._anim.pause()

    def _resume_anim(self):
        if self._anim is not None:
            self._anim.resume()

    def _update_state(self, event: StateEvent):
        state = self._state

        if state == State.AUTO_PAUSED:

            if event == StateEvent.TOGGLE_PAUSE_RESUME:
                # Proceed to start delay once the user starts the animation
                self._state = State.START_DELAY
                self._resume_anim()
            elif event == StateEvent.FRAME:
                # We expect to receive a few FRAME events at the very beginning before the animation
                # actually pauses, so we'll keep calling pause() until the animation pauses.
                self._pause_anim()

        elif state == State.START_DELAY:

            if event == StateEvent.FRAME:
                # Count the frames until we can actually start running
                self._start_delay_counter += 1
                if self._start_delay_counter >= self._start_delay:
                    self._state = State.RESUMING
            elif event == StateEvent.TOGGLE_PAUSE_RESUME:
                # Return to AUTO_PAUSED so that when we exit this state, we return to START_DELAY
                # and can finish the delay instead of directly to RUNNING
                self._state = State.AUTO_PAUSED
                self._pause_anim()

        elif state == State.RESUMING:

            # Intermediate state before RUNNING that exists so that the animation function
            # can re-draw the entire frame when resuming
            if event == StateEvent.FRAME:
                self._state = State.RUNNING

        elif state == State.RUNNING:

            if event == StateEvent.TOGGLE_PAUSE_RESUME:
                self._state = State.PAUSED
                self._pause_anim()

        elif state == State.PAUSED:

            if event == StateEvent.TOGGLE_PAUSE_RESUME:
                self._state = State.RESUMING
                self._resume_anim()

        elif state == State.DONE:
            pass

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
