import threading

from obcpy.protocol import packet
from obcpy.protocol import layer_impl
from obcpy.protocol import routing
from obcpy.protocol import routing_impl

from obcpy.cmd_sys import spec
from obcpy.cmd_sys import resp

from obcpy.utils import obc_time
from obcpy.utils import exc

from .serial import phy
from .serial import datalink

from . import log
from . import cmd_sys

class OBCSerialProtocol:
    """Implementation of the OBC Serial Protocol stack.
    """

    def __init__(self):
        """Initializes the protocol stack.
        """
        self._cmd_seq_num = 0

        # ============================================================================================================================================
        # RX PROTOCOL STACK
        # ============================================================================================================================================

                                                                                #                                                                     
                                                                                #                          ========================================   
        self._rx_app_resp_bridge  = routing_impl.QueuePacketBridge[             #                          |          RX APP RESP BRIDGE          |   
                                        datalink.OBCSerialDatagram              #                          ========================================   
                                    ](maxsize=100)                              #                                  |                |                 
                                                                                #                                  |        OBCSerialDatagram         
        self._rx_app_resp_raw     = routing_impl.ProtocolPacketSource[          #                                  |                |                 
                                        packet.RawPacket,                       #                                  |      ----------------------      
                                    ](                                          #                                  |      | RXRawProtocolLayer |      
                                        layer_impl.RXRawProtocolLayer(),        #                                  |      ----------------------      
                                        self._rx_app_resp_bridge                #                                  |                |                 
                                    )                                           #                                  |            RawPacket             
                                                                                #    .      .      .               |                |                 
        self._rx_app_resp         = cmd_sys.OBCAppCmdSysSource(                 #     .     .     .                |      ----------------------      
                                        self._rx_app_resp_raw                   #      .    .    .                 |      | OBCAppCmdSysSource |      
                                    )                                           #       \   |   /                  |      ----------------------      
                                                                                #        \  |  /                   /                |                 
        self._rx_app_log_multi = routing_impl.MultiPacketDest[log.OBCLog]()     #         \ | /                   /         OBCCmdSysMsgHeader        
                                                                                #         OBCLog                 /                  or                
        self._rx_app_log          = routing_impl.ProtocolPacketDest[            #           |                   /                 bytes               
                                        datalink.OBCSerialDatagram,             #           |                  /                                      
                                        log.OBCLog                              #            \                /                                       
                                    ](                                          #       ---------------      /                                        
                                        log.OBCAppLogRX(),                      #       | OBCAppLogRX |     /                                         
                                        self._rx_app_log_multi                  #       ---------------    /                                          
                                    )                                           #               \         /                                           
        self._rx_datalink_filter  = datalink.OBCSerialDataLinkRXDest(           #                \       /                                            
                                        self._rx_app_log,                       #                 \     /                                             
                                        self._rx_app_resp_bridge                #                  \   /                                              
                                    )                                           #                    |                                                
                                                                                #            OBCSerialDatagram                                        
        self._rx_datalink         = routing_impl.ProtocolPacketDest[            #                    |                                                
                                        phy.OBCSerialFrame,                     #                    |                                                
                                        datalink.OBCSerialDatagram              #         ------------------------                                    
                                    ](                                          #         | OBCSerialDataLinkRX) |                                    
                                        datalink.OBCSerialDataLinkRX(),         #         ------------------------                                    
                                        self._rx_datalink_filter                #                    |                                                
                                    )                                           #                    |                                                
                                                                                #              OBCSerialFrame                                         
        self._rx_phy              = routing_impl.ProtocolPacketDest[            #                    |                                                
                                        packet.RawPacket,                       #                    |                                                
                                        phy.OBCSerialFrame                      #           -------------------                                       
                                    ](                                          #           | (OBCSerialPhyRX) |                                      
                                        phy.OBCSerialPhyRX(),                   #           --------------------                                      
                                        self._rx_datalink                       #                    |                                                
                                    )                                           #                    |                                                
                                                                                #                RawPacket                                            
                                                                                #                                                                     

        # ============================================================================================================================================
        # TX PROTOCOL STACK
        # ============================================================================================================================================

                                                                                #                                                                     
        self._tx_bridge =   routing_impl.QueuePacketBridge[                     #   ==========================================================        
                                datalink.OBCSerialDatagram                      #   |                       TX BRIDGE                        |        
                            ]()                                                 #   ==========================================================        
                                                                                #                |                            |                       
                                                                                #        OBCSerialDatagram            OBCSerialDatagram               
        self._tx_phy  = routing_impl.ProtocolPacketSource[                      #                |                            |                       
                            phy.OBCSerialFrame                                  #                |                    ------------------              
                        ](                                                      #                |                    | OBCSerialPhyTX |              
                            phy.OBCSerialPhyTX(),                               #                |                    ------------------              
                            self._tx_bridge                                     #                |                            |                       
                        )                                                       #                |                            |                       
                                                                                #                |                      OBCSerialFrame                
        self._tx_datalink = routing_impl.ProtocolPacketDest[                    #                |                            |                       
                                packet.RawPacket,                               #                |                            .                       
                                datalink.OBCSerialDatagram                      #                |                            .                       
                            ](                                                  #      -----------------------                .                       
                                datalink.OBCSerialDataLinkTX(                   #      | OBCSerialDataLinkTX |                                        
                                    datalink.OBCSerialDatagram.MsgType.COMMS    #      -----------------------                                        
                                ),                                              #                |                                                    
                                self._tx_bridge                                 #                |                                                    
                            )                                                   #                |                                                    
                                                                                #            RawPacket                                                
        self._tx_app_cmd  = cmd_sys.OBCAppCmdSysDest(                           #                |                                                    
                                layer_impl.TXBufferedProtocolLayer(             #    ---------------------------                                      
                                    datalink.OBCSerialDatagram.MAX_DATA_SIZE    #    | TXBufferedProtocolLayer |                                      
                                ),                                              #    ---------------------------                                      
                                self._tx_datalink                               #                |                                                    
                            )                                                   #                |                                                    
                                                                                #        OBCCmdSysMsgHeader                                           
                                                                                #                or                                                   
                                                                                #            RawPacket                                                
                                                                                #                                                                     

    @property
    def tx_phy(self) -> routing.PacketSource[phy.OBCSerialFrame]:
        """Gets a reference to the PHY layer of the protocol stack in the TX direction.

        A PHY layer device can read packets from this PacketSource to retrieve the data
        to transmit.
        """
        return self._tx_phy

    @property
    def rx_phy(self) -> routing.PacketDest[packet.RawPacket]:
        """Gets a reference to the PHY layer of the protocol stack in the RX direction.

        A PHY layer device can write received packets to this PacketDest for the higher
        layers of the protocol to handle.
        """
        return self._rx_phy

    def add_log_listener(self, queue_size: int = 0) -> routing_impl.QueuePacketBridge[log.OBCLog]:
        """Adds a bridge to the protocol stack to receive OBC logs.

        Any other PacketSource's created by calling this method will still receive all of the logs.

        Args:
            queue_size: Maximum size of the queue (number of logs)

        Returns:
            A PacketSource for OBC logs.
        """

        bridge = routing_impl.QueuePacketBridge[log.OBCLog](maxsize=queue_size)
        self._rx_app_log_multi.add_dest(bridge)
        return bridge

    def remove_log_listener(self, listener: routing_impl.QueuePacketBridge[log.OBCLog]):
        """Removes a log listener previously added by `OBCSerialProtocol.add_log_listener()`

        Args:
            listener: Listener returned from `OBCSerialProtocol.add_log_listener()`
        """
        self._rx_app_log_multi.remove_dest(listener)

    # Send Command + Receive Response ------------------------------------------------------------------

    def send_cmd_recv_resp(self, cmd_sys_spec: spec.OBCCmdSysSpec, *args, date_time: obc_time.OBCDateTime = obc_time.IMMEDIATE, timeout: float = None) -> resp.OBCResponse:
        """Sends a command (with automatic argument encoding) and waits for the response (with automatic data decoding).

        Args:
            cmd_sys_spec: Specification for the command ID and automatic argument/response encoding/decoding.
            date_time: When the command should be executed. Defaults to IMMEDIATE.
            timeout: Timeout to send the command and receive the response (None waits indefinitely if necessary). Defaults to None.

        Raises:
            exc.OBCEncodeError: If the incorrect number of arguments is provided or one of the arguments has an invalid type or value.
            exc.OBCCmdSysResponseError: If a response is expected and the response data does not contain at least one byte for the response code.
                                        If a response is expected and the response code contained in the response data is invalid.

        Returns:
            A `resp.OBCResponse` object if the command has a response (raw or fields), otherwise None.
        """
        # Send Command
        self.send_cmd(cmd_sys_spec, *args, date_time=date_time, timeout=timeout)

        if cmd_sys_spec.resp or (not date_time.is_immediate):
            # Command that are specified to have a response or any command that is scheduled will have a response.

            # Read Response Header
            resp_header = self.recv_resp_header(timeout=timeout)

            if resp_header is None:
                raise exc.OBCCmdSysResponseError(f"No response header received for command: {cmd_sys_spec.name}")

            # Read Response Data
            resp_data = self.recv_resp_data(resp_header.data_len, timeout=timeout)

            if len(resp_data) != resp_header.data_len:
                raise exc.OBCCmdSysResponseError(f"Insufficient response data received (received {len(resp_data)} bytes, expected {resp_header.data_len} bytes) for command: {cmd_sys_spec.name}")

            return resp.OBCResponse.create(cmd_sys_spec, resp_header, resp_data)

        return None

    # Send Command -------------------------------------------------------------------------------------

    def send_cmd(self, cmd_sys_spec: spec.OBCCmdSysSpec, *args, date_time: obc_time.OBCDateTime = obc_time.IMMEDIATE, timeout: float = None) -> cmd_sys.OBCCmdSysMsgHeader:
        """Sends a command with automatic encoding of the command arguments.

        Args:
            cmd_sys_spec: Specification for the command ID and automatic argument encoding.
            date_time: When the command should be executed. Defaults to IMMEDIATE.
            timeout: Timeout to send the command (None waits indefinitely if necessary). Defaults to None.

        Raises:
            exc.OBCEncodeError: If the incorrect number of arguments is provided or one of the arguments
                                      has an invalid type or value.

        Returns:
            The `cmd_sys.OBCCmdSysMsgHeader` instance that was sent at the start of the command.
        """
        data = cmd_sys_spec.encode_args(*args)
        header = cmd_sys.OBCCmdSysMsgHeader(self._cmd_seq_num, cmd_sys_spec.id, date_time, 0, False, len(data))
        self.send_cmd_raw(header, data, timeout=timeout)
        self._tx_app_cmd.flush(timeout=timeout)
        return header

    def send_cmd_raw(self, header: cmd_sys.OBCCmdSysMsgHeader, data: bytes = None, timeout: float = None):
        """Sends a raw command (no automatic encoding of arguments).

        The call to this method may optionally include some or all of the command data.
        If not all of the data is sent with this call, the remaining data can be sent
        through subsequent calls to `OBCSerialProtocol.send_cmd_data()`.

        Args:
            header: A populated command header.
            data: Optional data to send immediately after the header. Defaults to None.
            timeout: Timeout to send the command (None waits indefinitely if necessary). Defaults to None.
        """
        self._tx_app_cmd.write_header(header, timeout=timeout)
        if data is not None:
            self.send_cmd_data(data, timeout=timeout)
        self._cmd_seq_num  = (self._cmd_seq_num + 1) & 0xFF

    def send_cmd_data(self, data: bytes, timeout: float = None):
        """Sends data for a command.

        This can be called after `OBCSerialProtocol.send_cmd_raw()` to finish sending command data.

        Args:
            data: Raw data to send.
            timeout: Timeout to send the command (None waits indefinitely if necessary). Defaults to None.
        """
        self._tx_app_cmd.write_data(data, timeout=timeout)

    # Receive Response ---------------------------------------------------------------------------------

    def recv_resp_header(self, timeout: float = None) -> cmd_sys.OBCCmdSysMsgHeader:
        """Reads enough bytes from the RX stack to get a command system message header for a response
        and parse it.

        Args:
            timeout: Timeout to read the response header (None waits indefinitely if necessary). Defaults to None.

        Returns:
            A parsed `cmd_sys.OBCCmdSysMsgHeader` or None if a timeout occurred
        """
        return self._rx_app_resp.read_header(timeout=timeout)

    def recv_resp_data(self, num_bytes: int, timeout: float = None) -> bytes:
        """Reads num_bytes of response data from the RX stack.

        Args:
            num_bytes: The number of bytes to read.
            timeout: Timeout to read num_bytes (None waits indefinitely if necessary). Defaults to None.

        Returns:
            The bytes read. If a timeout occurred, fewer than num_bytes may be returned.
        """
        return self._rx_app_resp.read(num_bytes, timeout=timeout)
