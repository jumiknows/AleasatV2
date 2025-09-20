"""Entry point for obc_grpc."""

import logging

logging.basicConfig(level=logging.NOTSET)
logging.root.setLevel(logging.NOTSET)

from utils.update import update_grpc_service

update_grpc_service()

import asyncio
from alea.obc_grpc.server import start_server
from alea.obc_grpc.repositories.obc import log, obc, health_check
from alea.obc_grpc.utils.update import update_grpc_service, check_fw_change


async def main():
    asyncio.create_task(log())
    # asyncio.create_task(health_check())
    # asyncio.create_task(check_fw_change())
    asyncio.create_task(start_server())

    while True:
        await asyncio.sleep(1)


if __name__ == "__main__":
    asyncio.run(main())
