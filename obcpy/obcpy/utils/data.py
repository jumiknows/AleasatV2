"""Utilities related to raw data manipulation.
"""

def bytes_to_hexstr(data: bytes, ascii_only: bool = False, try_decode: bool = True) -> str:
    """Convert a bytes object to a string

    If ascii_only is `True`, and the data can be decoded as ASCII, then the decoded ASCII string will be returned.

    If ascii_only is `False`, each byte will be converted to a pair of hex characters.
    If the string can be decoded as ASCII, the decoded string will be appended in brackets (unless try_decode is `False`).
    If try_decode is `False`, the ascii_only flag has no effect.

    Args:
        data: The bytes to convert.
        ascii_only: If True return only the decoded ASCII string if possible. Defaults to False.
        try_decode: If True, we will attempt to decode the bytes as ASCII. Defaults to True.

    Returns:
        A hex-string (and/or ASCII string) representation of the bytes
    """
    if len(data) == 0:
        return ""

    decoded = ""
    if try_decode:
        try:
            decoded = data.decode("ascii")
            # Remove NULL bytes - modest attempt at making the string printable
            decoded = decoded.replace("\x00", "")

            if ascii_only:
                return decoded
            else:
                if len(decoded) > 0:
                    decoded = f" ({decoded})"
        except UnicodeDecodeError:
            pass

    return f"{data.hex(' ').upper()}{decoded}"

def get_byte(value: int, idx: int) -> int:
    return ((value >> (idx * 8)) & 0xFF)

def bit(idx: int) -> int:
    return (1 << idx)
