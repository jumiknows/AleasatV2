from abc import ABC, abstractmethod
from typing import TypeVar, Generic, Callable, Iterable

A = TypeVar("A")
R = TypeVar("R")
D = TypeVar("D")

class PreComputed(ABC, Generic[A, R, D]):
    """Represents a set (possibly infinite) of pre-computed data (i.e. the next N elements are computed in advance,
    possibly in a background process and possibly multiple items in parallel).

    This class provides an iterator interface which will return pre-computed data if available or wait until more
    pre-computed data becomes available. Multiple iterators can be created for the same PreComputed instance by
    calling iter() multiple times. Each iterator will be independent (consuming data from one iterator will not
    remove it from the others) but they must be kept roughly in sync (new data will not be produced until there
    is space in all of the iterators to store it).

    There are two concrete implementations of this class:
        - PreComputedBG (pre-computation happens in a background process)
        - PreComputedFG (pre-computation happens in the foreground when new data is needed)
    """

    def __init__(self, compute_func: Callable[[A], R], buffer_size: int = 2, batch: bool = False):
        """Initializes a PreComputed instance.

        Args:
            compute_func:
                A callable that accepts an item from args as an input and returns some computed output data.

            buffer_size (optional):
                Maximum number of pre-computed data items to store. If batch is True, then this is the
                maximum number of batches of pre-computed data items to store. Defaults to 2.

            batch (optional):
                If True, the compute_func maps an iterable to an iterable (i.e. computes a batch of outputs
                for a batch of inputs in one call). Each item in args should be a batch of inputs.
                Defaults to False.
        """
        self._compute_func = compute_func
        self._buffer_size = buffer_size
        self._batch = batch

        self._args: Iterable[A] = None
        self._started = False

    @property
    def compute_func(self) -> Callable[[A], R]:
        return self._compute_func

    @property
    def args(self) -> Iterable[A]:
        return self._args

    @property
    def buffer_size(self) -> int:
        return self._buffer_size

    @property
    def batch(self) -> bool:
        return self._batch

    @property
    def started(self) -> bool:
        return self._started

    def start(self, args: Iterable[A]):
        """Prepare this PreCompute instance for iteration. Must be called before you start iterating
        and after creating all iterables via PreComputed.subscribe().
        
        The default implementation just stores args in an instance variable. Subclasses can override
        to provide custom functionality (e.g. starting a background process).

        Args:
            args:
                An iterable of the arguments to pass to self.compute_func to generate the data.
                This is typically a generator that produces arguments on-the-fly.
        """
        self._args = args
        self._started = True

    @abstractmethod
    def subscribe(self, un_batch: bool = True) -> Iterable[D]:
        """Create a new iterable over the pre-computed data produced by this class.

        All iterables must be created before calling PreComputed.start().

        Args:
            un_batch (optional):
                If True, "un-batch" the pre-computed data, yielding one item at a time
                instead of one batch at a time. Has no effect if this PreComputed
                instance was created with batch=False. Defaults to True.

        Raises:
            RuntimeError: If called after self.start()
        """
        raise NotImplementedError
