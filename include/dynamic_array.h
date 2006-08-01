#ifndef ZSDX_DYNAMIC_ARRAY_H
#define ZSDX_DYNAMIC_ARRAY_H

#include <cstdlib>
//#include <iostream>
//using namespace std;

template <typename T>
class DynamicArray {

 private:
  int size;
  int capacity;
  T *elements;

  void create_array();
  void destroy_array();

 public:
  DynamicArray();
  DynamicArray(const DynamicArray<T> &other);
  DynamicArray(int capacity);
  ~DynamicArray();

  inline bool is_empty(void) const { return get_size() == 0; }
  inline int get_size(void) const { return size; }
  inline int get_capacity(void) const { return capacity; }
  inline T get(int index) const { return elements[index]; }
  inline void set(int index, const T &element) { elements[index] = element; }
  void add(const T &element);
  bool remove(const T &element);

  inline bool contains(const T &element) const { return index_of(element) != -1; }
  int index_of(const T &element) const;
};

template <typename T>
DynamicArray<T>::DynamicArray() {
  elements = NULL;
  size = 0;
  capacity = 0;
}

template <typename T>
DynamicArray<T>::DynamicArray(const DynamicArray &other) {
  int i;
  T *current;

  size = other.size;
  capacity = other.capacity;

  create_array();
  current = elements;
  for (i = 0; i < size; i++, current++) {
    *current = other.get(i);
  }
}

template <typename T>
DynamicArray<T>::DynamicArray(int capacity) {
  this->size = 0;
  this->capacity = capacity;
  create_array();
}

template <typename T>
DynamicArray<T>::~DynamicArray() {
  destroy_array();
}

template <typename T>
void DynamicArray<T>::add(const T &element) {
  int i;
  T *old;

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
    old = elements;
    elements = new T[capacity];

    for (i = 0; i < size; i++) {
      elements[i] = old[i];
    }
    elements[i] = element;

    size++;
    delete [] old;
  }
}

template <typename T>
bool DynamicArray<T>::remove(const T &element) {
  int i = index_of(element);

  if (i == -1) {
    return false;
  }

  for (size--; i < size; i++) {
    elements[i] = elements[i + 1];
  }
  return true;
}

template <typename T>
int DynamicArray<T>::index_of(const T &element) const {
  bool found = false;
  int i;
  //cout << "index_of(" << element << ")" << endl;
  for (i = 0; i < size && !found; i++) {
    //cout << "get(" << i << "): " << get(i) << ", element: " << element << endl;
    found = (get(i) == element);
  }
  return (found) ? (i - 1) : -1;
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

#endif
