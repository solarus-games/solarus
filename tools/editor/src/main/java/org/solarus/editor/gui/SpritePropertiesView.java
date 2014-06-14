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
import javax.swing.*;
import java.util.*;
import org.solarus.editor.*;

/**
 * This components shows information about a sprite.
 */
public class SpritePropertiesView extends JPanel
        implements Observer, ProjectObserver {

    /**
     * The sprite observed.
     */
    private Sprite sprite;

    // components
    private final JLabel spriteIdView;
    private final SpriteNameView spriteNameView;

    /**
     * Constructor.
     */
    public SpritePropertiesView() {
        super(new GridBagLayout());

        setBorder(BorderFactory.createTitledBorder("Sprite properties"));

        GridBagConstraints constraints = new GridBagConstraints();
        constraints.insets = new Insets(5, 5, 5, 5); // margins
        constraints.anchor = GridBagConstraints.LINE_START; // alignment of the components
        constraints.fill = GridBagConstraints.HORIZONTAL;

        // tileset id
        constraints.gridx = 0;
        constraints.gridy = 0;
        add(new JLabel("Sprite id"), constraints);

        // tileset name
        constraints.gridy++;
        add(new JLabel("Sprite name"), constraints);

        constraints.weightx = 1;
        constraints.gridx = 1;
        constraints.gridy = 0;
        spriteIdView = new JLabel();
        add(spriteIdView, constraints);

        constraints.gridy++;
        spriteNameView = new SpriteNameView();
        add(spriteNameView, constraints);
    }

    /**
     * Sets the sprite observed.
     * @param sprite the sprite
     */
    public void setSprite(Sprite sprite) {
        if (this.sprite != null) {
            this.sprite.deleteObserver(this);
            Project.removeProjectObserver(this);
        }

        this.sprite = sprite;

        if (sprite != null) {
            sprite.addObserver(this);
            Project.addProjectObserver(this);
        }
        update(sprite, null);
    }

    /**
     * This function is called when the sprite is changed.
     * @param o the sprite, or null if no sprite is set
     * @param obj not used
     */
    @Override
    public void update(Observable o, Object obj) {

        // update the elementary components here
        if (sprite == null) {
            spriteIdView.setText("");
        }
        else {
            spriteIdView.setText(sprite.getId());
        }

        // tell the complex components to update themselves
        spriteNameView.update(sprite);
    }

    /**
     * Component to change the name of the sprite.
     */
    private class SpriteNameView extends JPanel {

        // subcomponents
        private JTextField textFieldName;
        private final JButton buttonSet;

        /**
         * Constructor.
         */
        public SpriteNameView() {
            super();
            setLayout(new BoxLayout(this, BoxLayout.X_AXIS));

            textFieldName = new JTextField(10);

            Dimension size = new Dimension(120, 25);
            textFieldName.setMinimumSize(size);

            buttonSet = new JButton("Set");

            textFieldName.setEnabled(false);
            buttonSet.setEnabled(false);

            ActionListener listener = new ActionListener() {
                    @Override
                    public void actionPerformed(ActionEvent ev) {

                        try {
                            String name = textFieldName.getText();
                            sprite.setName(name);
                        }
                        catch (QuestEditorException ex) {
                            GuiTools.errorDialog("Cannot change the tileset name: " + ex.getMessage());
                        }
                    }
                };

            buttonSet.addActionListener(listener);
            textFieldName.addActionListener(listener);

            add(textFieldName);
            add(Box.createRigidArea(new Dimension(5, 0)));
            add(buttonSet);
        }

        /**
         * This function is called when the sprite is changed.
         * The component is updated.
         * @param o the tileset, or null if no tileset is loaded
         */
        public void update(Observable o) {

            if (o == null) {
                textFieldName.setEnabled(false);
                buttonSet.setEnabled(false);
                textFieldName.setText(null);
            }
            else {
                textFieldName.setEnabled(true);
                buttonSet.setEnabled(true);
                textFieldName.setText(sprite.getName());
            }
        }
    }

    @Override
    public void currentProjectChanged() {
    }

    @Override
    public void resourceElementAdded(ResourceType resourceType, String id) {
    }

    @Override
    public void resourceElementRemoved(ResourceType resourceType, String id) {
    }

    @Override
    public void resourceElementMoved(ResourceType resourceType, String oldId,
            String newId) {
    }

    @Override
    public void resourceElementRenamed(ResourceType resourceType, String id,
            String name) {
        // This sprite has just been renamed.
        if (resourceType == ResourceType.SPRITE && id.equals(sprite.getId())) {
            update(null, null);
        }
    }
}
