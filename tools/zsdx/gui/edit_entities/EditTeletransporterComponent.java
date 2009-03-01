package zsdx.gui.edit_entities;

import java.awt.event.*;
import javax.swing.*;
import zsdx.*;
import zsdx.entities.*;
import zsdx.gui.*;
import zsdx.map_editor_actions.*;
import zsdx.map_editor_actions.edit_entities.*;

/**
 * A component to edit a teletransporter.
 */
public class EditTeletransporterComponent extends EditEntityComponent {

    // specific fields of a teletransporter
    private TeletransporterSubtypeChooser subtypeField;
    private TransitionChooser transitionField;
    private ResourceChooser mapField;
    private EntityChooser destinationPointField;

    private static final String samePointText = "<Same point>";
    private static final String sidePointText = "<Side of the map>";

    /**
     * Constructor.
     * @param map the map
     * @param entity the entity to edit
     */
    public EditTeletransporterComponent(Map map, MapEntity entity) {
	super(map, entity);
    }

    /**
     * Creates the specific fields for this kind of entity.
     */
    protected void createSpecificFields() {

	// subtype
	subtypeField = new TeletransporterSubtypeChooser();
	addField("Subtype", subtypeField);

	// transition
	transitionField = new TransitionChooser();
	addField("Transition", transitionField);

	// destination map
	mapField = new ResourceChooser(ResourceDatabase.RESOURCE_MAP, true);
	addField("Destination map", mapField);

	// destination point
	destinationPointField = new EntityChooser(null, EntityType.DESTINATION_POINT,
		new String[] {"", samePointText, sidePointText});
	addField("Destination point", destinationPointField);

	// load the entrance list for the selected map
	mapField.addActionListener(new ActionListenerChangeDestinationMap());
    }

    /**
     * Updates the information displayed in the fields.
     */
    public void update() {
	super.update(); // update the common fields

	Teletransporter teletransporter = (Teletransporter) entity;

	subtypeField.setSubtype(teletransporter.getSubtypeIndex());
	transitionField.setTransition(teletransporter.getTransition());
	mapField.setSelectedId(teletransporter.getDestinationMapId());
	
	String destinationPointName = teletransporter.getDestinationPointName();
	if (destinationPointName.equals("_same")) {
	    destinationPointName = samePointText;
	}
	else if (destinationPointName.equals("_side")) {
	    destinationPointName = sidePointText;
	}
	destinationPointField.setSelectedName(destinationPointName);
    }

    /**
     * Creates the map editor action object which corresponds
     * to the modifications indicated in the fields.
     * @return the action object corresponding to the modifications made
     * @throws ZSDXException if the destination map or the entrance name are left blank
     */
    protected ActionEditEntity getAction() throws ZSDXException {
	
	// retrieve the action corresponding to the common entity properties
	ActionEditEntity action = super.getAction();

	// add the properties specific to a teletransporter
	Teletransporter teletransporter = (Teletransporter) entity;

	int subtype = subtypeField.getSubtype();
	int transition = transitionField.getTransition();
	String destinationMapId = mapField.getSelectedId();
	String destinationPointName = destinationPointField.getSelectedName();

	if (destinationPointName.equals(samePointText)) {
	    destinationPointName = "_same";
	}
	else if (destinationPointName.equals(sidePointText)) {
	    destinationPointName = "_side";
	}

	if (destinationMapId.length() == 0) {
	    throw new ZSDXException("Please select a destination map");
	}

	if (destinationPointName.length() == 0) {
	    throw new ZSDXException("Please select a destination point on the destination map");
	}

	action.setSpecificAction(new ActionEditTeletransporter(map, teletransporter, subtype,
		transition, destinationMapId, destinationPointName));

	return action;
    }

    /**
     * Action performed when the destination map field is changed.
     * The entrance field is reloaded to show the entrances of this map.
     */
    private class ActionListenerChangeDestinationMap implements ActionListener {

	public void actionPerformed(ActionEvent ev) {
	    
	    try {
		String mapId = mapField.getSelectedId();
		
		if (mapId.length() != 0) { // a map has just been selected
		    destinationPointField.setMap(new Map(mapId));
		}
		else {
		    destinationPointField.setMap(null);
		}
	    }
	    catch (ZSDXException ex) {
		GuiTools.warningDialog("The destination map is invalid: " + ex.getMessage());
	    }
	}
    }

    /**
     * A combo box to select a subtype of teletransporter.
     */
    public class TeletransporterSubtypeChooser extends JComboBox {

	private final String[] names = {
	    "Invisible", "Yellow", "Blue"
	};

	/**
	 * Creates a new destructible item type chooser.
	 */
	public TeletransporterSubtypeChooser() {
	    super();

	    for (int i = 0; i < names.length; i++) {
		addItem(new KeyValue(i, names[i]));
	    }
	}

	/**
	 * Returns the type of teletransporter currently selected.
	 * @return the type of teletransporter currently selected
	 */
	public int getSubtype() {

	    KeyValue item = (KeyValue) getSelectedItem();
	    return Integer.parseInt(item.getKey());
	}

	/**
	 * Sets the subtype of teletransporter selected.
	 * @param type the subtype of teletransporter to make selected
	 */
	public void setSubtype(int subtype) {

	    KeyValue item = new KeyValue(subtype, null);
	    setSelectedItem(item);
	}
    }
}
