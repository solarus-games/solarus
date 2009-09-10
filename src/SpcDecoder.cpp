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
#include "SpcDecoder.h"

/*
#ifdef WIN32 // there is probably a better way to determine whether dlopen() and dlsym() are available

#define dlopen(filename, mode) NULL
#define dlclose(file)
#define dlsym(dl, sym) NULL
#define RTLD_NOW 0

#else

#include <dlfcn.h>

#endif
*/
extern "C" {
#include "openspc/openspc.h"
}

/**
 * Creates an SPC decoder.
 */
SpcDecoder::SpcDecoder(void) {

  // choose a library
  //this->openspc_plugin = dlopen("libopenspc.so", RTLD_NOW);
  this->library = /*(openspc_plugin == NULL) ? LIB_SNES_SPC :*/ LIB_OPENSPC;

  // initialize the SPC library
  if (this->library == LIB_SNES_SPC) {
    snes_spc_manager = spc_new();
    snes_spc_filter = spc_filter_new();
  }

#if ZSDX_DEBUG_LEVEL > 0
  static const std::string library_names[] = {"OpenSPC", "Snes_SPC"};
  std::cout << "Using SPC library " << library_names[library] << std::endl;
#endif
}

/**
 * Destructor.
 */
SpcDecoder::~SpcDecoder(void) {

  // uninitialize the SPC library
  switch (library) {

    case LIB_OPENSPC:
      //dlclose(openspc_plugin);
      break;

    case LIB_SNES_SPC:
      spc_filter_delete(snes_spc_filter);
      spc_delete(snes_spc_manager);
      break;
  }
}

/**
 * Loads an SPC file from the memory.
 * @param sound_data the memory area to read
 * @param sound_size size of the memory area in bytes
 */
void SpcDecoder::load(Sint16 *sound_data, size_t sound_size) {

  switch (library) {

    case LIB_OPENSPC:
    {
      //void (*OSPC_Init)(void*, size_t);
      //OSPC_Init = (void (*)(void*, size_t)) dlsym(openspc_plugin, "OSPC_Init");
      OSPC_Init(sound_data, sound_size);
      break;
    }

    case LIB_SNES_SPC:
    {
      spc_load_spc(snes_spc_manager, (short int*) sound_data, sound_size);
      spc_clear_echo(snes_spc_manager);
      spc_filter_clear(snes_spc_filter);
      break;
    }
  }
}


/**
 * Decodes a chunk of the previously loaded SPC data into PCM data.
 * @param decoded_data pointer to where you want the decoded data to be wrote
 * @param nb_samples number of samples to write
 */
void SpcDecoder::decode(Sint16 *decoded_data, int nb_samples) {

  switch (library) {

    case LIB_OPENSPC:
    {
      //int (*OSPC_Run)(int, short*, int);
      //OSPC_Run = (int (*)(int, short*, int)) dlsym(openspc_plugin, "OSPC_Run");
      OSPC_Run(-1, decoded_data, nb_samples * 2);
      break;
    }

    case LIB_SNES_SPC:
    {
      const char *err = spc_play(snes_spc_manager, nb_samples, (short int*) decoded_data);
      if (err != NULL) {
	DIE("Failed to decode SPC data: " << err);
      }
      spc_filter_run(snes_spc_filter, (short int*) decoded_data, nb_samples);
      break;
    }
  }
}

