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
 * This class tries to use the OpenSPC library. However, this library
 * does not seem to be available under all platforms. If OpenSPC cannot be dynamically
 * loaded, we use Snes_Spc which is slower.
 */
class SpcDecoder {

  private:

    // OpenSPC specific data
    void *openspc_plugin;

    // Snes_SPC specific data
    SNES_SPC *snes_spc_manager;   /**< the snes_spc object */
    SPC_Filter *snes_spc_filter;  /**< the snes_spc filter object */

    /**
     * The SPC decoding libraries supported.
     */
    enum SpcLibrary {
      LIB_OPENSPC,           /**< OpenSPC, an old but fast library, loaded as a plugin if available */
      LIB_SNES_SPC,          /**< Snes_Spc, a newer but slower library, directly included in the code */
    };

    SpcLibrary library;      /**< the library used */

  public:

    SpcDecoder(void);
    ~SpcDecoder(void);

    void load(Sint16 *sound_data, size_t sound_size);
    void decode(Sint16 *decoded_data, int nb_samples);
};

#endif

