package org.solarus.editor.gui;

import java.awt.event.MouseWheelEvent;
import java.awt.event.MouseWheelListener;

import javax.swing.JScrollBar;
import javax.swing.JScrollPane;

/**
 * The scroll pane of the map view.
 * It allows the scroll the map view vertically with the mouse wheel,
 * horizontally with shift + mouse wheel,
 * and to zoom with control + mouse wheel.
 */
public class MapViewScroller extends JScrollPane {

    public MapViewScroller(final MapView mapView) {
        super(mapView);

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
                        mapView.getViewSettings().zoomOut();
                    }
                    else {
                        mapView.getViewSettings().zoomIn();
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
