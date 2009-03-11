package zsdx.gui;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import java.util.*;
import zsdx.entities.*;

/**
 * Graphical component to select the animation properties of a tile pattern:
 * - no animation
 * - animation sequence 0-1-2
 * - animation sequence 0-1-2-1
 * For the animated tile patterns, a second component allows to select
 * how the 3 animations are separated in the tileset (vertically or
 * horizontally).
 */
public class AnimationView extends JPanel implements Observer, ActionListener {

    /**
     * The tile pattern observed.
     */
    private TilePattern tilePattern;

    /**
     * List to select the animation type of the tilepattern.
     */
    public JComboBox sequenceList;

    /**
     * List to select how the 3 animations are separated in the tileset.
     */
    public JComboBox separationList;
    
    /**
     * Text displayed in the first list.
     */
    private static final String[] sequenceItems = {
	"None",
	"1-2-3",
	"1-2-3-2"
    };

    /**
     * Text displayed in the second list.
     */
    private static final ImageIcon[] separationItems;

    // load the icons
    static {
	String path = "zsdx/images/";

	separationItems = new ImageIcon[2];
	separationItems[TilePattern.ANIMATION_SEPARATION_HORIZONTAL] = new ImageIcon(path + "animation_separation_horizontal.png");
	separationItems[TilePattern.ANIMATION_SEPARATION_VERTICAL] = new ImageIcon(path + "animation_separation_vertical.png");
    }

    /**
     * Constructor.
     */
    public AnimationView() {
	super();

	setLayout(new BoxLayout(this, BoxLayout.X_AXIS));

	// list for the animation sequence
	sequenceList = new JComboBox(sequenceItems);
	sequenceList.addActionListener(this);

	// list for the separation
	separationList = new JComboBox(separationItems);
	separationList.addActionListener(this);

	// add the two lists
	add(sequenceList);
	add(Box.createRigidArea(new Dimension(5, 0)));
	add(separationList);
    }

    /**
     * Sets the current tile pattern observed.
     */
    public void setCurrentTilePattern(TilePattern tilePattern) {
	if (this.tilePattern != null) {
	    this.tilePattern.deleteObserver(this);
	}

	this.tilePattern = tilePattern;
	
	if (tilePattern != null) {
	    tilePattern.addObserver(this);
	    sequenceList.setEnabled(true);
	    update(tilePattern, null);
	}
	else {
	    sequenceList.setEnabled(false);
	    separationList.setEnabled(false);
	}
    }

    /**
     * This method is called when the tile pattern is changed.
     */
    public void update(Observable o, Object params) {
	int sequence = tilePattern.getAnimationSequence();
	sequenceList.setSelectedIndex(sequence);

	if (tilePattern.isAnimated()) {
	    separationList.setSelectedIndex(tilePattern.getAnimationSeparation());
	    separationList.setEnabled(true);
	}
	else {
	    separationList.setEnabled(false);
	}
    }
    
    /**
     * This method is called when the selection of one of the two combo boxes is changed.
     */
    public void actionPerformed(ActionEvent ev) {

	sequenceList.hidePopup();
	separationList.hidePopup();

	try {
	    if (ev.getSource() == sequenceList) {
		int listIndex = sequenceList.getSelectedIndex();
		int animationIndex = tilePattern.getAnimationSequence();
		if (listIndex != animationIndex) {
		    // the tile pattern's animation sequence has changed
		    tilePattern.setAnimationSequence(listIndex);
		}
	    }
	    else {
		int listIndex = separationList.getSelectedIndex();
		int animationIndex = tilePattern.getAnimationSeparation();
		if (listIndex != animationIndex) {
		    // the direction of the animation separation has changed
		    tilePattern.setAnimationSeparation(listIndex);
		}
	    }
	}
	catch (TilesetException ex) {
	    JOptionPane.showMessageDialog(null,
					  ex.getMessage(),
					  "Error",
					  JOptionPane.ERROR_MESSAGE);
	    update(null, null);
	}
    }
}
