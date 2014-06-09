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

import java.util.*;
import javax.swing.JComboBox;
import org.solarus.editor.*;

/**
 * A combo box component to select an animation from the sprite.
 */
class SpriteAnimationChooser extends JComboBox<String> implements Observer {

    /**
     * The sprite observed.
     */
    private Sprite sprite;

    /**
     * Constructor.
     * @param sprite the sprite observed
     */
    public SpriteAnimationChooser(Sprite sprite) {

        sprite = null;
        setSprite(sprite);
    }

    /**
     * Loads the list of animations.
     */
    protected void buildList() {

        if (sprite == null) {
            return;
        }

        for (String name: sprite.getAnimationNames()) {
            addItem(name);
        }
    }

    /**
     * Reloads the list of animations.
     * After the list is reloaded, the same animation is selected again if it still exists.
     */
    protected void reloadList() {

        String selectedName = sprite != null ? sprite.getSelectedAnimationName() : "";

        removeAllItems();
        buildList();

        if (!selectedName.isEmpty()) {
            try {
                setSelected(selectedName);
            }
            catch (NoSuchElementException ex) {
                // The element no longer exists, nevermind.
            }
        }
    }

    /**
     * Sets the sprite observed.
     * @param sprite the sprite
     */
    public void setSprite(Sprite sprite) {

        if (this.sprite != sprite) {
            if (this.sprite != null) {
                this.sprite.deleteObserver(this);
            }

            this.sprite = sprite;
            reloadList();

            if (sprite != null) {
                sprite.addObserver(this);
            }
            update(sprite, null);
        }
    }

    /**
     * Returns the name of the currently selected animation.
     * @return the name of the selected animation
     */
    public String getSelected() {

        String selectedName = (String) getSelectedItem();
        return selectedName == null ? "" : selectedName;
    }

    /**
     * Changes the currently selected animation.
     * If the specified animation doesn't exist, a NoSuchElementException is raised.
     * @param animationName name of the animation you want to make selected in the combo box
     * @throws NoSuchElementException if the name specified does not exist in the combo box
     */
    public void setSelected(String animationName) throws NoSuchElementException {

        setSelectedItem(animationName);

        if (!animationName.isEmpty() && !getSelected().equals(animationName)) {
          throw new NoSuchElementException("No animation :'" + animationName + "' in the list");
        }
    }

    /**
     * This function is called when the sprite is changed.
     * @param o the sprite, or null if no sprite is set
     * @param obj not used
     */
    @Override
    public void update(Observable o, Object obj) {

        if (sprite == null) {
            removeAllItems();
        } else {
            // add, remove or rename animation
            if (obj instanceof String || obj instanceof SpriteAnimation) {
                reloadList();
            } else {
                setSelected(sprite.getSelectedAnimationName());
            }
        }
    }
}
