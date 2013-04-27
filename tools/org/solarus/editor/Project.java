/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-games.org
 *
 * Solarus Quest Editor is free software; you can redistribute it and/or modify
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
package org.solarus.editor;

import java.io.*;
import java.util.*;
import java.net.*;
import java.awt.image.*;
import javax.swing.*;
import javax.imageio.*;

/**
 * This class contains the information about the project currently open.
 * Most of the methods are static: they are applied to the current project.
 */
public class Project {

    /**
     * Root path of the project.
     */
    private String projectPath;

    /**
     * The resources associated to this project.
     */
    private ResourceDatabase resourceDatabase;

    /**
     * Table of the images that have been read.
     */
    private TreeMap<String, BufferedImage> editorImagesLoaded;
    private TreeMap<String, BufferedImage> projectImagesLoaded;

    /**
     * Objects to notify when a project is loaded.
     */
    private static List<ProjectObserver> observers = new ArrayList<ProjectObserver>();

    /**
     * The project currently open.
     */
    private static Project currentProject;

    /**
     * Constructs a ZSDX project with the specified path.
     * @param path root path of the project
     */
    private Project(String path) {
        this.projectPath = path;
        resourceDatabase = new ResourceDatabase(this);
        editorImagesLoaded = new TreeMap<String, BufferedImage>();
        projectImagesLoaded = new TreeMap<String, BufferedImage>();
    }

    /**
     * Creates a new project in the specified path and sets it
     * as the current project.
     * @param path root path of the project
     * @throws QuestEditorException if the project could not be created.
     */
    public static void createNew(String path) throws QuestEditorException {

        Project project = new Project(path);

        try {
            project.resourceDatabase.load();

            // if no exception was raised, a project exists (and has been successfully loaded)
            throw new QuestEditorException("A project already exists in this directory");
        }
        catch (IOException ex) {
            // normal case: there is no project file yet
            setCurrentProject(project);
            project.createInitialFiles();
        }
    }

    /**
     * Loads an existing project in the specified path and sets it
     * as the current project.
     * @param path root path of the project
     * @throws QuestEditorException if the project exists but the project file is not valid
     */
    public static void createExisting(String path) throws QuestEditorException {

        Project project = new Project(path);

        try {
            project.resourceDatabase.load();

            // normal case: a project exists and has been successfully loaded
            setCurrentProject(project);
        }
        catch (IOException ex) {
            // the project doesn't exist
            throw new QuestEditorException(ex.getMessage());
        }
    }

    /**
     * Sets the specified project as the current project.
     * The project observers are notified.
     * @param project the current project
     */
    private static void setCurrentProject(Project project) {
        currentProject = project;
        for (ProjectObserver o: observers) {
            o.currentProjectChanged();
        }
    }

    /**
     * Returns the file containing the database of the game resources (project_db.dat).
     * This method can be called even if this project is not the current project.
     * @return the file containing the database of the game resources
     */
    public File getResourceDatabaseFile() {
        // We want this method to work when this project is not the current one.
        return new File(projectPath + "/data/" + ResourceDatabase.fileName);
    }

    /**
     * Returns whether a project is currently loaded.
     * @return true if a project is loaded
     */
    public static boolean isLoaded() {
        return currentProject != null;
    }

    /**
     * Returns the resource database object of the current project project.
     * @return the game resource database
     */
    public static ResourceDatabase getResourceDatabase() {
        return currentProject.resourceDatabase;
    }

    /**
     * Returns a resource in the current project's resource database.
     * @param resourceType type of the resource to get
     * @return the resource of this type in the current project's resource database
     */
    public static Resource getResource(ResourceType resourceType) {
        return getResourceDatabase().getResource(resourceType);
    }

    /**
     * Returns the root path of the current project.
     * @return the root path
     */
    public static String getRootPath() {
        return currentProject.projectPath;
    }

    /**
     * Returns the data path of the current project.
     * @return the path of all data files
     */
    public static String getDataPath() {
        return getRootPath() + "/data";
    }

