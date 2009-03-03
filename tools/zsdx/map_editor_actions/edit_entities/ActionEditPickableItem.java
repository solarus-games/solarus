package zsdx.map_editor_actions.edit_entities;

import zsdx.*;
import zsdx.entities.*;

/**
 * Editing the properties specific to a pickable item:
 * the pickable item type and the savegame index.
 */
public class ActionEditPickableItem extends MapEditorAction {

    private PickableItem pickableItem;

    private int savegameVariableBefore;
    private int savegameVariableAfter;

    /**
     * Constructor.
     * @param map the map
     * @param pickableItem the pickable item to edit
     * @param savegameVariable the savegame index where the pickable item is saved
     */
    public ActionEditPickableItem(Map map, PickableItem pickableItem, int savegameVariable) {
	super(map);

	this.pickableItem = pickableItem;

	savegameVariableBefore = pickableItem.getSavegameVariable();
	savegameVariableAfter = savegameVariable;
    }

    /**
     * Executes the action.
     */
    public void execute() throws ZSDXException {
	pickableItem.setSavegameVariable(savegameVariableAfter);
    }

    /**
     * Undoes the action.
     */
    public void undo() throws ZSDXException {
	pickableItem.setSavegameVariable(savegameVariableBefore);
    }
}
