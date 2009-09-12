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

/*
 * By default, to decode SPC music files, OpenSPC is used.
 * OpenSPC is a fast library but is compatible only with i386 architectures.
 * To compile with Snes_Spc instead (a slower but portable library),
 * define the preprocessor symbol USE_SNES_SPC by uncommenting
 * the following line, or set it in your compilation command line
 * (for gcc, use the flag -DUSE_SNES_SPC).
 * The SPC library you will need to link to the executable depends on this choice.
 */
//#define USE_SNES_SPC // just uncomment this to use Snes_SPC instead of OpenSPC

#ifndef USE_SNES_SPC
#ifndef USE_OPENSPC
#define USE_OPENSPC // OpenSPC (faster) by default
#endif
#endif

#ifdef USE_SNES_SPC
#include <snes_spc/spc.h>
#endif

/**
 * This class encapsulates the SPC music decoding, thus allowing
 * the Music class to be independent of the SPC library choice.
 */
class SpcDecoder {

  private:

#ifdef USE_SNES_SPC

    // Snes_SPC specific data
    SNES_SPC *snes_spc_manager;   /**< the snes_spc object */
    SPC_Filter *snes_spc_filter;  /**< the snes_spc filter object */

#endif

  public:

    SpcDecoder(void);
    ~SpcDecoder(void);

    void load(Sint16 *sound_data, size_t sound_size);
    void decode(Sint16 *decoded_data, int nb_samples);
};

#endif

