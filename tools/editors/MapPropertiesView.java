package editors;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
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
    private JLabel mapNameView;
    private JLabel mapNbTilesView;
    private MapSizeView mapSizeView;
    private MapTilesetView mapTilesetView;
    private MapMusicView mapMusicView;
    private MapBackgroundColorView mapBackgroundColorView;

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

	// map name
	constraints.gridx = 0;
	constraints.gridy = 0;
	add(new JLabel("Map name"), constraints);

	// number of tiles
	constraints.gridy++;
	add(new JLabel("Number of tiles"), constraints);

	// size
	constraints.gridy++;
	add(new JLabel("Size"), constraints);

	// tileset
	constraints.gridy++;
	add(new JLabel("Tileset"), constraints);
	
	// music
	constraints.gridy++;
	add(new JLabel("Music"), constraints);

	// background color
	constraints.gridy++;
	add(new JLabel("Background color"), constraints);

	constraints.gridx = 1;
	constraints.gridy = 0;
	mapNameView = new JLabel();
	add(mapNameView, constraints);

       	constraints.gridy++;
	mapNbTilesView = new JLabel(); 
	add(mapNbTilesView, constraints);

       	constraints.gridy++;
	mapSizeView = new MapSizeView(); 
	add(mapSizeView, constraints);

	constraints.gridy++;
	mapTilesetView = new MapTilesetView();
       	add(mapTilesetView, constraints);

	constraints.gridy++;
	mapMusicView = new MapMusicView();
	add(mapMusicView, constraints);

	constraints.gridy++;
	mapBackgroundColorView = new MapBackgroundColorView();
	add(mapBackgroundColorView, constraints);
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
	mapNameView.setText(map.getName());
	mapNbTilesView.setText(Integer.toString(map.getNbTiles()));
	mapSizeView.update(o);
	mapTilesetView.update(o);
	mapMusicView.update(o);
	mapBackgroundColorView.update(o);
    }

    // components for the editable properties

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
			    map.setSize(size);
			}
			catch (NumberFormatException e) {
			    JOptionPane.showMessageDialog(null,
							  "The width and the height of the map must be multiples of 8.",
							  "Error",
							  JOptionPane.ERROR_MESSAGE);
			    update(map);
			}
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
    private class MapTilesetView extends JComboBox implements ActionListener {

	/**
	 * The tilesets, loaded when the JComboBox is filled.
	 */
	private Tileset[] tilesets;

	/**
	 * Constructor.
	 */
	public MapTilesetView() {

	    super();
	    setEnabled(false);
	    addActionListener(this);
	    update(Configuration.getInstance());
	}

	/**
	 * This function is called when the map or the configuration is changed.
	 * If it is the configuration, the tileset list is reloaded using ZSDX root path.
	 * If it is the map, then the selection is updated.
	 */
	public void update(Observable o) {

	    if (o instanceof Configuration) {
		removeAllItems();
		addItem("");
		
		String tilesetPath = Configuration.getInstance().getDefaultTilesetPath();
		File[] tilesetFiles = FileTools.getFilesWithExtension(tilesetPath, "zsd");

		if (tilesetFiles == null) {
		    return;
		}

		tilesets = new Tileset[tilesetFiles.length];
		
		File tilesetFile;
		for (int i = 0; i < tilesetFiles.length; i++) {
		    tilesetFile = tilesetFiles[i];
		    //	System.out.println("file " + i + ": " + tilesetFile);
		    
		    String name = null;
		    try {
			tilesets[i] = Tileset.load(tilesetFile);
			name = tilesets[i].getName();
		    }
		    catch (IOException e) {
			System.out.println("Cannot load the tileset from file '" + tilesetFile.getName() + ": " + e.getMessage());
		    }
		    addItem(name);
		}
	    }

	    // select the tileset
	    if (map != null) {
		setEnabled(true);
		String tilesetName = map.getTilesetName();
		
		if (tilesetName != null && !tilesetName.equals(getSelectedItem())) {
		    setSelectedItem(tilesetName);
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

	    String tilesetName = (String) getSelectedItem();
	    
	    if (tilesetName != null) {

		if (tilesetName.length() == 0) {
		    tilesetName = null;
		}

		try {
		    map.setTileset(tilesetName);
		}
		catch (IOException e) {
		    System.out.println("Cannot load the tileset '" + tilesetName + ": " + e.getMessage());		
		}
		catch (TilesetException e) {
		    System.out.println("Cannot change the tileset: " + e.getMessage());
		}
	    }
	}
    }

    /**
     * Component to change the tileset associated to the map.
     */
    private class MapMusicView extends JComboBox implements ActionListener {

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
	 * If it is the map, then the selection is updated.
	 */
	public void update(Observable o) {

	    if (o instanceof Configuration) {

		removeAllItems();
		addItem("");
		
		String musicPath = Configuration.getInstance().getDefaultMusicPath();
		File[] musicFiles = FileTools.getFilesWithExtension(musicPath, "it");
		
		if (musicFiles == null) {
		    return;
		}

		for (int i = 0; i < musicFiles.length; i++) {
		    addItem(FileTools.getFileNameWithoutExtension(musicFiles[i]));
		}
	    }

	    // select the music
	    if (map != null) {

		setEnabled(true);
		String music = map.getMusic();
		
		if (music != null) {
		    setSelectedItem(music);
		}
		else if (getItemCount() > 0) {
		    setSelectedIndex(0);
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

	    String music = (String) getSelectedItem();
	    
	    if (music != null) {

		if (music.length() == 0) {
		    music = null;
		}
		map.setMusic(music);
	    }
	}
    }

    /**
     * Component to change the background color of the map.
     */
    private class MapBackgroundColorView extends JComponent {
	
	/**
	 * Constructor.
	 */
	public MapBackgroundColorView() {
	    super();
	    setMinimumSize(new Dimension(20,20));
	    setPreferredSize(new Dimension(20,20));
	    setMaximumSize(new Dimension(20,20));

	    addMouseListener(new MouseAdapter() {

		    public void mouseClicked(MouseEvent mouseEvent) {

			if (mouseEvent.getButton() == MouseEvent.BUTTON1) {
			    ColorChooserDialog colorChooserDialog = new ColorChooserDialog();
			    Color color = colorChooserDialog.display(map.getBackgroundColor());
			    if (color != null) {
				map.setBackgroundColor(color);
			    }
			}
		    }
		});
	}
	
	/**
	 * This function is called when the map or the configuration is changed.
	 * If it is the map, the component is updated.
	 */
	public void update(Observable o) {

	    if (o instanceof Map) {
		repaint();
	    }
	}

	/**
	 * Draws the component.
	 * @param g graphic context
	 */
	public void paint(Graphics g) {
	    // the map is null if no map is open yet
	    if (map != null) {
		g.setColor(Color.BLACK);
		g.drawRect(0, 0, 19, 19);
		g.setColor(map.getBackgroundColor());
		g.fillRect(1, 1, 18, 18);
	    }
	}
    }
}
