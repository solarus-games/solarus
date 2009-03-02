package zsdx.map_editor_actions.edit_entities;

import zsdx.*;
import zsdx.entities.*;
import zsdx.entities.PickableItem.Subtype;

/**
 * Editing the properties specific to a pickable item:
 * the pickable item type and the savegame index.
 */
public class ActionEditPickableItem extends MapEditorAction {

    private PickableItem pickableItem;

    private Subtype subtypeBefore;
    private Subtype subtypeAfter;

    private int savegameVariableBefore;
    private int savegameVariableAfter;

    /**
     * Constructor.
     * @param map the map
     * @param pickableItem the pickable item to edit
     * @param subtype the type of pickable item
     * @param savegameVariable the savegame index where the pickable item is saved
     */
    public ActionEditPickableItem(Map map, PickableItem pickableItem, Subtype subtype, int savegameVariable) {
	super(map);

	this.pickableItem = pickableItem;

	subtypeBefore = (Subtype) pickableItem.getSubtype();
	subtypeAfter = subtype;

	savegameVariableBefore = pickableItem.getSavegameVariable();
	savegameVariableAfter = savegameVariable;
    }

    /**
     * Executes the action.
     */
    public void execute() throws ZSDXException {
	pickableItem.setPickableItem(subtypeAfter, savegameVariableAfter);
    }

    /**
     * Undoes the action.
     */
    public void undo() throws ZSDXException {
	pickableItem.setPickableItem(subtypeBefore, savegameVariableBefore);
    }
}
