from obcpy.protocol import packet
from obcpy.protocol import layer_impl
from obcpy.protocol import routing
from obcpy.protocol import routing_impl

from .serial import phy
from .serial import datalink

from . import log

class OBCSerialProtocol:
    """Implementation of the OBC Serial Protocol stack.
    """

    def __init__(self):
        """Initializes the protocol stack.
        """

        # ======================================================================================================================
        # RX PROTOCOL STACK
        # ======================================================================================================================

                                                                             #                              OBCCmdResp       (TODO)   
        self._rx_app_cmdresp = None                                          #                                  |                     
                                                                             #    .      .      .               |                     
                                                                             #     .     .     .        -------------------           
                                                                             #      .    .    .         | OBCAppCmdRespRX |  (TODO)   
                                                                             #       \   |   /          -------------------           
                                                                             #        \  |  /                   /                     
        self._rx_app_log_multi = routing_impl.MultiPacketDest[log.OBCLog]()  #         \ | /                   /                      
                                                                             #         OBCLog                 /                       
        self._rx_app_log          = routing_impl.ProtocolPacketDest[         #           |                   /                        
                                        datalink.OBCSerialDatagram,          #           |                  /                         
                                        log.OBCLog                           #            \                /                          
                                    ](                                       #       ---------------      /                           
                                        log.OBCAppLogRX(),                   #       | OBCAppLogRX |     /                            
                                        self._rx_app_log_multi               #       ---------------    /                             
                                    )                                        #               \         /                              
        self._rx_datalink_filter  = datalink.OBCSerialDataLinkRXDest(        #                \       /                               
                                        self._rx_app_log,                    #                 \     /                                
                                        self._rx_app_cmdresp                 #                  \   /                                 
                                    )                                        #                    |                                   
                                                                             #             OBCSerialDatagram                          
        self._rx_datalink         = routing_impl.ProtocolPacketDest[         #                    |                                   
                                        phy.OBCSerialFrame,                  #                    |                                   
                                        datalink.OBCSerialDatagram           #          ------------------------                      
                                    ](                                       #          | OBCSerialDataLinkRX) |                      
                                        datalink.OBCSerialDataLinkRX(),      #          ------------------------                      
                                        self._rx_datalink_filter             #                    |                                   
                                    )                                        #                    |                                   
                                                                             #              OBCSerialFrame                            
        self._rx_phy              = routing_impl.ProtocolPacketDest[         #                    |                                   
                                        packet.RawPacket,                    #                    |                                   
                                        phy.OBCSerialFrame                   #           -------------------                          
                                    ](                                       #           | (OBCSerialPhyRX) |                         
                                        phy.OBCSerialPhyRX(),                #           --------------------                         
                                        self._rx_datalink                    #                    |                                   
                                    )                                        #                    |                                   
                                                                             #                RawPacket                               

        # ======================================================================================================================
        # TX PROTOCOL STACK
        # ======================================================================================================================

                                                                                  #                                                                
        self._tx_bridge =   routing_impl.QueuePacketBridge[                       #   ==========================================================   
                                datalink.OBCSerialDatagram                        #   |                       TX BRIDGE                        |   
                            ]()                                                   #   ==========================================================   
                                                                                  #                |                            |                  
                                                                                  #        OBCSerialDatagram            OBCSerialDatagram          
        self._tx_phy  = routing_impl.ProtocolPacketSource[                        #                |                            |                  
                            phy.OBCSerialFrame                                    #                |                    ------------------         
                        ](                                                        #                |                    | OBCSerialPhyTX |         
                            phy.OBCSerialPhyTX(),                                 #                |                    ------------------         
                            self._tx_bridge                                       #                |                            |                  
                        )                                                         #                |                            |                  
                                                                                  #                |                      OBCSerialFrame           
        self._tx_datalink = routing_impl.ProtocolPacketDest[                      #                |                            |                  
                                packet.RawPacket,                                 #                |                            .                  
                                datalink.OBCSerialDatagram                        #                |                            .                  
                            ](                                                    #      -----------------------                .                  
                                datalink.OBCSerialDataLinkTX(                     #      | OBCSerialDataLinkTX |                                   
                                    datalink.OBCSerialDatagram.MsgType.COMMS      #      -----------------------                                   
                                ),                                                #                |                                               
                                self._tx_bridge                                   #                |                                               
                            )                                                     #                |                                               
                                                                                  #            RawPacket                                           
        self._tx_app_cmd  = routing_impl.ProtocolPacketDest[                      #                |                                               
                                packet.RawPacket,                                 #                |                                               
                                packet.RawPacket                                  #                |                                               
                            ](                                                    #    ---------------------------                                 
                                layer_impl.TXBufferedProtocolLayer(               #    | TXBufferedProtocolLayer |                                 
                                    datalink.OBCSerialDatagram.MAX_DATA_SIZE      #    ---------------------------                                 
                                ),                                                #                |                                               
                                self._tx_datalink                                 #                |                                               
                            )                                                     #                |                                               
                                                                                  #            RawPacket                                           

    @property
    def tx_phy(self) -> routing.PacketSource[phy.OBCSerialFrame]:
        return self._tx_phy

    @property
    def rx_phy(self) -> routing.PacketDest[packet.RawPacket]:
        return self._rx_phy

    def add_log_dest(self) -> routing.PacketSource[log.OBCLog]:
        """Adds a bridge to the protocol stack to receive OBC logs.

        Any other PacketSource's created by calling this method will still receive all of the logs.

        Returns:
            A PacketSource for OBC logs.
        """

        bridge = routing_impl.QueuePacketBridge[log.OBCLog]()
        self._rx_app_log_multi.add_dest(bridge)
        return bridge

    def send_cmd(self, cmd: str, timeout: float = None):
        """Send a command through the protocol stack.

        The command string is encoded as ASCII and sent as raw bytes to the top of the TX protocol stack.

        Args:
            cmd: A command string to send
            timeout: Timeout to send the command (None waits indefinitely if necessary). Defaults to None.
        """

        self._tx_app_cmd.write(packet.RawPacket(cmd.encode("ascii")), timeout=timeout)
        self._tx_app_cmd.flush(timeout=timeout)
