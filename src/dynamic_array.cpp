#include <cstdlib>
#include "dynamic_array.h"

// TODO: assertions

template <typename T>
DynamicArray<T>::DynamicArray() {
  elements = NULL;
  size = 0;
  capacity = 0;
}

template <typename T>
DynamicArray<T>::DynamicArray(const DynamicArray &other) {
  int i;
  T *current = elements;

  size = other.size;
  capacity = other.capacity;

  create_array();
  for (i = 0; i < size; i++, current++) {
    *current = other.get(i);
  }
}

template <typename T>
DynamicArray<T>::DynamicArray(int size) {
  this->size = size;
  capacity = size;
  create_array();
}

template <typename T>
DynamicArray<T>::DynamicArray(int size, int capacity) {
  this->size = size;
  this->capacity = capacity;
  create_array();
}

template <typename T>
DynamicArray<T>::~DynamicArray() {
  destroy_array();
}

template <typename T>
void DynamicArray<T>::add(T &element) {
  if (size < capacity) {
    set(size, element);
    size++;
  }
  else if (size == 0) { // particular case
    capacity = 16;
    create_array();
    set(0, element);
    size = 1;
  }
  else {
    // we must increase the array size
    capacity *= 2;
    T *old = elements;
    elements = new T[capacity];
    set(size, element);
    size++;
  }
}

template <typename T>
void DynamicArray<T>::create_array() {
  if (capacity == 0) {
    elements = NULL;
  }
  else {
    elements = new T[capacity];
  }
}

template <typename T>
void DynamicArray<T>::destroy_array() {
  if (capacity > 0) {
    delete [] elements;
  }
}

