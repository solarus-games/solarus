package zsdx.map_editor_actions;

import java.awt.*;
import zsdx.*;
import zsdx.entities.*;

/**
 * Editing all common properties of an entity : the name, the layer, the position,
 * the size and the direction. The subclasses of ActionEditEntity can define
 * the specific properties of the subclasses of MapEntity. 
 */
public class ActionEditEntity extends MapEditorAction {

    protected MapEntity entity; // the entity modified
    
    // common data of the entity
    private String nameBefore;
    private String nameAfter;
    
    private int layerBefore;
    private int layerAfter;
    
    private Rectangle positionBefore;
    private Rectangle positionAfter;
    
    private int directionBefore;
    private int directionAfter;
    
    // additional action specific to each entity type
    private MapEditorAction specificAction; 
    
    /**
     * Constructor.
     * The subclasses should call this constructor.
     * @param map the map
     * @param entity the entity edited
     * @param name the new name of the entity (or null if
     * the entity is not identifiable)
     * @param layer the new layer of the entity
     * @param position the new position of the entity
     * @param size the new size of the entity (or null if
     * the entity is not resizable)
     * @param direction the new direction of the entity
     * (or -1 if the entity has no direction property)
     */
    public ActionEditEntity(Map map, MapEntity entity, String name,
	    int layer, Point position, Dimension size, int direction) {
	super(map);
	
	this.entity = entity;
	this.specificAction = null;
	
	// name
	if (entity.hasName()) {
	    this.nameBefore = entity.getName();
	    this.nameAfter = name;
	}
	
	// layer
	this.layerBefore = entity.getLayer();
	this.layerAfter = layer;
	
	// position and size
	this.positionBefore = new Rectangle(entity.getX(), entity.getY(), 0, 0);
	this.positionAfter = new Rectangle(position.x, position.y, 0, 0);
	
	// size
	if (entity.isResizable()) {
	    this.positionBefore.setSize(entity.getSize());
	    this.positionAfter.setSize(size);
	}
	
	// direction
	if (entity.hasDirection()) {
	    this.directionBefore = entity.getDirection();
	    this.directionAfter = direction;
	}
    }
    
    /**
     * Executes the action.
     */
    public void execute() throws ZSDXException {
	
	// name
	if (entity.hasName()) {
	    entity.setName(nameAfter);
	}
	
	// layer
	map.setEntityLayer(entity, layerAfter);
	
	// position
	map.setEntityPosition(entity, positionAfter.x, positionAfter.y);

	// size
	if (entity.isResizable()) {
	    map.setEntitySize(entity, positionAfter.width, positionAfter.height);
	}
	
	// direction
	if (entity.hasDirection()) {
	    map.setEntityDirection(entity, directionAfter);
	}
	
	// specific data
	if (specificAction != null) {
	    specificAction.execute();
	}

	entity.updateImageDescription();
    }

    /**
     * Undoes the action.
     */
    public void undo() throws ZSDXException {

	// name
	if (entity.hasName()) {
	    entity.setName(nameBefore);
	}
	
	// layer
	map.setEntityLayer(entity, layerBefore);
	
	// position
	map.setEntityPosition(entity, positionBefore.x, positionBefore.y);
	
	// size
	if (entity.isResizable()) {
	    map.setEntitySize(entity, positionBefore.width, positionBefore.height);
	}
	
	// direction
	if (entity.hasDirection()) {
	    map.setEntityDirection(entity, directionBefore);
	}
	
	// specific data
	if (specificAction != null) {
	    specificAction.undo();
	}

	entity.updateImageDescription();
    }
    
    /**
     * Sets an additional action, specific to the entity type.
     * @param specificAction the specific action to add
     */
    public void setSpecificAction(MapEditorAction specificAction) {
	this.specificAction = specificAction;
    }
}
