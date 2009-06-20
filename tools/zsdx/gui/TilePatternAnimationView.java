/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
package zsdx.gui;

import zsdx.entities.*;
import zsdx.entities.TilePattern.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

/**
 * Graphical component to select the animation properties of a tile pattern:
 * - no animation
 * - animation sequence 0-1-2
 * - animation sequence 0-1-2-1
 * - parallax scrolling
 * For the animated tile patterns, a second component allows to select
 * how the 3 animations are separated in the tileset (vertically or
 * horizontally).
 */
public class TilePatternAnimationView extends JPanel implements ActionListener {

    /**
     * The tile pattern currently represented in this view (can be null) 
     */
    private TilePattern tilePattern;

    /**
     * Combo box to select the animation type of the tilepattern.
     */
    public EnumerationChooser<Animation> sequenceField;

    /**
     * Combo box to select how the 3 animations are separated in the tileset.
     */
    public EnumerationIconChooser<AnimationSeparation> separationField;

    /**
     * Constructor.
     */
    public TilePatternAnimationView() {
	super();

	setLayout(new BoxLayout(this, BoxLayout.X_AXIS));

	// list for the animation sequence
	sequenceField = new EnumerationChooser<Animation>(Animation.class);
	sequenceField.addActionListener(this);

	// list for the separation
	separationField = new EnumerationIconChooser<AnimationSeparation>(AnimationSeparation.class);
	separationField.addActionListener(this);

	// add the two lists
	add(sequenceField);
	add(Box.createRigidArea(new Dimension(5, 0)));
	add(separationField);
    }

    /**
     * This method is called when the tile pattern is changed.
     * @param tilePattern the tile pattern to represent in this view (can be null)
     */
    public void update(TilePattern tilePattern) {

	this.tilePattern = tilePattern;
	if (tilePattern == null) {
	    sequenceField.setEnabled(false);
	    separationField.setEnabled(false);
	}
	else {
	    Animation sequence = tilePattern.getAnimation();
	    sequenceField.setEnabled(true);
	    sequenceField.setValue(sequence);

	    if (tilePattern.isAnimated()) {
		separationField.setValue(tilePattern.getAnimationSeparation());
		separationField.setEnabled(true);
	    }
	    else {
		separationField.setEnabled(false);
	    }
	}
    }
    
    /**
     * This method is called when the selection of one of the two combo boxes is changed.
     */
    public void actionPerformed(ActionEvent ev) {

	sequenceField.hidePopup();
	separationField.hidePopup();

	try {
	    if (ev.getSource() == sequenceField) {
		Animation listValue = sequenceField.getValue();
		Animation tileValue = tilePattern.getAnimation();
		if (listValue != tileValue) {
		    // the tile pattern's animation sequence has changed
		    tilePattern.setAnimation(listValue);
		}
	    }
	    else {
		AnimationSeparation listValue = separationField.getValue();
		AnimationSeparation tileValue = tilePattern.getAnimationSeparation();
		if (listValue != tileValue) {
		    // the direction of the animation separation has changed
		    tilePattern.setAnimationSeparation(listValue);
		}
	    }
	}
	catch (TilesetException ex) {
	    JOptionPane.showMessageDialog(null,
					  ex.getMessage(),
					  "Error",
					  JOptionPane.ERROR_MESSAGE);
	    update(tilePattern);
	}
    }
}
