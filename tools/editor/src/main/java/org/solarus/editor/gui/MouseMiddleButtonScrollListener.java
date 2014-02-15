package org.solarus.editor.gui;

import java.awt.Point;
import java.awt.event.InputEvent;
import java.awt.event.MouseEvent;

import javax.swing.JComponent;
import javax.swing.JViewport;
import javax.swing.event.MouseInputAdapter;

/**
 * Moves a component inside a viewport by dragging it with the middle mouse button.
 */
public class MouseMiddleButtonScrollListener extends MouseInputAdapter {

    /**
     * Initial position of the cursor when dragging the view.
     */
    private Point dragInitialPoint;

    /**
     * The component to apply the scrolling to.
     * The parent of this component should be a JViewport.
     */
    private JComponent component;

    /**
     * Constructor.
     * @param component The component to apply the scrolling to.
     * The parent of this component should be a JViewport.
     */
    public MouseMiddleButtonScrollListener(JComponent component) {
        this.dragInitialPoint = new Point();
        this.component = component;
    }

    /**
     * Creates and installs middle button dragging on a component.
     * @param component The component to apply the scrolling to.
     */
    public static void install(JComponent component) {

        MouseMiddleButtonScrollListener listener =
                new MouseMiddleButtonScrollListener(component);
        component.addMouseListener(listener);
        component.addMouseMotionListener(listener);
    }

    /**
     * This method is called when the mouse is pressed on the component.
     */
    public void mousePressed(MouseEvent mouseEvent) {

        // detect the mouse button
        int button = mouseEvent.getButton();
        if (button == MouseEvent.BUTTON2) {
            // Middle button: drag the view
            dragInitialPoint = mouseEvent.getPoint();
        }
    }

    /**
     * This function is called when the mouse is dragged on the component.
     * In State.NORMAL, when no entity was clicked, a selection rectangle appears.
     * In State.SELECTING_AREA, the selection rectangle is updated.
     * In State.RESIZING_ENTITY, the rectangle of the tile is updated.
     */
    public void mouseDragged(MouseEvent mouseEvent) {

        boolean middleDrag = (mouseEvent.getModifiersEx() & InputEvent.BUTTON2_DOWN_MASK) != 0;

        if (middleDrag) {
            int x = mouseEvent.getX();
            int y = mouseEvent.getY();

            if (component.getParent() instanceof JViewport) {
                // We are in a viewport: drag the view with the middle mouse button.
                JViewport viewport = (JViewport) component.getParent();
                viewport.setViewPosition(new Point(
                        dragInitialPoint.x - component.getX() - x,
                        dragInitialPoint.y - component.getY() - y
                ));

                // Make sure we stay in the bounds of the MapView.
                viewport.scrollRectToVisible(component.getBounds());
            }
        }
    }
}
