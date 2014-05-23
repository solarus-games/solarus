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
import java.awt.event.*;
import java.util.*;
import javax.swing.*;
import javax.swing.event.*;
import org.solarus.editor.*;

/**
 * This components shows information about animations of a sprite.
 */
public class SpriteAnimationsView extends JPanel {

    /**
     * The current sprite.
     */
    private Sprite sprite;

    /**
     * Component with the properties of the sprite.
     */
    private final SpritePropertiesView spritePropertiesView;

    /**
     * Component with the properties of the sprite animation.
     */
    private final SpriteAnimationView spriteAnimationView;

    /**
     * The directions list.
     */
    private final JList<SpriteAnimationDirection> spriteAnimationDirectionsList;

    /**
     * The directions list pattern model.
     */
    private final SpriteAnimationDirectionsListModel spriteAnimationDirectionsListModel;

    /**
     * Component with the properties of the sprite animation direction.
     */
    private final SpriteAnimationDirectionView spriteAnimationDirectionView;

    /**
     * The icon associated to each direction.
     */
    private ArrayList<SpriteAnimationDirectionIcon> directionIcons;

    /**
     * Constructor.
     */
    public SpriteAnimationsView() {
        super();

        directionIcons = new ArrayList<SpriteAnimationDirectionIcon>();

        setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
        setBorder(BorderFactory.createEmptyBorder(5, 5, 5, 5));

        // sprite properties
        spritePropertiesView = new SpritePropertiesView();
        spritePropertiesView.setMaximumSize(new Dimension(Integer.MAX_VALUE, 120));
        spritePropertiesView.setAlignmentX(Component.LEFT_ALIGNMENT);

        // animation
        spriteAnimationView = new SpriteAnimationView();
        spriteAnimationView.setMaximumSize(new Dimension(Integer.MAX_VALUE, 120));
        spriteAnimationView.setAlignmentX(Component.LEFT_ALIGNMENT);

        // list
        spriteAnimationDirectionsListModel = new SpriteAnimationDirectionsListModel();
        spriteAnimationDirectionsList =
                new JList<SpriteAnimationDirection>(spriteAnimationDirectionsListModel);
        spriteAnimationDirectionsList.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
        spriteAnimationDirectionsList.setLayoutOrientation(JList.HORIZONTAL_WRAP);
        spriteAnimationDirectionsList.setVisibleRowCount(-1); // make the rows as wide as possible
        spriteAnimationDirectionsList.getSelectionModel().addListSelectionListener(
                new SpriteAnimationDirectionsListSelectionListener());
        spriteAnimationDirectionsList.setCellRenderer(new SpriteAnimationDirectionsListRenderer());

        spriteAnimationDirectionsList.addKeyListener(new KeyAdapter() {
                @Override
                public void keyPressed(KeyEvent keyEvent) {
                    if (keyEvent.getKeyCode() == KeyEvent.VK_DELETE) {
                        if (sprite != null && sprite.getSelectedDirection() != null) {
                            try {
                                sprite.removeDirection();
                            } catch (SpriteException ex) {
                                GuiTools.errorDialog("Cannot remove direction: " + ex.getMessage());
                            }
                        }
                    }
                }
            });

        JScrollPane listScroller = new JScrollPane(spriteAnimationDirectionsList);
        listScroller.setMaximumSize(new Dimension(Integer.MAX_VALUE, Integer.MAX_VALUE));
        listScroller.setAlignmentX(Component.LEFT_ALIGNMENT);

        // animation direction
        spriteAnimationDirectionView = new SpriteAnimationDirectionView();
        spriteAnimationDirectionView.setMaximumSize(new Dimension(Integer.MAX_VALUE, 120));
        spriteAnimationDirectionView.setAlignmentX(Component.LEFT_ALIGNMENT);

        add(spritePropertiesView);
        add(spriteAnimationView);
        add(listScroller);
        add(spriteAnimationDirectionView);
    }

    /**
     * Sets the sprite observed.
     * @param sprite the current sprite, or null if there is no sprite
     */
    public void setSprite(Sprite sprite) {

        if (sprite == this.sprite) {
            return;
        }

        if (this.sprite != null) {
            this.sprite.addObserver(spriteAnimationDirectionsListModel);
        }

        this.sprite = sprite;
        spritePropertiesView.setSprite(sprite);
        spriteAnimationView.setSprite(sprite);
        spriteAnimationDirectionView.setSprite(sprite);

        if (sprite != null) {
            sprite.addObserver(spriteAnimationDirectionsListModel);

            loadIcons();
            spriteAnimationDirectionsListModel.update(sprite, null);
        }
        else {
            repaint();
        }
    }

