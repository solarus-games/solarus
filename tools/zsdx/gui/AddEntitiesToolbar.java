package zsdx.gui;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import zsdx.*;
import zsdx.entities.*;

/**
 * A toolbar to choose an entity to add onto the map.
 */
public class AddEntitiesToolbar extends JComponent {

    private static int[][] subtypes = {
	{0}, // tile
	{0}, // destination point
	{Teletransporter.INVISIBLE, Teletransporter.YELLOW}, // teletransporter
	{0}, // pickable item
	{DestructibleItem.GRASS, DestructibleItem.BUSH, DestructibleItem.POT}, // destructible item
	{0}, // chest
    };

    /**
     * The map view associated to the toolbar.
     */
    private MapView mapView;

    /**
     * Array indicating for each cell the corresponding type of entity and its subtype.
     */
    private int[][] cells;

    private EntityImageDescription[] imageDescriptions;

    /**
     * Creates a new toolbar to add entities.
     * @param mapView the map view where the entities will be added
     */
    public AddEntitiesToolbar(MapView mapView) {
	super();
	this.mapView = mapView;
	mapView.setAddEntitiesToolbar(this); // to be notified when the map view state changes
	addMouseListener(new AddEntitiesToolbarMouseListener());

	// count the number of cells to build
	int nbCells = 0;
	for (int i = 1; i < MapEntity.ENTITY_NB_TYPES; i++) {

	    nbCells += subtypes[i].length;
	}

	this.cells = new int[nbCells][2];
	this.imageDescriptions = new EntityImageDescription[nbCells];

	int k = 0;
	for (int i = 1; i < MapEntity.ENTITY_NB_TYPES; i++) {
	    for (int j = 0; j < subtypes[i].length; j++) {

		EntityImageDescription imageDescription = MapEntity.getImageDescription(i, subtypes[i][j]);

		this.cells[k][0] = i;
		this.cells[k][1] = subtypes[i][j];
		this.imageDescriptions[k] = imageDescription;
		
		k++;
	    }
	}

    }
    
    /**
     * Returns the preferred size of the component.
     * @return the preferred size of the component
     */
    public Dimension getPreferredSize() {
	return new Dimension(700, 33);
    }
    
    /**
     * Draws the component.
     * @param g the graphic context
     */
    public void paint(Graphics g) {
	super.paint(g);

	// draw a line to separate the toolbar from the component above
	g.setColor(Color.black);
	g.drawLine(0, 0, getWidth(), 0);

	if (!Project.isLoaded()) {
	    // if no project is loaded, we cannot access the image files
	    return;
	}

	// get the entity type being added (if any)
	int entityTypeBeingAdded = mapView.getEntityTypeBeingAdded();
	int entitySubtypeBeingAdded = mapView.getEntitySubtypeBeingAdded();

	// draw the icons for all types of entities (except TileOnMap which has its own dedicated view)
	for (int i = 0; i < cells.length; i++) {

	    // draw the image
	    int x = i * 16;
	    int y = 0;
	    Rectangle positionInDestinationImage = new Rectangle(x, y, 16, 16);
	    imageDescriptions[i].paint(g, 2, true, positionInDestinationImage);

	    // draw the selection rectangle if we are currently adding this kind of entity
	    if (entityTypeBeingAdded == cells[i][0] && entitySubtypeBeingAdded == cells[i][1]) {
		int scaledX = x * 2;
		int scaledY = y * 2;
		g.setColor(Color.RED);
		g.drawRect(scaledX, scaledY, 32, 32);
		g.drawRect(scaledX + 1, scaledY + 1, 30, 30);
	    }
	}
    }

    /**
     * The mouse listener associated to this component.
     */
    class AddEntitiesToolbarMouseListener extends MouseAdapter {

	/**
	 * Constructor.
	 */
	public AddEntitiesToolbarMouseListener() {
	    
	}

	/**
	 * This method is called when the mouse is pressed onto the component.
	 */
	public void mousePressed(MouseEvent ev) {

	    int cell = ev.getX() / 32;

	    if (mapView.getMap() == null || cell >= cells.length) {
		return;
	    }

	    int typeClicked = cells[cell][0];
	    int subtypeClicked = cells[cell][1];
	    mapView.startAddingEntity(typeClicked, subtypeClicked);
	}
    }
}
