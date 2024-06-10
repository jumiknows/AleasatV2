import sys
import pathlib

GRAPHICS_DIR = pathlib.Path(__file__).resolve().parent.parent / "graphics"

# Override Graphics DIR if we're an executable
if getattr(sys, 'frozen', False):
    # executed if run as an executable
    from pathlib import Path
    import os
    GRAPHICS_DIR = Path(getattr(sys, '_MEIPASS', os.path.abspath(os.path.dirname(__file__)))) / "graphics"
