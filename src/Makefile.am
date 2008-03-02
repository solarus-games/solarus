AM_CPPFLAGS = -DDATADIR_="$(datadir)/zsdx" -O3 -Wall -Werror -ansi -I$(top_srcdir)/include

bin_PROGRAMS = zsdx
zsdx_SOURCES = 						\
  ZSDX.cpp						\
  FileTools.cpp						\
  Game.cpp						\
  GameResource.cpp					\
  Map.cpp						\
  MapEntrance.cpp					\
  MapEntity.cpp						\
  Moving.cpp						\
  MovingWithCollision.cpp				\
  MovingWithSmoothCollision.cpp				\
  Moving8ByPlayer.cpp					\
  EntityDetector.cpp					\
  MapExit.cpp						\
  AnimatedSprite.cpp					\
  SpriteAnimations.cpp					\
  SpriteAnimation.cpp					\
  SpriteAnimationDirection.cpp				\
  SimpleTile.cpp					\
  AnimatedTile.cpp					\
  TileOnMap.cpp						\
  Tile.cpp						\
  TileAnimationManager.cpp				\
  Color.cpp						\
  Link.cpp						\
  Music.cpp						\
  Tileset.cpp						\
  TransitionDisplayer.cpp


# additional files to provide in a tarball release
zsdxincludedir = $(top_srcdir)/include
EXTRA_DIST = \
  $(zsdxincludedir)/Common.h				\
  $(zsdxincludedir)/Types.h				\
  $(zsdxincludedir)/ZSDX.h				\
  $(zsdxincludedir)/FileTools.h				\
  $(zsdxincludedir)/GameResource.h			\
  $(zsdxincludedir)/Game.h				\
  $(zsdxincludedir)/Map.h				\
  $(zsdxincludedir)/MapEntrance.h			\
  $(zsdxincludedir)/MapEntity.h				\
  $(zsdxincludedir)/Moving.h				\
  $(zsdxincludedir)/MovingWithCollision.h		\
  $(zsdxincludedir)/MovingWithSmoothCollision.h		\
  $(zsdxincludedir)/Moving8ByPlayer.h			\
  $(zsdxincludedir)/EntityDetector.h			\
  $(zsdxincludedir)/MapExit.h				\
  $(zsdxincludedir)/AnimatedSprite.h			\
  $(zsdxincludedir)/SpriteAnimations.h			\
  $(zsdxincludedir)/SpriteAnimation.h			\
  $(zsdxincludedir)/SpriteAnimationDirection.h		\
  $(zsdxincludedir)/TileOnMap.h				\
  $(zsdxincludedir)/Tile.h				\
  $(zsdxincludedir)/SimpleTile.h			\
  $(zsdxincludedir)/AnimatedTile.h			\
  $(zsdxincludedir)/TileAnimationManager.h		\
  $(zsdxincludedir)/Color.h				\
  $(zsdxincludedir)/Link.h				\
  $(zsdxincludedir)/Music.h				\
  $(zsdxincludedir)/Tileset.h				\
  $(zsdxincludedir)/TransitionDisplayer.h		
