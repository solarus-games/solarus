#ifndef ZSDX_DYNAMIC_ARRAY_H
#define ZSDX_DYNAMIC_ARRAY_H

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
  DynamicArray(int size);
  DynamicArray(int size, int capacity);
  ~DynamicArray();

  inline bool is_empty(void) { return get_size() == 0; }
  inline int get_size(void) { return size; }
  inline int get_capacity(void) { return capacity; }
  inline T get(int index) { return elements[index]; }
  inline void set(int index, T &element) { elements[index] = element; }
  void add(T &element);
};

#endif
