AM_CPPFLAGS = -DDATADIR_="$(datadir)/zsdx" -O3 -Wall -Werror -ansi -I$(top_srcdir)/include
#AM_CPPFLAGS = -DDATADIR_="$(datadir)/zsdx" -g -Wall -Werror -ansi -I$(top_srcdir)/include

bin_PROGRAMS = zsdx
zsdx_SOURCES = 						\
  ZSDX.cpp						\
  FileTools.cpp						\
  Game.cpp						\
  GameResource.cpp					\
  Savegame.cpp						\
  SelectionMenu.cpp					\
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
  Sound.cpp						\
  Tileset.cpp						\
  TransitionEffect.cpp					\
  TransitionImmediate.cpp				\
  TransitionFade.cpp


# additional files to provide in a tarball release
zsdxincludedir = $(top_srcdir)/include
EXTRA_DIST = \
  $(zsdxincludedir)/Common.h				\
  $(zsdxincludedir)/Types.h				\
  $(zsdxincludedir)/ZSDX.h				\
  $(zsdxincludedir)/FileTools.h				\
  $(zsdxincludedir)/Game.h				\
  $(zsdxincludedir)/GameResource.h			\
  $(zsdxincludedir)/Savegame.h				\
  $(zsdxincludedir)/SelectionMenu.h			\
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
  $(zsdxincludedir)/AnimationListener.h			\
  $(zsdxincludedir)/TileOnMap.h				\
  $(zsdxincludedir)/Tile.h				\
  $(zsdxincludedir)/SimpleTile.h			\
  $(zsdxincludedir)/AnimatedTile.h			\
  $(zsdxincludedir)/TileAnimationManager.h		\
  $(zsdxincludedir)/Color.h				\
  $(zsdxincludedir)/Link.h				\
  $(zsdxincludedir)/Music.h				\
  $(zsdxincludedir)/Sound.h				\
  $(zsdxincludedir)/Tileset.h				\
  $(zsdxincludedir)/TransitionEffect.h			\
  $(zsdxincludedir)/TransitionImmediate.h		\
  $(zsdxincludedir)/TransitionFade.h
