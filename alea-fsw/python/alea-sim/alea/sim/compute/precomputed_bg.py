from typing import TypeVar, Generic, Callable, Generator, Iterable
from concurrent.futures import ProcessPoolExecutor
from multiprocessing import Process, Queue

from .precomputed import PreComputed

A = TypeVar("A")
R = TypeVar("R")
D = TypeVar("D")

# Cross-Process sentinel (simple object() won't work because the id changes across processes)
# The assumption is an object of this type will never be placed in the queue by the user
class _Sentinel:
    def __eq__(self, other):
        return self.__class__ == other.__class__

class QueueIterable(Generic[R, D]):
    """Multi-process-safe iterable that uses a Queue as the underlying data buffer

    Use QueueIterable.push() to add data to the iterable.

    Use iter(obj) and pass an instance of QueueIterable to create an iterator over this
    iterable.

    Use QueueIterable.finish() to mark the end of the iterable and the corresponding
    iterator created with iter(obj).

    NOTE: Iterating over an instance of this class consumes the items - they will no
          longer be available if you iterate over the class again.
    """

    def __init__(self, buffer_size: int = 2, batch: bool = False):
        self._q = Queue(maxsize=buffer_size)
        self._batch = batch

        # Object that indicates the dataset is complete when it's placed in the queue
        self._sentinel = _Sentinel()

    def push(self, value: R):
        self._q.put(value)

    def finish(self):
        self._q.put(self._sentinel)

    def __iter__(self) -> Generator[D, None, None]:
        while True:
            # Read results off the queue until we reach the self._sentinel object
            result = self._q.get()

            if self._sentinel == result:
                # No more data so finish off the generator
                return

            if self._batch:
                # In batch mode, the result from the queue is an iterable itself
                yield from result
            else:
                yield result

class PreComputedBG(PreComputed[A, R, D]):
    """Implementation of PreComputed in which data will be pre-computed in a background process.
    The entire PreComputedBG object instance must be pickleable, so it can be passed to the
    background process. This background process will be daemonized if chunk_size is 0 (since it
    will not be using a process pool and therefore will not have to spawn any child processes).
    """

    def __init__(self, compute_func: Callable[[A], R], buffer_size: int = 2, batch: bool = False, chunk_size: int = 0):
        """Initializes a PreComputedBG instance.

        Args:
            compute_func:
                See PreComputed.__init__().

            buffer_size (optional):
                See PreComputed.__init__().

            batch (optional):
                See PreComputed.__init__().

            chunk_size (optional):
                If batch is True and chunk_size > 0, each argument batch yielded by arg_gen will be distributed
                across a process pool with each process receiving chunk_size items to process at once.
                If chuck_size is 0, the process pool will not be used. Defaults to 0.
        """
        super().__init__(compute_func, buffer_size=buffer_size, batch=batch)
        self._chunk_size = chunk_size

        # A new iterable will be created for every iterator created on this class so that
        # multiple clients can consume the data in parallel.
        self._iterables: list[QueueIterable[R, D]] = []

        self._process = Process(target=self._pre_compute_bg, daemon=(self._chunk_size == 0))

    @property
    def chunk_size(self) -> int:
        return self._chunk_size

    def start(self, args: Iterable[A]):
        """Stores the args and starts the background pre-computation process. Must be called before you start iterating.

        Args:
            args: See PreComputed.start().
        """
        super().start(args) # Save the args so the background process can use it
        self._process.start()

    def stop(self):
        """Stop the background pre-computation process.
        """
        self._process.terminate()

    def _pre_compute_bg(self):
        """Function to run in a background process to pre-compute data items
        """
        if self.batch and (self.chunk_size > 0):
            # Start a ProcessPool
            with ProcessPoolExecutor() as executor:
                for arg in self.args:
                    # Distribute the computation across the process pool
                    results = executor.map(self.compute_func, arg, chunksize=self.chunk_size)
                    results = list(results) # Essentially wait for all the results to come in

                    for iterable in self._iterables:
                        iterable.push(results)
        else:
            for arg in self.args:
                result = self.compute_func(arg)

                for iterable in self._iterables:
                    iterable.push(result)

        # Mark the stream as done when we run out of arguments
        for iterable in self._iterables:
            iterable.finish()

    def subscribe(self) -> Iterable[D]:
        """Create a new iterable over the pre-computed data produced by this class.

        All iterables must be created before calling self.start() and before starting
        iterating.

        Raises:
            RuntimeError: If called after self.start()
        """
        if self.started:
            raise RuntimeError("Cannot create new iterable after calling start()")

        iterable: QueueIterable[R, D] = QueueIterable(buffer_size=self._buffer_size, batch=self._batch)
        self._iterables.append(iterable)
        return iterable
