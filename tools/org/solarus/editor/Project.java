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
package org.solarus.editor;

import java.io.*;
import java.text.*;
import java.util.*;
import java.net.*;
import java.awt.image.*;
import javax.swing.*;
import javax.imageio.*;

/**
 * This class contains the information about the ZSDX project currently open.
 * Most of the methods are static: they are applied to the current project.
 */
public class Project {

    /**
     * Root path of the project.
     */
    private String projectPath;

    /**
     * The game resources associated to this project.
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
     * Creates a new ZSDX project in the specified path and sets it
     * as the current project.
     * @param path root path of the project
     * @return the project created, or null if there was already an existing project
     */
    public static Project createNew(String path) {

	Project project = new Project(path);

	try {
	    project.resourceDatabase.load();
	    
	    // if no exception was raised, a project exists (and has been successfully loaded)
	    project = null;
	}
	catch (IOException ex) {
	    // normal case: there is no project file yet
	    setCurrentProject(project);
	}
	catch (ZSDXException ex) {
	    // a project exists (and the project file is not valid)
	    project = null;
	}

	return project;
    }

    /**
     * Loads an existing ZSDX project in the specified path and sets it
     * as the current project.
     * @param path root path of the project
     * @return the project created, or null if there is no project in this path
     * @throws ZSDXException if the project exists but the project file is not valid
     */
    public static Project createExisting(String path) throws ZSDXException {

	Project project = new Project(path);

	try {
	    project.resourceDatabase.load();
	    
	    // normal case: a project exists and has been successfully loaded
	    setCurrentProject(project);
	}
	catch (IOException ex) {
	    // the project doesn't exist
	    project = null;
	}

	return project;
    }
    
    /**
     * Sets the specified project as the current ZSDX project.
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
     * This method can be called event if this project is not the current project.
     * @return the file containing the database of the game resources
     */
    public File getResourceDatabaseFile() {
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
     * If the image have been already loaded, it is not loaded again.
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

	NumberFormat nf = NumberFormat.getInstance();
	nf.setMinimumIntegerDigits(4);
	nf.setGroupingUsed(false);

	return new File(getTilesetPath() + "/tileset"
		+ nf.format(Integer.parseInt(tilesetId)) + ".dat");
    }

    /**
     * Returns the image file of a tileset from its id for the current project.
     * @param tilesetId id of a tileset
     * @return the corresponding tileset file
     */
    public static File getTilesetImageFile(String tilesetId) {
	
	NumberFormat nf = NumberFormat.getInstance();
	nf.setMinimumIntegerDigits(4);
	nf.setGroupingUsed(false);

	return new File(getTilesetPath() + "/tileset"
		+ nf.format(Integer.parseInt(tilesetId)) + "_tiles.png");
    }

    /**
     * Returns the entities image file of a tileset from its id for the current project.
     * @param tilesetId id of a tileset
     * @return the corresponding tileset entities file
     */
    public static File getTilesetEntitiesImageFile(String tilesetId) {
	
	NumberFormat nf = NumberFormat.getInstance();
	nf.setMinimumIntegerDigits(4);
	nf.setGroupingUsed(false);

	return new File(getTilesetPath() + "/tileset"
		+ nf.format(Integer.parseInt(tilesetId)) + "_entities.png");
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

	NumberFormat nf = NumberFormat.getInstance();
	nf.setMinimumIntegerDigits(4);
	nf.setGroupingUsed(false);

	return new File(getMapPath() + "/map" + nf.format(Integer.parseInt(mapId)) + ".lua");
    }

    /**
     * Returns a map file knowing its id for the current project.
     * @param mapId id of a map
     * @return the file corresponding to this id
     */
    public static File getMapFile(String mapId) {

	NumberFormat nf = NumberFormat.getInstance();
	nf.setMinimumIntegerDigits(4);
	nf.setGroupingUsed(false);

	return new File(getMapPath() + "/map" + nf.format(Integer.parseInt(mapId)) + ".dat");
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
     * @param animationSetId id of a spite animation set
     * @return the file corresponding to this id
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
     * Returns the path of the languages files, determined with the current project root path.
     * @return the path of the languages files
     */
    public static String getDialogsPath() {
        return getDataPath() + "/languages";
    }

    /**
     * Returns a map file knowing its id for the current project.
     * @param mapId id of a map
     * @return the file corresponding to this id
     */
    public static File getDialogsFile(String fileName) {



	return new File(getDialogsPath() + File.separator + fileName);
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
}
