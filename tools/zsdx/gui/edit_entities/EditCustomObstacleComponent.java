package zsdx.gui.edit_entities;

import javax.swing.*;
import zsdx.*;
import zsdx.entities.*;
import zsdx.gui.*;
import zsdx.map_editor_actions.*;

/**
 * A component to edit a custom obstacle.
 */
public class EditCustomObstacleComponent extends EditEntityComponent {

    // specific fields of a custom obstacle
    private JCheckBox stopsHeroField;
    private JCheckBox stopsEnemiesField;
    private JCheckBox stopsNPCsField;
    private JCheckBox stopsBlocksField;

    /**
     * Constructor.
     * @param map the map
     * @param entity the entity to edit
     */
    public EditCustomObstacleComponent(Map map, MapEntity entity) {
	super(map, entity);
    }

    /**
     * Creates the specific fields for this kind of entity.
     */
    protected void createSpecificFields() {

	// hero
	stopsHeroField = new JCheckBox("Obstacle for the hero");
	addField("Hero", stopsHeroField);

	// enemies
	stopsEnemiesField = new JCheckBox("Obstacle for enemies");
	addField("Enemies", stopsEnemiesField);

	// non-playing characters
	stopsNPCsField = new JCheckBox("Obstacle for non-playing characters");
	addField("NPCs", stopsNPCsField);

	// blocks
	stopsBlocksField = new JCheckBox("Obstacle for blocks and statues");
	addField("Blocks", stopsBlocksField);
    }

    /**
     * Updates the information displayed in the fields.
     */
    public void update() {
	super.update(); // update the common fields

	CustomObstacle customObstacle = (CustomObstacle) entity;
	stopsHeroField.setSelected(customObstacle.getBooleanProperty("stopsHero"));
	stopsEnemiesField.setSelected(customObstacle.getBooleanProperty("stopsEnemies"));
	stopsNPCsField.setSelected(customObstacle.getBooleanProperty("stopsNPCs"));
	stopsBlocksField.setSelected(customObstacle.getBooleanProperty("stopsBlocks"));
    }

    /**
     * Returns the specific part of the action made on the entity.
     * @return the specific part of the action made on the entity
     */
    protected ActionEditEntitySpecific getSpecificAction() {

	return new ActionEditEntitySpecific(entity, 
		stopsHeroField.isSelected() ? 1 : 0, stopsEnemiesField.isSelected() ? 1 : 0,
		stopsNPCsField.isSelected() ? 1 : 0, stopsBlocksField.isSelected() ? 1 : 0);
    }
}
