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
package org.solarus.editor.gui;

import java.awt.event.MouseWheelEvent;
import java.awt.event.MouseWheelListener;
import java.util.Observer;
import javax.swing.JComponent;
import javax.swing.JScrollBar;
import javax.swing.JScrollPane;


public class ViewScroller extends JScrollPane {

    public static final String ZOOM_IN = "zoomIn";
    public static final String ZOOM_OUT = "zoomOut";

    /**
     * Constructor.
     * @param component the contained component
     */
    public ViewScroller(final JComponent component, final Observer observer) {

        super(component);

        getHorizontalScrollBar().setBlockIncrement(50);
        getHorizontalScrollBar().setUnitIncrement(10);
        getVerticalScrollBar().setBlockIncrement(50);
        getVerticalScrollBar().setUnitIncrement(10);

        // Replace the default scrolling by our custom listener.
        setWheelScrollingEnabled(false);
        addMouseWheelListener(new MouseWheelListener() {
            @Override
            public void mouseWheelMoved(MouseWheelEvent event) {
                if (event.isControlDown()) {
                    // Control + wheel: zoom.
                    if (event.getWheelRotation() > 0) {
                        observer.update(null, ZOOM_OUT);
                    }
                    else {
                        observer.update(null, ZOOM_IN);
                    }
                } else {
                    JScrollBar bar;
                    if (event.isShiftDown()) {
                        // Shift + wheel: scroll horizontally.
                        bar = getHorizontalScrollBar();
                    } else {
                        // Wheel alone: scroll vertically.
                        bar = getVerticalScrollBar();
                    }
                    int newValue = bar.getValue() + bar.getBlockIncrement()
                            * event.getUnitsToScroll();
                    bar.setValue(newValue);
                }
            }
        });
    }
}
