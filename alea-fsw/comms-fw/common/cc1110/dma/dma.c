// OpenLST
// Copyright (C) 2018 Planet Labs Inc.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
/**
 * @file dma.c
 * @brief Routines for Direct Memory Access (DMA)
 *
 * If you are unfamilar with DMA read Design Note DN107
 * "DMA and Radio Configuration" for a walkthrough
 *
 * @author Timothy Wriglesworth
 * @date May 13,2021
 */

// DMA setup and interrupt handler routines
#ifdef BOOTLOADER
// #pragma is a compiler directive: "The name to be used for the
// code segment, default CSEG. This is useful if you need to tell the
// compiler to put the code in a special segment so you can later on
// tell the linker to put this segment in  a special place in memory."
// pg. 27/97 SDCC Compiler User Guide. Use HOME in this case as an
// identifier
#pragma codeseg HOME
#endif
#include "dma.h"
#include "stringx.h"

// DMA Controller Configuration Structure
__xdata dma_config_t dma_configs[DMA_NUM_CHANNELS];

/**
 * @brief Set up the DMA peripheral
 * @sideeffect DMA controller configuration structure fields all set to 0
 */
void dma_init(void) {

    // Set the DMA controller configuration structure to 0 just to
    // make sure all the triggers default to NONE
    // We don't want to trigger accidently, which
    // is why we default to 0
    memsetx((__xdata char *) dma_configs, 0, sizeof(dma_configs));

    // The DMA channel 0 configuration struct
    DMA0CFGH = DMA_ADDR_HIGH(&dma_configs[0]);
    DMA0CFGL = DMA_ADDR_LOW(&dma_configs[0]);
    // The DMA channels 1-4 configuration structs
    // Yes its correct we passed &dma_configs[1] in only
    // See the CC1110 datasheet pg. 48/249
    DMA1CFGH = DMA_ADDR_HIGH(&dma_configs[1]);
    DMA1CFGL = DMA_ADDR_LOW(&dma_configs[1]);
}

/**
 * @brief Configure a DMA channel
 *
 * As data is read from the source address, it is placed in the DMA
 * controller FIFO. The four DMA channels must share the DMA controller
 * FIFO; the FIFO can only be accessed by a single channel at a time
 *
 * SRCINC and DESTINC:
 * When the DMA channel is armed or rearmed,
 * the source and destination addresses are
 * transferred to internal address pointers. These
 * pointers, and hence the source and
 * destination addresses, can be controlled to
 * increment, decrement, or not change between
 * byte/word transfers in order to give good flexibility.

 * TMODE and TRIG:
 * The transfer mode TMODE determines how the DMA
 * channel behaves when transferring data. The TRIG field in the
 * configuration determines which trigger the DMA channel is to use.
 *
 * @param[in] channel The DMA channel to use
 * @param[in] src The location the source
 * @param[in] dest The location destination
 * @param[in] trig_cfg The TMODE and TRIG
 * @param[in] inc_cfg The SRCINC and DESTINC
 * @sideeffect DMA controller configuration structure fields found in cc1110_regs.h
 */
void dma_configure_transfer(dma_channel_t channel,
                   __xdata uint8_t *src, __xdata uint8_t *dest,
                   uint8_t trig_cfg, uint8_t inc_cfg) {
    __xdata dma_config_t *cfg;

    // Assign base address of the DMA configuration struct
    // into the cfg pointer
    cfg = &dma_configs[channel];

    // High bits of the source address
    cfg->src_h = DMA_ADDR_HIGH(src);
     // Low bits of the source address
    cfg->src_l = DMA_ADDR_LOW(src);
    // High bits of the destination address
    cfg->dest_h = DMA_ADDR_HIGH(dest);
    // Low bits of the destination address
    cfg->dest_l = DMA_ADDR_LOW(dest);

    // Trigger DMA channel
    cfg->trig_cfg = trig_cfg;

    // Increment source input
    cfg->inc_cfg = inc_cfg;

}

/**
 * @brief  Configure the length settings on a DMA channel
 *
 * The number of bytes/words needed to be
 * moved from source to destination. When the
 * transfer count is reached, the DMA controller
 * rearms or disarms the DMA channel
 * (depending on transfer mode)
 * The transfer count can be of
 * fixed or variable length depending on how the
 * DMA channel is configured (pg. 103/249 CC1110 datasheet)
 *
 * @param[in] channel The DMA channel to use
 * @param[in] vlen The length transfer count VLEN = 001 or 010 or 011 or 100
 * @param[in] len of transfer count
 * @sideeffect Transfer count high bits (when applicable), VLEN and transfer count low bits (when applicable)
 */
void dma_configure_length(dma_channel_t channel, uint8_t vlen, uint16_t len) {
    __xdata dma_config_t *cfg;

    // Assign base address of the DMA configuration struct
    // into the cfg pointer
    cfg = &dma_configs[channel];

    // Transfer count high bits
    // Use bit masks to get the correct high bits
    cfg->len_h = ((len >> 8) & ~DMA_VLEN_MASK) | vlen;

    // Transfer count low bits
    // Use bit masks to get the correct low bits
    cfg->len_l = len & 0xff;
}

/**
 * @brief Reconfigure just the source address
 * This is especially useful for the AES input channel
 * Which needs to pull data from several sources (key, IV, data)
 *
 * @param[in] channel The DMA channel to use
 * @param[in] src The Memory location source
 * @sideeffects High and low bits of the source address
 */
void dma_configure_source_addr(dma_channel_t channel, __xdata uint8_t *src) {
    __xdata dma_config_t *cfg;

    // Assign base address of the DMA configuration struct
    // into the cfg pointer
    cfg = &dma_configs[channel];
    // Get the src address high byte
    cfg->src_h = DMA_ADDR_HIGH(src);

    // Get the src address low byte
    cfg->src_l = DMA_ADDR_LOW(src);
}

// Unused in bootloader
#ifndef BOOTLOADER

/**
 * @brief Reconfigure just the dest address
 *
 * @param[in] channel The DMA channel to use
 * @param[in] dest The Memory location dest
 * @sideeffects High and low bits of the dest address
 */
void dma_configure_dest_addr(dma_channel_t channel, __xdata uint8_t *dest) {
    __xdata dma_config_t *cfg;

    // Assign base address of the DMA configuration struct
    // into the cfg pointer
    cfg = &dma_configs[channel];
    // Get the dest address high byte
    cfg->dest_h = DMA_ADDR_HIGH(dest);

    // Get the dest address low byte
    cfg->dest_l = DMA_ADDR_LOW(dest);
}

#endif
