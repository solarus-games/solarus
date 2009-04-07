/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
package zsdx.gui;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import java.util.*;
import zsdx.*;
import zsdx.Map;
import zsdx.map_editor_actions.*;

/**
 * This component shows the properties of a map and allows to edit them.
 */
public class MapPropertiesView extends JPanel implements Observer {

    /**
     * The map observed.
     */
    private Map map;

    // subcomponents
    private JLabel idField;
    private NameField nameField;
    private SizeField sizeField;
    private WorldField worldField;
    private JLabel floorLabel;
    private FloorField floorField;
    private JLabel locationLabel;
    private LocationField locationField;
    private EnableSmallKeysField enableSmallKeysField;
    private SmallKeysVariableField smallKeysVariableField;
    private TilesetField tilesetField;
    private MusicField musicField;

    /**
     * Constructor.
     */
    public MapPropertiesView() {
	super(new GridBagLayout());

	setBorder(BorderFactory.createTitledBorder("Map properties"));

	GridBagConstraints leftConstraints = new GridBagConstraints();
	leftConstraints.insets = new Insets(5, 5, 5, 5); // margins
	leftConstraints.anchor = GridBagConstraints.FIRST_LINE_START;
	leftConstraints.gridx = 0;
	leftConstraints.weightx = 0.5;
	leftConstraints.weighty = 0;
	leftConstraints.fill = GridBagConstraints.HORIZONTAL;

	GridBagConstraints rightConstraints = new GridBagConstraints();
	rightConstraints.insets = new Insets(5, 5, 5, 5); // margins
	rightConstraints.anchor = GridBagConstraints.FIRST_LINE_START;
	rightConstraints.gridx = 1;
	rightConstraints.weightx = 1;

	// map id
	leftConstraints.gridy = 0;
	add(new JLabel("Map id"), leftConstraints);
	idField = new JLabel();
	rightConstraints.gridy = 0;
	rightConstraints.fill = GridBagConstraints.HORIZONTAL;
	add(idField, rightConstraints);

	// map name
	leftConstraints.gridy++;
	add(new JLabel("Map name"), leftConstraints);
	rightConstraints.gridy++;
	rightConstraints.fill = GridBagConstraints.HORIZONTAL;
	nameField = new NameField();
	add(nameField, rightConstraints);

	// size
	leftConstraints.gridy++;
	add(new JLabel("Size"), leftConstraints);
	rightConstraints.gridy++;
	rightConstraints.fill = GridBagConstraints.NONE;
	sizeField = new SizeField();
	add(sizeField, rightConstraints);

	// world
	leftConstraints.gridy++;
	add(new JLabel("World"), leftConstraints);
	rightConstraints.gridy++;
	rightConstraints.fill = GridBagConstraints.HORIZONTAL;
	worldField = new WorldField();
	add(worldField, rightConstraints);

	// floor
	leftConstraints.gridy++;
	floorLabel = new JLabel("Floor");
	add(floorLabel, leftConstraints);
	rightConstraints.gridy++;
	rightConstraints.fill = GridBagConstraints.HORIZONTAL;
	floorField = new FloorField();
	add(floorField, rightConstraints);

	// location
	leftConstraints.gridy++;
	locationLabel = new JLabel("Location in its world");
	add(locationLabel, leftConstraints);
	rightConstraints.gridy++;
	rightConstraints.fill = GridBagConstraints.NONE;
	locationField = new LocationField();
	add(locationField, rightConstraints);

	// small keys variable
	smallKeysVariableField = new SmallKeysVariableField();
	enableSmallKeysField = new EnableSmallKeysField();
	leftConstraints.gridy++;
	add(enableSmallKeysField, leftConstraints);
	rightConstraints.gridy++;
	rightConstraints.fill = GridBagConstraints.NONE;
	add(smallKeysVariableField, rightConstraints);

	// tileset
	leftConstraints.gridy++;
	add(new JLabel("Tileset"), leftConstraints);
	rightConstraints.gridy++;
	rightConstraints.fill = GridBagConstraints.HORIZONTAL;
	tilesetField = new TilesetField();
       	add(tilesetField, rightConstraints);
	
	// music
	leftConstraints.gridy++;
	add(new JLabel("Music"), leftConstraints);
	rightConstraints.gridy++;
	rightConstraints.fill = GridBagConstraints.HORIZONTAL;
	musicField = new MusicField();
	add(musicField, rightConstraints);

	// invisible component to fill the remaining space
	leftConstraints.gridy++;
	leftConstraints.gridwidth = 3;
	leftConstraints.weighty = 1;
	leftConstraints.fill = GridBagConstraints.BOTH;
	JLabel emptySpace = new JLabel();
	emptySpace.setPreferredSize(new Dimension(0, 0));
	add(emptySpace, leftConstraints);
    }

