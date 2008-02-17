package zsdx;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

import zsdx.map_editor_actions.*;

import java.util.*;
import java.io.*;

/**
 * This component shows the properties of a map and allows to edit them.
 */
public class MapPropertiesView extends JPanel implements Observer {

    /**
     * The map observed.
     */
    private Map map;

    // components
    private JLabel mapIdView;
    private MapNameView mapNameView;
    private JLabel mapNbTilesView;
    private JLabel mapNbActiveEntitiesView;
    private MapSizeView mapSizeView;
    private MapTilesetView mapTilesetView;
    private MapMusicView mapMusicView;

    /**
     * Constructor.
     */
    public MapPropertiesView() {
	super(new GridBagLayout());

	Configuration.getInstance().addObserver(this);
	setBorder(BorderFactory.createTitledBorder("Map properties"));

	GridBagConstraints constraints = new GridBagConstraints();
	constraints.insets = new Insets(5, 5, 5, 5); // margins
	constraints.anchor = GridBagConstraints.LINE_START;

	// map id
	constraints.gridx = 0;
	constraints.gridy = 0;
	add(new JLabel("Map id"), constraints);

	// map name
	constraints.gridy++;
	add(new JLabel("Map name"), constraints);

	// number of tiles
	constraints.gridy++;
	add(new JLabel("Tiles"), constraints);

	// number of tiles
	constraints.gridy++;
	add(new JLabel("Active entities"), constraints);

	// size
	constraints.gridy++;
	add(new JLabel("Size"), constraints);

	// tileset
	constraints.gridy++;
	add(new JLabel("Tileset"), constraints);
	
	// music
	constraints.gridy++;
	add(new JLabel("Music"), constraints);

	constraints.weightx = 1;
	constraints.gridx = 1;
	constraints.gridy = 0;
	mapIdView = new JLabel();
	add(mapIdView, constraints);

       	constraints.gridy++;
	mapNameView = new MapNameView();
	add(mapNameView, constraints);

       	constraints.gridy++;
	mapNbTilesView = new JLabel(); 
	add(mapNbTilesView, constraints);

       	constraints.gridy++;
	mapNbActiveEntitiesView = new JLabel(); 
	add(mapNbActiveEntitiesView, constraints);

       	constraints.gridy++;
	mapSizeView = new MapSizeView(); 
	add(mapSizeView, constraints);

	constraints.gridy++;
	mapTilesetView = new MapTilesetView();
       	add(mapTilesetView, constraints);

	constraints.gridy++;
	mapMusicView = new MapMusicView();
	add(mapMusicView, constraints);
    }

    /**
     * Sets the observed map.
     */
    public void setMap(Map map) {
	if (this.map != null) {
	    this.map.deleteObserver(this);
	}

	this.map = map;
	map.addObserver(this);

	update(map, null);
    }
    
    /**
     * This function is called when the map or the configuration is changed.
     */
    public void update(Observable o, Object obj) {
	mapIdView.setText(map.getId());
	mapNameView.update(o);
	mapNbTilesView.setText(Integer.toString(map.getNbTiles()));
	mapNbActiveEntitiesView.setText(Integer.toString(map.getNbInteractiveEntities() + map.getNbMovingEntities()));
	mapSizeView.update(o);
	mapTilesetView.update(o);
	mapMusicView.update(o);
    }

    // components for the editable properties

    /**
     * Component to change the name of the map.
     */
    private class MapNameView extends JPanel {

	// subcomponents
	private JTextField textFieldName;
	private JButton buttonSet;	

	/**
	 * Constructor.
	 */
	public MapNameView() {
	    super();
	    setLayout(new BoxLayout(this, BoxLayout.X_AXIS));

	    textFieldName = new JTextField(10);

	    Dimension size = new Dimension(120, 25);
	    textFieldName.setMinimumSize(size);

	    buttonSet = new JButton("Set");

	    textFieldName.setEnabled(false);
	    buttonSet.setEnabled(false);

	    ActionListener listener = new ActionListener() {
		    public void actionPerformed(ActionEvent ev) {

			try {
			    String name = textFieldName.getText();
			    map.getHistory().doAction(new ActionRenameMap(map, name));
			}
			catch (ZSDXException ex) {
			    WindowTools.errorDialog("Cannot change the map name: " + ex.getMessage());
			}
			update(map);
		    }
		};

	    buttonSet.addActionListener(listener);
	    textFieldName.addActionListener(listener);

	    add(textFieldName);
	    add(Box.createRigidArea(new Dimension(5, 0)));
	    add(buttonSet);
	}

	/**
	 * This function is called when the map or the configuration is changed.
	 * If it is the map, the component is updated.
	 */
	public void update(Observable o) {

	    if (o instanceof Map) {
		textFieldName.setEnabled(true);
		buttonSet.setEnabled(true);
		
		textFieldName.setText(map.getName());
	    }
	}
    }
    
