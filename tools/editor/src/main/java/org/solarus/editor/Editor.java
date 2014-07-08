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
package org.solarus.editor;

import org.solarus.editor.gui.*;
import java.awt.Dimension;

/**
 * Main class of the quest editor.
 */
public class Editor {

    /**
     * Entry point.
     * Usage: java org.solarus.editor.Editor [quest_path]  
     */
    public static void main(String[] args) {

        String questPath = (args.length != 0) ? args[0] : null;

        // create the main window
        EditorWindow window = new EditorWindow(questPath);
        window.pack();
        window.setSize(new Dimension(1024, 720));
        window.setVisible(true);
    }
}

