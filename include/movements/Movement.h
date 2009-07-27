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
#ifndef ZSDX_MOVEMENT_H
#define ZSDX_MOVEMENT_H

#include "Common.h"

/**
 * The movement of an entity on the map, visible or not.
 * A moving entity has an instance of Movement which modifies its coordinates.
 * An instance of Movement is attached to one and only one instance of MapEntity.
 *
 * This class implements a simple speed vector: you can set the speed and the direction.
 * Subclassses can implement totally different movements, using or not what is
 * provided by the Movement class.
 */
class Movement {

 protected:

  /**
   * The entity controlled by this movement.
   * If it is NULL, the movement is applied to the internal x and y fields of the Movement class.
   */
  MapEntity *entity;

 private:

  int x; /**< x coordinate of the object controlled by this movement */
  int y; /**< y coordinate of the object controlled by this movement */

  // speed vector

  /**
   * X speed of the entity, between -100 and 100.
   * 0: stopped
   * positive value: moving to the right
   * negative value: moving to the left
   */
  double x_speed;

  /**
   * Y speed of the entity, between -100 and 100.
   * 0: stopped
   * positive value: moving downwards
   * negative value: moving upwards
   */
  double y_speed;

  /**
   * Date of the next x move in ticks.
   */
  Uint32 next_move_date_x;

  /**
   * Date of the next y move in ticks.
   */
  Uint32 next_move_date_y;

  /**
   * Date of the last x or y move.
   */
  Uint32 last_move_date;

  // the following fields are redundant and can be computed
  // with x_speed and y_speed

  /**
   * Delay in ticks between an x move of 2 pixels.
   * x_delay = 200 / |x_speed|
   */
  Uint32 x_delay;

  /**
   * Delay in ticks between an y move of 2 pixels.
   * y_delay = 200 / |y_speed|
   */
  Uint32 y_delay;

  /**
   * Number of pixels of the next x move : 0, 1 or -1.
   * The smallest move is two pixels because an entity
   * doesn't need to make a move of only one pixel. 
   */
  int x_move;

  /**
   * Number of pixels of the next y move : 0, 1 or -1.
   */
  int y_move;

  /**
   * Indicates whether the movement is suspended.
   */
  bool suspended;

 protected:

  /**
   * Indicates when the movement was suspended.
   */
  Uint32 when_suspended;

  int get_x_move(void);
  int get_y_move(void);
  void set_x_move(int x_move);
  void set_y_move(int y_move);

  Uint32 get_next_move_date_x(void);
  Uint32 get_next_move_date_y(void);
  void set_next_move_date_x(Uint32 next_move_date_x);
  void set_next_move_date_y(Uint32 next_move_date_y);

  Uint32 get_x_delay(void);
  Uint32 get_y_delay(void);
  void set_x_delay(Uint32 x_delay);
  void set_y_delay(Uint32 y_delay);

  void translate_x(int dx);
  void translate_y(int dy);
  void translate(int dx, int dy);

  virtual void update_x(void);
  virtual void update_y(void);

 public:

  Movement(void);
  virtual ~Movement(void);

  // entity
  virtual void set_entity(MapEntity *entity);

  // position
  int get_x(void);
  int get_y(void);
  void set_x(int x);
  void set_y(int y);
  virtual void set_position(int x, int y);

  virtual void update(void);
  bool has_to_move_now(void);

  // movement

  double get_x_speed();
  double get_y_speed();
  double get_speed(void);
  bool is_stopped(void);
  bool is_started(void);
  virtual bool is_finished(void);

  void set_x_speed(double x_speed);
  void set_y_speed(double y_speed);
  void set_speed(double speed);
  void stop(void);

  bool is_suspended(void);
  virtual void set_suspended(bool suspended);

  void set_direction(int direction);
  void set_direction(double angle);
};

#endif
