package zsdx.map_editor_actions;

import java.awt.*;
import zsdx.*;

/**
 * Editing all common properties of an entity : the name, the layer, the position,
 * the size and the direction. The subclasses of ActionEditEntity can define
 * the specific properties of the subclasses of MapEntity. 
 */
public class ActionEditEntity extends MapEditorAction {

    protected MapEntity entity; // the entity modified
    
    private String nameBefore;
    private String nameAfter;
    
    private int layerBefore;
    private int layerAfter;
    
    private Dimension positionBefore;
    private Dimension positionAfter;
    
    private Dimension sizeBefore;
    private Dimension sizeAfter;
    
    private int directionBefore;
    private int directionAfter;
    
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
	    int layer, Dimension position, Dimension size, int direction) {
	super(map);
	
	this.entity = entity;
	
	// name
	if (entity.hasName()) {
	    this.nameBefore = entity.getName();
	    this.nameAfter = name;
	}
	
	// layer
	this.layerBefore = entity.getLayer();
	this.layerAfter = layer;
	
	// position
	this.positionBefore = new Dimension(entity.getX(), entity.getY());
	this.positionAfter = new Dimension(position);
	
	// size
	if (entity.isResizable()) {
	    this.sizeBefore = new Dimension(entity.getWidth(), entity.getHeight());
	    this.sizeAfter = new Dimension(size);
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
	entity.setLayer(layerAfter);
	
	// position
	entity.setPositionInMap(positionAfter.width, positionAfter.height);
	
	// size
	if (entity.isResizable()) {
	    entity.setSize(sizeAfter.width, sizeAfter.height);
	}
	
	// direction
	if (entity.hasDirection()) {
	    entity.setDirection(directionAfter);
	}
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
	entity.setLayer(layerBefore);
	
	// position
	entity.setPositionInMap(positionBefore.width, positionBefore.height);
	
	// size
	if (entity.isResizable()) {
	    entity.setSize(sizeBefore.width, sizeBefore.height);
	}
	
	// direction
	if (entity.hasDirection()) {
	    entity.setDirection(directionBefore);
	}
    }
}
