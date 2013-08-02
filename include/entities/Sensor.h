/*
 * Copyright (C) 2006-2013 Christopho, Solarus - http://www.solarus-games.org
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
#ifndef SOLARUS_SENSOR_H
#define SOLARUS_SENSOR_H

#include "Common.h"
#include "entities/Detector.h"

/**
 * \brief An invisible entity that triggers something when it is walked on.
 *
 * A sensor is an invisible entity that triggers an event
 * when the hero or another entity walks on it. Unlike the switches,
 * the hero cannot avoid a sensor by jumping.
 */
class Sensor: public Detector {

  public:

    Sensor(const std::string& name, Layer layer, int x, int y, int width, int height);
    ~Sensor();

    EntityType get_type() const;
    bool can_be_drawn();

    bool is_obstacle_for(MapEntity& other);
    void notify_collision(MapEntity& entity_overlapping, CollisionMode collision_mode);
    void notify_collision_with_explosion(Explosion& explosion, CollisionMode collision_mode);
    void activate(Hero& hero);
    void update();

  private:

    bool activated_by_hero;      /**< true if the sensor is activated by the hero */
    bool notifying_script;       /**< true if the map script is currently being notified */
};

#endif

