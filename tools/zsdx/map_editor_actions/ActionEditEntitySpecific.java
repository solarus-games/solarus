package zsdx.map_editor_actions;

import java.util.*;
import zsdx.*;
import zsdx.entities.*;

/**
 * Editing the specific properties of an entity. The subclasses of ActionEditEntity can define
 * the specific properties of the subclasses of MapEntity. 
 */
public class ActionEditEntitySpecific extends MapEditorAction {

    /**
     * The entity modified by this action.
     */
    private MapEntity entity;

    /**
     * The properties and their values.
     */
    private LinkedHashMap<String, String> propertiesBefore;
    private LinkedHashMap<String, String> propertiesAfter;

    /**
     * Constructor.
     */
    public ActionEditEntitySpecific(MapEntity entity, String ... properties) {
	this.entity = entity;
	propertiesBefore = new LinkedHashMap<String, String>(entity.getProperties());
	propertiesAfter = new LinkedHashMap<String, String>();

	int i = 0;
	for (String name: propertiesBefore.keySet()) {
	    propertiesAfter.put(name, properties[i++]);
	}
    }

    /**
     * Constructor.
     */
    public ActionEditEntitySpecific(MapEntity entity, int ... properties) {
	this.entity = entity;
	propertiesBefore = new LinkedHashMap<String, String>(entity.getProperties());
	propertiesAfter = new LinkedHashMap<String, String>();

	int i = 0;
	for (String name: propertiesBefore.keySet()) {
	    propertiesAfter.put(name, Integer.toString(properties[i++]));
	}
    }

    /**
     * Executes the action.
     */
    public void execute() throws ZSDXException {
	entity.setProperties(propertiesAfter);
    }

    /**
     * Undoes the action.
     */
    public void undo() throws ZSDXException {
	entity.setProperties(propertiesBefore);
    }
}
