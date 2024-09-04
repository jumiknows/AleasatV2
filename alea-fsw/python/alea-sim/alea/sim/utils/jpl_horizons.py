from typing import TypeVar, Generic, Callable, Iterator, Generator
from enum import Enum
from io import StringIO
from enum import Enum

import requests

import numpy as np

T = TypeVar("T")

class JPLHorizons:
    URL = "https://ssd.jpl.nasa.gov/api/horizons_file.api"

    BATCH_FILE_TEMPLATE = """
!$$SOF
MAKE_EPHEM=YES
COMMAND={command}
TLE='{tle}'
EPHEM_TYPE=VECTORS
CENTER='500@{center}'
TLIST_TYPE=JD
TIME_TYPE=TT
TLIST={tlist}
VEC_TABLE='{table_type}'
REF_SYSTEM='ICRF'
REF_PLANE='FRAME'
VEC_CORR='{vec_corr}'
CAL_TYPE='M'
OUT_UNITS='{units}'
VEC_LABELS='YES'
VEC_DELTA_T='NO'
CSV_FORMAT='YES'
OBJ_DATA='NO'
"""

    class Center(Enum):
        EARTH = "399"
        TLE   = "TLE"

    class Command(Enum):
        SUN = "10"
        TLE = "TLE"

    class VectorCorrection(Enum):
        GEOMETRIC   = "NONE"
        ASTROMETRIC = "LT"   # astrometric light-time corrected states
        APPARENT    = "LT+S" # astrometric states corrected for stellar aberration

    class Units(Enum):
        KM_S = "KM-S" # kilometres and seconds
        AU_D = "AU-D" # astronomical units and days

    class TableType(Enum):
        POSITION  = 1
        STATE_VEC = 2

    class ResponseParser(Generic[T]):
        class State(Enum):
            BEFORE_SOE = 0
            DATA       = 1

        SOE_MARKER = "$$SOE"
        EOE_MARKER = "$$EOE"

        def __init__(self, handler: Callable[[str, Iterator[str]], T]):
            self._handler = handler
            self._state = self.State.BEFORE_SOE

        def parse(self, resp_text: str):
            resp_iter = iter(resp_text.splitlines())

            data: list[T] = []

            while True:
                try:
                    line = next(resp_iter)
                except StopIteration:
                    if self._state == self.State.BEFORE_SOE:
                        raise ValueError(f"No data present in response:\n{resp_text}")
                    break

                if self._state == self.State.BEFORE_SOE:
                    if line == self.SOE_MARKER:
                        self._state = self.State.DATA
                elif self._state == self.State.DATA:
                    if line == self.EOE_MARKER:
                        break
                    else:
                        entry = self._handler(line, resp_iter)
                        data.append(entry)
                else:
                    raise ValueError(f"Invalid parser state: {self._state}")

            return data

    def get_vector_table(self, tle: list[str], tlist: list[float], command: Command, center: Center,
                         vec_corr: VectorCorrection = VectorCorrection.GEOMETRIC, units: Units = Units.KM_S,
                         table_type: TableType = TableType.POSITION) -> list[tuple[float, np.ndarray]]:
        tle = "\n".join(tle)
        tlist = "\n".join([f"'{t}'" for t in tlist])

        input_str = self.BATCH_FILE_TEMPLATE.format(tle=tle, tlist=tlist, command=command.value, center=center.value,
                                                    vec_corr=vec_corr.value, units=units.value, table_type=table_type.value)
        input_file = StringIO(input_str)

        response = requests.post(self.URL, data={"format": "text"}, files={'input': input_file})
        resp_text = response.text

        def handler(first_line: str, next_lines: Iterator[str]) -> tuple[float, np.ndarray]:
            fields = first_line.split(",")
            t = float(fields[0].strip())
            data = [float(field.strip()) for field in fields[2:-1]] # last field is an empty string that we can ignore (CSV output has trailing commas)
            return (t, np.array(data))

        parser = self.ResponseParser(handler)
        results = parser.parse(resp_text)
        return results
