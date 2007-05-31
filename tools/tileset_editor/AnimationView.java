package tileset_editor;

import java.awt.*;
import javax.swing.*;
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
public class AnimationView extends JPanel implements Observer {

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
	String path = "tileset_editor/images/";

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

	// list for the separation
	listSeparation = new JComboBox(itemsSeparation);

	// add the two lists
	add(listSequence);
	add(Box.createRigidArea(new Dimension(5, 0)));
	add(listSeparation);

	setCurrentTile(null);
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

	if (sequence == Tile.ANIMATION_NONE) {
	    listSeparation.setEnabled(false);
	}
	else {
	    listSeparation.setEnabled(true);
	    listSeparation.setSelectedIndex(tile.getAnimationSeparation());
	}
    }

}
