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
public class SpriteAnimationsView extends JPanel implements Scrollable {

    /**
     * The current sprite.
     */
    private Sprite sprite;

    /**
     * Component with a tree of animations and directions for the sprite.
     */
    private SpriteTree spriteTree;

    /**
     * Component with the properties of a sprite animation.
     */
    private final SpriteAnimationView animationView;

    /**
     * Component with the properties of a sprite animation direction.
     */
    private final SpriteAnimationDirectionView directionView;

    /**
     * The icon associated to each direction.
     */
    private ArrayList<SpriteAnimationDirectionIcon> directionIcons;

    /**
     * Constructor.
     */
    public SpriteAnimationsView() {
        super(new GridBagLayout());

        GridBagConstraints constraints = new GridBagConstraints();
        constraints.insets = new Insets(5, 5, 5, 5);

        // Tree.
        spriteTree = new SpriteTree();
        final JScrollPane treeScroller = new JScrollPane(spriteTree);
        JPanel treePanel = new JPanel();
        treePanel.setLayout(new BorderLayout());
        treePanel.setBorder(BorderFactory.createTitledBorder("Sprite sheet"));
        treePanel.add(treeScroller);

        constraints.gridx = 0;
        constraints.gridy = 0;
        constraints.weightx = 1.0;
        constraints.weighty = 0.25;
        constraints.anchor = GridBagConstraints.FIRST_LINE_START;
        constraints.fill = GridBagConstraints.BOTH;
        add(treePanel, constraints);

        // Current animation.
        animationView = new SpriteAnimationView();
        JPanel animationPanel = new JPanel(new BorderLayout());
        animationPanel.setBorder(BorderFactory.createTitledBorder("Selected animation"));
        animationPanel.add(animationView);

        constraints.gridx = 0;
        constraints.gridy = 1;
        constraints.weightx = 1.0;
        constraints.weighty = 0.0;
        constraints.anchor = GridBagConstraints.FIRST_LINE_START;
        constraints.fill = GridBagConstraints.HORIZONTAL;
        add(animationPanel, constraints);

        // Current animation direction.
        directionView = new SpriteAnimationDirectionView();
        final JScrollPane directionScroller = new JScrollPane(directionView);
        directionScroller.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_NEVER);
        JPanel directionPanel = new JPanel(new BorderLayout());
        directionPanel.setBorder(BorderFactory.createTitledBorder("Selected direction"));
        directionPanel.add(directionScroller);

        constraints.gridx = 0;
        constraints.gridy = 2;
        constraints.weightx = 1.0;
        constraints.weighty = 0.75;
        constraints.anchor = GridBagConstraints.FIRST_LINE_START;
        constraints.fill = GridBagConstraints.BOTH;
        add(directionPanel, constraints);
    }

    /**
     * Sets the sprite observed.
     * @param sprite the current sprite, or null if there is no sprite
     */
    public void setSprite(Sprite sprite) {

        if (sprite == this.sprite) {
            return;
        }

        this.sprite = sprite;
        spriteTree.setSprite(sprite);
        animationView.setSprite(sprite);
        directionView.setSprite(sprite);
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

    @Override
    public Dimension getPreferredScrollableViewportSize() {
        return getPreferredSize();
    }

    @Override
    public int getScrollableUnitIncrement(Rectangle rctngl, int i, int i1) {
        return 16;
    }

    @Override
    public int getScrollableBlockIncrement(Rectangle rctngl, int i, int i1) {
        return 160;
    }

    @Override
    public boolean getScrollableTracksViewportWidth() {
        return true;
    }

    @Override
    public boolean getScrollableTracksViewportHeight() {
        return true;
    }

}
