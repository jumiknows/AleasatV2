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

#ifndef INCLUDED_PDUENCODE_VARIABLE_LEN_SYNCWORD_WITH_PN9_IMPL_H
#define INCLUDED_PDUENCODE_VARIABLE_LEN_SYNCWORD_WITH_PN9_IMPL_H

#include <pduencode/variable_len_syncword_with_pn9.h>

namespace gr {
  namespace pduencode {

    class variable_len_syncword_with_pn9_impl : public variable_len_syncword_with_pn9
    {
     private:
      uint32_t d_syncword = 0;
      int d_max_len = 0;
      int d_extra_footer_bytes = 0;
      int d_threshold = 0;
      bool d_debug_log = false;

      int d_max_total_len = 0;
      std::vector<uint8_t> d_pn9_sequence;

      bool d_in_packet = false;

      std::vector<uint8_t> d_current_packet;
      int d_current_packet_pos = 0;
      int d_current_packet_expected_size = 0;

      std::vector<uint8_t> get_pn9_sequence(uint16_t state);

      int popcount(uint32_t val);

      void handle_bit(uint8_t c);

      void publish_message();

     public:
      variable_len_syncword_with_pn9_impl(uint32_t syncword, int max_len, int extra_footer_bytes, int threshold,  bool debug_log);
      ~variable_len_syncword_with_pn9_impl();

      int work(int noutput_items,
          gr_vector_const_void_star &input_items,
          gr_vector_void_star &output_items) override;

    };

  } // namespace pduencode
} // namespace gr

#endif /* INCLUDED_PDUENCODE_VARIABLE_LEN_SYNCWORD_WITH_PN9_IMPL_H */

