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
  LinkAnimations.cpp					\
  Music.cpp						\
  TilesetsCreation.inc					\
  MapsCreation.inc					\
  Tileset.cpp						\
  tilesets/TilesetHouse.cpp				\
  maps/MapLinkHouse.cpp					\
  maps/MapLinkHouseEntities.inc				\
  maps/MapLinkHouseSecretRoom.cpp			\
  maps/MapLinkHouseSecretRoomEntities.inc		\
  maps/MapRupeeHouse.cpp				\
  maps/MapRupeeHouseEntities.inc			


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
  $(zsdxincludedir)/LinkAnimations.h			\
  $(zsdxincludedir)/Music.h				\
  $(zsdxincludedir)/MapList.inc				\
  $(zsdxincludedir)/TilesetList.inc			\
  $(zsdxincludedir)/Tileset.h				\
  $(zsdxincludedir)/tilesets/TilesetHouse.h		\
  $(zsdxincludedir)/maps/MapLinkHouse.h			\
  $(zsdxincludedir)/maps/MapLinkHouseSecretRoom.h	\
  $(zsdxincludedir)/maps/MapRupeeHouse.h		
