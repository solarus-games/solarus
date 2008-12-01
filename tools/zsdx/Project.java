package zsdx;

import java.io.*;
import java.text.*;
import java.util.*;
import java.net.*;
import java.awt.Image;
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
    private TreeMap<String, Image> imagesLoaded;
    
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
	imagesLoaded = new TreeMap<String, Image>();
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
     * Returns the file containing the database of the game resources (projet_db.zsd).
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
     * @param resourceType type of the resource to get: ResourceDatabse.RESOURCE_MAP,
     * ResourceDatabse.RESOURCE_MUSIC, etc.
     * @return the resource of this type in the current project's resource database
     */
    public static Resource getResource(int resourceType) {
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
     * Returns the path of the image files of the game,
     * determined with the current project root path.
     * @return the path of the image files of the game
     */
    public static String getGameImagePath() {
	return getDataPath() + "/images";
    }

    /**
     * Loads an image of the editor from a specified file name.
     * The image loaded is not project dependent.
     * If the image have been already loaded, it is not loaded again.
     * @param imageFileName the name of the image file to read, relative to the
     * images directory of the editor
     * @return the image
     */
    public static Image getEditorImage(String imageFileName) {

	// see if the image has been already loaded
	Image image = currentProject.imagesLoaded.get(imageFileName);

	if (image == null) {
	    try {
                URL url = Project.class.getResource("/zsdx/images/" + imageFileName);
                if (url == null) {
                    throw new IOException("File not found");
                }
		image = ImageIO.read(url);
		currentProject.imagesLoaded.put(imageFileName, image);
	    }
	    catch (IOException ex) {
		System.err.println("Cannot load image '" + imageFileName + "': " + ex.getMessage());
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
		+ nf.format(Integer.parseInt(tilesetId)) + ".zsd");
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

	return new File(getGameImagePath() + "/tilesets/tileset"
		+ nf.format(Integer.parseInt(tilesetId)) + ".png");
    }

    /**
     * Returns the path of the map files, determined with the current project root path.
     * @return the path of the map files
     */
    public static String getMapPath() {
	return getDataPath() + "/maps";
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

	return new File(getMapPath() + "/map" + nf.format(Integer.parseInt(mapId)) + ".zsd");
    }

    /**
     * Returns the path of the music files, determined with the current project root path.
     * @return the path of the music files
     */
    public static String getMusicPath() {
	return getDataPath() + "/music";
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
