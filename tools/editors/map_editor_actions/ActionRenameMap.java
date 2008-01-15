package editors.map_editor_actions;

import editors.*;

/**
 * Changing the name of a map.
 */
public class ActionRenameMap extends MapEditorAction {

    private String nameBefore;
    private String nameAfter;

    /**
     * Creates an action to change a map name.
     * @param map the map
     * @param nameBefore old name of the map
     * @param nameAfter new name of the map
     */
    public ActionRenameMap(Map map, String nameBefore, String nameAfter) {
	super(map);
	this.nameBefore = nameBefore;
	this.nameAfter = nameAfter;
    }

    /**
     * Executes the action.
     */
    public void execute() throws MapException {

	map.setName(nameAfter);
    }

    /**
     * Undoes the action.
     */
    public void undo() throws MapException {

	map.setName(nameBefore);
    }

}
