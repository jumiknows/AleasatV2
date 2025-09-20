import asyncio
import time
import pathlib
from alea.ttc import ttc_base
from alea.ttc.ttc import TTC
from alea.ttc.device.serial_device import get_serial_ports

from opentelemetry._logs import get_logger, get_logger_provider, std_to_otel
from opentelemetry.sdk._logs import LogRecord
from time import time_ns
from opentelemetry.sdk.resources import Resource
import os
from dotenv import load_dotenv, find_dotenv
from opentelemetry._logs import set_logger_provider
from opentelemetry.exporter.otlp.proto.http._log_exporter import OTLPLogExporter
from opentelemetry.sdk._logs import LoggerProvider, LoggingHandler
from opentelemetry.sdk._logs.export import BatchLogRecordProcessor
from opentelemetry.sdk.resources import Resource
import logging

# MOps is using the next js environment variable convention
# https://nextjs.org/docs/pages/building-your-application/configuring/environment-variables#environment-variable-load-order
# For Node.js, dotenvx is used
# https://dotenvx.com/docs/advanced/run-convention

NODE_ENV = ["production", "development", "test"]
for env in NODE_ENV:
    load_dotenv(dotenv_path=find_dotenv(f".env.{env}.local"))
load_dotenv(dotenv_path=find_dotenv(".env.local"))
for env in NODE_ENV:
    load_dotenv(dotenv_path=find_dotenv(f".env.{env}"))
load_dotenv(dotenv_path=find_dotenv(".env"))


logger = logging.getLogger(__name__)
obcLogger = logging.getLogger("OBC")

obc = TTC(
    ttc_base.TTCBase.InterfaceType.OBC_SERIAL,
)
logs = obc.add_log_listener(queue_size=100)
log_recievers: list[asyncio.Queue] = []

port = os.getenv("PORT")

if port is not None:
    obc.start(port)
else:
    ports = get_serial_ports()

    while len(ports) == 0:
        logging.error("No serial ports found. Retrying...")
        ports = get_serial_ports()
        time.sleep(5)

    logger.info(f"Starting OBC")
    logger.debug(f"All available ports: {ports}")

    port_num = 0
    while port_num < len(ports):
        try:
            obc.start(ports[port_num])

            for i in range(5):
                try:
                    obc.ping()
                    break
                except Exception as e:
                    pass

            obc.reset()
            time.sleep(1)

            for i in range(4):
                try:
                    obc.ping()
                    break
                except Exception as e:
                    pass

            obc.ping()
            break

        except Exception as e:
            obc.stop()
            port_num += 1
            if port_num >= len(ports):
                logging.exception(
                    f"Error finding OBC port: {type(e).__name__}, Retrying..."
                )
                port_num = 0
            time.sleep(1)


def reload_obc():
    obc.load_cmd_sys_specs(
        [config_dir_path / "cmd_sys.json", config_dir_path / "cmd_sys_test.json"]
    )
    obc.load_log_specs(config_dir_path / "log_specs.json")
    obc.load_task_specs(
        [config_dir_path / "obc_tasks.json"],
    )


async def log():
    while True:
        try:
            recvd_logs = logs.read(timeout=0.0001)
            if recvd_logs:
                for recvd_log in recvd_logs:
                    body = f"[Log ID: {recvd_log.log_id:03d}, Name: {recvd_log.group_name}] "
                    body += f"[Sig ID: {recvd_log.signal_id:03d}, Name: {recvd_log.signal_name}] "
                    body += f"[Signal Description: {recvd_log.signal_desc}] "
                    body += f"{recvd_log.data}"

                    obcLogger.log(recvd_log.signal_level.value * 10 + 10, body)

                    for reciever in log_recievers:
                        try:
                            reciever.put_nowait(recvd_log)
                        except asyncio.QueueFull:
                            pass
        except Exception as e:
            logging.exception(f"[Logging] {type(e).__name__}")
        await asyncio.sleep(0)


async def health_check():
    while True:
        for i in range(5):
            try:
                obc.ping()
                break
            except Exception as e:
                logging.warning(
                    f"Health check failed: {type(e).__name__} - Attempt {i + 1}/5",
                    exc_info=True,
                )

                # reset OBC if health check fails 5 times
                if i == 4:
                    logging.error("Health check failed 5 times. Resetting OBC.")

                    obc.reset()
                else:
                    # wait for 1 second before trying again
                    await asyncio.sleep(1)

        await asyncio.sleep(5)
