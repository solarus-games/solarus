/**
 * This header should be included by each class header file.
 */

#ifndef ZSDX_COMMON_H
#define ZSDX_COMMON_H

#define _CRT_SECURE_NO_DEPRECATE // allow sprintf in Visual C++...

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;
#include <cstdlib>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#include "Types.h"

#define MIN(x,y) (((x) > (y)) ? (y) : (x))
#define MAX(x,y) (((x) > (y)) ? (x) : (y))

/**
 * This macro should be used to exit the program properly on an error message.
 * The message parameter can contain several elements separated by the '<<' operator.
 */
#define DIE(message) do { ostringstream oss; oss << message; throw oss.str(); } while (0)

#endif
