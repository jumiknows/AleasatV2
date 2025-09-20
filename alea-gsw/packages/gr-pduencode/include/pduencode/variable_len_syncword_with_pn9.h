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

#ifndef INCLUDED_PDUENCODE_VARIABLE_LEN_SYNCWORD_WITH_PN9_H
#define INCLUDED_PDUENCODE_VARIABLE_LEN_SYNCWORD_WITH_PN9_H

#include <pduencode/api.h>
#include <gnuradio/block.h>
#include <gnuradio/sync_block.h>

namespace gr
{
  namespace pduencode
  {

    /*!
     * \brief <+description of block+>
     * \ingroup pduencode
     *
     */
    class PDUENCODE_API variable_len_syncword_with_pn9 : virtual public gr::sync_block
    {
    public:
      typedef std::shared_ptr<variable_len_syncword_with_pn9> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of pduencode::variable_len_syncword_with_pn9.
       *
       * To avoid accidental use of raw pointers, pduencode::variable_len_syncword_with_pn9's
       * constructor is in a private implementation
       * class. pduencode::variable_len_syncword_with_pn9::make is the public interface for
       * creating new instances.
       */
      static sptr make(uint32_t syncword, int max_len, int extra_footer_bytes, int threshold, bool debug_log);
    };

  } // namespace pduencode
} // namespace gr

#endif /* INCLUDED_PDUENCODE_VARIABLE_LEN_SYNCWORD_WITH_PN9_H */
