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
     * Current format of data files supported by the editor.
     */
    public static final String solarusFormat = "1.4";

    /**
     * Root path of the project.
     */
    private String projectPath;

    /**
     * Last part of the project path.
     */
    private String pathBaseName;

    /**
     * Properties of this quest.
     */
    private QuestProperties questProperties;

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
     * Creates a new or existing project with the specified path.
     * @param path Root path of the project.
     */
    private Project(String path) {
        this.projectPath = path;
        if (!path.isEmpty() && path.charAt(path.length() - 1) == File.separatorChar) {
            // Remove the trailing separator.
            path = path.substring(0, path.length() - 1);
        }
        int lastSeparatorIndex = path.lastIndexOf(File.separator);
        if (lastSeparatorIndex != -1) {
            this.pathBaseName = path.substring(lastSeparatorIndex + 1);
        }
        else {
            this.pathBaseName = path;
        }

        questProperties = new QuestProperties(this);
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

        if (project.getQuestPropertiesFile().exists()) {
            throw new QuestEditorException("A project already exists in this directory");
        }

        project.createInitialFiles();
        project.questProperties.load();
        project.resourceDatabase.load();
        project.resourceDatabase.ensureResourceDirectoriesExists();
        setCurrentProject(project);
    }

    /**
     * Loads an existing project in the specified path and sets it
     * as the current project.
     * @param path root path of the project
     * @throws QuestEditorException if the project exists but the project file is not valid
     */
    public static void createExisting(String path) throws QuestEditorException {

        Project project = new Project(path);

        project.questProperties.load();
        project.resourceDatabase.load();
        project.resourceDatabase.ensureResourceDirectoriesExists();
        setCurrentProject(project);
    }

    /**
     * Sets the specified project as the current project.
     * The project observers are notified.
     * @param project the current project
     */
    private static void setCurrentProject(Project project) {
        currentProject = project;
        for (ProjectObserver o: new ArrayList<ProjectObserver>(observers)) {
            o.currentProjectChanged();
        }
    }

    /**
     * Returns the file containing the quest properties (quest.dat).
     * This method can be called even if this project is not the current project.
     * @return The file containing the quest properties.
     */
    public File getQuestPropertiesFile() {
        return new File(projectPath + "/data/" + QuestProperties.fileName);
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
     * Returns the last part of the current project's path.
     * @return The quest directory name.
     */
    public static String getPathBaseName() {
        return currentProject.pathBaseName;
    }

    /**
     * Returns the root path of the current project.
     * @return The root path.
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
     * @throws IOException If the image could not be loaded.
     */
    public static BufferedImage getEditorImage(String imageFileName) throws IOException {

        // see if the image has been already loaded
        BufferedImage image = currentProject.editorImagesLoaded.get(imageFileName);

        if (image == null) {
            try {
                String path = "/org/solarus/editor/images/" + imageFileName;
                URL url = Project.class.getResource(path);
                if (url == null) {
                    throw new IOException("File not found");
                }
                image = ImageIO.read(url);
                currentProject.editorImagesLoaded.put(imageFileName, image);
            }
            catch (IOException ex) {
                throw new IOException("Cannot load image '" + imageFileName + "'");
            }
        }
        return image;
    }

    /**
     * Loads an image of the editor from a specified file name.
     * If the image have been already loaded, it is not loaded again.
     * If the image cannot be loaded, an empty 16x16 image is returned instead.
     * @param imageFileName the name of the image file to read, relative to the
     * images directory of the editor
     * @return the image
     */
    public static BufferedImage getEditorImageOrEmpty(String imageFileName) {

        try {
            return getEditorImage(imageFileName);
        }
        catch (IOException ex) {
            return new BufferedImage(16, 16, BufferedImage.TYPE_INT_ARGB);
        }
    }

    /**
     * Loads an image icon of the editor from a specified file name.
     * The image loaded is not project dependent.
     * @param imageFileName the name of the image file to read, relative to the
     * images directory of the editor
     * @return the image
     * @throws IOException If the image could not be loaded.
     */
    public static ImageIcon getEditorImageIcon(String imageFileName)
        throws IOException {

        try {
            String path = "/org/solarus/editor/images/" + imageFileName;
            URL url = Project.class.getResource(path);
            if (url == null) {
                throw new IOException("File not found");
            }
            return new ImageIcon(url);
        }
        catch (IOException ex) {
            throw new IOException("Cannot load image '" + imageFileName + "'");
        }
    }

    /**
     * Loads an image icon of the editor from a specified file name.
     * The image loaded is not project dependent.
     * If the image cannot be loaded, an empty image is returned instead.
     * @param imageFileName the name of the image file to read, relative to the
     * images directory of the editor
     * @return the image
     */
    public static ImageIcon getEditorImageIconOrEmpty(String imageFileName) {

        try {
            return getEditorImageIcon(imageFileName);
        }
        catch (IOException ex) {
            return new ImageIcon();
        }
    }

    /**
     * Loads an image of the project from a specified file name.
     * If the image has been already loaded, it is not loaded again.
     * @param imageFileName the name of the image file to read, relative to the
     * data path of the project
     * @return the image
     * @throws IOException If the image could not be loaded.
     */
    public static BufferedImage getProjectImage(String imageFileName)
        throws IOException {

        try {
            // See if the image has been already loaded.
            BufferedImage image = currentProject.projectImagesLoaded.get(imageFileName);

            if (image == null) {
                String path = getDataPath() + "/" + imageFileName;

                image = ImageIO.read(new File(path));
                currentProject.projectImagesLoaded.put(imageFileName, image);
            }

            return image;
        }
        catch (IOException ex) {
            throw new IOException("Cannot load image '" + imageFileName + "'");
        }
    }

    /**
     * Loads an image of the project from a specified file name.
     * If the image has been already loaded, it is not loaded again.
     * If the image cannot be loaded, an empty 16x16 image is returned instead.
     * @param imageFileName the name of the image file to read, relative to the
     * data path of the project
     * @return the image
     */
    public static BufferedImage getProjectImageOrEmpty(String imageFileName) {

        try {
            return getProjectImage(imageFileName);
        }
        catch (IOException ex) {
            return new BufferedImage(16, 16, BufferedImage.TYPE_INT_ARGB);
        }
    }

    /**
     * Returns the main Lua script file.
     * @return the main Lua script file.
     */
    public static File getMainScriptFile() {
        return new File(getDataPath() + "/main.lua");
    }

    /**
     * Returns the path of the tileset files, determined with the current project root path.
     * @return the path of the tileset files
     */
    public static String getTilesetPath() {
        return getDataPath() + "/" + ResourceType.TILESET.getDirName();
    }

    /**
     * Returns the tilesets directory.
     * @return The tilesets directory.
     */
    public static File getTilesetDir() {
        return new File(getTilesetPath());
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
        return getDataPath() + "/" + ResourceType.MAP.getDirName();
    }

    /**
     * Returns the maps directory.
     * @return The maps directory.
     */
    public static File getMapDir() {
        return new File(getMapPath());
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
        return getDataPath() + "/"  + ResourceType.SPRITE.getDirName();
    }

    /**
     * Returns the sprites directory.
     * @return The sprites directory.
     */
    public static File getSpriteDir() {
        return new File(getSpritePath());
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
        return getDataPath() + "/" + ResourceType.MUSIC.getDirName();
    }

    /**
     * Returns the musics directory.
     * @return The musics directory.
     */
    public static File getMusicDir() {
        return new File(getMusicPath());
    }

    /**
     * Returns the path of the sound files, determined with the current project root path.
     * @return the path of the sound files
     */
    public static String getSoundPath() {
        return getDataPath() + "/" + ResourceType.SOUND.getDirName();
    }

    /**
     * Returns the sounds directory.
     * @return The sounds directory.
     */
    public static File getSoundDir() {
        return new File(getSoundPath());
    }

    /**
     * Returns the languages path.
     * @return The languages path.
     */
    public static String getLanguagePath() {
        return getDataPath() + "/" +  ResourceType.LANGUAGE.getDirName();
    }

    /**
     * Returns the languages directory.
     * @return The languages directory.
     */
    public static File getLanguageDir() {
        return new File(getLanguagePath());
    }

    /**
     * Returns the path of the languages files, for a language.
     * @param languageId Id of a language.
     * @return The path of the corresponding languages files.
     */
    public static String getLanguagePath(String languageId) {
        return getLanguagePath() + "/" + languageId;
    }

    /**
     * Returns the directory of the languages files, for a language.
     * @param languageId Id of a language.
     * @return The directory of the corresponding languages files.
     */
    public static File getLanguageDir(String languageId) {
        return new File(getLanguagePath(languageId));
    }

    /**
     * Returns the path of the language-specific images for a language.
     * @param languageId Id of a language.
     * @return The path of the corresponding language images.
     */
    public static String getLanguageImagePath(String languageId) {
        return getLanguagePath(languageId) + "/images";
    }

    /**
     * Returns the direcotry of the language-specific images for a language.
     * @param languageId Id of a language.
     * @return The directory of the corresponding language images.
     */
    public static File getLanguageImageDir(String languageId) {
        return new File(getLanguageImagePath(languageId));
    }

    /**
     * Returns the path of the language-specific texts for a language.
     * @param languageId Id of a language.
     * @return The path of the corresponding language texts.
     */
    public static String getLanguageTextPath(String languageId) {
        return getLanguagePath(languageId) + "/text";
    }

    /**
     * Returns the direcotry of the language-specific texts for a language.
     * @param languageId Id of a language.
     * @return The directory of the corresponding language texts.
     */
    public static File getLanguageTextDir(String languageId) {
        return new File(getLanguageTextPath(languageId));
    }

    /**
     * Returns a dialogs file for the current project.
     * @param languageId Id of a language.
     * @return The dialogs file in this language.
     */
    public static File getDialogsFile(String languageId) {
        return new File(getLanguageTextPath(languageId) + "/dialogs.dat");
    }

    /**
     * Returns a strings file for the current project.
     * @param languageId Id of a language.
     * @return The dialogs file in this language.
     */
    public static File getStringsFile(String languageId) {
        return new File(getLanguageTextPath(languageId) + "/strings.dat");
    }

    /**
     * Returns the path of the enemy scripts.
     * @return The path of the enemy scripts.
     */
    public static String getEnemyPath() {
        return getDataPath() + "/" +  ResourceType.ENEMY.getDirName();
    }

    /**
     * Returns the enemies directory.
     * @return The enemies directory.
     */
    public static File getEnemyDir() {
        return new File(getEnemyPath());
    }

    /**
     * Returns an enemy script file for the current project.
     * @param enemyId Id of an enemy.
     * @return The enemy script file.
     */
    public static File getEnemyScriptFile(String enemyId) {
        return new File(getEnemyPath() + "/" + enemyId + ".lua");
    }

    /**
     * Returns the path of the custom entity scripts.
     * @return The path of the custom entity scripts.
     */
    public static String getEntityPath() {
        return getDataPath() + "/" + ResourceType.ENTITY.getDirName();
    }

    /**
     * Returns the custom entities directory.
     * @return The custom entities directory.
     */
    public static File getEntityDir() {
        return new File(getEntityPath());
    }

    /**
     * Returns a custom entity script file for the current project.
     * @param entityId Id of a custom entity model.
     * @return The custom entity script file.
     */
    public static File getEntityScriptFile(String entityId) {
        return new File(getEntityPath() + "/" + entityId + ".lua");
    }

    /**
     * Returns the path of the item scripts.
     * @return The path of the item scripts.
     */
    public static String getItemPath() {
        return getDataPath() + "/" + ResourceType.ITEM.getDirName();
    }

    /**
     * Returns the items directory.
     * @return The items directory.
     */
    public static File getItemDir() {
        return new File(getItemPath());
    }

    /**
     * Returns an equipment item script file for the current project.
     * @param itemId Id of an equipment item.
     * @return The item script file.
     */
    public static File getItemScriptFile(String itemId) {
        return new File(getItemPath() + "/" + itemId + ".lua");
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
            String dataPath = projectPath + "/data";
            File dataDir = new File(dataPath);
            if (!dataDir.exists()) {
                if (!dataDir.mkdir()) {
                    throw new IOException("Failed to create the \"data\" directory");
                }
            }

            // Create the resource database file.
            File resourceDatabaseFile = getResourceDatabaseFile();
            if (resourceDatabaseFile.exists()) {
                // This file determines whether the project exists or not.
                // Therefore, it must not exist in this function.
                throw new QuestEditorException("A project already exists in this directory.");
            }

            // Create the various initial files.
            FileTools.extractZipFile("data.solarus.zip", dataPath);
        }
        catch (IOException ex) {
            ex.printStackTrace();
            throw new QuestEditorException("Failed to extract data.solarus.zip: " + ex.getMessage());
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
            getResourceDatabase().save();

            // Notify the GUI.
            for (ProjectObserver o: new ArrayList<ProjectObserver>(observers)) {
                o.resourceElementAdded(resourceType, id);
            }
        }
        catch (IOException ex) {
            throw new QuestEditorException(ex.getMessage());
        }
    }

    /**
     * Ensures that a parent directory of a file exists.
     * @param file the file
     * @return true if the directory exists or has been create, false otherwise
     */
    public static boolean ensureParentDirectoryExists(File file) {

        if (!file.exists()) {

            file = file.getParentFile();

            if (!file.exists()) {

                if (file.getAbsolutePath().equals(Project.getDataPath())) {
                    return false;
                }
                if (ensureParentDirectoryExists(file)) {
                    file.mkdir();
                }
            }
        }

        return true;
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

        File file;
        switch (resourceType) {

        case MAP:
            file = getMapFile(id);
            ensureParentDirectoryExists(file);
            file.createNewFile();
            getMapScriptFile(id).createNewFile();
            break;

        case TILESET:
            file = getTilesetFile(id);
            ensureParentDirectoryExists(file);
            file.createNewFile();
            break;

        case LANGUAGE:
            getLanguageDir().mkdir();
            getLanguageDir(id).mkdir();
            getLanguageImageDir(id).mkdir();
            getLanguageTextDir(id).mkdir();
            getDialogsFile(id).createNewFile();
            getStringsFile(id).createNewFile();
            break;

        case ENEMY:
            file = getEnemyScriptFile(id);
            ensureParentDirectoryExists(file);
            file.createNewFile();
            break;

        case ITEM:
            file = getItemScriptFile(id);
            ensureParentDirectoryExists(file);
            file.createNewFile();
            break;

        case ENTITY:
            file = getEntityScriptFile(id);
            ensureParentDirectoryExists(file);
            file.createNewFile();
            break;

        case SPRITE:
            file = getSpriteFile(id);
            ensureParentDirectoryExists(file);
            file.createNewFile();
            break;

        case SOUND:
        case MUSIC:
        case FONT:
            // There is nothing to do here.
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
            getResourceDatabase().save();

            // Notify the GUI.
            for (ProjectObserver o: new ArrayList<ProjectObserver>(observers)) {
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

        case ENTITY:
            getEntityScriptFile(id).delete();
            break;

        case SPRITE:
            getSpriteFile(id).delete();
            break;

        case MUSIC:
            // TODO
            break;

        case SOUND:
            // TODO
            break;

        case FONT:
            // TOOD
            break;

        default:
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
            getResourceDatabase().save();

            // Notify the GUI.
            for (ProjectObserver o: new ArrayList<ProjectObserver>(observers)) {
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

        case ENTITY:
            renameFile(getItemScriptFile(oldId), getItemScriptFile(newId));
            break;

        case SPRITE:
            renameFile(getSpriteFile(oldId), getSpriteFile(newId));
            break;

        case SOUND:
            // TODO
            break;

        case MUSIC:
            // TODO
            break;

        case FONT:
            // TODO
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
    public static void renameResourceElement(ResourceType resourceType,
            String id, String name)
            throws QuestEditorException {

        // Move the resource in the resource list.
        getResource(resourceType).setElementName(id, name);
        getResourceDatabase().save();

        // Notify the GUI.
        for (ProjectObserver o: new ArrayList<ProjectObserver>(observers)) {
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

        // make the destination directory if no exists
        File destinationDir = destinationFile.getParentFile();
        if (!destinationDir.exists()) {
            destinationDir.mkdirs();
        }

        sourceFile.renameTo(destinationFile);
    }
}