    /**
     * Loads an image of the editor from a specified file name.
     * If the image have been already loaded, it is not loaded again.
     * @param imageFileName the name of the image file to read, relative to the
     * images directory of the editor
     * @return the image
     */
    public static BufferedImage getEditorImage(String imageFileName) {

        // see if the image has been already loaded
        BufferedImage image = currentProject.editorImagesLoaded.get(imageFileName);

        if (image == null) {
            try {
                String path = "/org/solarus/editor/images/" + imageFileName;
                URL url = Project.class.getResource(path);
                if (url == null) {
                    throw new IOException("File not found: " + path);
                }
                image = ImageIO.read(url);
                currentProject.editorImagesLoaded.put(imageFileName, image);
            }
            catch (IOException ex) {
                System.err.println("Cannot load image '" + imageFileName + "': " + ex.getMessage());
                ex.printStackTrace();
                System.exit(1);
            }
        }
        return image;
    }

    /**
     * Loads an image icon of the editor from a specified file name.
     * The image loaded is not project dependent.
     * @param imageFileName the name of the image file to read, relative to the
     * images directory of the editor
     * @return the image
     */
    public static ImageIcon getEditorImageIcon(String imageFileName) {

        ImageIcon icon = null;
        try {
            String path = "/org/solarus/editor/images/" + imageFileName;
            URL url = Project.class.getResource(path);
            if (url == null) {
                throw new IOException("File not found: " + path);
            }
            icon = new ImageIcon(url);
        }
        catch (IOException ex) {
            System.err.println("Cannot load image '" + imageFileName + "': " + ex.getMessage());
            ex.printStackTrace();
            System.exit(1);
        }

        return icon;
    }

    /**
     * Loads an image of the project from a specified file name.
     * If the image has been already loaded, it is not loaded again.
     * @param imageFileName the name of the image file to read, relative to the
     * data path of the project
     * @return the image
     */
    public static BufferedImage getProjectImage(String imageFileName) {

        // see if the image has been already loaded
        BufferedImage image = currentProject.projectImagesLoaded.get(imageFileName);

        if (image == null) {
          String path = getDataPath() + "/" + imageFileName;

          try {
            image = ImageIO.read(new File(path));
            currentProject.projectImagesLoaded.put(imageFileName, image);
          }
          catch (IOException ex) {
            System.err.println("Cannot load image '" + imageFileName + "': " + ex.getMessage());
            ex.printStackTrace();
            System.exit(1);
          }
        }

        return image;
    }

    /**
     * Returns the quest properties file.
     * @return The quest properties file.
     */
    public static File getQuestPropertiesFile() {
        return new File(getDataPath() + "/quest.dat");
    }

    /**
     * Returns the languages path.
     * @return The languages path.
     */
    public static String getLanguagePath() {
        return getDataPath() + "/languages";
    }

    /**
     * Returns the languages directory.
     * @return The languages directory.
     */
    public static File getLanguageDir() {
        return new File(getLanguagePath());
    }

    /**
     * Returns the language list file.
     * @return The language list file.
     */
    public static File getLanguageListFile() {
        return new File(getLanguagePath() + "/languages.dat");
    }

    /**
     * Returns the path of the text directory.
     * @return The path of the text directory.
     */
    public static String getTextPath() {
        return getDataPath() + "/text";
    }

    /**
     * Returns the text directory.
     * @return The text directory.
     */
    public static File getTextDir() {
        return new File(getTextPath());
    }

    /**
     * Returns the fonts file.
     * @return The font list file.
     */
    public static File getFontsFile() {       
        return new File(getTextPath() + "/fonts.dat");
    }

    /**
     * Returns the path of the tileset files, determined with the current project root path.
     * @return the path of the tileset files
     */
    public static String getTilesetPath() {
        return getDataPath() + "/tilesets";
    }

    /**
     * Returns a tileset file from its id for the current project.
     * @param tilesetId id of a tileset
     * @return the corresponding tileset file
     */
    public static File getTilesetFile(String tilesetId) {

        return new File(getTilesetPath() + "/" + tilesetId + ".dat");
    }

