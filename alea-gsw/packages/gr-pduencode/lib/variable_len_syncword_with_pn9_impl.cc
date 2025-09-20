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
#include <gnuradio/sync_block.h>
#include <iomanip>
#include "variable_len_syncword_with_pn9_impl.h"

namespace gr
{
  namespace pduencode
  {

    variable_len_syncword_with_pn9::sptr
    variable_len_syncword_with_pn9::make(uint32_t syncword, int max_len, int extra_footer_bytes, int threshold, bool debug_log)
    {
      return gnuradio::get_initial_sptr(new variable_len_syncword_with_pn9_impl(syncword, max_len, extra_footer_bytes, threshold, debug_log));
    }

    /*
     * The private constructor
     */
    variable_len_syncword_with_pn9_impl::variable_len_syncword_with_pn9_impl(uint32_t syncword, int max_len, int extra_footer_bytes, int threshold, bool debug_log)
        : gr::sync_block("variable_len_syncword_with_pn9",
                         gr::io_signature::make(1, 1, sizeof(uint8_t)),
                         gr::io_signature::make(0, 0, 0)),
          d_syncword(syncword),
          d_max_len(max_len),
          d_extra_footer_bytes(extra_footer_bytes),
          d_threshold(threshold),
          d_debug_log(debug_log)
    {
      // sets up ports to be of type pdu
      message_port_register_out(pmt::mp("out"));

      d_max_total_len = d_max_len + d_extra_footer_bytes;
      d_pn9_sequence = get_pn9_sequence(0x1ff);
      d_in_packet = false;
    }

    /*
     * Our virtual destructor.
     */
    variable_len_syncword_with_pn9_impl::~variable_len_syncword_with_pn9_impl()
    {
    }

    std::vector<uint8_t> variable_len_syncword_with_pn9_impl::get_pn9_sequence(uint16_t state)
    {
      std::vector<uint8_t> seq;
      // CC1110 PN9 algorithm
      for (int i = 0; i < d_max_len; ++i)
      {
        seq.push_back(static_cast<uint8_t>(state));
        for (int j = 0; j < 8; ++j)
        {
          state = (state >> 1) + (((state & 1) ^ (state >> 5) & 1) << 8);
        }
      }
      std::string pn9;
      std::stringstream ss;
      for (auto &i : seq)
      {
        ss << std::setfill('0') << std::setw(2) << std::hex << static_cast<uint16_t>(i);
        pn9 += ss.str() + ", ";
        ss.str("");
      }
      if (d_debug_log)
      {
        d_logger->info("pn9 sequence: " + pn9);
      }
      return seq;
    }

    int variable_len_syncword_with_pn9_impl::popcount(uint32_t val)
    {
      int c = 0;
      for (; val != 0; val &= val - 1)
      {
        c++;
      }
      return c;
    }

    void variable_len_syncword_with_pn9_impl::handle_bit(uint8_t b)
    {
      static auto start = std::chrono::high_resolution_clock::now();
      static auto stop = std::chrono::high_resolution_clock::now();

      static uint32_t current_syncword = 0;
      static uint8_t current_byte = 0;
      static uint8_t num_bits_set = 0;
      if (b > 1)
      {
        d_logger->warn("c > 1: " + std::to_string(b));
      }
      if (!d_in_packet)
      {
        current_syncword = (current_syncword << 1) ^ b;
        if (popcount(~(current_syncword ^ d_syncword)) > d_threshold)
        {
          d_in_packet = true;
          start = std::chrono::high_resolution_clock::now();
          if (d_debug_log)
          {
            d_logger->info("syncword match: " + std::bitset<32>(current_syncword).to_string());
          }
        }
      }
      else
      {
        // big endian
        current_byte = (current_byte << 1) ^ b;
        num_bits_set++;
        if (num_bits_set == 8)
        {
          uint8_t got_byte = current_byte;
          current_byte = 0;
          num_bits_set = 0;

          got_byte = got_byte ^ d_pn9_sequence[d_current_packet_pos];
          d_current_packet.push_back(got_byte);
          d_current_packet_pos++;

          if (d_current_packet_pos == 1)
          {
            // this is length byte
            // expected packet size is 1 more than value of length field
            d_current_packet_expected_size = got_byte + 1;
            if (d_debug_log)
            {
              d_logger->info("length byte: " + std::to_string(got_byte));
            }
          }
          else
          {
            // this is payload byte
          }

          // TODO: handle length byte larger than d_max_len

          if (d_current_packet.size() == d_current_packet_expected_size)
          {
            publish_message();
            stop = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
            // may report shorter time than actual due to syncword plus part of packet possibly being buffered and sent at the same time
            if (d_debug_log)
            {
              d_logger->info("packet time taken us: " + std::to_string(duration.count()));
              std::ostringstream ss;
              ss << std::hex << std::uppercase << std::setfill('0');
              for (int c : d_current_packet)
              {
                ss << std::setw(2) << c;
              }
              d_logger->info("packet: " + ss.str());
            }
            d_current_packet.clear();
            d_current_packet_pos = 0;
            d_current_packet_expected_size = 0;
            d_in_packet = false;
          }
          else if (d_current_packet.size() > d_max_len + 1)
          {
            // something went wrong
            if (d_debug_log)
            {
              d_logger->info("Error: packet size longer than " + std::to_string(d_max_len + 1));
            }
            d_current_packet.clear();
            d_current_packet_pos = 0;
            d_current_packet_expected_size = 0;
            d_in_packet = false;
          }
        }
        // TODO: handle d_extra_footer_bytes
      }
    }

    void variable_len_syncword_with_pn9_impl::publish_message()
    {
      message_port_pub(
          pmt::mp("out"),
          pmt::cons(pmt::PMT_NIL, pmt::init_u8vector(d_current_packet.size(), d_current_packet)));
    }

    int variable_len_syncword_with_pn9_impl::work(int noutput_items,
                                                  gr_vector_const_void_star &input_items,
                                                  gr_vector_void_star &output_items)
    {
      const uint8_t *in0 = reinterpret_cast<const uint8_t *>(input_items[0]);
      for (int i = 0; i < noutput_items; ++i)
      {
        this->handle_bit(in0[i]);
      }

      return noutput_items;
    }

  } /* namespace pduencode */
} /* namespace gr */
