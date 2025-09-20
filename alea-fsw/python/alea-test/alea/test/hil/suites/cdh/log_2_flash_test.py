import numpy as np

from alea.test import timeout
from alea.test.hil import hil_test

from alea.ttc.protocol.app import app_log
from alea.common import alea_time

DELIMITER = b'\xBE\xEE\xEE\xEF'
DELIMITER_SIZE = 4
HEADER_SIZE = 7
class Log2FlashTest(hil_test.HILTest):

    @timeout.timeout(50)
    def test_ping(self):
        resp = self.ttc.ping()
        self.assertTrue(resp.is_success)

    @timeout.timeout(600)
    def test_log_2_flash(self):
        num_logs = 30
        request_size = 1000

        resp = self.ttc.send_obc_cmd("TEST_LOG_2_FLASH", num_logs, timeout=600)
        self.assertTrue(resp.is_success)

        resp = self.ttc.send_obc_cmd("GET_LOGFILE_INFO", timeout=600)
        self.assertTrue(resp.is_success)
        
        print("Current Logfile:", resp.data["current_logfile"])
        print("Logfile Sizes:")
        for i, size in enumerate(resp.data["logfile_sizes"]):
            print(f"  Logfile {i}: {size} bytes")

        resp = self.ttc.send_obc_cmd("GET_LOGS", request_size, timeout=600)
        self.assertTrue(resp.is_success)
        
        log_specs = self.ttc._log_specs
        reordered_data = reorder_log_bytes_reversed_chunks(resp.data["data"])
        logs = decode_logs_with_magic(reordered_data, log_specs)

        print("*************************************************************************")

        print("BYTES RECEIVED: ", len(resp.data["data"]))
        for log in logs:
            print(log)

def reorder_log_bytes_reversed_chunks(data: bytes, chunk_size: int = 800) -> bytes:
    chunks = [data[i:i + chunk_size] for i in range(0, len(data), chunk_size)]
    chunks.reverse()
    return b''.join(chunks)    

def decode_logs_with_magic(data: bytes, specs) -> list:
    i = 0
    logs = []

    while i < len(data) - (DELIMITER_SIZE + HEADER_SIZE):
        if data[i:i+DELIMITER_SIZE] != DELIMITER:
            i += 1
            continue

        ts = int.from_bytes(data[i+DELIMITER_SIZE:i+DELIMITER_SIZE+4], "big")
        log_id = data[i+DELIMITER_SIZE+4]
        sig_id = data[i+DELIMITER_SIZE+5]
        payload_len = data[i+DELIMITER_SIZE+6]

        total_len = DELIMITER_SIZE + HEADER_SIZE + payload_len
        if i + total_len > len(data):
            break

        try:
            group_spec, signal_spec = specs.get(group_id=log_id, signal_id=sig_id)

            data_field = "None"
            if payload_len > 0:
                payload = data[i+DELIMITER_SIZE+7:i+DELIMITER_SIZE+7+payload_len]
                data_field = signal_spec.decode_data(payload)

            decoded = app_log.OBCLog(
                date_time=alea_time.ALEADateTime.from_timestamp(ts),
                log_id=log_id,
                signal_id=sig_id,
                payload_len=payload_len,
                group_name=group_spec.name,
                group_desc=group_spec.desc,
                signal_level=signal_spec.level,
                signal_name=signal_spec.name,
                signal_desc=signal_spec.desc,
                data=data_field
            )
            logs.append(decoded)
        except Exception as e:
            print(f"Bad log at offset {i}: {e}")

        i += total_len

    return logs

if __name__ == '__main__':
    hil_test.main(Log2FlashTest)
