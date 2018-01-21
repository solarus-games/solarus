/*
 * Copyright (C) 2006-2018 Christopho, Solarus - http://www.solarus-games.org
 *
 * Solarus is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Solarus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef SOLARUS_ABILITY_H
#define SOLARUS_ABILITY_H

namespace Solarus {

/**
 * \brief The different built-in abilities managed by the engine.
 */
enum class Ability {

  TUNIC,                  /**< Tunic of the hero. Determines his sprite. */
  SWORD,                  /**< Sword of the hero. 0 means no sword, a
                           * greater values determine the sword sprite. */
  SWORD_KNOWLEDGE,        /**< Allows to do the super spin attack. */
  SHIELD,                 /**< Allows to stop attacks from enemies. */
  LIFT,                   /**< Lifting destructible objects. */
  SWIM,                   /**< Swimming in deep water. */
  JUMP_OVER_WATER,        /**< Automatically jumping when arriving in water
                           * without the SWIM ability. */
  RUN,                    /**< Running faster. */
  PUSH,                   /**< Pushing obstacles. */
  GRAB,                   /**< Grabbing obstacles. */
  PULL,                   /**< Pulling obstacles. */
  DETECT_WEAK_WALLS,      /**< Be notified of weak walls nearby. */
};

}

#endif

