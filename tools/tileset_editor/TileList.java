package tileset_editor;

import java.awt.*;
import javax.swing.*;
import java.util.*;

/**
 * This component shows a list of tiles and allows the user to edit them.
 */
public class TileList extends JPanel implements Observer {
    
    /**
     * The current tileset.
     */
    private Tileset tileset;

    /**
     * Text indicating the tileset name.
     */
    private JLabel labelTilesetName;

    /**
     * Constructor.
     */
    public TileList() {
	super();

	setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));

	// north : a vertical box layout with the tileset name and the buttons
	// center : the table

	labelTilesetName = new JLabel("Tileset name: ");
	labelTilesetName.setMaximumSize(new Dimension(400, 30));
	labelTilesetName.setAlignmentX(Component.LEFT_ALIGNMENT);

	JPanel buttons = new JPanel(new GridLayout(1, 3));
	buttons.setMaximumSize(new Dimension(400, 30));
	buttons.setAlignmentX(Component.LEFT_ALIGNMENT);
	buttons.add(new JButton("New"));
	buttons.add(new JButton("Edit"));
	buttons.add(new JButton("Remove"));

	add(labelTilesetName);
	add(Box.createRigidArea(new Dimension(0, 5)));
	add(buttons);
	add(Box.createVerticalGlue());
    }

    /**
     * Sets the observed tileset.
     */
    public void setTileset(Tileset tileset) {
	this.tileset = tileset;
	tileset.addObserver(this);
    }

    /**
     * This function is called when the tileset changes.
     */
    public void update(Observable o, Object obj) {
	
    }
}
