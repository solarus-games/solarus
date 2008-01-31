package editors.map_editor_actions;

import java.awt.Rectangle;
import editors.*;

/**
 * Resizing a map entity.
 */
public class ActionResizeEntity extends MapEditorAction {

    private MapEntity entity;
    private Rectangle positionBefore;
    private Rectangle positionAfter;
    
    /**
     * Constructor.
     * @param map the map
     * @param entity the entity to resize
     * @param position the new entity size (this object is not modified by this class)
     */
    public ActionResizeEntity(Map map, MapEntity entity, Rectangle position) {
	super(map);
	
	this.entity = entity;
	this.positionBefore = new Rectangle(entity.getPositionInMap());
	this.positionAfter = new Rectangle(position);
    }

    /**
     * Executes the action.
     */
    public void execute() throws MapException {
	map.setEntityPosition(entity, positionAfter);
    }

    /**
     * Undoes the action.
     */
    public void undo() throws MapException {
	map.setEntityPosition(entity, positionBefore);
    }

}
