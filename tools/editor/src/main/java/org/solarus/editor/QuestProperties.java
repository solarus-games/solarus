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

    private static final String[] propertyNames = {
      "solarus_version",
      "write_dir",
      "title_bar",
      "normal_quest_size",
      "min_quest_size",
      "max_quest_size"
    };

    /**
     * Constructor.
     */
    public QuestProperties(Project project) {

        this.project = project;
        this.properties = new LinkedHashMap<String, String>();
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

            checkVersionCompatibility();
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

    	File dbFile = project.getQuestPropertiesFile();
        try (PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(dbFile)))) {
            out.println("quest{");
            // Save each property.
            for (Entry<String, String> property: properties.entrySet()) {
                if (property.getValue() != null) {
                    out.print("  ");
                    out.print(property.getKey());
                    out.print(" = \"");
                    out.print(property.getValue());
                    out.print("\",");
                    out.println();
                }
            }
            out.println("}");
            out.println();
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
                for (String key: propertyNames) {
                    String value = table.get(key).optjstring(null);
                    properties.put(key, value);
                }
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

    /**
     * @brief Checks that the Solarus version of this quest is compatible with
     * the current version of the editor.
     * @throws QuestEditorException If the version of the quest is not
     * specified in quest.dat or if it does not correspond to the version of
     * the editor.
     */
    private void checkVersionCompatibility() throws QuestEditorException {

        String questVersion = properties.get("solarus_version");
        if (questVersion == null) {
            throw new QuestEditorException("No Solarus version is specified in your quest.dat file!");
        }

        try {
            int dotIndex1 = questVersion.indexOf('.');
            int dotIndex2 = questVersion.indexOf('.', dotIndex1 + 1);
            if (dotIndex2 != -1) {
                // Remove the patch version (it does not break compatibility).
                questVersion = questVersion.substring(0, dotIndex2 - 1);
            }
            int questMajor = Integer.parseInt(questVersion.substring(0, dotIndex1));
            int questMinor = Integer.parseInt(questVersion.substring(dotIndex1 + 1));

            dotIndex1 = Project.solarusFormat.indexOf('.');
            int editorMajor = Integer.parseInt(Project.solarusFormat.substring(0, dotIndex1));
            int editorMinor = Integer.parseInt(Project.solarusFormat.substring(dotIndex1 + 1));

            if (questMajor > editorMajor ||
                    (questMajor == editorMajor && questMinor > editorMinor)) {
                throw new ObsoleteEditorException(questVersion);
            }
            else if (questMajor < editorMajor ||
                    (questMajor == editorMajor && questMinor < editorMinor)) {
                throw new ObsoleteQuestException(questVersion);
            }
        }
        catch (NumberFormatException ex) {
            throw new QuestEditorException("Invalid syntax of solarus_version: '" + questVersion + "'");
        }
    }
}