    /**
     * Component to change the map size.
     */
    private class MapSizeView extends JPanel {

	// subcomponents
	private JTextField textFieldWidth;
	private JTextField textFieldHeight;
	private JButton buttonSet;

	/**
	 * Constructor.
	 */
	public MapSizeView() {
	    super();
	    setLayout(new BoxLayout(this, BoxLayout.X_AXIS));

	    textFieldWidth = new JTextField(3);
	    textFieldHeight = new JTextField(3);

	    Dimension size = new Dimension(40, 25);
	    textFieldWidth.setMinimumSize(size);
	    textFieldHeight.setMinimumSize(size);
// 	    textFieldWidth.setMaximumSize(size);
// 	    textFieldHeight.setMaximumSize(size);

	    buttonSet = new JButton("Set");

	    textFieldWidth.setEnabled(false);
	    textFieldHeight.setEnabled(false);
	    buttonSet.setEnabled(false);

	    ActionListener listener = new ActionListener() {
		    public void actionPerformed(ActionEvent ev) {

			try {
			    int width = Integer.parseInt(textFieldWidth.getText());
			    int height = Integer.parseInt(textFieldHeight.getText());
			    Dimension size = new Dimension(width, height);
			    if (!size.equals(map.getSize())) {
				map.getHistory().doAction(new ActionChangeMapSize(map, size));
			    }
			}
			catch (NumberFormatException ex) {

			}
			catch (ZSDXException ex) {
			    WindowTools.errorDialog("Cannot change the map size: " + ex.getMessage());
			}
			update(map);
		    }
		};

	    buttonSet.addActionListener(listener);
	    textFieldWidth.addActionListener(listener);
	    textFieldHeight.addActionListener(listener);

	    add(textFieldWidth);
	    add(Box.createRigidArea(new Dimension(5, 0)));
	    add(new JLabel("x"));
	    add(Box.createRigidArea(new Dimension(5, 0)));
	    add(textFieldHeight);
	    add(Box.createRigidArea(new Dimension(5, 0)));
	    add(buttonSet);
	}

	/**
	 * This function is called when the map or the configuration is changed.
	 * If it is the map, the component is updated.
	 */
	public void update(Observable o) {

	    if (o instanceof Map) {
		textFieldWidth.setEnabled(true);
		textFieldHeight.setEnabled(true);
		buttonSet.setEnabled(true);
		
		textFieldWidth.setText(Integer.toString(map.getSize().width));
		textFieldHeight.setText(Integer.toString(map.getSize().height));
	    }
	}
    }

    /**
     * Component to change the tileset associated to the map.
     */
    private class MapTilesetView extends ResourceChooser implements ActionListener {

	/**
	 * Constructor.
	 */
	public MapTilesetView() {

	    super(ResourceDatabase.RESOURCE_TILESET, true);
	    setEnabled(false);
	    addActionListener(this);
	}

	/**
	 * This function is called when the map or the configuration is changed.
	 * If it is the map, then the selection is updated.
	 */
	public void update(Observable o) {

	    if (o instanceof Map) {

		setEnabled(true);
		String currentTilesetId = map.getTilesetId();
		String selectedTilesetId = getSelectedId();
		
		if (!selectedTilesetId.equals(currentTilesetId)) {
		    setSelectedId(currentTilesetId);
		}
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
			WindowTools.warningDialog("Some tiles of the map have been removed because they don't exist in this tileset.");
		    }
		}
		catch (ZSDXException ex) {
		    WindowTools.errorDialog("Cannot load the tileset '" + selectedTilesetId + "': " + ex.getMessage());		
		}
	    }
	}
    }

    /**
     * Component to change the tileset associated to the map.
     */
    private class MapMusicView extends MusicChooser implements ActionListener {

	/**
	 * Constructor.
	 */
	public MapMusicView() {

	    super();
	    setEnabled(false);
	    addActionListener(this);
	    update(Configuration.getInstance());
	}

	/**
	 * This function is called when the map or the configuration is changed.
	 * If it is the configuration, the music list is reloaded using ZSDX root path.
	 * Then the selection is updated.
	 */
	public void update(Observable o) {

	    if (o instanceof Configuration) {
		super.update(o, null);
	    }

	    if (map != null) {
		setEnabled(true);

		// select the music
		String selectedMusic = getSelectedId();
		String currentMusic = map.getMusic();
		
		if (!selectedMusic.equals(currentMusic)) {
		    setSelectedId(currentMusic);
		}
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
		    WindowTools.errorDialog("Cannot change the background music: " + ex.getMessage());
		}
	    }
	}
    }
}
