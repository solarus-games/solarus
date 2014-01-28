/*
 * Copyright (C) 2006-2014 Christopho, Solarus - http://www.solarus-games.org
 *
 * Solarus Quest Editor is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Solarus Quest Editor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
package org.solarus.editor.gui;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;

import java.util.*;
import org.solarus.editor.*;
import org.solarus.editor.Map;
import org.solarus.editor.map_editor_actions.*;

/**
 * This component shows the properties of a map and allows to edit them.
 */
public class MapPropertiesView extends JPanel
    implements Observer, ProjectObserver {

    /**
     * The map observed.
     */
    private Map map;

    /**
     * The map editor panel this view belongs to.
     */
    private MapEditorPanel mapEditor;

    // Subcomponents.
    private JLabel idField;
    private NameField nameField;
    private SizeField sizeField;
    private WorldField worldField;
    private EnableFloorField enableFloorField;
    private FloorField floorField;
    private JLabel locationLabel;
    private LocationField locationField;
    private TilesetField tilesetField;
    private MusicField musicField;

    /**
     * Constructor.
     */
    public MapPropertiesView(MapEditorPanel mapEditor) {
        super(new GridBagLayout());
        this.mapEditor = mapEditor;

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
        floorField = new FloorField();
        enableFloorField = new EnableFloorField();
        leftConstraints.gridy++;
        add(enableFloorField, leftConstraints);
        rightConstraints.gridy++;
        rightConstraints.fill = GridBagConstraints.NONE;
        add(floorField, rightConstraints);
        
        // location
        leftConstraints.gridy++;
        locationLabel = new JLabel("Location in its world");
        add(locationLabel, leftConstraints);
        rightConstraints.gridy++;
        rightConstraints.fill = GridBagConstraints.NONE;
        locationField = new LocationField();
        add(locationField, rightConstraints);

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
            Project.removeProjectObserver(this);
        }

        this.map = map;

        if (this.map != null) {
            this.map.addObserver(this);
            Project.addProjectObserver(this);
        }
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
        enableFloorField.update(map);
        floorField.update(map);
        locationField.update(map);
        tilesetField.update(map);
        musicField.update(map);
    }

    // components for the editable properties

    /**
     * Component to change the name of the map.
     */
    private class NameField extends JTextField {

        private boolean updating = false;  // To avoid reentrant updates.

        /**
         * Constructor.
         */
        public NameField() {
            super(10);

            getDocument().addDocumentListener(new DocumentListener() {

                private void textChanged() {

                    if (!updating) {
                        try {
                            String name = getText();
                            map.setName(name);
                        }
                        catch (QuestEditorException ex) {
                            GuiTools.errorDialog("Cannot change the map name: " + ex.getMessage());
                        }
                    }
                }

                @Override
                public void removeUpdate(DocumentEvent e) {
                    textChanged();
                }
                
                @Override
                public void insertUpdate(DocumentEvent e) {
                    textChanged();
                }
                
                @Override
                public void changedUpdate(DocumentEvent e) {
                    textChanged();
                }
            });
        }

        /**
         * This function is called when the map is changed.
         * The component is updated.
         */
        public void update(Observable o) {

            updating = true;
            if (map != null) {
                if (!getText().equals(map.getName())) {
                    setText(map.getName());
                }
            }
            else {
                setText("");
            }
            updating = false;
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
                    catch (QuestEditorException ex) {
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
     * Component to change the world of the map.
     */
    private class WorldField extends JTextField {

        private boolean updating = false;  // To avoid reentrant updates.

        /**
         * Constructor.
         */
        public WorldField() {
            super(10);

            getDocument().addDocumentListener(new DocumentListener() {

                private void textChanged() {

                    if (!updating) {
                        try {
                            final String currentWorld = map.getWorld();
                            map.getHistory().doAction(new MapEditorAction() {

                                public void execute() throws MapException {
                                    String world = getText();
                                    if (world.isEmpty()) {
                                        world = null;
                                    }
                                    MapPropertiesView.this.map.setWorld(world);
                                }

                                public void undo() throws MapException {
                                    MapPropertiesView.this.map.setWorld(currentWorld);
                                }
                            });
                        }
                        catch (QuestEditorException ex) {
                            GuiTools.errorDialog("Cannot change the map's world: " + ex.getMessage());
                        }
                        update(map);
                    }
                }

                @Override
                public void removeUpdate(DocumentEvent e) {
                    textChanged();
                }
                
                @Override
                public void insertUpdate(DocumentEvent e) {
                    textChanged();
                }
                
                @Override
                public void changedUpdate(DocumentEvent e) {
                    textChanged();
                }
            });

            update((Map) null);
        }

        /**
         * This function is called when the map is changed.
         * The component is updated.
         */
        public void update(Observable o) {

            updating = true;
            if (map != null) {
                setEnabled(true);
                String world = map.getWorld();
                if (world == null) {
                    world = "";
                }
                if (!getText().equals(world)) {
                    setText(world);
                }
            }
            else {
                setEnabled(false);
                setText("");
            }
            updating = false;
        }
    }

    /**
     * Component to enable or disable the floor property in this map.
     */
    private class EnableFloorField extends JCheckBox implements ActionListener {

        /**
         * Constructor.
         */
        public EnableFloorField() {
            super("Set a floor:");
            addActionListener(this);
            update((Map) null);
        }

        /**
         * This method is called when the user changes the value of this field.
         */
        public void actionPerformed(ActionEvent ev) {

            final Integer currentFloor = map.getFloor();
            final boolean currentlyEnabled = (currentFloor != null);

            try {
                map.getHistory().doAction(new MapEditorAction() {
    
                    private final Map map = MapPropertiesView.this.map;
    
                    public void execute() throws MapException {
                        if (currentlyEnabled) { // uncheck the box
                            map.setFloor(null);
                        }
                        else { // check the box
                            map.setFloor(floorField.getNumber());
                        }
                    }
    
                    public void undo() throws MapException {
                        if (currentlyEnabled) { // undo unchecking
                            map.setFloor(currentFloor);
                        }
                        else { // undo checking the box
                            map.setFloor(null);
                        }
                    }
                });
            }
            catch (QuestEditorException ex) {
                GuiTools.errorDialog("Cannot change the floor: " + ex.getMessage());
            }
            update(map);
        }

        /**
         * This function is called when the map is changed.
         * The component is updated.
         */
        public void update(Observable o) {

            if (map != null) {
                setSelected(map.getFloor() != null);
                setEnabled(true);
            }
            else {
                setSelected(false);
                setEnabled(false);
            }
        }
    }

    /**
     * Component to choose the floor of this map.
     */
    private class FloorField extends NumberChooser implements ChangeListener {

        /**
         * Constructor.
         */
        public FloorField() {
            super(0, Integer.MIN_VALUE, Integer.MAX_VALUE);

            addChangeListener(this);
            update((Map) null);
        }

        /**
         * This function is called when the map is changed.
         * The component is updated.
         */
        public void update(Observable o) {

            if (map != null) {
                Integer floor = map.getFloor();
                if (floor != null) {
                    setNumber(floor);
                    setEnabled(true);
                }
                else {
                    setEnabled(false);
                }
            }
            else {
                setEnabled(false);
            }
        }

        /**
         * This method is called when the user changes the value of this field.
         */
        public void stateChanged(ChangeEvent ev) {

            final Integer currentFloor = map.getFloor();
            final Integer selectedFloor = enableFloorField.isSelected() ? getNumber() : null;

            if (selectedFloor != currentFloor
                    || selectedFloor == null
                    || currentFloor == null
                    || selectedFloor.intValue() != currentFloor.intValue()) {

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
              catch (QuestEditorException ex) {
                GuiTools.errorDialog("Cannot change the floor: " + ex.getMessage());
              }
            }
            update(map);
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
            catch (QuestEditorException ex) {
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
     * Component to change the tileset associated to the map.
     */
    private class TilesetField extends JPanel implements ActionListener {

        /**
         * The tileset selector.
         */
        private ResourceChooser tilesetChooser;

        /**
         * Constructor.
         */
        public TilesetField() {
            super();
            setLayout(new BoxLayout(this, BoxLayout.LINE_AXIS));

            tilesetChooser = new ResourceChooser(ResourceType.TILESET, true);
            tilesetChooser.setPreferredSize(new Dimension(100, 24));
            add(tilesetChooser);
            tilesetChooser.addActionListener(this);

            JButton refreshButton = new JButton(Project.getEditorImageIconOrEmpty("icon_refresh.png"));
            refreshButton.setPreferredSize(new Dimension(24, 24));
            refreshButton.setToolTipText("Reload tileset");
            refreshButton.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent ev) {

                    if (map == null) {
                        return;
                    }

                    String selectedTilesetId = tilesetChooser.getSelectedId();
                    if (selectedTilesetId.isEmpty()) {
                        return;
                    }

                    // Reload the tileset.
                    try {
                        map.setTileset("");
                        map.setTileset(selectedTilesetId);
                        if (map.badTiles()) {
                            GuiTools.warningDialog("Some tiles of the map have been removed because they don't exist in this tileset.");
                        }
                    }
                    catch (QuestEditorException ex) {
                        GuiTools.errorDialog("Cannot reload tileset '" + selectedTilesetId + "': " + ex.getMessage());
                    }
                }
            });
            add(refreshButton);


            JButton editButton = new JButton(Project.getEditorImageIconOrEmpty("icon_edit.png"));
            editButton.setPreferredSize(new Dimension(24, 24));
            editButton.setToolTipText("Edit tileset");
            editButton.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent ev) {

                    if (map == null) {
                        return;
                    }

                    String selectedTilesetId = tilesetChooser.getSelectedId();
                    if (!selectedTilesetId.isEmpty()) {
                        mapEditor.getMainWindow().openResourceElement(
                            ResourceType.TILESET, selectedTilesetId
                        );
                    }
                }
            });
            add(editButton);

            update((Map) null);
        }

        /**
         * This function is called when the map is changed.
         * The selection is updated.
         */
        public void update(Observable o) {

            if (map != null) {

                String currentTilesetId = map.getTilesetId();
                String selectedTilesetId = tilesetChooser.getSelectedId();

                if (!selectedTilesetId.equals(currentTilesetId)) {
                    tilesetChooser.setSelectedId(currentTilesetId);
                }
                tilesetChooser.setEnabled(true);
            }
            else {
                tilesetChooser.setEnabled(false);
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

            String selectedTilesetId = tilesetChooser.getSelectedId();
            String currentTilesetId = map.getTilesetId();

            if (!currentTilesetId.equals(selectedTilesetId)) {

                try {
                    map.getHistory().doAction(new ActionChangeTileset(map, selectedTilesetId));

                    if (map.badTiles()) {
                        GuiTools.warningDialog("Some tiles of the map have been removed because they don't exist in this tileset.");
                    }
                }
                catch (QuestEditorException ex) {
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
                catch (QuestEditorException ex) {
                    GuiTools.errorDialog("Cannot change the background music: " + ex.getMessage());
                }
            }
        }
    }

    @Override
    public void currentProjectChanged() {
    }

    @Override
    public void resourceElementAdded(ResourceType resourceType, String id) {
    }

    @Override
    public void resourceElementRemoved(ResourceType resourceType, String id) {
    }

    @Override
    public void resourceElementMoved(ResourceType resourceType, String oldId,
            String newId) {
    }

    @Override
    public void resourceElementRenamed(ResourceType resourceType, String id,
            String name) {
        if (resourceType == ResourceType.MAP && id.equals(map.getId())) {
            // This map has just been renamed.
            update(null, null);
        }
    }
}
