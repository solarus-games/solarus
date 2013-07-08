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
import org.luaj.vm2.*;
import org.luaj.vm2.lib.*;
import org.luaj.vm2.compiler.*;

/**
 * This class contains the list of the game resources and their name in the project.
 * Each resource name is associated with an id.
 * This class stores the information of the file project_db.dat.
 */
public class ResourceDatabase extends Observable {

    /**
     * Name of the file storing the resource database.
     */
    public static final String fileName = "project_db.dat";

    /**
     * The project.
     */
    private Project project;

    /**
     * The resources.
     */
    private Resource[] resources;

    /**
     * Creates the list of the game resources from the file project_db.dat.
     */
    public ResourceDatabase(Project project) {

        this.project = project;
        this.resources = new Resource[ResourceType.values().length];
        for (ResourceType resourceType: ResourceType.values()) {
            resources[resourceType.getId()] = new Resource();
        }
    }

    /**
     * Returns a resource.
     * @param resourceType type of the resource to get
     * @return the resource of this type
     */
    public Resource getResource(ResourceType resourceType) {
        return resources[resourceType.getId()];
    }

    /**
     * Clears all resources.
     */
    public void clear() {
        for (ResourceType resourceType: ResourceType.values()) {
            getResource(resourceType).clear();
        }
    }

    /**
     * Reads the file project_db.dat of the project, i.e. the list
     * of the game resources and their name.
     * @throws QuestEditorException if the file contains an error.
     * @throws IOException if the file could not be loaded.
     */
    public void load() throws QuestEditorException, IOException {

        try {
            File dbFile = project.getResourceDatabaseFile();
            LuaC.install();
            LuaTable environment = LuaValue.tableOf();

            for (ResourceType resourceType: ResourceType.values()) {
                environment.set(resourceType.getLuaName(),
                        new ResourceElementDeclarationFunction(resourceType));
            }

            LuaFunction code = LoadState.load(new FileInputStream(dbFile),
                dbFile.getName(), environment);
            code.call();
        }
        /* TODO enable this once the caller is fixed
        catch (IOException ex) {
            throw new QuestEditorException(ex.getMessage());
        }
        */
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
            File dbFile = project.getResourceDatabaseFile();
            PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(dbFile)));
            Iterator<String> it;

            // save each resource
            for (ResourceType resourceType: ResourceType.values()) {
                Resource resource = getResource(resourceType);
                it = resource.iterator();

                while (it.hasNext()) {
                    String id = it.next();
                    String name = resource.getElementName(id);
                    out.print(resourceType.getLuaName());
                    out.print("{ id = \"");
                    out.print(id);
                    out.print("\", name = \"");
                    out.print(name);
                    out.print("\" }");
                    out.println();
                }
                out.println();
            }
            out.close();
        }
        catch (IOException ex) {
            throw new QuestEditorException(ex.getMessage());
        }
        catch (QuestEditorException ex) {
            System.err.println("Unexpected error: " + ex.getMessage());
            ex.printStackTrace();
            System.exit(1);
        }
    }

    /**
     * @brief Lua function called by the quest resource list file to declare
     * a resource element.
     */
    private class ResourceElementDeclarationFunction extends OneArgFunction {

        private ResourceType type;

        /**
         * @brief Constructor for a specified type of resource.
         * @param type The resource type to use.
         */
        public ResourceElementDeclarationFunction(ResourceType type) {
            super();
            this.type = type;
        }

        public LuaValue call(LuaValue arg) {

            try {
                LuaTable table = arg.checktable();
                String id = table.get("id").checkjstring();
                String name = table.get("name").checkjstring();
                getResource(type).setElementName(id, name);
            }
            catch (QuestEditorException ex) {
                // Error in the input file.
                throw new LuaError(ex);
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

