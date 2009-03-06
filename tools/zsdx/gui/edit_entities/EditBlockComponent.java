package zsdx.gui.edit_entities;

import java.awt.event.*;
import zsdx.*;
import zsdx.entities.*;
import zsdx.gui.*;
import zsdx.map_editor_actions.*;
import zsdx.entities.Block.*;

/**
 * A component to edit a block.
 */
public class EditBlockComponent extends EditEntityComponent {

    // specific fields of this entity
    private EnumerationChooser<Skin> skinField;
    private EnumerationChooser<MaximumMoves> maximumMovesField;

    /**
     * Constructor.
     * @param map the map
     * @param entity the entity to edit
     */
    public EditBlockComponent(Map map, MapEntity entity) {
	super(map, entity);
    }

    /**
     * Creates the specific fields for this kind of entity.
     */
    protected void createSpecificFields() {

	// skin name
	skinField = new EnumerationChooser<Skin>(Skin.class);
	addField("Skin", skinField);

	// maximum moves
	maximumMovesField = new EnumerationChooser<MaximumMoves>(MaximumMoves.class);
	addField("Maximum moves", maximumMovesField);

	// listeners
	subtypeField.addActionListener(new ActionListener() {
	    public void actionPerformed(ActionEvent ev) {
		EntitySubtype subtype = subtypeField.getValue();
		skinField.setEnabled(subtype == Subtype.NORMAL_BLOCK);
	    }
	});
    }

    /**
     * Updates the information displayed in the fields.
     */
    public void update() {
	super.update(); // update the common fields

	Block block = (Block) entity;
	Skin skin = Skin.get(block.getIntegerProperty("skin"));
	MaximumMoves maximumMoves = MaximumMoves.get(block.getIntegerProperty("maximumMoves"));

	skinField.setEnabled(block.getSubtype() == Subtype.NORMAL_BLOCK);
	skinField.setValue(skin);
	maximumMovesField.setValue(maximumMoves);
    }

    /**
     * Returns the specific part of the action made on the entity.
     * @return the specific part of the action made on the entity
     */
    protected ActionEditEntitySpecific getSpecificAction() {

	Skin skin = skinField.getValue();
	MaximumMoves maximumMoves = maximumMovesField.getValue();
	return new ActionEditEntitySpecific(entity, skin.getId(), maximumMoves.getId());
    }
}
