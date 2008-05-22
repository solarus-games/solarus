package zsdx.gui;

import javax.swing.*;

/**
 * This class provides some tools for the graphical components.
 */
public class GuiTools {

    /**
     * You don't need to instanciate this class.
     */
    private GuiTools() {

    }

    /**
     * Tries to set a nice look and feel.
     */
    public static void setLookAndFeel() {

	// set Windows look and feel by default
	try {
	    UIManager.setLookAndFeel("com.sun.java.swing.plaf.windows.WindowsLookAndFeel");
	    return;
	}
	catch (Exception e) {
	}

	// try Mac OS
	try {
	    UIManager.setLookAndFeel("it.unitn.ing.swing.plaf.macos.MacOSLookAndFeel");
	    return;
	}
	catch (Exception e) {
	}

	// otherwise, try GTK
// 	try {
// 	    UIManager.setLookAndFeel("com.sun.java.swing.plaf.gtk.GTKLookAndFeel");
// 	    return;
// 	}
// 	catch (Exception e) {
// 	}
    }

    /**
     * Shows a dialog box with an error message.
     * @param message message the error message to show
     */
    public static void errorDialog(String message) {

	JOptionPane.showMessageDialog(null,
				      message,
				      "Error",
				      JOptionPane.ERROR_MESSAGE);
    }
    
    /**
     * Shows a dialog box with a warning message.
     * @param message message the warning message to show
     */
    public static void warningDialog(String message) {

	JOptionPane.showMessageDialog(null,
				      message,
				      "Warning",
				      JOptionPane.WARNING_MESSAGE);
    }
    
    /**
     * Shows a dialog box with a question and the "yes" and "no" options.
     * @param message message the question to show
     * @return true if the user chose "yes"
     */
    public static boolean yesNoDialog(String message) {

	int result = JOptionPane.showConfirmDialog(null,
		message,
		"Question",
		JOptionPane.YES_NO_OPTION);
	
	return (result == JOptionPane.YES_OPTION);
    }
    
    /**
     * Rounds an integer number to the closest multiple of 8.
     * This method is useful to handle the clicks on the game components.
     * @param x an integer (can be negative)
     * @return the multiple of 8 the closest to the parameter 
     */
    public static int round8(int x) {
	return roundN(x, 8);
    }
    
    /**
     * Rounds an integer number to the closest multiple of n.
     * This method is useful to handle the clicks on the game components.
     * @param x an integer (can be negative)
     * @param n a stepsize
     * @return the multiple of n the closest to x 
     */
    public static int roundN(int x, int n) {
	return (x + (n / 2) * Integer.signum(x)) / n * n;
    }
}
