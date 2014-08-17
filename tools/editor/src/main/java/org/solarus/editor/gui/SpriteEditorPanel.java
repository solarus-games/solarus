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

import java.awt.*;
import java.awt.event.MouseEvent;
import java.awt.event.MouseMotionAdapter;
import javax.swing.*;

import org.solarus.editor.*;

/**
 * Component to edit a sprite.
 */
public class SpriteEditorPanel extends AbstractEditorPanel {

    /**
     * The current sprite.
     */
    private final Sprite sprite;

    /**
     * Component with the animations of the sprite.
     */
    private final SpriteAnimationsView spriteAnimationsView;

    /**
     * The sprite animation image.
     */
    private final SpriteImageView spriteImageView;

     /**
     * Creates a sprite editor.
     * @param mainWindow The main window of the quest editor.
     * @param spriteId Id of the sprite to open.
     * @throws QuestEditorException If the sprite could not be loaded.
     */
    public SpriteEditorPanel(EditorWindow mainWindow, String spriteId)
            throws QuestEditorException {
        super(mainWindow, getEditorId(spriteId));

        setLayout(new BorderLayout());

        // sprite animations and directions
        spriteAnimationsView = new SpriteAnimationsView();
        spriteAnimationsView.setMinimumSize(new Dimension(0, 0));  // Allow the splitter to hide entirely the component.

        // sprite animation image
        spriteImageView = new SpriteImageView();
        spriteImageView.setMinimumSize(new Dimension(0, 0));  // Allow the splitter to hide entirely the component.
        JScrollPane spriteImageScroller = new ViewScroller(spriteImageView, spriteImageView);
        spriteImageScroller.setAlignmentY(Component.TOP_ALIGNMENT);

        MouseCoordinates mouseCoordinates = new MouseCoordinates(spriteImageView);
        mouseCoordinates.setAlignmentX(LEFT_ALIGNMENT);

        JPanel rightPanel = new JPanel(new BorderLayout());
        rightPanel.add(spriteImageScroller, BorderLayout.CENTER);
        rightPanel.add(mouseCoordinates, BorderLayout.SOUTH);

        JSplitPane mainPanel = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT,
                spriteAnimationsView, rightPanel);
        mainPanel.setContinuousLayout(true);
        mainPanel.setDividerLocation(340);
        // we must put our main panel in another panel
        // otherwise the background color of the window is bad
        add(mainPanel);

        sprite = new Sprite(spriteId);

        if (sprite != null) {
            // select the default animation initially and its first direction
            sprite.setSelectedAnimation(sprite.getDefaultAnimationName());
            sprite.setSelectedDirectionNb(0);
        }

        // Notify the children views.
        spriteAnimationsView.setSprite(sprite);
        spriteImageView.setSprite(sprite);

    }

    /**
     * Returns the id of any sprite editor that edits the specified sprite.
     * @param spriteId Id of a sprite.
     * @return Id of an editor that edits this sprite.
     */
    public static String getEditorId(String spriteId) {
        return Project.getSpriteFile(spriteId).getAbsolutePath();
    }

    /**
     * Returns the type of resource represented by this editor.
     * @return The type of resource.
     */
    @Override
    public ResourceType getResourceType() {
        return ResourceType.SPRITE;
    }

    /**
     * Returns the id of the resource element open in the editor.
     * @return The id of the sprite.
     */
    @Override
    public String getResourceId() {
        return sprite.getId();
    }

    /**
     * This function is called when the user wants to close the current sprite.
     * If the sprite is not saved, we propose to save it.
     * @return false if the user canceled.
     */
    @Override
    public boolean checkCurrentFileSaved() {
        boolean result = true;

        if (sprite != null && !sprite.isSaved()) {
            int answer = JOptionPane.showConfirmDialog(this,
                    "Sprite '" + sprite.getName() + "' has been modified. Do you want to save it?",
                    "Save the modifications",
                    JOptionPane.YES_NO_CANCEL_OPTION,
                    JOptionPane.WARNING_MESSAGE);
            if (answer == JOptionPane.YES_OPTION) {
                save();
            } else if (answer == JOptionPane.CANCEL_OPTION) {
                result = false;
            }
        }

        return result;
    }

    /**
     * Give the name of the resource opened in the editor
     * @return the name of the map
     */
    public Sprite getSprite() {
        return sprite;
    }

    /**
     * Give the name of the resource opened in the editor
     * @return the name of the sprite
     */
    @Override
    public String getTitle() {
        return sprite == null ? "" : "Sprite " + sprite.getName();
    }

    /**
     * Saves the current resource.
     */
    @Override
    public void save() {

        try {
            sprite.save();
        } catch (QuestEditorException ex) {
            GuiTools.errorDialog("Could not save the sprite: " + ex.getMessage());
        }
    }

    /**
     * Closes this editor without confirmation.
     */
    @Override
    public void close() {
        sprite.deleteObservers();

        // Notify the children views.
        spriteAnimationsView.setSprite(null);
        spriteImageView.setSprite(null);
    }

    /**
     * A JLabel that shows mouse coordinates on a sprite image view.
     */
    private class MouseCoordinates extends JLabel {

       /**
        * Constructor.
        */
       public MouseCoordinates(final SpriteImageView imageView) {

           setCoordinates(0, 0);

           imageView.addMouseMotionListener(new MouseMotionAdapter() {

                   @Override
                   public void mouseMoved(MouseEvent e) {

                       Point coordinate = imageView.getMouseInImage(e.getX(), e.getY());
                       setCoordinates(coordinate.x, coordinate.y);
                   }

                   @Override
                   public void mouseDragged(MouseEvent e) {
                       mouseMoved(e);
                   }
               });
       }

       /**
        * Changes the coordinates shown in the label.
        * @param x the new x coordinate (will be rounded)
        * @param y the new y coordinate (will be rounded)
        */
       private void setCoordinates(int x, int y) {

           int xShown = GuiTools.round8(x);
           int yShown = GuiTools.round8(y);
           setText("(" + xShown + ", " + yShown + ")");
       }
    }
}
