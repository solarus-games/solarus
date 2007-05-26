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
    public JList listSequence;

    /**
     * List to select how the 3 animations are separated in the tileset.
     */
    public JList listSeparation;
    
    /**
     * Text displayed in the first list.
     */
    private static final String[] sequenceNames = {
	"No animation",
	"Animation 1-2-3",
	"Animation 1-2-3-2"
    };

    /**
     * Text displayed in the second list.
     */
    private static final String[] separationNames = {
	"Horizontal separation",
	"Vertical separation"
    };

    /**
     * Constructor.
     */
    public AnimationView(Tile tile) {
	super();
	this.tile = tile;
	tile.addObserver(this);

	setLayout(new BoxLayout(this, BoxLayout.X_AXIS));

	// list for the animation sequence
	listSequence = new JList(new AbstractListModel() {

		public Object getElementAt(int index) {
		    return sequenceNames[index];
		}

		public int getSize() {
		    return 3;
		}
	    });

	listSequence.setDragEnabled(false);
	listSequence.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);

	// list for the separation
	listSeparation = new JList(new AbstractListModel() {

		public Object getElementAt(int index) {
		    return separationNames[index];
		}

		public int getSize() {
		    return 2;
		}
	    });

	listSeparation.setDragEnabled(false);
	listSeparation.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);

	// add the two lists
	add(listSequence);
	add(Box.createRigidArea(new Dimension(2,0)));
	add(listSeparation);

	update(tile, null);
    }

    /**
     * This method is called when the tile is changed.
     */
    public void update(Observable o, Object params) {
	int sequence = tile.getAnimationSequence();
	listSequence.setSelectedIndex(sequence);

	if (sequence == Tile.NO_ANIMATION) {
	    listSeparation.setEnabled(false);
	}
	else {
	    listSeparation.setEnabled(true);
	    listSeparation.setSelectedIndex(tile.getAnimationSeparation());
	}
    }

}
