package zsdx.gui;

import javax.swing.*;
import java.lang.reflect.*;

/**
 * A combo box to select a value among the specified enumeration.
 * @param e the enumeration type returned and set by getValue() and setValue() respectively
 */
public class EnumerationChooser<E extends Enum<E>> extends JComboBox {

    /**
     * The enumeration displayed in this combo box.
     */
    private Class<E> enumeration;

    /**
     * Creates a new entity enumeration chooser.
     * @param enumeration the enumeration to display in this combo box
     */
    public EnumerationChooser(Class<E> enumeration) {
	super();

	this.enumeration = enumeration;

	try {
	    String[] humanNames = (String[]) enumeration.getField("humanNames").get(null);
	    Enum[] values = (Enum[]) enumeration.getMethod("values").invoke(null, (Object[]) null);
	    for (int i = 0; i < values.length; i++) {
		addItem(new KeyValue(values[i].name(), humanNames[i]));
	    }
	}
	catch (NoSuchFieldException ex) {
	    System.err.println("The field 'humanNames' is missing in enumeration " + enumeration.getName());
	    ex.printStackTrace();
	    System.exit(1);
	}
	catch (NoSuchMethodException ex) {
	    System.err.println("The method 'values' is missing in enumeration " + enumeration.getName());
	    ex.printStackTrace();
	    System.exit(1);
	}
	catch (IllegalAccessException ex) {
	    System.err.println("Cannot access a member in enumeration " + enumeration.getName() + ": ex.getMessage()");
	    ex.printStackTrace();
	    System.exit(1);
	}
	catch (InvocationTargetException ex) {
	    ex.getCause().printStackTrace();
	    System.exit(1);
	}
    }

    /**
     * Returns the value currently selected.
     * @return the value currently selected
     */
    public E getValue() {
	KeyValue item = (KeyValue) getSelectedItem();
	return E.valueOf(enumeration, item.getKey());
    }

    /**
     * Sets the value selected.
     * @param value the value to make selected
     */
    public void setValue(E value) {
	KeyValue item = new KeyValue(value.name(), null);
	setSelectedItem(item);
    }
}
