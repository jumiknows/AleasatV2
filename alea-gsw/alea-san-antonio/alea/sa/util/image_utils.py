import datetime

class CapturedImage:
    def __init__(self, image_data: bytes, timestamp: datetime.datetime):
        self._image_data = image_data
        self._timestamp = timestamp

    @property
    def image_data(self) -> bytes:
        return self._image_data

    @property
    def timestamp(self) -> datetime.datetime:
        return self._timestamp
