package editors;

/**
 * An action of the user on the editor.
 * The actions are saved in a MapEditorHistory object, so that the user can
 * undo an redo them.
 */
public abstract class MapEditorAction {

    /**
     * The map.
     */
    protected final Map map;

    /**
     * Constructor.
     * @param map the map
     */
    public MapEditorAction(Map map) {
	this.map = map;
    }

    /**
     * Executes the action.
     */
    public abstract void execute() throws MapException;

    /**
     * Undoes the action.
     */
    public abstract void undo() throws MapException;

}
