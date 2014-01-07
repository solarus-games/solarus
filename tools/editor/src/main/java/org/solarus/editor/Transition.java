/*
 * Copyright (C) 2006-2014 Christopho, Solarus - http://www.solarus-games.org
 *
 * Solarus Quest Editor is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Solarus Quest Editor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
package org.solarus.editor;
import java.util.*;

/**
 * Indicates a type of transition between two maps.
 */
public enum Transition {

    IMMEDIATE("immediate"),
    FADE("fade"),
    SCROLLING("scrolling");

    public static final String[] humanNames = {
      "Immediate",
      "Fade in/out",
      "Scrolling"
    };

    private final String id;

    private Transition(String id) {
      this.id = id;
    }

    public String getId() {
        return id;
    }

    public static Transition get(String id) {
        for (Transition transition: values()) {
            if (transition.getId().equals(id)) {
                return transition;
            }
        }
        throw new NoSuchElementException(
                "No transition style with name '" + id + "'");
    }
}