    /**
     * Sets the observed map.
     * @param map the current map, or null if no map is loaded
     */
    public void setMap(Map map) {
	if (this.map != null) {
	    this.map.deleteObserver(this);
	}

	this.map = map;
	
	if (map != null) {
	    map.addObserver(this);
	}
	
	update(map, null);
    }
    
    /**
     * This function is called when the map is changed.
     * @param o the map (or null if the map has just been closed)
     * @param obj unused
     */
    public void update(Observable o, Object obj) {

	// update the elementary components here
	if (map != null) {
	    idField.setText(map.getId());
	}
	else {
	    idField.setText("");
	}

	// tell the complex components to update themselves
	nameField.update(map);
	worldField.update(map);
	sizeField.update(map);
	floorField.update(map);
	locationField.update(map);
	enableSmallKeysField.update(map);
	smallKeysVariableField.update(map);
	tilesetField.update(map);
	musicField.update(map);
    }

    // components for the editable properties

    /**
     * Component to change the name of the map.
     */
    private class NameField extends JPanel {

	// subcomponents
	private JTextField textFieldName;
	private JButton buttonSet;	

	/**
	 * Constructor.
	 */
	public NameField() {
	    super();
	    setLayout(new BoxLayout(this, BoxLayout.X_AXIS));

	    textFieldName = new JTextField(10);
	    buttonSet = new JButton("Set");

	    ActionListener listener = new ActionListener() {
		    public void actionPerformed(ActionEvent ev) {

			try {
			    String name = textFieldName.getText();
			    map.getHistory().doAction(new ActionRenameMap(map, name));
			}
			catch (ZSDXException ex) {
			    GuiTools.errorDialog("Cannot change the map name: " + ex.getMessage());
			}
			update(map);
		    }
		};

	    buttonSet.addActionListener(listener);
	    textFieldName.addActionListener(listener);

	    add(textFieldName);
	    add(Box.createRigidArea(new Dimension(5, 0)));
	    add(buttonSet);

	    update((Map) null);
	}

	/**
	 * This function is called when the map is changed.
	 * The component is updated.
	 */
	public void update(Observable o) {

	    if (map != null) {
		textFieldName.setEnabled(true);
		buttonSet.setEnabled(true);
		textFieldName.setText(map.getName());
	    }
	    else {
		textFieldName.setEnabled(false);
		buttonSet.setEnabled(false);
		textFieldName.setText("");
	    }
	}
    }

    /**
     * Component to change the map size.
     */
    private class SizeField extends CoordinatesField {

	/**
	 * Constructor.
	 */
	public SizeField() {
	    super();
	    setMinimum(Map.MINIMUM_WIDTH, Map.MINIMUM_HEIGHT);
	    setStepSize(8, 8);

	    addChangeListener(new ChangeListener() {
		public void stateChanged(ChangeEvent ev) {

		    try {
			Point coords = getCoordinates();
			Dimension size = new Dimension(coords.x, coords.y);

			if (hasChanged(ev, map.getWidth(), map.getHeight())) {
			    map.getHistory().doAction(new ActionChangeMapSize(map, size));
			}
		    }
		    catch (NumberFormatException ex) {

		    }
		    catch (ZSDXException ex) {
			GuiTools.errorDialog("Cannot change the map size: " + ex.getMessage());
		    }
		    update(map);
		}
	    });
	}

	/**
	 * This function is called when the map is changed.
	 * The component is updated.
	 */
	public void update(Observable o) {

	    if (map != null) {
		setEnabled(true);
		Dimension size = map.getSize();
		setCoordinates(size.width, size.height);
	    }
	    else {
		setEnabled(false);
	    }
	}
    }

