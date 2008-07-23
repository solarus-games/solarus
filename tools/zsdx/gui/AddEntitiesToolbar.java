package zsdx.gui;

import zsdx.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import java.lang.reflect.*;

/**
 * A toolbar to choose an entity to add onto the map.
 */
public class AddEntitiesToolbar extends JComponent {
    
    /**
     * The map view associated to the toolbar.
     */
    private MapView mapView;
    
    /**
     * Creates a new toolbar to add entities.
     * @param mapView the map view where the entities will be added
     */
    public AddEntitiesToolbar(MapView mapView) {
	super();
	this.mapView = mapView;
	mapView.setAddEntitiesToolbar(this); // to be notified when the map view state changes
	addMouseListener(new AddEntitiesToolbarMouseListener());
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
	
	// draw the icons for all types of entities except TileOnMap
	Class<?> entityClass = null;
	try {
	    for (int i = 1; i < MapEntity.ENTITY_NB_TYPES; i++) {
		
		// get the image properties
		entityClass = MapEntity.entityClasses[i];
		Field imageDescriptionField = entityClass.getField("imageDescription");
		EntityImageDescription imageDescription = (EntityImageDescription) imageDescriptionField.get(null);
				
		// draw the image
		int x = (i - 1) * 16;
		int y = 0;
		Rectangle positionInDestinationImage = new Rectangle(x, y, 16, 16);
		imageDescription.paint(g, 2, true, positionInDestinationImage);
		
		// draw the selection rectangle if we are currently adding this kind of entity
		if (entityTypeBeingAdded == i) {
		    x = x * 2;
		    y = y * 2;
		    g.setColor(Color.RED);
		    g.drawRect(x, y, 32, 32);
		    g.drawRect(x + 1, y + 1, 30, 30);
		}
	    }
	}
	catch (NoSuchFieldException ex) {
	    System.err.println("The field 'imageDescription' is missing in class " + entityClass.getName());
	    ex.printStackTrace();
	    System.exit(1);
	}
	catch (IllegalAccessException ex) {
	    System.err.println("Cannot access the field 'imageDescription' in class " + entityClass.getName());
	    ex.printStackTrace();
	    System.exit(1);
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
	 * Returns the type of entity clicked corresponding to a mouse event.
	 * @param ev the mouse event
	 * @return the type of entity clicked
	 */
	public int getEntityTypeClicked(MouseEvent ev) {
	    return ev.getX() / 32 + 1;
	}
	
	/**
	 * This method is called when the mouse is pressed onto the component.
	 */
	public void mousePressed(MouseEvent ev) {
	    
	    if (mapView.getMap() == null) {
		return;
	    }
	    
	    int typeClicked = getEntityTypeClicked(ev);
	    mapView.startAddingEntity(typeClicked);
	}
    }
}
