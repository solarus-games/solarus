/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
package org.solarus.editor.gui;

import java.awt.*;
import javax.swing.*;

/**
 * A simple dialog with a JColorChooser.
 */
public class ColorChooserDialog extends OkCancelDialog {

    /**
     * The JColorChooser component.
     */
    private JColorChooser colorChooser;
    
    /**
     * The color selected by the user.
     */
    private Color selectedColor;

    /**
     * Constructor.
     */
    public ColorChooserDialog() {
	super("Please pick a color", false);
	colorChooser = new JColorChooser();
	setComponent(colorChooser);
    }

    /**
     * Takes into account the color choice made by the user.
     */
    protected void applyModifications() {
	this.selectedColor = colorChooser.getColor();
    }

    /**
     * Displays the dialog box and returns the selected color.
     * @param initialColor initial color of the JColorChooser
     * @return the color selected by the user, or null if the dialog was cancelled.
     */
    public Color display(Color initialColor) {
	colorChooser.setColor(initialColor);
	super.display();

	return selectedColor;
    }
}
