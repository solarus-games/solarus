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
     * @param name new name of the map
     */
    public ActionRenameMap(Map map, String name) {
	super(map);
	this.nameBefore = map.getName();
	this.nameAfter = name;
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