    /**
     * Returns the image file of a tileset from its id for the current project.
     * @param tilesetId id of a tileset
     * @return the corresponding tileset file
     */
    public static File getTilesetImageFile(String tilesetId) {

        return new File(getTilesetPath() + "/" + tilesetId + ".tiles.png");
    }

    /**
     * Returns the entities image file of a tileset from its id for the current project.
     * @param tilesetId id of a tileset
     * @return the corresponding tileset entities file
     */
    public static File getTilesetEntitiesImageFile(String tilesetId) {

        return new File(getTilesetPath() + "/" + tilesetId + ".entities.png");
    }

    /**
     * Returns the path of the map files, determined with the current project root path.
     * @return the path of the map files
     */
    public static String getMapPath() {
        return getDataPath() + "/maps";
    }

    /**
     * Returns a map script file knowing its id for the current project.
     * @param mapId id of a map
     * @return the script file corresponding to this id
     */
    public static File getMapScriptFile(String mapId) {

        return new File(getMapPath() + "/" + mapId + ".lua");
    }

    /**
     * Returns a map file knowing its id for the current project.
     * @param mapId id of a map
     * @return the file corresponding to this id
     */
    public static File getMapFile(String mapId) {

        return new File(getMapPath() + "/" + mapId + ".dat");
    }

    /**
     * Returns the path of the sprite files, determined with the current project root path.
     * @return the path of the sprite files
     */
    public static String getSpritePath() {
        return getDataPath() + "/sprites";
    }

    /**
     * Returns a sprite animation set description file knowing its id for the current project.
     * @param animationSetId Id of a spite animation set.
     * @return The file corresponding to this id.
     */
    public static File getSpriteFile(String animationSetId) {

        return new File(getSpritePath() + "/" + animationSetId + ".dat");
    }

    /**
     * Returns the path of the music files, determined with the current project root path.
     * @return the path of the music files
     */
    public static String getMusicPath() {
        return getDataPath() + "/music";
    }

    /**
     * Returns the path of the languages files,
     * determined with the current project root path.
     * @param languageId Id of a language.
     * @return The path of the languages files.
     */
    public static String getLanguagePath(String languageId) {
        return getDataPath() + "/languages/" + languageId;
    }

    /**
     * Returns a dialogs file for the current project.
     * @param languageId Id of a language.
     * @return The dialogs file in this language.
     */
    public static File getDialogsFile(String languageId) {
        return new File(getLanguagePath(languageId) + "/text/dialogs.dat");
    }

    /**
     * Returns a strings file for the current project.
     * @param languageId Id of a language.
     * @return The dialogs file in this language.
     */
    public static File getStringsFile(String languageId) {
        return new File(getLanguagePath(languageId) + "/text/strings.dat");
    }

    /**
     * Returns an enemy script file for the current project.
     * @param enemyId Id of an enemy.
     * @return The enemy script file.
     */
    public static File getEnemyScriptFile(String enemyId) {
        return new File(getDataPath() + "/enemies/" + enemyId + ".lua");
    }

    /**
     * Returns an equipment item script file for the current project.
     * @param itemId Id of an equipment item.
     * @return The item script file.
     */
    public static File getItemScriptFile(String itemId) {
        return new File(getDataPath() + "/items/" + itemId + ".lua");
    }

    /**
     * Adds an object to notify when a project is created or loaded.
     * @param observer the object to notify
     */
    public static void addProjectObserver(ProjectObserver observer) {
        observers.add(observer);
    }

    /**
     * Removes an object to notify when a project is created or loaded.
     * @param observer the object to stop notifying
     */
    public static void removeProjectObserver(ProjectObserver observer) {
        observers.remove(observer);
    }

