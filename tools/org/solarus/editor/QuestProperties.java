/*
 * Copyright (C) 2006-2013 Christopho, Solarus - http://www.solarus-games.org
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

import java.io.*;
import java.util.*;
import java.util.Map.Entry;

import org.luaj.vm2.*;
import org.luaj.vm2.lib.*;
import org.luaj.vm2.compiler.*;

/**
 * This class contains the properties of the quest.
 * This class stores the information of the file quest.dat.
 */
public class QuestProperties extends Observable {

    /**
     * Name of the file storing the quest properties.
     */
    public static final String fileName = "quest.dat";

    /**
     * The project.
     */
    private Project project;

    /**
     * All properties of the quest.
     */
    private LinkedHashMap<String, String> properties;

    /**
     * Constructor.
     */
    public QuestProperties(Project project) {

        this.project = project;
    }

    /**
     * Reads the file quest.dat of the project.
     * @throws QuestEditorException If the file could not be loaded or
     * contains an error.
     */
    public void load() throws QuestEditorException {

        try {
            File file = project.getQuestPropertiesFile();
            LuaC.install();
            LuaTable environment = LuaValue.tableOf();

            environment.set("quest", new QuestPropertiesFunction());

            LuaFunction code = LoadState.load(new FileInputStream(file),
                file.getName(), environment);
            code.call();
        }
        catch (IOException ex) {
            throw new QuestEditorException(ex.getMessage());
        }
        catch (LuaError ex) {
            if (ex.getCause() != null) {
                throw new QuestEditorException(ex.getCause().getMessage());
            }
            else {
                throw new QuestEditorException(ex.getMessage());
            }
        }
    }

    /**
     * Saves the list of the game resources and their names into the file project_db.dat.
     * @throws QuestEditorException if the file could not be written
     */
    public void save() throws QuestEditorException {

        try {
            File dbFile = project.getQuestPropertiesFile();
            PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(dbFile)));

            // Save each property.
            for (Entry<String, String> property: properties.entrySet()) {
                // TODO
                out.println();
            }
            out.close();
        }
        catch (IOException ex) {
            throw new QuestEditorException(ex.getMessage());
        }
    }

    /**
     * @brief Lua function called by the quest properties list file to declare
     * the quest properties.
     */
    private class QuestPropertiesFunction extends OneArgFunction {

        /**
         * @brief Constructor.
         */
        public QuestPropertiesFunction() {
            super();
        }

        public LuaValue call(LuaValue arg) {

            try {
                LuaTable table = arg.checktable();
                String solarusVersion = table.get("solarus_version").checkjstring();
                String writeDir = table.get("write_dir").optjstring(null);
                String titleBar = table.get("title_bar").optjstring(null);
                String normalQuestSize = table.get("normal_quest_size").optjstring(null);
                String minQuestSize = table.get("min_quest_size").optjstring(null);
                String maxQuestSize = table.get("max_quest_size").optjstring(null);
            }
            catch (LuaError ex) {
                // Error in the input file.
                throw ex;
            }
            catch (Exception ex) {
                // Error in the editor.
                ex.printStackTrace();
                throw new LuaError(ex);
            }
            return LuaValue.NIL;
        }
    }
}

