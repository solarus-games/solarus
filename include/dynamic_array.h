#ifndef ZSDX_DYNAMIC_ARRAY_H
#define ZSDX_DYNAMIC_ARRAY_H

#include <cstdlib>

/* Generic class for a dynamic array of anything
 */
template <typename T>
class DynamicArray {

 private:
  /* Number of elements in the dynamic array
   */
  int size;

  /* Number of allocated slots in the internal array
   */
  int capacity;

  /* Internal storage of the elements
   */
  T *elements;

  /* Create the internal array
   */
  void create_array();

  /* Destroy the internal array
   */
  void destroy_array();

 public:
  /* Create an empty dynamic array
   */
  DynamicArray();

  /* Copy constructor
   */
  DynamicArray(const DynamicArray<T> &other);

  /* Create an empty dynamic array with the given capacity
   */
  DynamicArray(int capacity);

  /* Destructor
   */
  ~DynamicArray();

  /* Return whether the dynamic array is empty
   */
  inline bool is_empty(void) const { return get_size() == 0; }

  /* Return the number of elements
   */
  inline int get_size(void) const { return size; }

  /* Return the capacity of the internal array
   */
  inline int get_capacity(void) const { return capacity; }

  /* Return an element
   */
  inline T get(int index) const { return elements[index]; }

  /* Set an element
   */
  inline void set(int index, const T &element) { elements[index] = element; }

  /* Add an element at the end of the dynamic array
   * The internal array is expanded if needed.
   */
  void add(const T &element);

  /* Remove an element from the dynamic array
   * Warning: this operation is quite expensive because
   * the data after this element are shift
   */
  bool remove(const T &element);

  /* Remove everything
   * The capacity is unchanged.
   */
  void clear(void);

  /* Search an element in the dynamic array
   */
  inline bool contains(const T &element) const { return index_of(element) != -1; }

  /* Search the location of the first occurence of an element
   * Return -1 if the element is not found.
   */
  int index_of(const T &element) const;
};

template <typename T>
DynamicArray<T>::DynamicArray() {
  elements = NULL;
  size = 0;
  capacity = 0;
  // the internal array will be created by create_array() when needed
}

template <typename T>
DynamicArray<T>::DynamicArray(const DynamicArray &other) {
  int i;
  T *current;

  size = other.size;
  capacity = other.capacity;
  create_array(); // we create the internal array with the same capacity

  current = elements;
  for (i = 0; i < size; i++, current++) {
    *current = other.get(i); // we copy every element
  }
}

template <typename T>
DynamicArray<T>::DynamicArray(int capacity) {
  this->size = 0;
  this->capacity = capacity;
  create_array(); // we create the internal array with the wanted capacity
}

template <typename T>
DynamicArray<T>::~DynamicArray() {
  destroy_array();
}

template <typename T>
void DynamicArray<T>::add(const T &element) {
  int i;
  T *old;

  if (size < capacity) { // very easy if the array is not full
    set(size, element);
    size++;
  }
  else if (size == 0) { // particular case
    capacity = 16;      // the internal array is not initialized yet
    create_array();
    set(0, element);
    size = 1;
  }
  else {
    // unfortunately we must increase the array size
    capacity *= 2;
    old = elements;
    elements = new T[capacity];

    for (i = 0; i < size; i++) {
      elements[i] = old[i];
    }
    elements[i] = element;

    size++;
    delete[] old;
  }
}

template <typename T>
bool DynamicArray<T>::remove(const T &element) {
  int i = index_of(element);

  if (i == -1) {
    return false; // the element was not found
  }

  for (size--; i < size; i++) { // we shift the last elements
    elements[i] = elements[i + 1];
  }
  return true;
}

template <typename T>
void DynamicArray<T>::clear(void) {
  size = 0; // the capacity is unchanged
}

template <typename T>
int DynamicArray<T>::index_of(const T &element) const {
  bool found = false;
  int i;
  for (i = 0; i < size && !found; i++) {
    found = (get(i) == element);
  }
  return (found) ? (i - 1) : -1;
}

template <typename T>
void DynamicArray<T>::create_array() {
  if (capacity == 0) {
    elements = NULL; // don't create an array of 0 elements
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