    /**
     * Creates the initial files tree of a new project.
     * @throw QuestEditorException If something went wrong while creating the files.
     */
    private void createInitialFiles() throws QuestEditorException {

        try {
            // Data directory.
            File dataDir = new File(getDataPath());
            if (!dataDir.exists()) {
                if (!dataDir.mkdir()) {
                    throw new QuestEditorException("Failed to create the \"data\" directory");
                }
            }
    
            // Create the resource database file.
            File resourceDatabaseFile = getResourceDatabaseFile();
            if (!resourceDatabaseFile.createNewFile()) {
                // This file determines whether the project exists or not.
                // Therefore, it must not exist in this function.
                throw new QuestEditorException("Failed to create the resource database file '"
                        + resourceDatabaseFile.getPath() + "'");
            }

            // Create the various needed files if not existing.
            getQuestPropertiesFile().createNewFile();
            getLanguageDir().mkdir();
            getLanguageListFile().createNewFile();
            getTextDir().mkdir();
            getFontsFile().createNewFile();
        }
        catch (IOException ex) {
            ex.printStackTrace();
            throw new QuestEditorException(ex.getMessage());
        }
    }

    /**
     * Creates a new resource element: create the files,
     * registers it in the list of resources and notifies project observers.
     *
     * @param resourceType Type of element to create.
     * @param id Id of the element to create.
     * @param name A human-readable name for this element. 
     * @throws QuestEditorException If the element could not be created.
     */
    public static void newResourceElement(ResourceType resourceType,
            String id, String name)
            throws QuestEditorException {

        try {
            // Create the files.
            createResourceElementFiles(resourceType, id);

            // Add the resource to the resource list.
            getResource(resourceType).addElement(id, name);

            // Notify the GUI.
            for (ProjectObserver o: observers) {
                o.resourceElementAdded(resourceType, id);
            }
        }
        catch (IOException ex) {
            throw new QuestEditorException(ex.getMessage());
        }
    }

    /**
     * Creates the appropriate files for a new resource element,
     * unless they already exist.
     *
     * @param resourceType Type of element to create.
     * @param id Id of the element being created.
     * @throws IOException If a file could not be created.
     */
    private static void createResourceElementFiles(
            ResourceType resourceType, String id)
            throws IOException {

        switch (resourceType) {

        case MAP:
            getMapFile(id).createNewFile();
            getMapScriptFile(id).createNewFile();
            break;

        case TILESET:
            getTilesetFile(id).createNewFile();
            break;

        case LANGUAGE:
            getDialogsFile(id).createNewFile();
            getStringsFile(id).createNewFile();
            break;

        case ENEMY:
            getEnemyScriptFile(id).createNewFile();
            break;

        case ITEM:
            getItemScriptFile(id).createNewFile();
            break;

        case SPRITE:
            getSpriteFile(id).createNewFile();
            break;
        }
    }

    /**
     * Deletes a resource element.
     * @param resourceType Type of element to delete.
     * @param id Id of the element to delete.
     * @throws QuestEditorException If the element could not be found
     * or could not be deleted.
     */
    public static void deleteResourceElement(ResourceType resourceType, String id)
            throws QuestEditorException {

        try {
            // Delete the files.
            deleteResourceElementFiles(resourceType, id);

            // Remove the resource to the resource list.
            getResource(resourceType).removeElement(id);

            // Notify the GUI.
            for (ProjectObserver o: observers) {
                o.resourceElementRemoved(resourceType, id);
            }
        }
        catch (IOException ex) {
            throw new QuestEditorException(ex.getMessage());
        }
    }

    /**
     * Deletes the files of new resource element,
     * unless they don't exist.
     *
     * @param resourceType Type of element to create.
     * @param id Id of the element being deleted.
     * @throws IOException If a file could not be deleted.
     */
    private static void deleteResourceElementFiles(
            ResourceType resourceType, String id)
            throws IOException {

        switch (resourceType) {

        case MAP:
            getMapFile(id).delete();
            getMapScriptFile(id).delete();
            break;

        case TILESET:
            getTilesetFile(id).delete();
            getTilesetImageFile(id).delete();
            getTilesetEntitiesImageFile(id).delete();
            break;

        case LANGUAGE:
            getDialogsFile(id).delete();
            getStringsFile(id).delete();
            break;

        case ENEMY:
            getEnemyScriptFile(id).delete();
            break;

        case ITEM:
            getItemScriptFile(id).delete();
            break;

        case SPRITE:
            getSpriteFile(id).delete();
            break;
        }
    }

