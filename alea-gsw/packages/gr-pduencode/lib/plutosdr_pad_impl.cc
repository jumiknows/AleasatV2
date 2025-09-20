/* -*- c++ -*- */
/*
 * Copyright 2022 gr-pduencode author.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "plutosdr_pad_impl.h"

namespace gr {
  namespace pduencode {

    plutosdr_pad::sptr
    plutosdr_pad::make(int pad_bytes, int pad_multiple, unsigned char pad_value, bool prepend)
    {
      return gnuradio::get_initial_sptr
        (new plutosdr_pad_impl(pad_bytes, pad_multiple, pad_value, prepend));
    }


    /*
     * The private constructor
     */
    plutosdr_pad_impl::plutosdr_pad_impl(int pad_bytes, int pad_multiple, unsigned char pad_value, bool prepend)
      : gr::block("plutosdr_pad",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(0, 0, 0)),
              d_pad_bytes(pad_bytes),
              d_pad_multiple(pad_multiple),
              d_pad_value(pad_value),
              d_prepend(prepend)
    {
      // sets up ports to be of type pdu
      message_port_register_out(pmt::mp("out"));
      message_port_register_in(pmt::mp("in"));
      set_msg_handler(pmt::mp("in"), [this](pmt::pmt_t msg) { this->msg_handler(msg); });
    }

    /*
     * Our virtual destructor.
     */
    plutosdr_pad_impl::~plutosdr_pad_impl()
    {
    }

    int plutosdr_pad_impl::npadding_bytes(int pkt_byte_len)
    {
      int npad = 0;
      int remainder;

      if (pkt_byte_len == 0)
      {
        return npad;
      }

      // always add d_pad_bytes
      npad += d_pad_bytes;
      pkt_byte_len += d_pad_bytes;

      remainder = pkt_byte_len % d_pad_multiple;
      if (remainder == 0)
      {
        return npad;
      }

      // pad until multiple of d_pad_multiple
      npad += (d_pad_multiple - remainder);
      return npad;
    }

    void plutosdr_pad_impl::msg_handler(pmt::pmt_t pmt_msg)
    {
      // convert received pdu from pdu to std::vector<uint8_t>
      std::vector<uint8_t> msg = pmt::u8vector_elements(pmt::cdr(pmt_msg));

      if (d_prepend)
      {
        std::vector<uint8_t> msg2(npadding_bytes(msg.size()), d_pad_value);
        msg.insert(msg.begin(), msg2.begin(), msg2.end());
      }
      else
      {
        msg.resize(msg.size() + npadding_bytes(msg.size()), d_pad_value);
      }

      // outputs new pdu
      message_port_pub(
          pmt::mp("out"),
          pmt::cons(pmt::car(pmt_msg), pmt::init_u8vector(msg.size(), msg)));
    }

  } /* namespace pduencode */
} /* namespace gr */

