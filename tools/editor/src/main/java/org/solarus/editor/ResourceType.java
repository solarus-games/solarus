package org.solarus.editor;

/**
 * Enumeration to identify the different kinds of resources of a quest.
 */
public enum ResourceType {

    MAP("Map", "map", "maps"),
    TILESET("Tileset", "tileset", "tilesets"),
    SPRITE("Sprite", "sprite", "sprites"),
    MUSIC("Music", "music", "musics"),
    SOUND("Sound", "sound", "sounds"),
    ITEM("Item", "item", "items"),
    ENEMY("Enemy", "enemy", "enemies"),
    ENTITY("Custom entity", "entity", "entities"),
    LANGUAGE("Language", "language", "languages"),
    FONT("Font", "font", "fonts");

    /**
     * The human-readable name of the resource type.
     */
    private String name;

    /**
     * The name of this resource type for Lua.
     */
    private String luaName;


    /**
     * The name of the directory that contains resources of this type.
     * The name is relative to the project data directory.
     */
    private String dirName;

    /**
     * Creates a new resource type.
     * @param name Name of the resource type.
     * @param luaName Lua name of the resource type.
     * @param dirName Directory name of the resource type.
     */
    private ResourceType(String name, String luaName, String dirName) {
        this.name = name;
        this.luaName = luaName;
        this.dirName = dirName;
    }

    /**
     * Returns the value with the specified id.
     * @param id id of the value to get
     * @return the value with this id
     * @throws QuestEditorException if there is no resource type with this id
     */
    public static ResourceType get(int id) throws QuestEditorException {

        try {
            return values()[id];
        }
        catch (ArrayIndexOutOfBoundsException ex) {
            throw new QuestEditorException("Unknown resource type '" + id + "'");
        }
    }

    /**
     * Returns the id of this value.
     * @return the id
     */
    public int getId() {
        return ordinal();
    }

    /**
     * Returns the human-readable name of the resource type.
     * This name will be used everywhere in the GUI.
     * @return The name of the resource type.
     */
    public String getName() {
        return name;
    }

    /**
     * Returns the Lua name of the resource type.
     * @return The Lua name of the resource type.
     */
    public String getLuaName() {
        return luaName;
    }

    /**
     * Returns the directory name of the resource type.
     * @return The directory name of the resource type.
     */
    public String getDirName() {
        return dirName;
    }


    /**
     * Returns a string representation of this resource type.
     * @return The name of the resource type.
     */
    public String toString() {
        return getName();
    }
}

