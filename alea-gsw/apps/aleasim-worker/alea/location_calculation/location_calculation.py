from alea.sim.kernel.kernel import AleasimKernel
from alea.sim.epa.orbit_dynamics import OrbitDynamicsModel, OrbitDynamicsConfig

import asyncio
from dotenv import load_dotenv, find_dotenv
import logging
import os

from bullmq import Queue, Worker
from redis import Sentinel
import signal
import traceback

# Load environment variables
NODE_ENV = ["production", "development", "test"]
for env in NODE_ENV:
    load_dotenv(dotenv_path=find_dotenv(f".env.{env}.local"))
load_dotenv(dotenv_path=find_dotenv(".env.local"))
for env in NODE_ENV:
    load_dotenv(dotenv_path=find_dotenv(f".env.{env}"))
load_dotenv(dotenv_path=find_dotenv(".env"))

redis_url = f"redis://{os.getenv('REDIS_HOST')}:{os.getenv('REDIS_PORT')}"
connection = {"connection": redis_url}

queue = Queue("ORBIT_PROPAGATION", connection)
results = Queue("results", connection)


def init_orbit_dynamics_TLE(kernel, tleLine1, tleLine2) -> OrbitDynamicsModel:
    """
    Initializes Orbit Dynamics Model with TLE for Kernel
    """
    config = OrbitDynamicsConfig(
        propagator=OrbitDynamicsConfig.Propagator.SGP4,
        parameter_type=OrbitDynamicsConfig.ParameterType.TLE,
        tle=[tleLine1, tleLine2],
    )

    orbitModel = OrbitDynamicsModel(kernel, cfg=config)
    kernel.add_model(orbitModel, create_shared_mem=True)

    return orbitModel


def get_new_position(tleLine1, tleLine2, time_seconds) -> list:

    kernel = AleasimKernel(dt=1)
    orbit_model = init_orbit_dynamics_TLE(kernel, tleLine1, tleLine2)
    kernel.advance(time_seconds)

    position = orbit_model.position_eci
    return position


async def position_process(job, token=None) -> list:
    try:
        tleLine1 = job.data["tleLine1"]
        tleLine2 = job.data["tleLine2"]
        sim_time = job.data["simulationTime"]

        new_position = get_new_position(
            tleLine1=tleLine1, tleLine2=tleLine2, time_seconds=sim_time
        ).tolist()
        result = [float(x) for x in new_position]

        await results.add("position_result", {"result": result})

        return result

    except Exception as e:
        traceback.print_exception(type(e), e, e.__traceback__)
        logging.error(f"Error in position calculation: {e}")
        raise


async def run_worker():
    # Event used to trigger shutdown
    shutdown_event = asyncio.Event()

    def signal_handler(sig, frame):
        logging.info(f"Signal {sig} received. Shutting down worker...")
        shutdown_event.set()

    # Register signal handlers
    signal.signal(signal.SIGINT, signal_handler)
    signal.signal(signal.SIGTERM, signal_handler)

    # Worker setup
    worker = Worker("ORBIT_PROPAGATION", position_process, connection)

    def on_completed(job, result, *_):
        logging.info(f"Job {job.id} completed with result: {result}")

    def on_failed(job, err, *_):
        logging.error(f"Job {job.id} failed: {err}")
        traceback.print_exception(type(err), err, err.__traceback__)

    worker.on("completed", on_completed)
    worker.on("failed", on_failed)

    try:
        logging.info("Worker started. Waiting for jobs...")

        await shutdown_event.wait()

    finally:
        logging.debug("Cleaning up worker...")
        await worker.close()
        await queue.close()
        await results.close()
        logging.info("Worker shut down successfully.")


async def test_code():
    shutdown_event = asyncio.Event()

    def signal_handler(sig, frame):
        shutdown_event.set()

    # Register signal handlers for graceful shutdown
    signal.signal(signal.SIGINT, signal_handler)
    signal.signal(signal.SIGTERM, signal_handler)

    tleLine1 = "1 63630U 25081A   25142.57315618  .00003258  00000+0  28905-3 0  9998"
    tleLine2 = "2 63630  45.3984 201.2708 0009520 297.7299  62.2612 14.98214501  4574"
    testData = {"tleLine1": tleLine1, "tleLine2": tleLine2, "simulationTime": 20}

    worker = Worker("ORBIT_PROPAGATION", position_process, connection)

    # Event hooks
    worker.on(
        "completed",
        lambda job, result, *_: logging.info(
            f"Job {job.id} completed with result: {result}"
        ),
    )
    worker.on(
        "failed",
        lambda job, err, *_: logging.error(f"Job {job.id} failed: {err}"),
    )

    try:
        # Add job to queue
        job = await queue.add("position", testData)
        logging.info(f"Added job {job.id}")

        timeout = 30
        start_time = asyncio.get_event_loop().time()

        while asyncio.get_event_loop().time() - start_time <= timeout:
            if shutdown_event.is_set():
                logging.warning("Shutdown triggered before job finished.")
                break

            state = await job.getState()
            if state in {"completed", "failed"}:
                logging.info(f"Job finished with state: {state}")
                if state == "failed":
                    try:
                        reason = await job.failedReason()
                        logging.error(f"Failure reason: {reason}")
                    except Exception as err:
                        logging.error(f"Failed to retrieve job failure reason: {err}")
                break

            await asyncio.sleep(0.5)
        else:
            logging.warning(f"Job {job.id} timed out.")

    except Exception as e:
        logging.error(f"Error in test_code: {e}")
        raise

    finally:
        shutdown_event.set()  # In case the loop ends without signal
        logging.debug("Cleaning up worker...")
        await worker.close()
        await queue.close()
        await results.close()
        logging.debug("Worker shut down successfully.")
