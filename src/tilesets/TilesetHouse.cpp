/*
 * Tileset generated automatically by the tileset editor.
 */

#include <SDL/SDL.h>
#include "tilesets/TilesetHouse.h"
#include "SimpleTile.h"
#include "AnimatedTile.h"

/**
 * Loads the tileset.
 */
void TilesetHouse::load(void) {
  load_tileset_image("images/tilesets/House.png");
  SDL_Rect position_in_tileset;

  // tile 1
  position_in_tileset.x = 0;
  position_in_tileset.y = 0;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 1);

  // tile 2
  position_in_tileset.x = 0;
  position_in_tileset.y = 8;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 2);

  // tile 3
  position_in_tileset.x = 8;
  position_in_tileset.y = 0;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 3);

  // tile 4
  position_in_tileset.x = 8;
  position_in_tileset.y = 8;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 4);

  // tile 5
  position_in_tileset.x = 16;
  position_in_tileset.y = 0;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 5);

  // tile 6
  position_in_tileset.x = 32;
  position_in_tileset.y = 0;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 6);

  // tile 7
  position_in_tileset.x = 48;
  position_in_tileset.y = 0;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 7);

  // tile 8
  position_in_tileset.x = 64;
  position_in_tileset.y = 0;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 8);

  // tile 9
  position_in_tileset.x = 72;
  position_in_tileset.y = 0;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 9);

  // tile 10
  position_in_tileset.x = 64;
  position_in_tileset.y = 8;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 10);

  // tile 11
  position_in_tileset.x = 72;
  position_in_tileset.y = 8;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 11);

  // tile 12
  position_in_tileset.x = 80;
  position_in_tileset.y = 0;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 12);

  // tile 13
  position_in_tileset.x = 88;
  position_in_tileset.y = 0;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 13);

  // tile 14
  position_in_tileset.x = 80;
  position_in_tileset.y = 8;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 14);

  // tile 15
  position_in_tileset.x = 88;
  position_in_tileset.y = 8;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 15);

  // tile 16
  position_in_tileset.x = 96;
  position_in_tileset.y = 0;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 16);

  // tile 17
  position_in_tileset.x = 112;
  position_in_tileset.y = 0;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 17);

  // tile 18
  position_in_tileset.x = 0;
  position_in_tileset.y = 16;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 18);

  // tile 19
  position_in_tileset.x = 8;
  position_in_tileset.y = 16;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 19);

  // tile 20
  position_in_tileset.x = 0;
  position_in_tileset.y = 24;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 20);

  // tile 21
  position_in_tileset.x = 8;
  position_in_tileset.y = 24;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 21);

  // tile 22
  position_in_tileset.x = 16;
  position_in_tileset.y = 16;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 22);

  // tile 23
  position_in_tileset.x = 24;
  position_in_tileset.y = 16;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 23);

  // tile 24
  position_in_tileset.x = 16;
  position_in_tileset.y = 24;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 24);

  // tile 25
  position_in_tileset.x = 24;
  position_in_tileset.y = 24;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 25);

  // tile 26
  position_in_tileset.x = 32;
  position_in_tileset.y = 16;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 26);

  // tile 27
  position_in_tileset.x = 40;
  position_in_tileset.y = 16;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 27);

  // tile 28
  position_in_tileset.x = 32;
  position_in_tileset.y = 24;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 28);

  // tile 29
  position_in_tileset.x = 40;
  position_in_tileset.y = 24;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 29);

  // tile 30
  position_in_tileset.x = 0;
  position_in_tileset.y = 32;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 30);

  // tile 31
  position_in_tileset.x = 8;
  position_in_tileset.y = 32;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 31);

  // tile 32
  position_in_tileset.x = 0;
  position_in_tileset.y = 40;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 32);

  // tile 33
  position_in_tileset.x = 8;
  position_in_tileset.y = 40;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 33);

  // tile 34
  position_in_tileset.x = 16;
  position_in_tileset.y = 32;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 34);

  // tile 35
  position_in_tileset.x = 24;
  position_in_tileset.y = 32;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 35);

  // tile 36
  position_in_tileset.x = 16;
  position_in_tileset.y = 40;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 36);

  // tile 37
  position_in_tileset.x = 24;
  position_in_tileset.y = 40;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 37);

  // tile 38
  position_in_tileset.x = 32;
  position_in_tileset.y = 32;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 38);

  // tile 39
  position_in_tileset.x = 40;
  position_in_tileset.y = 32;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 39);

  // tile 40
  position_in_tileset.x = 32;
  position_in_tileset.y = 40;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 40);

  // tile 41
  position_in_tileset.x = 40;
  position_in_tileset.y = 40;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 41);

  // tile 42
  position_in_tileset.x = 48;
  position_in_tileset.y = 16;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 42);

  // tile 43
  position_in_tileset.x = 64;
  position_in_tileset.y = 16;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 43);

  // tile 44
  position_in_tileset.x = 48;
  position_in_tileset.y = 32;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 44);

  // tile 45
  position_in_tileset.x = 64;
  position_in_tileset.y = 32;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 45);

  // tile 46
  position_in_tileset.x = 80;
  position_in_tileset.y = 16;
  position_in_tileset.w = 16;
  position_in_tileset.h = 32;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 46);

  // tile 47
  position_in_tileset.x = 96;
  position_in_tileset.y = 16;
  position_in_tileset.w = 16;
  position_in_tileset.h = 32;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 47);

  // tile 48
  position_in_tileset.x = 112;
  position_in_tileset.y = 16;
  position_in_tileset.w = 16;
  position_in_tileset.h = 32;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 48);

  // tile 49
  position_in_tileset.x = 0;
  position_in_tileset.y = 48;
  position_in_tileset.w = 24;
  position_in_tileset.h = 24;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 49);

  // tile 50
  position_in_tileset.x = 24;
  position_in_tileset.y = 48;
  position_in_tileset.w = 24;
  position_in_tileset.h = 24;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 50);

  // tile 51
  position_in_tileset.x = 0;
  position_in_tileset.y = 72;
  position_in_tileset.w = 24;
  position_in_tileset.h = 24;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 51);

  // tile 52
  position_in_tileset.x = 24;
  position_in_tileset.y = 72;
  position_in_tileset.w = 24;
  position_in_tileset.h = 24;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 52);

  // tile 53
  position_in_tileset.x = 48;
  position_in_tileset.y = 48;
  position_in_tileset.w = 16;
  position_in_tileset.h = 24;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 53);

  // tile 54
  position_in_tileset.x = 64;
  position_in_tileset.y = 48;
  position_in_tileset.w = 24;
  position_in_tileset.h = 16;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 54);

  // tile 55
  position_in_tileset.x = 48;
  position_in_tileset.y = 72;
  position_in_tileset.w = 24;
  position_in_tileset.h = 16;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 55);

  // tile 56
  position_in_tileset.x = 72;
  position_in_tileset.y = 64;
  position_in_tileset.w = 16;
  position_in_tileset.h = 24;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 56);

  // tile 57
  position_in_tileset.x = 88;
  position_in_tileset.y = 48;
  position_in_tileset.w = 24;
  position_in_tileset.h = 24;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 57);

  // tile 58
  position_in_tileset.x = 112;
  position_in_tileset.y = 48;
  position_in_tileset.w = 24;
  position_in_tileset.h = 24;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 58);

  // tile 59
  position_in_tileset.x = 88;
  position_in_tileset.y = 72;
  position_in_tileset.w = 24;
  position_in_tileset.h = 24;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 59);

  // tile 60
  position_in_tileset.x = 112;
  position_in_tileset.y = 72;
  position_in_tileset.w = 24;
  position_in_tileset.h = 24;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 60);

  // tile 61
  position_in_tileset.x = 0;
  position_in_tileset.y = 96;
  position_in_tileset.w = 24;
  position_in_tileset.h = 24;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 61);

  // tile 62
  position_in_tileset.x = 24;
  position_in_tileset.y = 96;
  position_in_tileset.w = 24;
  position_in_tileset.h = 24;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 62);

  // tile 63
  position_in_tileset.x = 0;
  position_in_tileset.y = 120;
  position_in_tileset.w = 24;
  position_in_tileset.h = 24;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 63);

  // tile 64
  position_in_tileset.x = 24;
  position_in_tileset.y = 120;
  position_in_tileset.w = 24;
  position_in_tileset.h = 24;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 64);

  // tile 65
  position_in_tileset.x = 48;
  position_in_tileset.y = 96;
  position_in_tileset.w = 16;
  position_in_tileset.h = 24;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 65);

  // tile 66
  position_in_tileset.x = 64;
  position_in_tileset.y = 96;
  position_in_tileset.w = 24;
  position_in_tileset.h = 16;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 66);

  // tile 67
  position_in_tileset.x = 48;
  position_in_tileset.y = 120;
  position_in_tileset.w = 24;
  position_in_tileset.h = 16;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 67);

  // tile 68
  position_in_tileset.x = 72;
  position_in_tileset.y = 112;
  position_in_tileset.w = 16;
  position_in_tileset.h = 24;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 68);

  // tile 69
  position_in_tileset.x = 88;
  position_in_tileset.y = 96;
  position_in_tileset.w = 24;
  position_in_tileset.h = 24;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 69);

  // tile 70
  position_in_tileset.x = 112;
  position_in_tileset.y = 96;
  position_in_tileset.w = 24;
  position_in_tileset.h = 24;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 70);

  // tile 71
  position_in_tileset.x = 88;
  position_in_tileset.y = 120;
  position_in_tileset.w = 24;
  position_in_tileset.h = 24;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 71);

  // tile 72
  position_in_tileset.x = 112;
  position_in_tileset.y = 120;
  position_in_tileset.w = 24;
  position_in_tileset.h = 24;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 72);

  // tile 79
  position_in_tileset.x = 128;
  position_in_tileset.y = 0;
  position_in_tileset.w = 32;
  position_in_tileset.h = 16;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 79);

  // tile 80
  position_in_tileset.x = 160;
  position_in_tileset.y = 0;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 80);

  // tile 81
  position_in_tileset.x = 168;
  position_in_tileset.y = 0;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 81);

  // tile 82
  position_in_tileset.x = 168;
  position_in_tileset.y = 8;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 82);

  // tile 83
  position_in_tileset.x = 160;
  position_in_tileset.y = 8;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 83);

  // tile 84
  position_in_tileset.x = 176;
  position_in_tileset.y = 0;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 84);

  // tile 85
  position_in_tileset.x = 184;
  position_in_tileset.y = 0;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 85);

  // tile 86
  position_in_tileset.x = 176;
  position_in_tileset.y = 8;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 86);

  // tile 87
  position_in_tileset.x = 184;
  position_in_tileset.y = 8;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 87);

  // tile 88
  position_in_tileset.x = 192;
  position_in_tileset.y = 0;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 88);

  // tile 89
  position_in_tileset.x = 200;
  position_in_tileset.y = 0;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 89);

  // tile 90
  position_in_tileset.x = 192;
  position_in_tileset.y = 8;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 90);

  // tile 91
  position_in_tileset.x = 200;
  position_in_tileset.y = 8;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 91);

  // tile 92
  position_in_tileset.x = 208;
  position_in_tileset.y = 0;
  position_in_tileset.w = 8;
  position_in_tileset.h = 16;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 92);

  // tile 93
  position_in_tileset.x = 128;
  position_in_tileset.y = 16;
  position_in_tileset.w = 16;
  position_in_tileset.h = 32;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 93);

  // tile 94
  position_in_tileset.x = 144;
  position_in_tileset.y = 16;
  position_in_tileset.w = 16;
  position_in_tileset.h = 32;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 94);

  // tile 95
  position_in_tileset.x = 160;
  position_in_tileset.y = 16;
  position_in_tileset.w = 16;
  position_in_tileset.h = 32;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 95);

  // tile 96
  position_in_tileset.x = 176;
  position_in_tileset.y = 16;
  position_in_tileset.w = 48;
  position_in_tileset.h = 32;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 96);

  // tile 97
  position_in_tileset.x = 192;
  position_in_tileset.y = 48;
  position_in_tileset.w = 32;
  position_in_tileset.h = 24;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 97);

  // tile 98
  position_in_tileset.x = 192;
  position_in_tileset.y = 72;
  position_in_tileset.w = 32;
  position_in_tileset.h = 32;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 98);

  // tile 99
  position_in_tileset.x = 136;
  position_in_tileset.y = 48;
  position_in_tileset.w = 32;
  position_in_tileset.h = 24;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 99);

  // tile 100
  position_in_tileset.x = 168;
  position_in_tileset.y = 48;
  position_in_tileset.w = 24;
  position_in_tileset.h = 32;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 100);

  // tile 101
  position_in_tileset.x = 136;
  position_in_tileset.y = 72;
  position_in_tileset.w = 24;
  position_in_tileset.h = 32;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 101);

  // tile 102
  position_in_tileset.x = 160;
  position_in_tileset.y = 80;
  position_in_tileset.w = 32;
  position_in_tileset.h = 24;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 102);

  // tile 110
  position_in_tileset.x = 0;
  position_in_tileset.y = 144;
  position_in_tileset.w = 8;
  position_in_tileset.h = 16;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 110);

  // tile 111
  position_in_tileset.x = 24;
  position_in_tileset.y = 144;
  position_in_tileset.w = 8;
  position_in_tileset.h = 16;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 111);

  // tile 112
  position_in_tileset.x = 0;
  position_in_tileset.y = 160;
  position_in_tileset.w = 8;
  position_in_tileset.h = 16;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 112);

  // tile 113
  position_in_tileset.x = 24;
  position_in_tileset.y = 160;
  position_in_tileset.w = 8;
  position_in_tileset.h = 16;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 113);

  // tile 114
  position_in_tileset.x = 8;
  position_in_tileset.y = 144;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 114);

  // tile 115
  position_in_tileset.x = 8;
  position_in_tileset.y = 160;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 115);

  // tile 116
  position_in_tileset.x = 32;
  position_in_tileset.y = 144;
  position_in_tileset.w = 16;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 116);

  // tile 117
  position_in_tileset.x = 48;
  position_in_tileset.y = 144;
  position_in_tileset.w = 16;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 117);

  // tile 118
  position_in_tileset.x = 32;
  position_in_tileset.y = 168;
  position_in_tileset.w = 16;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 118);

  // tile 119
  position_in_tileset.x = 48;
  position_in_tileset.y = 168;
  position_in_tileset.w = 16;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 119);

  // tile 120
  position_in_tileset.x = 48;
  position_in_tileset.y = 136;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 120);

  // tile 121
  position_in_tileset.x = 56;
  position_in_tileset.y = 136;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 121);

  // tile 122
  position_in_tileset.x = 64;
  position_in_tileset.y = 136;
  position_in_tileset.w = 8;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 122);

  // tile 123
  position_in_tileset.x = 136;
  position_in_tileset.y = 104;
  position_in_tileset.w = 8;
  position_in_tileset.h = 32;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 123);

  // tile 124
  position_in_tileset.x = 160;
  position_in_tileset.y = 104;
  position_in_tileset.w = 8;
  position_in_tileset.h = 32;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 124);

  // tile 125
  position_in_tileset.x = 144;
  position_in_tileset.y = 104;
  position_in_tileset.w = 16;
  position_in_tileset.h = 32;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 125);

  // tile 126
  position_in_tileset.x = 32;
  position_in_tileset.y = 152;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 126);

  // tile 127
  position_in_tileset.x = 48;
  position_in_tileset.y = 152;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 127);

  // tile 128
  position_in_tileset.x = 224;
  position_in_tileset.y = 40;
  position_in_tileset.w = 16;
  position_in_tileset.h = 24;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 128);

  // tile 129
  position_in_tileset.x = 240;
  position_in_tileset.y = 40;
  position_in_tileset.w = 16;
  position_in_tileset.h = 24;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 129);

  // tile 130
  position_in_tileset.x = 256;
  position_in_tileset.y = 40;
  position_in_tileset.w = 16;
  position_in_tileset.h = 24;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 130);

  // tile 131
  position_in_tileset.x = 224;
  position_in_tileset.y = 64;
  position_in_tileset.w = 48;
  position_in_tileset.h = 24;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 131);

  // tile 132
  position_in_tileset.x = 272;
  position_in_tileset.y = 64;
  position_in_tileset.w = 16;
  position_in_tileset.h = 24;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 132);

  // tile 133
  position_in_tileset.x = 224;
  position_in_tileset.y = 0;
  position_in_tileset.w = 32;
  position_in_tileset.h = 40;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 133);

  // tile 134
  position_in_tileset.x = 256;
  position_in_tileset.y = 0;
  position_in_tileset.w = 24;
  position_in_tileset.h = 40;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 134);

  // tile 135
  position_in_tileset.x = 0;
  position_in_tileset.y = 176;
  position_in_tileset.w = 8;
  position_in_tileset.h = 16;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 135);

  // tile 136
  position_in_tileset.x = 24;
  position_in_tileset.y = 176;
  position_in_tileset.w = 8;
  position_in_tileset.h = 16;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 136);

  // tile 137
  position_in_tileset.x = 0;
  position_in_tileset.y = 192;
  position_in_tileset.w = 8;
  position_in_tileset.h = 16;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 137);

  // tile 138
  position_in_tileset.x = 24;
  position_in_tileset.y = 192;
  position_in_tileset.w = 8;
  position_in_tileset.h = 16;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 138);

  // tile 139
  position_in_tileset.x = 8;
  position_in_tileset.y = 176;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 139);

  // tile 140
  position_in_tileset.x = 8;
  position_in_tileset.y = 192;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 140);

  // tile 141
  position_in_tileset.x = 32;
  position_in_tileset.y = 176;
  position_in_tileset.w = 16;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 141);

  // tile 142
  position_in_tileset.x = 48;
  position_in_tileset.y = 176;
  position_in_tileset.w = 16;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 142);

  // tile 143
  position_in_tileset.x = 32;
  position_in_tileset.y = 200;
  position_in_tileset.w = 16;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 143);

  // tile 144
  position_in_tileset.x = 48;
  position_in_tileset.y = 200;
  position_in_tileset.w = 16;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 144);

  // tile 145
  position_in_tileset.x = 32;
  position_in_tileset.y = 184;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 145);

  // tile 146
  position_in_tileset.x = 48;
  position_in_tileset.y = 184;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 146);

  // tile 147
  position_in_tileset.x = 0;
  position_in_tileset.y = 208;
  position_in_tileset.w = 32;
  position_in_tileset.h = 16;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 147);

  // tile 148
  position_in_tileset.x = 0;
  position_in_tileset.y = 224;
  position_in_tileset.w = 32;
  position_in_tileset.h = 16;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 148);

  // tile 149
  position_in_tileset.x = 32;
  position_in_tileset.y = 208;
  position_in_tileset.w = 16;
  position_in_tileset.h = 32;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 149);

  // tile 150
  position_in_tileset.x = 48;
  position_in_tileset.y = 208;
  position_in_tileset.w = 16;
  position_in_tileset.h = 32;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 150);

  // tile 151
  position_in_tileset.x = 64;
  position_in_tileset.y = 144;
  position_in_tileset.w = 8;
  position_in_tileset.h = 96;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 151);

  // tile 152
  position_in_tileset.x = 72;
  position_in_tileset.y = 144;
  position_in_tileset.w = 8;
  position_in_tileset.h = 96;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 152);

  // tile 153
  position_in_tileset.x = 80;
  position_in_tileset.y = 144;
  position_in_tileset.w = 16;
  position_in_tileset.h = 96;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 153);

  // tile 154
  position_in_tileset.x = 96;
  position_in_tileset.y = 144;
  position_in_tileset.w = 32;
  position_in_tileset.h = 96;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 154);

  // tile 155
  position_in_tileset.x = 128;
  position_in_tileset.y = 144;
  position_in_tileset.w = 32;
  position_in_tileset.h = 96;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 155);

  // tile 156
  position_in_tileset.x = 160;
  position_in_tileset.y = 144;
  position_in_tileset.w = 96;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 156);

  // tile 157
  position_in_tileset.x = 160;
  position_in_tileset.y = 152;
  position_in_tileset.w = 96;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 157);

  // tile 158
  position_in_tileset.x = 160;
  position_in_tileset.y = 160;
  position_in_tileset.w = 96;
  position_in_tileset.h = 16;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 158);

  // tile 159
  position_in_tileset.x = 160;
  position_in_tileset.y = 176;
  position_in_tileset.w = 96;
  position_in_tileset.h = 32;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 159);

  // tile 160
  position_in_tileset.x = 160;
  position_in_tileset.y = 208;
  position_in_tileset.w = 96;
  position_in_tileset.h = 32;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 160);

  // tile 161
  position_in_tileset.x = 64;
  position_in_tileset.y = 240;
  position_in_tileset.w = 32;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 161);

  // tile 162
  position_in_tileset.x = 64;
  position_in_tileset.y = 248;
  position_in_tileset.w = 32;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 162);

  // tile 163
  position_in_tileset.x = 96;
  position_in_tileset.y = 240;
  position_in_tileset.w = 32;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 163);

  // tile 164
  position_in_tileset.x = 96;
  position_in_tileset.y = 248;
  position_in_tileset.w = 32;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 164);

  // tile 165
  position_in_tileset.x = 64;
  position_in_tileset.y = 256;
  position_in_tileset.w = 32;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 165);

  // tile 166
  position_in_tileset.x = 64;
  position_in_tileset.y = 264;
  position_in_tileset.w = 32;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 166);

  // tile 167
  position_in_tileset.x = 96;
  position_in_tileset.y = 256;
  position_in_tileset.w = 32;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 167);

  // tile 168
  position_in_tileset.x = 96;
  position_in_tileset.y = 264;
  position_in_tileset.w = 32;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 168);

  // tile 169
  position_in_tileset.x = 64;
  position_in_tileset.y = 272;
  position_in_tileset.w = 32;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 169);

  // tile 170
  position_in_tileset.x = 64;
  position_in_tileset.y = 280;
  position_in_tileset.w = 32;
  position_in_tileset.h = 8;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 170);

  // tile 171
  position_in_tileset.x = 128;
  position_in_tileset.y = 240;
  position_in_tileset.w = 8;
  position_in_tileset.h = 32;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 171);

  // tile 172
  position_in_tileset.x = 136;
  position_in_tileset.y = 240;
  position_in_tileset.w = 8;
  position_in_tileset.h = 32;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 172);

  // tile 173
  position_in_tileset.x = 144;
  position_in_tileset.y = 240;
  position_in_tileset.w = 8;
  position_in_tileset.h = 32;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 173);

  // tile 174
  position_in_tileset.x = 152;
  position_in_tileset.y = 240;
  position_in_tileset.w = 8;
  position_in_tileset.h = 32;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 174);

  // tile 175
  position_in_tileset.x = 160;
  position_in_tileset.y = 240;
  position_in_tileset.w = 8;
  position_in_tileset.h = 32;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 175);

  // tile 176
  position_in_tileset.x = 168;
  position_in_tileset.y = 240;
  position_in_tileset.w = 8;
  position_in_tileset.h = 32;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 176);

  // tile 177
  position_in_tileset.x = 96;
  position_in_tileset.y = 272;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 177);

  // tile 178
  position_in_tileset.x = 112;
  position_in_tileset.y = 272;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 178);

  // tile 179
  position_in_tileset.x = 128;
  position_in_tileset.y = 272;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 179);

  // tile 180
  position_in_tileset.x = 144;
  position_in_tileset.y = 272;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 180);

  // tile 181
  position_in_tileset.x = 160;
  position_in_tileset.y = 272;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 181);

  // tile 182
  position_in_tileset.x = 176;
  position_in_tileset.y = 272;
  position_in_tileset.w = 16;
  position_in_tileset.h = 16;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE_NONE), 182);

  // tile 183
  position_in_tileset.x = 200;
  position_in_tileset.y = 240;
  position_in_tileset.w = 32;
  position_in_tileset.h = 24;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 183);

  // tile 184
  position_in_tileset.x = 232;
  position_in_tileset.y = 240;
  position_in_tileset.w = 24;
  position_in_tileset.h = 32;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 184);

  // tile 185
  position_in_tileset.x = 200;
  position_in_tileset.y = 264;
  position_in_tileset.w = 24;
  position_in_tileset.h = 32;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 185);

  // tile 186
  position_in_tileset.x = 224;
  position_in_tileset.y = 272;
  position_in_tileset.w = 32;
  position_in_tileset.h = 24;
  create_tile(new SimpleTile(position_in_tileset, OBSTACLE), 186);

}
