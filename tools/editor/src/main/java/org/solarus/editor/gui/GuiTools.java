/*
 * Copyright (C) 2006-2014 Christopho, Solarus - http://www.solarus-games.org
 *
 * Solarus Quest Editor is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Solarus Quest Editor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
package org.solarus.editor.gui;

import javax.swing.*;

/**
 * This class provides some tools for the graphical components.
 */
public class GuiTools {

    /**
     * You don't need to instantiate this class.
     */
    private GuiTools() {

    }

    /**
     * Tries to set a nice look and feel.
     */
    public static void setLookAndFeel() {

        // Try to set the Windows look and feel if available, otherwise
        // the default one is fine.
        try {
            UIManager.setLookAndFeel("com.sun.java.swing.plaf.windows.WindowsLookAndFeel");
        }
        catch (Exception ex) {
        }
    }

    /**
     * Shows a dialog box with an information message.
     * @param message the information message to show
     */
    public static void informationDialog(String message) {

        JOptionPane.showMessageDialog(null,
                                      message,
                                      "Info",
                                      JOptionPane.INFORMATION_MESSAGE);
    }

    /**
     * Shows a dialog box with an error message.
     * @param message the error message to show
     */
    public static void errorDialog(String message) {

        JOptionPane.showMessageDialog(null,
                                      message,
                                      "Error",
                                      JOptionPane.ERROR_MESSAGE);
    }

    /**
     * Shows a dialog box with a warning message.
     * @param message the warning message to show
     */
    public static void warningDialog(String message) {

        JOptionPane.showMessageDialog(null,
                                      message,
                                      "Warning",
                                      JOptionPane.WARNING_MESSAGE);
    }

    /**
     * Shows a dialog box with a question and the "yes" and "no" options.
     * @param message the question to show
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
     * Shows a dialog box with a message and the "OK" and "Cancel" options.
     * @param message message the message to show
     * @return true if the user chose "OK"
     */
    public static boolean okCancelDialog(String message) {

        int result = JOptionPane.showConfirmDialog(null,
                message,
                "Confirmation",
                JOptionPane.OK_CANCEL_OPTION,
                JOptionPane.INFORMATION_MESSAGE);

        return (result == JOptionPane.OK_OPTION);
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

