package editors.map_editor_actions;

import java.awt.Dimension;
import editors.*;

/**
 * Changing the size of the map.
 */
public class ActionChangeMapSize extends MapEditorAction {

    private final Dimension sizeBefore;
    private final Dimension sizeAfter;

    /**
     * Constructor.
     * @param map the map
     */
    public ActionChangeMapSize(Map map, Dimension sizeAfter) {
	super(map);

	this.sizeBefore = map.getSize();
	this.sizeAfter = sizeAfter;
    }

    /**
     * Executes the action.
     */
    public void execute() {
	map.setSize(sizeAfter);
    }

    /**
     * Undoes the action.
     */
    public void undo() {
	map.setSize(sizeBefore);
    }

}