    /**
     * Component to choose the world where this map is.
     */
    private class WorldField extends JComboBox implements ActionListener {

	/**
	 * Constructor.
	 */
	public WorldField() {
	    super();
	    
	    addItem(new KeyValue(-1, "Inside world"));
	    addItem(new KeyValue(0, "Outside world"));
	    for (int i = 1; i <= 20; i++) {
		addItem(new KeyValue(i, "Dungeon " + i));
	    }

	    addActionListener(this);   
	    update((Map) null);
	}

	/**
	 * This function is called when the map is changed.
	 * The selection is updated.
	 */
	public void update(Observable o) {

	    if (map != null) {

		int currentWorld = map.getWorld();
		int selectedWorld = getSelectedWorld();

		if (selectedWorld != currentWorld) {
		    setSelectedWorld(currentWorld);
		}
		setEnabled(true);
	    }
	    else {
		setEnabled(false);
	    }
	}
	
	/**
	 * Returns the world currently selected.
	 * @return the world currently selected
	 */
	public int getSelectedWorld() {
	    KeyValue item = (KeyValue) getSelectedItem();
	    return Integer.parseInt(item.getKey());
	}

	/**
	 * Selects a world in the combo box.
	 * @param world the world to make selected
	 */
	public void setSelectedWorld(int world) {
	    KeyValue item = new KeyValue(world, null);
	    setSelectedItem(item);
	}

	/**
	 * This method is called when the user changes the selected item.
	 * The tileset of the map is changed.
	 */
	public void actionPerformed(ActionEvent ev) {

	    if (map == null) {
		return;
	    }

	    final int selectedWorld = getSelectedWorld();
	    final int currentWorld = map.getWorld();

	    if (currentWorld != selectedWorld) {

		try {
		    map.getHistory().doAction(new MapEditorAction() {

			private final Map map = MapPropertiesView.this.map;

			public void execute() throws MapException {
			    map.setWorld(selectedWorld);
			}

			public void undo() throws MapException {
			    map.setWorld(currentWorld);
			}
		    });
		}
		catch (ZSDXException ex) {
		    GuiTools.errorDialog(ex.getMessage());
		}
	    }
	}
    }

    /**
     * Component to choose the floor where this map is.
     */
    private class FloorField extends JComboBox implements ActionListener {

	/**
	 * Constructor.
	 */
	public FloorField() {
	    super();

	    addItem(new KeyValue(-100, "No floor"));
	    for (int i = -16; i <= 15; i++) {
		addItem(new KeyValue(i, "Floor " + i));
	    }
	    addItem(new KeyValue(-99, "Unknown floor '?'"));

	    addActionListener(this);
	}

	/**
	 * This function is called when the map is changed.
	 * The selection is updated.
	 */
	public void update(Observable o) {

	    if (map != null) {

		int currentFloor = map.getFloor();
		int selectedFloor = getSelectedFloor();

		if (selectedFloor != currentFloor) {
		    setSelectedFloor(currentFloor);
		}
		setEnabled(!map.isInOutsideWorld());
		floorLabel.setEnabled(!map.isInOutsideWorld());
	    }
	    else {
		setEnabled(false);
		floorLabel.setEnabled(true);
	    }
	}
	
	/**
	 * Returns the dungeon currently selected.
	 * @return the dungeon currently selected (may be -1)
	 */
	public int getSelectedFloor() {
	    KeyValue item = (KeyValue) getSelectedItem();
	    return Integer.parseInt(item.getKey());
	}

	/**
	 * Selects a dungeon in the combo box.
	 * @param dungeon the dungeon to make selected (may be -1)
	 */
	public void setSelectedFloor(int dungeon) {
	    KeyValue item = new KeyValue(dungeon, null);
	    setSelectedItem(item);
	}

	/**
	 * This method is called when the user changes the selected item.
	 * The tileset of the map is changed.
	 */
	public void actionPerformed(ActionEvent ev) {

	    if (map == null) {
		return;
	    }

	    final int selectedFloor = getSelectedFloor();
	    final int currentFloor = map.getFloor();

	    if (currentFloor != selectedFloor) {
		
		try {
		    map.getHistory().doAction(new MapEditorAction() {

			private final Map map = MapPropertiesView.this.map;

			public void execute() throws MapException {
			    map.setFloor(selectedFloor);
			}

			public void undo() throws MapException {
			    map.setFloor(currentFloor);
			}
		    });
		}
		catch (ZSDXException ex) {
		    GuiTools.errorDialog(ex.getMessage());
		}
	    }
	}
    }