    /**
     * Changes the id of a resource element.
     * @param resourceType Type of element to move.
     * @param oldId Old id of the element.
     * @param newId New id of the element.
     * @throws QuestEditorException If the element could not be found
     * or its id could not be changed.
     */
    public static void moveElement(ResourceType resourceType, String oldId, String newId)
            throws QuestEditorException {

        try {
            // Rename the files.
            moveResourceElementFiles(resourceType, oldId, newId);
    
            // Move the resource in the resource list.
            getResource(resourceType).moveElement(oldId, newId);
    
            // Notify the GUI.
            for (ProjectObserver o: observers) {
                o.resourceElementMoved(resourceType, oldId, newId);
            }
        }
        catch (IOException ex) {
            throw new QuestEditorException(ex.getMessage());
        }
    }

    /**
     * Moves the files of a resource element.
     *
     * @param resourceType Type of element.
     * @param oldId Id of the element to move.
     * @param newId Id new id of the element.
     * @throws IOException If a file could not be moved.
     */
    private static void moveResourceElementFiles(
            ResourceType resourceType, String oldId, String newId)
            throws IOException {

        switch (resourceType) {

        case MAP:
            renameFile(getMapFile(oldId), getMapFile(newId));
            renameFile(getMapScriptFile(oldId), getMapScriptFile(newId));
            break;

        case TILESET:
            renameFile(getTilesetFile(oldId), getTilesetFile(newId));
            renameFile(getTilesetImageFile(oldId), getTilesetImageFile(newId));
            renameFile(getTilesetEntitiesImageFile(oldId), getTilesetEntitiesImageFile(newId));
            break;

        case LANGUAGE:
            renameFile(getDialogsFile(oldId), getDialogsFile(newId));
            renameFile(getStringsFile(oldId), getStringsFile(newId));
            break;

        case ENEMY:
            renameFile(getEnemyScriptFile(oldId), getEnemyScriptFile(newId));
            break;

        case ITEM:
            renameFile(getItemScriptFile(oldId), getItemScriptFile(newId));
            break;

        case SPRITE:
            renameFile(getSpriteFile(oldId), getSpriteFile(newId));
            break;
        }
    }

    /**
     * Changes the human-readable name of a resource element.
     * @param resourceType Type of element.
     * @param id Id of the element to rename.
     * @param name New name of the element.
     * @throws QuestEditorException If the element could not be found
     * or it could not be renamed.
     */
    public static void renameElement(ResourceType resourceType,
            String id, String name)
            throws QuestEditorException {

        // Move the resource in the resource list.
        getResource(resourceType).setElementName(id, name);

        // Notify the GUI.
        for (ProjectObserver o: observers) {
            o.resourceElementRenamed(resourceType, id, name);
        }
    }

    /**
     * Renames a file into another.
     * If the source file exists, the destination file must not exist.
     * If the source file does not exist, the source file must exist
     * (then we consider that the operation was already done and this is not
     * an error).
     * Other configurations are errors and throw an exception.
     * @param sourceFile The source file.
     * @param destinationFile The destination file.
     * @throws IOException
     */
    private static void renameFile(File sourceFile, File destinationFile)
            throws IOException {

        if (sourceFile.exists()) {
            // Usual case: the source file exists and the destination is not
            // expected to exist.
            if (destinationFile.exists()) {
                throw new IOException("Cannot rename file '" + sourceFile +
                        "' to '" + destinationFile + ": destination file already exists");
            }
        }
        else {
            // Second allowed case: the source file does not exist and the
            // destination file exist. Let's assume it is not a mistake: the
            // user already renamed the file manually.
            if (!destinationFile.exists()) {
                throw new IOException("Cannot rename file '" + sourceFile +
                        "' to '" + destinationFile + ": source file does not exist");
            }
        }

        sourceFile.renameTo(destinationFile);
    }
}
