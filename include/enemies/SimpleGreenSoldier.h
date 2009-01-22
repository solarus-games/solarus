#ifndef ZSDX_SIMPLE_GREEN_SOLDIER_H
#define ZSDX_SIMPLE_GREEN_SOLDIER_H

#include "Common.h"
#include "entities/Enemy.h"

class SimpleGreenSoldier: public Enemy {

 public:

  SimpleGreenSoldier(const ConstructionParameters &params);
  ~SimpleGreenSoldier(void);

  void initialize(void);
  void update(void);

  void walk(int direction);
};

#endif
