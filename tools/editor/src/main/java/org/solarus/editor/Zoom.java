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

import java.util.NoSuchElementException;

/**
 * Indicates the zoom value in a grapĥic view.
 */
public enum Zoom {

    VERY_SMALL(0.25, "25%"),
    SMALL(0.5, "50%"),
    NORMAL(1.0, "100%"),
    BIG(2.0, "200%"),
    VERY_BIG(4.0, "400%");

    private final double value;
    private final String label;

    /**
     * Constructor.
     * @param value the value of zoom
     * @param label the label of zoom
     */
    private Zoom(double value, String label) {

      this.value = value;
      this.label = label;
    }

    /**
     * Retuns the value of this zoom.
     * @return the zoom value
     */
    public double getValue() {

        return value;
    }

    /**
     * Retunrs the label of this zoom.
     * @return the zoom label
     */
    public String getLabel() {

        return label;
    }

    /**
     * Returns the corresponding zoom of a given value.
     * @param value the value of wanted zoom
     * @return the found zoom.
     */
    public static Zoom get(double value) {

        for (Zoom zoom: values()) {
            if (zoom.value == value) {
                return zoom;
            }
        }
        throw new NoSuchElementException("No zoom " + value);
    }

    /**
     * Returns the index of this zoom.
     * @return the index of this zoom
     */
    public int getIndex() {

        Zoom zooms[] = values();
        for (int i = 0; i < zooms.length; i++) {
            if (zooms[i] == this) {
                return i;
            }
        }
        return -1;
    }

    /**
     * Check if is the first zoom.
     * @return true if this zoom is the first, false otherwise
     */
    public boolean isFirstValue() {

        return this == values()[0];
    }

    /**
     * Check if is the last zoom.
     * @return true is this zoom is the last, false otherwise
     */
    public boolean isLastValue() {

        Zoom zooms[] = values();
        return this == zooms[zooms.length - 1];
    }

    /**
     * Returns the previous zoom.
     * @return the previous zoom or null if doesn't exists
     */
    public Zoom getPrevious() {

        Zoom previousZoom = null;

        for (Zoom zoom: values()) {
            if (zoom == this) {
                return previousZoom;
            }
            previousZoom = zoom;
        }
        return null;
    }

    /**
     * Returns the next zoom.
     * @return the next zoom or null if doesn't exists
     */
    public Zoom getNext() {

        Zoom previousZoom = null;

        for (Zoom zoom: values()) {
            if (previousZoom != null && previousZoom == this) {
                return zoom;
            }
            previousZoom = zoom;
        }
        return null;
    }
}
