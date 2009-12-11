/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef ZSDX_SPC_DECODER_H
#define ZSDX_SPC_DECODER_H

#include "Common.h"
#include "snes_spc/spc.h"

/**
 * This class encapsulates the SPC music decoding, thus allowing
 * the Music class to be independent of the SPC library.
 *
 * To decode SPC music files, the Snes_SPC library code is used.
 * Snes_SPC is a recent SPC decoding library and exists in two versions:
 * - a fast version, that renders music with a very nice quality (recommended);
 * - a highly accurate version, that renders music with a better accuracy, but three times slower.
 * The source code of ZSDX include the fast version of Snes_SPC, which I recommend
 * since the sound generated is good enough.
 */
class SpcDecoder {

  private:

    // Snes_SPC specific data
    SNES_SPC *snes_spc_manager;   /**< the snes_spc object */
    SPC_Filter *snes_spc_filter;  /**< the snes_spc filter object */

  public:

    SpcDecoder(void);
    ~SpcDecoder(void);

    void load(int16_t *sound_data, size_t sound_size);
    void decode(int16_t *decoded_data, int nb_samples);
};

#endif

