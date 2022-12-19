"""Utilities related to raw data manipulation.
"""

def bytes_to_hexstr(data: bytes, ascii_only: bool = False) -> str:
    """Convert a bytes object to a string

    If ascii_only is `True`, and the data can be decoded as ASCII, then the decoded ASCII string will be returned.

    If ascii_only is `False`, each byte will be converted to a pair of hex characters.
    If the string can be decoded as ASCII, the decoded string will be appended in brackets.

    Args:
        data: The bytes to convert.
        ascii_only: If True return only the decoded ASCII string if possible. Defaults to False.

    Returns:
        A hex-string (and/or ASCII string) representation of the bytes
    """
    try:
        decoded = data.decode('ascii')
        if ascii_only:
            return decoded
        else:
            decoded = f" ({decoded})"
    except UnicodeDecodeError:
        decoded = ""

    return f"{data.hex(' ').upper()}{decoded}"
