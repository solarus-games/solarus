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

import org.solarus.editor.*;
import org.solarus.editor.entities.*;
import javax.swing.*;

/**
 * \brief A component that where the user can type the new id of a tile pattern
 * in a tileset.
 *
 * The component is composed of a text field to input the new id and of a
 * checkbox to update maps refering to this tile pattern.
 */
public class TilePatternIdRefactoringComponent extends JPanel {

    /**
     * The text field where the user enters the new id.
     */
    private JTextField patternIdField;

    /**
     * Constructor.
     * @param tileset A tileset.
     * @param oldPatternId The tile pattern id to change in this tileset.
     */
    public TilePatternIdRefactoringComponent(Tileset tileset, String oldPatternId) {
        super();

        setLayout(new BoxLayout(this, BoxLayout.PAGE_AXIS));
        setBorder(BorderFactory.createEmptyBorder(5, 5, 5, 5));

        JPanel patternIdPanel = new JPanel();
        patternIdPanel.setLayout(new BoxLayout(patternIdPanel, BoxLayout.LINE_AXIS));
        patternIdPanel.setAlignmentX(1.0f);
        JLabel patternIdLabel = new JLabel("Tile pattern id:");
        patternIdPanel.add(patternIdLabel);
        patternIdPanel.add(Box.createHorizontalStrut(10));
        patternIdField = new JTextField(oldPatternId, 20);
        patternIdField.selectAll();
        patternIdPanel.add(patternIdField);
        patternIdPanel.setAlignmentX(0.0f);
        add(patternIdPanel);
        add(Box.createVerticalStrut(10));
        JCheckBox updateMapsCheckBox = new JCheckBox("Update references in existing maps");
        updateMapsCheckBox.setSelected(true);
        updateMapsCheckBox.setAlignmentX(0.0f);
        add(updateMapsCheckBox);
    }

    /**
     * Returns the new id entered by the user.
     */
    public String getTilePatternId() {
        return patternIdField.getText();
    }
}

