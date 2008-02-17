package zsdx.gui;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

import zsdx.Tile;
import zsdx.TilesetException;

import java.util.*;

/**
 * Graphical component to select the animation properties of a tile:
 * - no animation
 * - animation sequence 0-1-2
 * - animation sequence 0-1-2-1
 * For the animated tiles, a second component allows to select
 * how the 3 animations are separated in the tileset (vertically or
 * horizontally).
 */
public class AnimationView extends JPanel implements Observer, ActionListener {

    /**
     * The tile observed.
     */
    private Tile tile;

    /**
     * List to select the animation type of the tile.
     */
    public JComboBox listSequence;

    /**
     * List to select how the 3 animations are separated in the tileset.
     */
    public JComboBox listSeparation;
    
    /**
     * Text displayed in the first list.
     */
    private static final String[] itemsSequence = {
	"None",
	"1-2-3",
	"1-2-3-2"
    };

    /**
     * Text displayed in the second list.
     */
    private static final ImageIcon[] itemsSeparation;

    // load the icons
    static {
	String path = "zsdx/images/";

	itemsSeparation = new ImageIcon[2];
	itemsSeparation[Tile.ANIMATION_SEPARATION_HORIZONTAL] = new ImageIcon(path + "animation_separation_horizontal.png");
	itemsSeparation[Tile.ANIMATION_SEPARATION_VERTICAL] = new ImageIcon(path + "animation_separation_vertical.png");
    }

    /**
     * Constructor.
     */
    public AnimationView() {
	super();

	setLayout(new BoxLayout(this, BoxLayout.X_AXIS));

	// list for the animation sequence
	listSequence = new JComboBox(itemsSequence);
	listSequence.addActionListener(this);

	// list for the separation
	listSeparation = new JComboBox(itemsSeparation);
	listSeparation.addActionListener(this);


	// add the two lists
	add(listSequence);
	add(Box.createRigidArea(new Dimension(5, 0)));
	add(listSeparation);
    }

    /**
     * Sets the current tile observed.
     */
    public void setCurrentTile(Tile tile) {
	if (this.tile != null) {
	    this.tile.deleteObserver(this);
	}

	this.tile = tile;
	
	if (tile != null) {
	    tile.addObserver(this);
	    listSequence.setEnabled(true);
	    update(tile, null);
	}
	else {
	    listSequence.setEnabled(false);
	    listSeparation.setEnabled(false);
	}
    }

    /**
     * This method is called when the tile is changed.
     */
    public void update(Observable o, Object params) {
	int sequence = tile.getAnimationSequence();
	listSequence.setSelectedIndex(sequence);

	if (tile.isAnimated()) {
	    listSeparation.setSelectedIndex(tile.getAnimationSeparation());
	    listSeparation.setEnabled(true);
	}
	else {
	    listSeparation.setEnabled(false);
	}
    }
    
    /**
     * This method is called when the selection of one of the two combo boxes is changed.
     */
    public void actionPerformed(ActionEvent ev) {

	listSequence.hidePopup();
	listSeparation.hidePopup();

	try {
	    if (ev.getSource() == listSequence) {
		int listIndex = listSequence.getSelectedIndex();
		int animationIndex = tile.getAnimationSequence();
		if (listIndex != animationIndex) {
		    // the tile's animation sequence has changed
		    tile.setAnimationSequence(listIndex);
		}
	    }
	    else {
		int listIndex = listSeparation.getSelectedIndex();
		int animationIndex = tile.getAnimationSeparation();
		if (listIndex != animationIndex) {
		    // the direction of the animation separation has changed
		    tile.setAnimationSeparation(listIndex);
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
