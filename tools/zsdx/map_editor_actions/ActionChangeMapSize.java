package zsdx.map_editor_actions;

import java.awt.Dimension;

import zsdx.*;

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
	this.sizeBefore = map.getSize();
    }

    /**
     * Executes the action.
     */
    public void execute() throws MapException {
	map.setSize(sizeAfter);
    }

    /**
     * Undoes the action.
     */
    public void undo() throws MapException {
	map.setSize(sizeBefore);
    }

}
