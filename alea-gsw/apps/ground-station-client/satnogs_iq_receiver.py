#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# SPDX-License-Identifier: GPL-3.0
#
# GNU Radio Python Flow Graph
# Title: satnogs_iq_receiver
# Author: Manolis Surligas (surligas@gmail.com)
# Description: Generic IQ receiver with arbitrary output sampling rate
# GNU Radio version: 3.10.1.1

from gnuradio import blocks
from gnuradio import gr
from gnuradio.filter import firdes
from gnuradio.fft import window
import sys
import signal
from argparse import ArgumentParser
from gnuradio.eng_arg import eng_float, intx
from gnuradio import eng_notation
from gnuradio import soapy
import math




class satnogs_iq_receiver(gr.top_block):

    def __init__(self, antenna="", bb_freq=0.0, bw=0.0, dc_removal="False", dev_args="", doppler_correction_per_sec=20, downlink_destination_ips='This will be changed', enable_doppler_correction=1, enable_iq_dump=0, file_path="test.wav", gain=0.0, gain_mode="Overall", iq_file_path="/tmp/iq.dat", lo_offset=100e3, other_settings="", out_samp_rate=0.0, ppm=0, rigctl_host="127.0.0.1", rigctl_port=4532, rx_freq=100e6, samp_rate_rx=0.0, soapy_rx_device="driver=invalid", stream_args="", tune_args="", udp_dump_port=0, uplink_origin_ip='This will be changed', waterfall_file_path="/tmp/waterfall.dat"):
        gr.top_block.__init__(self, "satnogs_iq_receiver", catch_exceptions=True)

        ##################################################
        # Parameters
        ##################################################
        self.antenna = antenna
        self.bb_freq = bb_freq
        self.bw = bw
        self.dc_removal = dc_removal
        self.dev_args = dev_args
        self.doppler_correction_per_sec = doppler_correction_per_sec
        self.downlink_destination_ips = downlink_destination_ips
        self.enable_doppler_correction = enable_doppler_correction
        self.enable_iq_dump = enable_iq_dump
        self.file_path = file_path
        self.gain = gain
        self.gain_mode = gain_mode
        self.iq_file_path = iq_file_path
        self.lo_offset = lo_offset
        self.other_settings = other_settings
        self.out_samp_rate = out_samp_rate
        self.ppm = ppm
        self.rigctl_host = rigctl_host
        self.rigctl_port = rigctl_port
        self.rx_freq = rx_freq
        self.samp_rate_rx = samp_rate_rx
        self.soapy_rx_device = soapy_rx_device
        self.stream_args = stream_args
        self.tune_args = tune_args
        self.udp_dump_port = udp_dump_port
        self.uplink_origin_ip = uplink_origin_ip
        self.waterfall_file_path = waterfall_file_path

        ##################################################
        # Blocks
        ##################################################
        self.soapy_rtlsdr_source_0 = None
        dev = 'driver=rtlsdr'
        stream_args = ''
        tune_args = ['']
        settings = ['']

        self.soapy_rtlsdr_source_0 = soapy.source(dev, "fc32", 1, '',
                                  stream_args, tune_args, settings)
        self.soapy_rtlsdr_source_0.set_sample_rate(0, 1e6)
        self.soapy_rtlsdr_source_0.set_gain_mode(0, False)
        self.soapy_rtlsdr_source_0.set_frequency(0, 433e6)
        self.soapy_rtlsdr_source_0.set_frequency_correction(0, 0)
        self.soapy_rtlsdr_source_0.set_gain(0, 'TUNER', 20)
        self.blocks_file_sink_0 = blocks.file_sink(gr.sizeof_gr_complex*1, '/app/out/output.iq', False)
        self.blocks_file_sink_0.set_unbuffered(False)


        ##################################################
        # Connections
        ##################################################
        self.connect((self.soapy_rtlsdr_source_0, 0), (self.blocks_file_sink_0, 0))


    def get_antenna(self):
        return self.antenna

    def set_antenna(self, antenna):
        self.antenna = antenna

    def get_bb_freq(self):
        return self.bb_freq

    def set_bb_freq(self, bb_freq):
        self.bb_freq = bb_freq

    def get_bw(self):
        return self.bw

    def set_bw(self, bw):
        self.bw = bw

    def get_dc_removal(self):
        return self.dc_removal

    def set_dc_removal(self, dc_removal):
        self.dc_removal = dc_removal

    def get_dev_args(self):
        return self.dev_args

    def set_dev_args(self, dev_args):
        self.dev_args = dev_args

    def get_doppler_correction_per_sec(self):
        return self.doppler_correction_per_sec

    def set_doppler_correction_per_sec(self, doppler_correction_per_sec):
        self.doppler_correction_per_sec = doppler_correction_per_sec

    def get_downlink_destination_ips(self):
        return self.downlink_destination_ips

    def set_downlink_destination_ips(self, downlink_destination_ips):
        self.downlink_destination_ips = downlink_destination_ips

    def get_enable_doppler_correction(self):
        return self.enable_doppler_correction

    def set_enable_doppler_correction(self, enable_doppler_correction):
        self.enable_doppler_correction = enable_doppler_correction

    def get_enable_iq_dump(self):
        return self.enable_iq_dump

    def set_enable_iq_dump(self, enable_iq_dump):
        self.enable_iq_dump = enable_iq_dump

    def get_file_path(self):
        return self.file_path

    def set_file_path(self, file_path):
        self.file_path = file_path

    def get_gain(self):
        return self.gain

    def set_gain(self, gain):
        self.gain = gain

    def get_gain_mode(self):
        return self.gain_mode

    def set_gain_mode(self, gain_mode):
        self.gain_mode = gain_mode

    def get_iq_file_path(self):
        return self.iq_file_path

    def set_iq_file_path(self, iq_file_path):
        self.iq_file_path = iq_file_path

    def get_lo_offset(self):
        return self.lo_offset

    def set_lo_offset(self, lo_offset):
        self.lo_offset = lo_offset

    def get_other_settings(self):
        return self.other_settings

    def set_other_settings(self, other_settings):
        self.other_settings = other_settings

    def get_out_samp_rate(self):
        return self.out_samp_rate

    def set_out_samp_rate(self, out_samp_rate):
        self.out_samp_rate = out_samp_rate

    def get_ppm(self):
        return self.ppm

    def set_ppm(self, ppm):
        self.ppm = ppm

    def get_rigctl_host(self):
        return self.rigctl_host

    def set_rigctl_host(self, rigctl_host):
        self.rigctl_host = rigctl_host

    def get_rigctl_port(self):
        return self.rigctl_port

    def set_rigctl_port(self, rigctl_port):
        self.rigctl_port = rigctl_port

    def get_rx_freq(self):
        return self.rx_freq

    def set_rx_freq(self, rx_freq):
        self.rx_freq = rx_freq

    def get_samp_rate_rx(self):
        return self.samp_rate_rx

    def set_samp_rate_rx(self, samp_rate_rx):
        self.samp_rate_rx = samp_rate_rx

    def get_soapy_rx_device(self):
        return self.soapy_rx_device

    def set_soapy_rx_device(self, soapy_rx_device):
        self.soapy_rx_device = soapy_rx_device

    def get_stream_args(self):
        return self.stream_args

    def set_stream_args(self, stream_args):
        self.stream_args = stream_args

    def get_tune_args(self):
        return self.tune_args

    def set_tune_args(self, tune_args):
        self.tune_args = tune_args

    def get_udp_dump_port(self):
        return self.udp_dump_port

    def set_udp_dump_port(self, udp_dump_port):
        self.udp_dump_port = udp_dump_port

    def get_uplink_origin_ip(self):
        return self.uplink_origin_ip

    def set_uplink_origin_ip(self, uplink_origin_ip):
        self.uplink_origin_ip = uplink_origin_ip

    def get_waterfall_file_path(self):
        return self.waterfall_file_path

    def set_waterfall_file_path(self, waterfall_file_path):
        self.waterfall_file_path = waterfall_file_path



