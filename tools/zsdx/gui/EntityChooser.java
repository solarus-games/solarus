package zsdx.gui;

import javax.swing.*;
import java.util.*;
import zsdx.*;
import zsdx.Map;

/**
 * A combo box to select an entity of a certain type.
 * This component displays the name of the entities.
 */
public class EntityChooser extends JComboBox {
    
    /**
     * The map.
     */
//    private Map map;

    /**
     * The kind of entity displayed in the combo box (doors, ennemies...).
     */
//    private int entityType;
    
    /**
     * Indicates whether the first element of the combo box should be
     * an empty option (showing that no element is selected).
     */
//    private boolean showEmptyOption;

    /**
     * Creates a new entity chooser.
     * @param map the map
     * @param entityType the type of entity to show in the combo box
     */
    public EntityChooser(Map map, int entityType, boolean showEmptyOption) {
	super();
	/*
	this.map = map;
	this.entityType = entityType;
	this.showEmptyOption = showEmptyOption;
*/
	
	if (showEmptyOption) {
	    addItem("");
	}

	List<MapEntity> entities = map.getEntitiesOfType(entityType);
	for (MapEntity entity: entities) {
	    addItem(entity.getName());
	}
    }
    
    /**
     * Returns the name of the selected entity.
     * @return the name of the selected entity, or null if no entity is selected
     */
    public String getSelectedName() {
	return (String) getSelectedItem();
    }
    
    /**
     * Selects an entity in a combo box.
     * @param name the name of the entity to select
     */
    public void setSelectedName(String name) {
	setSelectedItem(name);
    }
}
