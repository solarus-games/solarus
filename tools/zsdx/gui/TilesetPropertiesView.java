package zsdx.gui;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import java.util.*;
import zsdx.entities.*;

/**
 * This components shows information about a tileset : the name
 * and the background color.
 */
public class TilesetPropertiesView extends JPanel implements Observer {

    /**
     * The tileset observed.
     */
    private Tileset tileset;
    
    // components
    private JLabel tilesetIdView;
    private TilesetNameView tilesetNameView;
    private JLabel tilesetNbTilesView;
    private TilesetBackgroundColorView tilesetBackgroundColorView;

    /**
     * Constructor.
     */
    public TilesetPropertiesView() {
	super(new GridBagLayout());

	setBorder(BorderFactory.createTitledBorder("Tileset properties"));

	GridBagConstraints constraints = new GridBagConstraints();
	constraints.insets = new Insets(5, 5, 5, 5); // margins
	constraints.anchor = GridBagConstraints.LINE_START; // alignment of the components

	// tileset id
	constraints.gridx = 0;
	constraints.gridy = 0;
	add(new JLabel("Tileset id"), constraints);

	// tileset name
	constraints.gridy++;
	add(new JLabel("Tileset name"), constraints);

	// number of tiles
	constraints.gridy++;
	add(new JLabel("Number of tiles"), constraints);

	// background color
	constraints.gridy++;
	add(new JLabel("Background color"), constraints);

	constraints.weightx = 1;
	constraints.gridx = 1;
	constraints.gridy = 0;
	tilesetIdView = new JLabel();
	add(tilesetIdView, constraints);

       	constraints.gridy++;
	tilesetNameView = new TilesetNameView();
	add(tilesetNameView, constraints);

       	constraints.gridy++;
	tilesetNbTilesView = new JLabel(); 
	add(tilesetNbTilesView, constraints);

	constraints.gridy++;
	tilesetBackgroundColorView = new TilesetBackgroundColorView();
	add(tilesetBackgroundColorView, constraints);
    }

    /**
     * Sets the tileset observed.
     */
    public void setTileset(Tileset tileset) {
	if (this.tileset != null) {
	    this.tileset.deleteObserver(this);
	}

	this.tileset = tileset;
	
	if (tileset != null) {
	    tileset.addObserver(this);
	}
	update(tileset, null);
    }
    
    /**
     * This function is called when the tileset is changed.
     * @param o the tileset, or null if no tileset is set
     * @param obj not used
     */
    public void update(Observable o, Object obj) {

	// update the elementary components here
	if (tileset == null) {
	    tilesetIdView.setText("");
	    tilesetNbTilesView.setText("");
	}
	else {
	    tilesetIdView.setText(tileset.getId());
	    tilesetNbTilesView.setText(Integer.toString(tileset.getNbTiles()));
	}
	
	// tell the complex components to update themselves
	tilesetNameView.update(tileset);
	tilesetBackgroundColorView.update(tileset);
    }

    /**
     * Component to change the name of the tileset.
     */
    private class TilesetNameView extends JPanel {

	// subcomponents
	private JTextField textFieldName;
	private JButton buttonSet;	

	/**
	 * Constructor.
	 */
	public TilesetNameView() {
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

			String name = textFieldName.getText();
			tileset.setName(name);

			update(tileset);
		    }
		};

	    buttonSet.addActionListener(listener);
	    textFieldName.addActionListener(listener);

	    add(textFieldName);
	    add(Box.createRigidArea(new Dimension(5, 0)));
	    add(buttonSet);
	}

	/**
	 * This function is called when the tileset is changed.
	 * The component is updated.
	 * @param o the tileset, or null if no tileset is loaded
	 */
	public void update(Observable o) {
	    
	    if (o == null) {
		textFieldName.setEnabled(false);
		buttonSet.setEnabled(false);
		textFieldName.setText(null);
	    }
	    else {
		textFieldName.setEnabled(true);
		buttonSet.setEnabled(true);		
		textFieldName.setText(tileset.getName());
	    }
	}
    }
    
    /**
     * Component to change the background color of the tileset.
     */
    private class TilesetBackgroundColorView extends JComponent {
	
	/**
	 * Constructor.
	 */
	public TilesetBackgroundColorView() {
	    super();
	    setMinimumSize(new Dimension(20,20));
	    setPreferredSize(new Dimension(20,20));
	    setMaximumSize(new Dimension(20,20));

	    addMouseListener(new MouseAdapter() {

		    public void mouseClicked(MouseEvent mouseEvent) {

			if (mouseEvent.getButton() == MouseEvent.BUTTON1) {
			    ColorChooserDialog colorChooserDialog = new ColorChooserDialog();
			    Color color = colorChooserDialog.display(tileset.getBackgroundColor());
			    if (color != null) {
				tileset.setBackgroundColor(color);
			    }
			}
		    }
		});
	}
	
	/**
	 * This function is called when the tileset is changed.
	 * @param o the tileset, or null if no tileset is loaded
	 */
	public void update(Observable o) {
	    repaint();
	}

	/**
	 * Draws the component.
	 * @param g graphic context
	 */
	public void paint(Graphics g) {
	    // the tileset is null if no tileset is open yet
	    if (tileset != null) {
		g.setColor(Color.BLACK);
		g.drawRect(0, 0, 19, 19);
		g.setColor(tileset.getBackgroundColor());
		g.fillRect(1, 1, 18, 18);
	    }
	}
    }
}