def argument_parser():
    description = 'Generic IQ receiver with arbitrary output sampling rate'
    parser = ArgumentParser(description=description)
    parser.add_argument(
        "--antenna", dest="antenna", type=str, default="",
        help="Set antenna [default=%(default)r]")
    parser.add_argument(
        "--bb-freq", dest="bb_freq", type=eng_float, default=eng_notation.num_to_str(float(0.0)),
        help="Set Baseband CORDIC frequency (if the device supports it) [default=%(default)r]")
    parser.add_argument(
        "--bw", dest="bw", type=eng_float, default=eng_notation.num_to_str(float(0.0)),
        help="Set Bandwidth [default=%(default)r]")
    parser.add_argument(
        "--dc-removal", dest="dc_removal", type=str, default="False",
        help="Set Remove automatically the DC offset (if the device support it) [default=%(default)r]")
    parser.add_argument(
        "--dev-args", dest="dev_args", type=str, default="",
        help="Set Device arguments [default=%(default)r]")
    parser.add_argument(
        "--doppler-correction-per-sec", dest="doppler_correction_per_sec", type=intx, default=20,
        help="Set doppler_correction_per_sec [default=%(default)r]")
    parser.add_argument(
        "--downlink-destination-ips", dest="downlink_destination_ips", type=str, default='This will be changed',
        help="Set Device arguments [default=%(default)r]")
    parser.add_argument(
        "--enable-doppler-correction", dest="enable_doppler_correction", type=intx, default=1,
        help="Set enable_doppler_correction [default=%(default)r]")
    parser.add_argument(
        "--enable-iq-dump", dest="enable_iq_dump", type=intx, default=0,
        help="Set enable_iq_dump [default=%(default)r]")
    parser.add_argument(
        "--file-path", dest="file_path", type=str, default="test.wav",
        help="Set file_path [default=%(default)r]")
    parser.add_argument(
        "--gain", dest="gain", type=eng_float, default=eng_notation.num_to_str(float(0.0)),
        help="Set gain [default=%(default)r]")
    parser.add_argument(
        "--gain-mode", dest="gain_mode", type=str, default="Overall",
        help="Set gain_mode [default=%(default)r]")
    parser.add_argument(
        "--iq-file-path", dest="iq_file_path", type=str, default="/tmp/iq.dat",
        help="Set iq_file_path [default=%(default)r]")
    parser.add_argument(
        "--lo-offset", dest="lo_offset", type=eng_float, default=eng_notation.num_to_str(float(100e3)),
        help="Set lo_offset [default=%(default)r]")
    parser.add_argument(
        "--other-settings", dest="other_settings", type=str, default="",
        help="Set Soapy Channel other settings [default=%(default)r]")
    parser.add_argument(
        "--out-samp-rate", dest="out_samp_rate", type=eng_float, default=eng_notation.num_to_str(float(0.0)),
        help="Set Output sampling rate [default=%(default)r]")
    parser.add_argument(
        "--ppm", dest="ppm", type=eng_float, default=eng_notation.num_to_str(float(0)),
        help="Set ppm [default=%(default)r]")
    parser.add_argument(
        "--rigctl-host", dest="rigctl_host", type=str, default="127.0.0.1",
        help="Set rigctl_host [default=%(default)r]")
    parser.add_argument(
        "--rigctl-port", dest="rigctl_port", type=intx, default=4532,
        help="Set rigctl_port [default=%(default)r]")
    parser.add_argument(
        "--rx-freq", dest="rx_freq", type=eng_float, default=eng_notation.num_to_str(float(100e6)),
        help="Set rx_freq [default=%(default)r]")
    parser.add_argument(
        "--samp-rate-rx", dest="samp_rate_rx", type=eng_float, default=eng_notation.num_to_str(float(0.0)),
        help="Set Device Sampling rate [default=%(default)r]")
    parser.add_argument(
        "--soapy-rx-device", dest="soapy_rx_device", type=str, default="driver=invalid",
        help="Set soapy_rx_device [default=%(default)r]")
    parser.add_argument(
        "--stream-args", dest="stream_args", type=str, default="",
        help="Set Soapy Stream arguments [default=%(default)r]")
    parser.add_argument(
        "--tune-args", dest="tune_args", type=str, default="",
        help="Set Soapy Channel Tune arguments [default=%(default)r]")
    parser.add_argument(
        "--udp-dump-port", dest="udp_dump_port", type=intx, default=0,
        help="Set Device arguments [default=%(default)r]")
    parser.add_argument(
        "--uplink-origin-ip", dest="uplink_origin_ip", type=str, default='This will be changed',
        help="Set Device arguments [default=%(default)r]")
    parser.add_argument(
        "--waterfall-file-path", dest="waterfall_file_path", type=str, default="/tmp/waterfall.dat",
        help="Set waterfall_file_path [default=%(default)r]")
    return parser


