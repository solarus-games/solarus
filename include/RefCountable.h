/*
 * Copyright (C) 2006-2014 Christopho, Solarus - http://www.solarus-games.org
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
#ifndef SOLARUS_REF_COUNTABLE_H
#define SOLARUS_REF_COUNTABLE_H

#include "Common.h"

namespace solarus {

/**
 * \brief An object with an optional reference counting system.
 *
 * Note that if you don't need the refcount system, you don't have to use it.
 * You can just leave the refcount to zero and delete the object normally.
 * You don't need to care about refcounts if your object is not allocated
 * dynamically or if you don't share pointers.
 */
class RefCountable {

  public:

    RefCountable();
    virtual ~RefCountable();

    // Reference counting.
    int get_refcount() const;
    void increment_refcount();
    void decrement_refcount();
    static void ref(RefCountable* object);
    static void unref(RefCountable* object);

    template <typename T>
    static std::shared_ptr<T> make_refcount_ptr(T* pointer);

  private:

    int refcount;                /**< Number of pointers to the object
                                  * (0 means that it can be deleted). */

};

/**
 * \brief Returns an std::shared_ptr with a custom deleter that calls
 * RefCountable::unref() instead of deleting the object.
 *
 * TODO This is a temporary utility function to ease the transition from
 * RefCountable to shared_ptr.
 * Once everyone uses shared pointers instead of calling ref/unref manually
 * on raw pointers, we can remove the RefCountable class, remove this function
 * and use std::make_shared() with the standard deleter,
 * provided that this function is never called twice on the same object.
 *
 * \param pointer The raw pointer to share.
 * \return The shared pointer created.
 */
template <typename T>
std::shared_ptr<T> RefCountable::make_refcount_ptr(T* pointer) {

  // The shared_ptr counts for 1 in our refcount system.

  RefCountable::ref(pointer);
  return std::shared_ptr<T>(pointer, [](T* pointer) {
    RefCountable::unref(pointer);
  });

}

}

#endif

