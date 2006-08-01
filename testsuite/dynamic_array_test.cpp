#include <cassert>
#include "dynamic_array.h"
#include <iostream>

int main(int argc, char **argv) {
  DynamicArray<int> tab(5);

  assert(tab.get_size() == 0);
  assert(tab.get_capacity() == 5);
  assert(tab.is_empty());
  assert(!tab.contains(1));

  tab.add(4);
  assert(tab.contains(4));
  assert(!tab.is_empty());
  assert(tab.get(0) == 4);
  assert(tab.get_size() == 1);
  assert(tab.get_capacity() == 5);

  tab.add(10);
  tab.add(10);
  tab.add(30);
  tab.add(40);

  assert(tab.contains(10));
  assert(tab.get(1) == 10);
  assert(tab.get(2) == 10);
  assert(tab.get(3) == 30);
  assert(tab.get(4) == 40);
  assert(tab.index_of(4) == 0);
  assert(tab.index_of(10) == 1);
  assert(tab.index_of(30) == 3);
  assert(tab.index_of(99) == -1);
  assert(tab.get_size() == 5);
  assert(tab.get_capacity() == 5);

  tab.add(7);
  assert(tab.contains(7));
  assert(tab.get(5) == 7);
  assert(tab.get_size() == 6);
  assert(tab.get_capacity() == 10);

  assert(tab.remove(30));
  assert(!tab.contains(30));
  assert(tab.get_size() == 5);
  assert(tab.get_capacity() == 10);
  assert(tab.get(0) == 4);
  assert(tab.get(1) == 10);
  assert(tab.get(2) == 10);
  assert(tab.get(3) == 40);
  assert(tab.get(4) == 7);

  tab.remove(10);
  assert(tab.contains(10));
  assert(tab.get(0) == 4);
  assert(tab.get(1) == 10);
  assert(tab.get(2) == 40);
  assert(tab.get(3) == 7);
  assert(tab.get_size() == 4);
  assert(tab.get_capacity() == 10);

  tab.set(1, 100);
  assert(!tab.contains(10));
  assert(tab.contains(100));
  assert(tab.get(0) == 4);
  assert(tab.get(1) == 100);
  assert(tab.get(2) == 40);
  assert(tab.get(3) == 7);
  assert(tab.get_size() == 4);
  assert(tab.get_capacity() == 10);

  DynamicArray<int> *tab2 = new DynamicArray<int>(tab);

  assert(tab2->contains(100));
  assert(tab2->get(0) == 4);
  assert(tab2->get(1) == 100);
  assert(tab2->get(2) == 40);
  assert(tab2->get(3) == 7);
  assert(tab2->get_size() == 4);
  assert(tab2->get_capacity() == 10);

  delete tab2;
  return 0;
}