    /**
     * Component to change the map location.
     */
    private class LocationField extends CoordinatesField implements ChangeListener {

	/**
	 * Constructor.
	 */
	public LocationField() {
	    super();
	    setMinimum(0, 0);
	    setStepSize(8, 8);
	    
	    addChangeListener(this);
	}

	/**
	 * This method is called when the user changes the value of this field.
	 */
	public void stateChanged(ChangeEvent ev) {

	    final Point selectedLocation = getCoordinates();
	    final Point currentLocation = map.getLocation();

	    try {
		if (hasChanged(ev, currentLocation.x, currentLocation.y)) {

		    map.getHistory().doAction(new MapEditorAction() {

			private final Map map = MapPropertiesView.this.map;

			public void execute() {
			    map.setLocation(selectedLocation);
			}

			public void undo() {
			    map.setLocation(currentLocation);
			}
		    });
		}
	    }
	    catch (NumberFormatException ex) {

	    }
	    catch (ZSDXException ex) {
		GuiTools.errorDialog("Cannot change the map location: " + ex.getMessage());
	    }
	    update(map);
	}

	/**
	 * This function is called when the map is changed.
	 * The component is updated.
	 */
	public void update(Observable o) {

	    if (map != null) {
		setEnabled(true);
		setCoordinates(map.getLocation());
	    }
	    else {
		setEnabled(false);
	    }
	}
    }

    /**
     * Component to enable or disable the small keys in this map.
     */
    private class EnableSmallKeysField extends JCheckBox implements ActionListener {

	/**
	 * Constructor.
	 */
	public EnableSmallKeysField() {
	    super("Enable small keys:");
	    addActionListener(this);
	    update((Map) null);
	}

	/**
	 * This method is called when the user changes the value of this field.
	 */
	public void actionPerformed(ActionEvent ev) {

	    if (map.isInDungeon()) {
		update(map);
		return;
	    }

	    final int currentSmallKeyVariable = map.getSmallKeysVariable();
	    final boolean currentlyEnabled = (currentSmallKeyVariable != -1);

	    try {
		map.getHistory().doAction(new MapEditorAction() {

		    private final Map map = MapPropertiesView.this.map;

		    public void execute() throws MapException {
			if (currentlyEnabled) { // uncheck the box
			    map.setSmallKeysVariable(-1);
			}
			else { // check the box
			    map.setSmallKeysVariable(smallKeysVariableField.getNumber());
			}
		    }

		    public void undo() throws MapException {
			if (currentlyEnabled) { // undo unchecking
			    map.setSmallKeysVariable(currentSmallKeyVariable);
			}
			else { // undo checking the box
			    map.setSmallKeysVariable(-1);
			}
		    }
		});
	    }
	    catch (ZSDXException ex) {
		GuiTools.errorDialog("Cannot change the keys variable: " + ex.getMessage());
	    }
	    update(map);
	}

	/**
	 * This function is called when the map is changed.
	 * The component is updated.
	 */
	public void update(Observable o) {

	    if (map != null) {
		setSelected(map.getSmallKeysVariable() != -1);
		setEnabled(true);
	    }
	    else {
		setSelected(false);
		setEnabled(false);
	    }
	}
    }

    /**
     * Component to choose the variable where the small keys are saved for this map.
     */
    private class SmallKeysVariableField extends JPanel implements ChangeListener {

	private JLabel label;
	private NumberChooser numberChooser;

	/**
	 * Constructor.
	 */
	public SmallKeysVariableField() {
	    super(new BorderLayout());
	    
	    numberChooser = new NumberChooser();
	    numberChooser.addChangeListener(this);
	    add(numberChooser, BorderLayout.EAST);
	    label = new JLabel(" saved in variable ");
	    add(label, BorderLayout.CENTER);
	    update((Map) null);
	}

	public int getNumber() {
	    return numberChooser.getNumber();
	}

