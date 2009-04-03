#ifndef ZSDX_SWITCH_H
#define ZSDX_SWITCH_H

#include "Common.h"
#include "entities/Detector.h"

/**
 * A switch is a button that the hero can push.
 * Some switches require a block to be enabled.
 * Some switches become disabled when the hero or the block leave it.
 * A switch can be visible or invisible.
 */
class Switch: public Detector {

 public:

  /**
   * Subtypes of teletransporters.
   */
  enum Subtype {
    INVISIBLE = 0, /**< an invisible switch, usually to detect the hero position */
    NORMAL    = 1, /**< a classical switch */
  };

 private:

  Subtype subtype;           /**< subtype of teletransporter */
  bool needs_block;          /**< indicates that a block or a statue is required to enable to switch */
  bool disable_when_leaving; /**< indicates that the switch becomes disabled when the hero or the block leaves it */

  bool enabled;              /**< indicates that the switch is currently enabled */

 public:

  Switch(std::string name, Layer layer, int x, int y,
	 Subtype subtype, bool needs_block, bool disabled_when_leaving);
  ~Switch(void);

  EntityType get_type(void);

  void collision(MapEntity *entity_overlapping, CollisionMode collision_mode);
};

#endif
