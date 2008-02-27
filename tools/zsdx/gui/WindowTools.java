package zsdx.gui;

import javax.swing.*;

/**
 * This class provides some tools for the graphical components.
 */
public class WindowTools {

    /**
     * You don't need to instanciate this class.
     */
    private WindowTools() {

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
		"Warning",
		JOptionPane.YES_NO_OPTION);
	
	return (result == JOptionPane.YES_OPTION);
    }
}