	public void setEnabled(boolean enable) {
	    super.setEnabled(enable);
	    label.setEnabled(enable);
	    numberChooser.setEnabled(enable);
	}

	/**
	 * This function is called when the map is changed.
	 * The component is updated.
	 */
	public void update(Observable o) {

	    if (map != null) {
		numberChooser.setNumber(map.getSmallKeysVariable());
		setEnabled(map.getSmallKeysVariable() != -1 && !map.isInDungeon());
	    }
	    else {
		setEnabled(false);
	    }
	}

	/**
	 * This method is called when the user changes the value of this field.
	 */
	public void stateChanged(ChangeEvent ev) {

	    if (map.isInDungeon()) {
		update(map);
		return;
	    }

	    final int currentSmallKeyVariable = map.getSmallKeysVariable();
	    final int selectedSmallKeyVariable = enableSmallKeysField.isSelected() ? numberChooser.getNumber() : -1;

	    try {
		map.getHistory().doAction(new MapEditorAction() {

		    private final Map map = MapPropertiesView.this.map;

		    public void execute() throws MapException {
			map.setSmallKeysVariable(selectedSmallKeyVariable);
		    }

		    public void undo() throws MapException {
			map.setSmallKeysVariable(currentSmallKeyVariable);
		    }
		});
	    }
	    catch (ZSDXException ex) {
		GuiTools.errorDialog("Cannot change the keys variable: " + ex.getMessage());
	    }
	    update(map);
	}
    }

    /**
     * Component to change the tileset associated to the map.
     */
    private class TilesetField extends ResourceChooser implements ActionListener {

	/**
	 * Constructor.
	 */
	public TilesetField() {
	    super(ResourceType.TILESET, true);
	    addActionListener(this);
	    update((Map) null);
	}

	/**
	 * This function is called when the map is changed.
	 * The selection is updated.
	 */
	public void update(Observable o) {

	    if (map != null) {

		String currentTilesetId = map.getTilesetId();
		String selectedTilesetId = getSelectedId();

		if (!selectedTilesetId.equals(currentTilesetId)) {
		    setSelectedId(currentTilesetId);
		}
		setEnabled(true);
	    }
	    else {
		setEnabled(false);
	    }
	}

	/**
	 * This method is called when the user changes the selected item.
	 * The tileset of the map is changed.
	 */
	public void actionPerformed(ActionEvent ev) {

	    if (map == null) {
		return;
	    }

	    String selectedTilesetId = getSelectedId();
	    String currentTilesetId = map.getTilesetId();

	    if (!currentTilesetId.equals(selectedTilesetId)) {

		try {
		    map.getHistory().doAction(new ActionChangeTileset(map, selectedTilesetId));
			
		    if (map.badTiles()) {
			GuiTools.warningDialog("Some tiles of the map have been removed because they don't exist in this tileset.");
		    }
		}
		catch (ZSDXException ex) {
		    GuiTools.errorDialog("Cannot load the tileset '" + selectedTilesetId + "': " + ex.getMessage());		
		}
	    }
	}
    }

    /**
     * Component to change the tileset associated to the map.
     */
    private class MusicField extends MusicChooser implements ActionListener {

	/**
	 * Constructor.
	 */
	public MusicField() {
	    super();
	    addActionListener(this);
	    
	    update((Map) null);
	}

	/**
	 * This function is called when the map is changed.
	 * Then the selection is updated.
	 */
	public void update(Observable o) {

	    if (map != null) {
		setEnabled(true);

		// select the music
		String selectedMusic = getSelectedId();
		String currentMusic = map.getMusic();

		if (!selectedMusic.equals(currentMusic)) {
		    setSelectedId(currentMusic);
		}
	    }
	    else {
		setEnabled(false);
	    }
	}
	
	/**
	 * This method is called when the user changes the selected item.
	 * The music of the map is changed.
	 */
	public void actionPerformed(ActionEvent ev) {
	    if (map == null) {
		return;
	    }

	    String music = getSelectedId();
	    
	    if (!music.equals(map.getMusic())) {

		try {
		    map.getHistory().doAction(new ActionChangeMusic(map, music));
		}
		catch (ZSDXException ex) {
		    GuiTools.errorDialog("Cannot change the background music: " + ex.getMessage());
		}
	    }
	}
    }
}
