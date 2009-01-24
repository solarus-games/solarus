package zsdx.gui;

import javax.swing.*;
import zsdx.entities.*;

/**
 * A combo box to select a type of destructible item.
 */
public class EnemyChooser extends JComboBox {

    /**
     * Creates a new destructible item type chooser.
     */
    public EnemyChooser() {
	super();

	for (Enemy.Subtype subtype: Enemy.Subtype.values()) {
	    addItem(subtype.name());
	}
    }

    /**
     * Returns the subtype of enemy currently selected.
     * @return the subtype of enemy currently selected
     */
    public Enemy.Subtype getEnemySubtype() {
	return Enemy.Subtype.valueOf((String) getSelectedItem());
    }

    /**
     * Sets the subtype of enemy selected.
     * @param type the subtype of enemy to make selected
     */
    public void setEnemySubtype(Enemy.Subtype subtype) {
	setSelectedItem(subtype.name());
    }

    /**
     * Sets the subtype of enemy selected.
     * @param type the subtype of enemy to make selected
     */
    public void setEnemySubtype(int subtype) {
	setSelectedItem(Enemy.Subtype.values()[subtype]);
    }
}
