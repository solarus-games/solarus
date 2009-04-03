#include "movements/RandomWalkMovement.h"
#include "Random.h"
using namespace std;

const string RandomWalkMovement::random_directions[] = {"0", "2", "4", "6"};

/**
 * Creates a random walk movement object.
 * @param map the map
 * @param speed speed of the movement
 */
RandomWalkMovement::RandomWalkMovement(Map *map, int speed):
  PathMovement(map, get_random_direction(), speed, false, true) {

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
string RandomWalkMovement::get_random_direction() {

  string c = random_directions[Random::get_number(4)];
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
