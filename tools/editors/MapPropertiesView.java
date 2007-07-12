package editors;

import java.awt.*;
import javax.swing.*;
import java.util.*;

/**
 * This component shows the properties of a map and allows to edit them.
 */
public class MapPropertiesView extends JPanel implements Observer {

    /**
     * The map observed.
     */
    private Map map;

    // components
    private JLabel labelMapName;

    /**
     * Constructor.
     */
    public MapPropertiesView() {
	super(new GridBagLayout());

	setBorder(BorderFactory.createTitledBorder("Map properties"));
	setMinimumSize(new Dimension(200, 0));

	GridBagConstraints constraints = new GridBagConstraints();
	constraints.insets = new Insets(5, 5, 5, 5); // margins
	constraints.anchor = GridBagConstraints.LINE_START; // alignment of the components

	// map name
	constraints.gridx = 0;
	constraints.gridy = 0;
	add(new JLabel("Map name:"), constraints);

	constraints.gridx = 0;
	constraints.gridx = 1;
	labelMapName = new JLabel();
	add(labelMapName, constraints);
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
     * This function is called when the map is changed.
     */
    public void update(Observable o, Object obj) {
	labelMapName.setText(map.getName());
    }
}
