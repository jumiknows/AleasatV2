import grpc
import alea.obc_grpc.generated.obc_pb2_grpc as obc_pb2_grpc
from alea.obc_grpc.services.generated import obc_service
from importlib import reload
import asyncio
import logging

logger = logging.getLogger(__name__)
server: grpc.aio.Server | None = None
port = "50051"


async def start_server():
    global server

    if server is not None:
        await server.stop(None)
        server = None

    server = grpc.aio.server()

    obc_pb2_grpc.add_obcServicer_to_server(obc_service.Obc(), server)

    server.add_insecure_port(f"[::]:{port}")

    await server.start()

    logger.debug("Server started, listening on " + port)

    await server.wait_for_termination()


def reload_server():
    reload(obc_service)

    if server is None:
        return

    asyncio.create_task(start_server())
