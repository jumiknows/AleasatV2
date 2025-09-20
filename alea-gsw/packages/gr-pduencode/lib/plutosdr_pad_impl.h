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

#ifndef INCLUDED_PDUENCODE_PLUTOSDR_PAD_IMPL_H
#define INCLUDED_PDUENCODE_PLUTOSDR_PAD_IMPL_H

#include <pduencode/plutosdr_pad.h>

namespace gr {
  namespace pduencode {

    class plutosdr_pad_impl : public plutosdr_pad
    {
     private:
      int d_pad_bytes;
      int d_pad_multiple;
      char d_pad_value;
      bool d_prepend;

     public:
      plutosdr_pad_impl(int pad_bytes, int pad_multiple, unsigned char pad_value, bool prepend);
      ~plutosdr_pad_impl();

      // calculates number of padding bytes to add
      int npadding_bytes(int pkt_byte_len);

      // runs when pdu is received
      // adds usrp padding to tail of pdu and outputs
      void msg_handler(pmt::pmt_t pmt_msg);

    };

  } // namespace pduencode
} // namespace gr

#endif /* INCLUDED_PDUENCODE_PLUTOSDR_PAD_IMPL_H */