    /**
     * Loads the icons for the sprite animation direction list.
     */
    private void loadIcons() {

        directionIcons.clear();

        SpriteAnimation animation = sprite.getSelectedAnimation();
        if (animation != null) {
            for (int i = 0; i < animation.getNbDirections(); i++) {
                SpriteAnimationDirection direction = animation.getDirection(i);
                directionIcons.add(new SpriteAnimationDirectionIcon(direction, sprite));
            }
        }
    }

    /**
     * List selection listener associated to the animation directions list.
     * When the user selects a direction, it becomes selected also in the sprite.
     */
    private class SpriteAnimationDirectionsListSelectionListener implements ListSelectionListener {

        /**
         * This function is called when the selection is changed.
         */
        @Override
        public void valueChanged(ListSelectionEvent e) {

            // get the direction just selected in the list
            int selectedDirection = spriteAnimationDirectionsList.getSelectedIndex();

            // select this direction in the sprite
            if (selectedDirection != -1) {
                try {
                    sprite.setSelectedDirectionNb(selectedDirection);
                } catch (SpriteException ex) {
                    GuiTools.errorDialog("Cannot select direction: " + ex.getMessage());
                }
            }
            else {
                sprite.unselectDirection();
            }
        }
    }

    /**
     * List model for the animation directions list.
     */
    private class SpriteAnimationDirectionsListModel
        extends AbstractListModel<SpriteAnimationDirection> implements Observer {

        /**
         * Returns the number of directions.
         */
        @Override
        public int getSize() {
            int size = 0;

            if (sprite != null) {
                SpriteAnimation animation = sprite.getSelectedAnimation();
                if (animation != null) {
                    size = animation.getNbDirections();
                }
            }

            return size;
        }

        /**
         * Returns the a component of the specified direction.
         */
        @Override
        public SpriteAnimationDirection getElementAt(int rank) {

            SpriteAnimationDirection direction = null;
            if (sprite != null) {
                SpriteAnimation animation = sprite.getSelectedAnimation();
                if (animation != null) {
                    direction = animation.getDirection(rank);
                }
            }
            return direction;
        }

        /**
         * This function is called when the sprite changes.
         * @param o the sprite
         * @param params information about what has changed:
         *   - a SpriteAnimation: indicates that this animation has just been
         *     created or has changed
         *   - a string: indicates that the animation with this name has just been removed
         *   - an Integer: indicates that the direction of the selected animation with
         * this number has just been removed
         *   - null: other cases
         */
        @Override
        public void update(Observable o, Object params) {

            loadIcons();

            // update the enabled state of the buttons
            int spriteSelectedDirectionNb = sprite.getSelectedDirectionNb();
            int listSelectedRank = spriteAnimationDirectionsList.getSelectedIndex();

            if (sprite.getSelectedDirection() != null) {
                // an existing direction is selected
                // make this direction selected in the list
                if (spriteSelectedDirectionNb != listSelectedRank) {
                    spriteAnimationDirectionsList.setSelectedIndex(spriteSelectedDirectionNb);
                    spriteAnimationDirectionsList.ensureIndexIsVisible(spriteSelectedDirectionNb);
                }
            }
            else if (sprite.getSelectedDirection() == null) {
                // no direction is selected anymore
                spriteAnimationDirectionsList.removeSelectionInterval(listSelectedRank, listSelectedRank);
            }
            // redraw the table
            fireContentsChanged(this, 0, getSize() - 1);

            // notify the animation direction view
            spriteAnimationDirectionView.setSelectedDirection(sprite.getSelectedDirectionNb());
        }
    }

    /**
     * This class defines how the list elements are displayed.
     */
    private class SpriteAnimationDirectionsListRenderer implements ListCellRenderer<SpriteAnimationDirection> {

        /**
         * Returns a component representing a tile.
         */
        @Override
        public Component getListCellRendererComponent(
                JList<? extends SpriteAnimationDirection> list, SpriteAnimationDirection value,
                int rank, boolean isSelected, boolean cellHasFocus) {

            if (rank >= directionIcons.size()) {
                // the icon doesn't exist yet
                loadIcons();
            }
            return directionIcons.get(rank);
        }
    }
}
