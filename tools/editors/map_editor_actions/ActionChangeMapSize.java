package editors.map_editor_actions;

import java.awt.Dimension;
import editors.*;

/**
 * Changing the size of the map.
 */
public class ActionChangeMapSize extends MapEditorAction {

    private Dimension sizeBefore;
    private Dimension sizeAfter;

    /**
     * Constructor.
     * @param map the map
     * @param size the new size of the map, in pixels
     */
    public ActionChangeMapSize(Map map, Dimension size) {
	super(map);
	this.sizeAfter = size;
    }

    /**
     * Executes the action.
     */
    public void execute() throws MapException {
	this.sizeBefore = map.getSize();
	map.setSize(sizeAfter);
    }

    /**
     * Undoes the action.
     */
    public void undo() throws MapException {
	map.setSize(sizeBefore);
    }

}
