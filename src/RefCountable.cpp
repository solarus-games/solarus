/*
 * Copyright (C) 2006-2013 Christopho, Solarus - http://www.solarus-games.org
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
#include "RefCountable.h"
#include "lowlevel/Debug.h"
#include <sstream>

namespace solarus {

/**
 * \brief Creates a refcountable object.
 */
RefCountable::RefCountable():
  refcount(0) {

}

/**
 * \brief Destroys this refcountable object.
 */
RefCountable::~RefCountable() {

  if (refcount != 0) {
    std::ostringstream oss;
    oss << "This object is still used somewhere else: refcount is " << refcount;
    Debug::die(oss.str());
  }
}


/**
 * \brief Returns the current refcount of this object.
 *
 * You must not delete this object if its refcount is not zero.
 *
 * \return the current refcount of this object
 */
int RefCountable::get_refcount() const {
  return refcount;
}

/**
 * \brief Increments the refcount of this object.
 */
void RefCountable::increment_refcount() {

  refcount++;
}

/**
 * \brief Decrements the refcount of this object.
 */
void RefCountable::decrement_refcount() {

  Debug::check_assertion(refcount > 0, "The refcount is already 0");
  refcount--;
}

/**
 * \brief Inccrements the refcount of an object.
 * \param object Pointer to the object to increment_refcount. Cannot be NULL.
 */
void RefCountable::ref(RefCountable* object) {

  object->increment_refcount();
}

/**
 * \brief Decrements the refcount of an object and destroys it if necessary.
 * \param object Pointer to the object to decrement_refcount. If the pointer is NULL,
 * nothing happens.
 */
void RefCountable::unref(RefCountable* object) {

  if (object == NULL) {
    // No-op. This is allowed.
    return;
  }

  object->decrement_refcount();
  if (object->get_refcount() == 0) {
    delete object;
  }
}

}

