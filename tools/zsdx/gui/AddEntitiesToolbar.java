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

    /**
     * Defines the entities available in the toolbar.
     * Each element is a type of entity and a possible subtype.
     */
    private static int[][] cells = {
	{MapEntity.ENTITY_DESTINATION_POINT, DestinationPoint.INVISIBLE},
	{MapEntity.ENTITY_DESTINATION_POINT, DestinationPoint.GRAY},
	{MapEntity.ENTITY_TELETRANSPORTER, Teletransporter.INVISIBLE},
	{MapEntity.ENTITY_TELETRANSPORTER, Teletransporter.YELLOW},
	{MapEntity.ENTITY_PICKABLE_ITEM, 0},
	{MapEntity.ENTITY_DESTRUCTIBLE_ITEM, DestructibleItem.GRASS},
	{MapEntity.ENTITY_DESTRUCTIBLE_ITEM, DestructibleItem.BUSH},
	{MapEntity.ENTITY_DESTRUCTIBLE_ITEM, DestructibleItem.POT},
	{MapEntity.ENTITY_DESTRUCTIBLE_ITEM, DestructibleItem.STONE_SMALL_WHITE},
	{MapEntity.ENTITY_CHEST, 0},
	{MapEntity.ENTITY_JUMP_SENSOR, 0},
	{MapEntity.ENTITY_ENEMY, Enemy.Subtype.SIMPLE_GREEN_SOLDIER.ordinal()},
	{MapEntity.ENTITY_INTERACTIVE, InteractiveEntity.Subtype.CUSTOM.ordinal()},
	{MapEntity.ENTITY_INTERACTIVE, InteractiveEntity.Subtype.NON_PLAYING_CHARACTER.ordinal()},
	{MapEntity.ENTITY_INTERACTIVE, InteractiveEntity.Subtype.SIGN.ordinal()}
    };

    /**
     * The map view associated to the toolbar.
     */
    private MapView mapView;

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
	int nbCells = cells.length;
	this.imageDescriptions = new EntityImageDescription[nbCells];

	for (int i = 0; i < nbCells; i++) {
	    EntityImageDescription imageDescription = MapEntity.getImageDescription(cells[i][0], cells[i][1]);
	    this.imageDescriptions[i] = imageDescription;
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
    private class AddEntitiesToolbarMouseListener extends MouseAdapter {

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
