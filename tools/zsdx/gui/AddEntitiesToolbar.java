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
	{EntityType.DESTINATION_POINT.getIndex(), DestinationPoint.INVISIBLE},
	{EntityType.DESTINATION_POINT.getIndex(), DestinationPoint.GRAY},
	{EntityType.TELETRANSPORTER.getIndex(), Teletransporter.INVISIBLE},
	{EntityType.TELETRANSPORTER.getIndex(), Teletransporter.YELLOW},
	{EntityType.PICKABLE_ITEM.getIndex(), 0},
	{EntityType.DESTRUCTIBLE_ITEM.getIndex(), DestructibleItem.GRASS},
	{EntityType.DESTRUCTIBLE_ITEM.getIndex(), DestructibleItem.BUSH},
	{EntityType.DESTRUCTIBLE_ITEM.getIndex(), DestructibleItem.POT},
	{EntityType.DESTRUCTIBLE_ITEM.getIndex(), DestructibleItem.STONE_SMALL_WHITE},
	{EntityType.CHEST.getIndex(), 0},
	{EntityType.JUMP_SENSOR.getIndex(), 0},
	{EntityType.ENEMY.getIndex(), Enemy.Subtype.SIMPLE_GREEN_SOLDIER.ordinal()},
	{EntityType.INTERACTIVE.getIndex(), InteractiveEntity.Subtype.CUSTOM.ordinal()},
	{EntityType.INTERACTIVE.getIndex(), InteractiveEntity.Subtype.NON_PLAYING_CHARACTER.ordinal()},
	{EntityType.INTERACTIVE.getIndex(), InteractiveEntity.Subtype.SIGN.ordinal()}
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
	    EntityImageDescription imageDescription = MapEntity.getImageDescription(EntityType.get(cells[i][0]), cells[i][1]);
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
	EntityType entityTypeBeingAdded = mapView.getEntityTypeBeingAdded();
	int entitySubtypeBeingAdded = mapView.getEntitySubtypeBeingAdded();

	// draw the icons for all types of entities (except TileOnMap which has its own dedicated view)
	for (int i = 0; i < cells.length; i++) {

	    // draw the image
	    int x = i * 16;
	    int y = 0;
	    Rectangle positionInDestinationImage = new Rectangle(x, y, 16, 16);
	    imageDescriptions[i].paint(g, 2, true, positionInDestinationImage);

	    // draw the selection rectangle if we are currently adding this kind of entity
	    if (entityTypeBeingAdded != null &&
		    entityTypeBeingAdded.getIndex() == cells[i][0] &&
		    entitySubtypeBeingAdded == cells[i][1]) {
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

	    EntityType typeClicked = EntityType.get(cells[cell][0]);
	    int subtypeClicked = cells[cell][1];
	    mapView.startAddingEntity(typeClicked, subtypeClicked);
	}
    }
}
