package editors;

import java.awt.*;
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
     * Shows a dialog box with an error message.
     * @param message message the error message to show
     */
    public static void errorDialog(String message) {

	JOptionPane.showMessageDialog(null,
				      message,
				      "Error",
				      JOptionPane.ERROR_MESSAGE);
    }
    
}
