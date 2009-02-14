package zsdx.gui;

import javax.swing.*;
import zsdx.entities.Enemy.Subtype;

/**
 * A combo box to select a subtype of enemy.
 */
public class EnemyChooser extends JComboBox {

    /**
     * Creates a new destructible item type chooser.
     */
    public EnemyChooser() {
	super();

	for (Subtype subtype: Subtype.values()) {
	    addItem(subtype.name());
	}
    }

    /**
     * Returns the subtype of enemy currently selected.
     * @return the subtype of enemy currently selected
     */
    public Subtype getEnemySubtype() {
	return Subtype.valueOf((String) getSelectedItem());
    }

    /**
     * Sets the subtype of enemy selected.
     * @param type the subtype of enemy to make selected
     */
    public void setEnemySubtype(Subtype subtype) {
	setSelectedItem(subtype.name());
    }
}
