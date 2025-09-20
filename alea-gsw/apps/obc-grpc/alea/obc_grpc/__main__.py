import subprocess
import sys
import os

while True:
    subprocess.run(
        [
            "opentelemetry-instrument",
            "python",
            "alea/obc_grpc/main.py",
        ],
        env=os.environ,
        # shell=True,
        check=True,
    )
