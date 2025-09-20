import pathlib
import os
import logging
import aiohttp
import json
import subprocess
from alea.obc_grpc.repositories.obc import obc, reload_obc
from alea.obc_grpc.server import reload_server
from alea.obc_grpc.utils.generate_proto import generate_proto_file
from alea.obc_grpc.utils.generate_service import generate_service_file
import asyncio
import sys

from dotenv import load_dotenv

load_dotenv()

logger = logging.getLogger(__name__)

config_dir_path = pathlib.Path(__file__).resolve().parent.parent.parent / "../configs"
proto_dir_path = (
    pathlib.Path(__file__).resolve().parent.parent.parent.parent.parent.parent
    / "protos"
)
os.makedirs(proto_dir_path, exist_ok=True)
service_dir_path = (
    pathlib.Path(__file__).resolve().parent.parent / "services" / "generated"
)
pb2_dir_path = pathlib.Path(__file__).resolve().parent.parent / "generated"
os.makedirs(pb2_dir_path, exist_ok=True)

repoApiURL = os.environ.get("REPO_URL")


def update_grpc_service():
    # async def update_grpc_service(fwHashCommit: str = None):
    # if repoApiURL == None:
    #     logging.warning(f"Missing REPO URL falling back to default config file")
    #     return

    # if fwHashCommit == None:
    #     fwHash = obc.send_obc_cmd("FW_INFO").data["fw_hash"]
    #     fwHashCommit = hex(fwHash).split("x")[-1]

    # async with aiohttp.ClientSession() as session:
    #     # Get the cmd sys spec config
    #     url = f"{repoApiURL}/commits/{fwHashCommit}/obc-config"
    #     async with session.get(url) as resp:
    #         configs = await resp.json()
    #         for configName, configContent in configs.items():
    #             configPath = config_dir_path / f"{configName}.json"
    #             with open(configPath, "w", newline="\n") as config_file:
    #                 json.dump(configContent, config_file, indent=2)

    #     # Get the proto file
    #     url = f"{repoApiURL}/commits/{fwHashCommit}/obc-proto"
    #     async with session.get(url, params={"includeTestSpec": "true"}) as resp:
    #         newProtoFile = await resp.text()
    #         protoFilePath = proto_dir_path / "obc.proto"
    #         with open(protoFilePath, "w", newline="\n") as protoFile:
    #             protoFile.write(newProtoFile)

    #     # Get the obc service file
    #     url = f"{repoApiURL}/commits/{fwHashCommit}/obc-grpc-service"
    #     async with session.get(url, params={"includeTestSpec": "true"}) as resp:
    #         newObcServiceFile = await resp.text()
    #         obcServiceFilePath = service_dir_path / "obc_service.py"
    #         with open(obcServiceFilePath, "w", newline="\n") as obcServiceFile:
    #             obcServiceFile.write(newObcServiceFile)

    generate_service_file(
        "../../../alea-fsw/obc-fw/python/alea-obcfw/alea/obcfw/cmd_sys/data/cmd_sys.json",
        "../../../alea-fsw/obc-fw/python/alea-obcfw/alea/obcfw/cmd_sys/data/cmd_sys_test.json",
        "alea/obc_grpc/services/generated/obc_service.py",
    )

    generate_proto_file(
        "../../../alea-fsw/obc-fw/python/alea-obcfw/alea/obcfw/cmd_sys/data/cmd_sys.json",
        "../../../alea-fsw/obc-fw/python/alea-obcfw/alea/obcfw/cmd_sys/data/cmd_sys_test.json",
        "../../protos/obc.proto",
    )

    # Generate the new grpc service pb2_grpc files
    subprocess.call(
        f"python -m grpc_tools.protoc --proto_path=../../protos --python_out=./alea/obc_grpc/generated --grpc_python_out=./alea/obc_grpc/generated --pyi_out=./alea/obc_grpc/generated obc.proto",
        shell=True,
    )

    # fix the generated pb2_grpc file import
    with open(pb2_dir_path / "obc_pb2_grpc.py", "r+", newline="\n") as pb2_grpc_file:
        f = pb2_grpc_file.readlines()
        f[5] = "import alea.obc_grpc.generated.obc_pb2 as obc__pb2\n"
        pb2_grpc_file.seek(0)
        pb2_grpc_file.writelines(f)
        pb2_grpc_file.truncate()


async def check_fw_change():
    fwHash = None

    try:
        os.makedirs("cache", exist_ok=True)
        with open("cache/fw_hash", "r") as f:
            fwHash = f.read()
    except:
        pass

    while True:
        if repoApiURL == None:
            return

        try:
            currentFwHash = obc.send_obc_cmd("FW_INFO").data["fw_hash"]
            if fwHash == None or int(fwHash) != currentFwHash:
                await update_grpc_service()
                with open("cache/fw_hash", "w") as f:
                    f.write(str(currentFwHash))
                os._exit(0)
        except Exception as e:
            logger.exception(f"Error checking for FW change: {e}")

        await asyncio.sleep(60)
