/**
 * This header should be included by each source file.
 */

#ifndef ZSDX_COMMON_H
#define ZSDX_COMMON_H

using namespace std;
#include <iostream>
#include <cstdlib>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#ifndef ZSDX_CONFIG_H
#define ZSDX_CONFIG_H
#include "config.h"
#endif

#include "Types.h"

#define MIN(x,y) ((x > y) ? (y) : (x))
#define MAX(x,y) ((x > y) ? (x) : (y))

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__CYGWIN__)
#define WINDOWS 1
#endif

#endif
