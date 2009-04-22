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
#include "movements/RandomWalkMovement.h"
#include "Random.h"
using namespace std;

const string RandomWalkMovement::random_directions[] = {"0", "2", "4", "6"};

/**
 * Creates a random walk movement object.
 * @param speed speed of the movement
 */
RandomWalkMovement::RandomWalkMovement(int speed):
  PathMovement(get_random_direction(), speed, false, true) {

}

/**
 * Destructor.
 */
RandomWalkMovement::~RandomWalkMovement(void) {

}

/**
 * Returns one of the four main directions.
 * @return a random direction
 */
const string RandomWalkMovement::get_random_direction() {

  const string &c = random_directions[Random::get_number(4)];
  int length = Random::get_number(5) + 3;
  string path = "";
  for (int i = 0; i < length; i++) {
    path += c;
  }

  return path;
}

/**
 * Updates the movements: detects the collisions
 * in order to restart the movement.
 */
void RandomWalkMovement::update(void) {

  PathMovement::update();

  if (!suspended && is_stopped()) {
    // there was a collision
    set_speed(initial_speed);
    start_next_move();
  }
}

/**
 * Starts the next step of the movement.
 */
void RandomWalkMovement::start_next_move(void) {

  if (remaining_path.size() == 0) {
    remaining_path = get_random_direction();
  }

  PathMovement::start_next_move();
}

/**
 * Returns whether the movement is finished.
 * @return false
 */
bool RandomWalkMovement::is_finished(void) {
  return false;
}