def main(top_block_cls=satnogs_iq_receiver, options=None):
    if options is None:
        options = argument_parser().parse_args()
    tb = top_block_cls(antenna=options.antenna, bb_freq=options.bb_freq, bw=options.bw, dc_removal=options.dc_removal, dev_args=options.dev_args, doppler_correction_per_sec=options.doppler_correction_per_sec, downlink_destination_ips=options.downlink_destination_ips, enable_doppler_correction=options.enable_doppler_correction, enable_iq_dump=options.enable_iq_dump, file_path=options.file_path, gain=options.gain, gain_mode=options.gain_mode, iq_file_path=options.iq_file_path, lo_offset=options.lo_offset, other_settings=options.other_settings, out_samp_rate=options.out_samp_rate, ppm=options.ppm, rigctl_host=options.rigctl_host, rigctl_port=options.rigctl_port, rx_freq=options.rx_freq, samp_rate_rx=options.samp_rate_rx, soapy_rx_device=options.soapy_rx_device, stream_args=options.stream_args, tune_args=options.tune_args, udp_dump_port=options.udp_dump_port, uplink_origin_ip=options.uplink_origin_ip, waterfall_file_path=options.waterfall_file_path)

    def sig_handler(sig=None, frame=None):
        tb.stop()
        tb.wait()

        sys.exit(0)

    signal.signal(signal.SIGINT, sig_handler)
    signal.signal(signal.SIGTERM, sig_handler)

    tb.start()

    tb.wait()


if __name__ == '__main__':
    main()
