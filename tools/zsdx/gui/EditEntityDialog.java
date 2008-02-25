package zsdx.gui;

import zsdx.*;

/**
 * A dialog box with some options to edit a map entity.
 */
public class EditEntityDialog extends OkCancelDialog {

    /**
     * The component to edit the entity.
     */
    private EditEntityComponent entityComponent;
    
    /**
     * Constructor.
     */
    public EditEntityDialog(Map map, MapEntity entity) {
	super("Edit an entity", false);
	entityComponent = EditEntityComponent.create(map, entity);
	setComponent(entityComponent);
    }

    /**
     * Takes into account the modifications made by the user in the dialog box.
     * @throws ZSDXException if the user has made incorrect modifications
     */
    protected void applyModifications() throws ZSDXException {
	entityComponent.applyModifications();	
    }
}
