from typing import Union, List, Iterator
from enum import Enum

from obcpy.data_fmt import data_field_impl
from obcpy.utils import exc

class OBCRTOSTraceEvent(Enum):
    # Must match values used in traceTASK_SWITCHED_IN and traceTASK_SWITCHED_OUT macros in FreeRTOSConfig.h
    TASK_SWITCHED_IN  = 0
    TASK_SWITCHED_OUT = 1

class OBCRTOSTraceEntry:
    def __init__(self, time_us: int, event: OBCRTOSTraceEvent, task_id: int):
        self._time_us = time_us
        self._event   = event
        self._task_id = task_id

    @property
    def time_us(self) -> int:
        return self._time_us

    @property
    def event(self) -> OBCRTOSTraceEvent:
        return self._event

    @property
    def task_id(self) -> int:
        return self._task_id

    def __str__(self) -> str:
        return f"{self.time_us}: Task {self.task_id} {self.event.name}"

    @classmethod
    def from_u32(cls, data: int, prev_time_us: int) -> "OBCRTOSTraceEntry":
        delta_t = (data >> 6) & ((1 << 26) - 1)
        time_us = (prev_time_us + delta_t) if prev_time_us is not None else 0
        event   = OBCRTOSTraceEvent((data >> 5) & 1)
        task_id = data & ((1 << 5) - 1)

        return OBCRTOSTraceEntry(time_us, event, task_id)

class OBCTaskRun:
    def __init__(self, start_time_us: int, duration_us: int, task_id: int):
        self._start_time_us = start_time_us
        self._duration_us   = duration_us
        self._task_id       = task_id

    @property
    def start_time_us(self) -> int:
        return self._start_time_us

    @property
    def duration_us(self) -> int:
        return self._duration_us

    @property
    def task_id(self) -> int:
        return self._task_id

    def __str__(self) -> str:
        return f"{self.start_time_us}: Task {self.task_id} for {self.duration_us} us"

class OBCRTOSTrace:
    def __init__(self, entries: List[OBCRTOSTraceEntry]):
        self._entries = entries

    def __str__(self) -> str:
        return "\n".join(map(lambda entry : str(entry), self._entries))

    def __iter__(self) -> Iterator[OBCRTOSTraceEntry]:
        return self._entries.__iter__()

    def __getitem__(self, val: Union[int, slice]) -> Union[OBCRTOSTraceEntry, List[OBCRTOSTraceEntry]]:
        return self._entries[val]

    def to_task_runs(self) -> List[OBCTaskRun]:
        task_runs: List[OBCTaskRun] = []

        last_event: OBCRTOSTraceEvent = None
        switch_in_time_us: int = None
        switch_in_task: int = None
        time_offset_us = None

        for entry in self._entries:
            if entry.event == OBCRTOSTraceEvent.TASK_SWITCHED_IN:
                if last_event == OBCRTOSTraceEvent.TASK_SWITCHED_IN:
                    raise exc.OBCTaskTraceError("Consecutive TASK_SWITCHED_IN events")

                if time_offset_us is None:
                    time_offset_us = entry.time_us

                switch_in_time_us = entry.time_us
                switch_in_task = entry.task_id
            elif entry.event == OBCRTOSTraceEvent.TASK_SWITCHED_OUT:
                if last_event == OBCRTOSTraceEvent.TASK_SWITCHED_OUT:
                    raise exc.OBCTaskTraceError("Consecutive TASK_SWITCHED_OUT events")
                elif last_event == None:
                    continue

                if entry.task_id != switch_in_task:
                    raise exc.OBCTaskTraceError("Task changed without event")
                    break

                start_time_us = (switch_in_time_us - time_offset_us)
                duration_us   = (entry.time_us - switch_in_time_us)

                task_runs.append(OBCTaskRun(start_time_us, duration_us, entry.task_id))
            else:
                # Ignore other events
                continue

            last_event = entry.event

        return task_runs

    @classmethod
    def from_bytes(cls, data: bytes) -> "OBCRTOSTrace":
        u32_arr_field = data_field_impl.U32("", int(len(data) / 4))
        data_u32s = u32_arr_field.decode(data)

        prev_time_us = None
        entries: List[OBCRTOSTraceEntry] = []
        for entry_u32 in data_u32s:
            entry = OBCRTOSTraceEntry.from_u32(entry_u32, prev_time_us)
            prev_time_us = entry.time_us
            entries.append(entry)

        return OBCRTOSTrace(entries)
