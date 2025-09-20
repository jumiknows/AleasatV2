import unittest
from typing import Generator
import time

from alea.sim.compute.precomputed import PreComputed
from alea.sim.compute.precomputed_fg import PreComputedFG
from alea.sim.compute.precomputed_bg import PreComputedBG

class DummyComputeFunc:
    def __call__(self, arg: int) -> int:
        # Pretend to be slow the just return the argument
        time.sleep(0.05)
        return arg

class ArgGen:
    def __init__(self, stop: int = 20, batch: bool = False, batch_size: int = 5):
        self._stop = stop
        self._batch = batch
        self._batch_size = batch_size

    def __iter__(self) -> Generator[int, None, None]:
        i = 0
        while i < self._stop:
            if self._batch:
                yield list(range(i, i + self._batch_size))
                i += self._batch_size
            else:
                yield i
                i += 1

class TestPreComputedBG(PreComputedBG[int, int, int]):
    def __init__(self, batch: bool = False):
        super().__init__(DummyComputeFunc(), buffer_size=10, batch=batch, chunk_size=1)

class TestPreComputedFG(PreComputedFG[int, int, int]):
    def __init__(self, batch: bool = False):
        super().__init__(DummyComputeFunc(), buffer_size=10, batch=batch)

class PreComputedTest(unittest.TestCase):
    @staticmethod
    def args(batch: bool = False):
        return ArgGen(stop=20, batch=batch, batch_size=5)

    def _check_all_values(self, pre_comp: PreComputed, args: ArgGen):
        i = 0
        pre_comp_iter = pre_comp.subscribe()

        pre_comp.start(args)
        for x in pre_comp_iter:
            self.assertEqual(i, x)
            i += 1

        self.assertEqual(x, (args._stop - 1))

    def test_foreground(self):
        batch = False
        pre_comp = TestPreComputedFG(batch=batch)
        self._check_all_values(pre_comp, self.args(batch))

    def test_foreground_batch(self):
        batch = True
        pre_comp = TestPreComputedFG(batch=batch)
        self._check_all_values(pre_comp, self.args(batch))

    def test_background(self):
        batch = False
        pre_comp = TestPreComputedBG(batch=batch)
        self._check_all_values(pre_comp, self.args(batch))

    def test_background_batch(self):
        batch = True
        pre_comp = TestPreComputedBG(batch=batch)
        self._check_all_values(pre_comp, self.args(batch))

    def test_compare_foreground_background(self):
        loop_delay = 0.1

        # ----------------------------------------
        # Foreground
        # ----------------------------------------
        fg = TestPreComputedFG()
        fg_iter = fg.subscribe()

        fg.start(self.args())

        start = time.time()
        for x in fg_iter:
            time.sleep(loop_delay)
        fg_elapsed = time.time() - start

        print(f"Foreground: {fg_elapsed} s")

        # ----------------------------------------
        # Background
        # ----------------------------------------
        bg = TestPreComputedBG()
        bg_iter = bg.subscribe()

        bg.start(self.args())

        start = time.time()
        for x in bg_iter:
            time.sleep(loop_delay)
        bg_elapsed = time.time() - start

        print(f"Background: {bg_elapsed} s")

        # Background should be faster than foreground
        self.assertLess(bg_elapsed, fg_elapsed)

    def test_compare_foreground_batch(self):
        loop_delay = 0.1

        # ----------------------------------------
        # Foreground
        # ----------------------------------------
        fg = TestPreComputedFG()
        fg_iter = fg.subscribe()

        fg.start(self.args())

        start = time.time()
        for x in fg_iter:
            time.sleep(loop_delay)
        fg_elapsed = time.time() - start

        print(f"Foreground: {fg_elapsed} s")

        # ----------------------------------------
        # Foreground Batch
        # ----------------------------------------
        fg_batch = TestPreComputedFG(batch=True)
        fg_batch_iter = fg_batch.subscribe()

        fg_batch.start(self.args(True))

        start = time.time()
        for x in fg_batch_iter:
            time.sleep(loop_delay)
        fg_batch_elapsed = time.time() - start

        print(f"Foreground Batch: {fg_batch_elapsed} s")

        # Batch should speed things up
        self.assertLess(fg_batch_elapsed, fg_elapsed)

    def test_compare_background_batch(self):
        # ----------------------------------------
        # Background
        # ----------------------------------------
        bg = TestPreComputedBG()
        bg_iter = bg.subscribe()

        bg.start(self.args())

        start = time.time()
        for x in bg_iter:
            pass
        bg_elapsed = time.time() - start

        print(f"Background: {bg_elapsed} s")

        # ----------------------------------------
        # Background Batch
        # ----------------------------------------
        bg_batch = TestPreComputedBG(batch=True)
        bg_batch_iter = bg_batch.subscribe()

        bg_batch.start(self.args(True))

        start = time.time()
        for x in bg_batch_iter:
            pass
        bg_batch_elapsed = time.time() - start

        print(f"Background Batch: {bg_batch_elapsed} s")

        # Batch should speed things up
        self.assertLess(bg_batch_elapsed, bg_elapsed)

if __name__ == "__main__":
    unittest.main()
