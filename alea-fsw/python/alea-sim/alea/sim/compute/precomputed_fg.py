from typing import TypeVar, Generic, Callable, Generator, Iterable
from collections import deque
import functools
import operator

from .precomputed import PreComputed

A = TypeVar("A")
R = TypeVar("R")
D = TypeVar("D")

class ForegroundIterable(Generic[A, R, D]):
    """Iterable for use with PreComputedFG that fills the data buffers of all iterables created on the
    parent PreComputedFG instance with the pre-computed data any time there is space available in all
    of the iterables. This way the data can be computed once, but consumed multiple times by parallel
    iterables.
    """

    def __init__(self, parent: "PreComputedFG", compute_func: Callable[[A], R], buffer_size: int = 2, un_batch: bool = False):
        self._parent = parent
        self._compute_func = compute_func
        self._un_batch = un_batch

        self._buffer = deque(maxlen=buffer_size)

    def push(self, value: R):
        self._buffer.append(value)

    @property
    def has_space(self) -> bool:
        return len(self._buffer) < self._buffer.maxlen

    def __iter__(self) -> Generator[D, None, None]:
        if self._parent.args is None:
            raise RuntimeError(f"Cannot iterate when args is None. Make sure you called PreComputedFG.start() before iterating.")

        arg_iter = iter(self._parent.args)

        while True:
            # Foreground pre-computation
            while self._parent.all_have_space():
                try:
                    arg = next(arg_iter)
                except StopIteration:
                    # No more arguments so stop trying pre-compute
                    break

                result = self._compute_func(arg)
                self._parent.push_all(result)

            if len(self._buffer) == 0:
                # No more data so finish off the generator
                return

            next_result = self._buffer.popleft()
            if self._un_batch:
                # un_batch indicates next_result is an Iterable itself and we should yield
                # from it rather than yield the whole thing at once
                yield from next_result
            else:
                yield next_result

class PreComputedFG(PreComputed[A, R, D]):
    """Implementation of PreComputed in which data will be pre-computed in the foreground thread
    when data is read (by iteration) as long as there is more data available and there is space in
    all of the iterables that have been created.
    """

    def __init__(self, compute_func: Callable[[A], R], buffer_size: int = 2, batch: bool = False):
        super().__init__(compute_func, buffer_size=buffer_size, batch=batch)

        self._iterables: list[ForegroundIterable[A, R, D]] = []

    def all_have_space(self) -> bool:
        """
        Returns:
            True if all the iterables that have been created have space for new pre-computed data.
        """
        return functools.reduce(operator.and_, map(lambda x : x.has_space, self._iterables))

    def push_all(self, result: R):
        """Push a pre-computed value to all iterables that were created.
        """
        for iterable in self._iterables:
            iterable.push(result)

    def subscribe(self, un_batch: bool = True) -> Iterable[D]:
        if self.started:
            raise RuntimeError("Cannot create new iterable after calling start()")

        # Can only un_batch if this PreComputed produces batches to begin with
        un_batch = un_batch and self.batch

        iterable: ForegroundIterable[A, R, D] = ForegroundIterable(self, self._compute_func, buffer_size=self._buffer_size, un_batch=un_batch)
        self._iterables.append(iterable)
        return iterable
